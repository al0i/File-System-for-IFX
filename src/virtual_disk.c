#include <stdio.h>
#include <stdlib.h>

#include "virtual_disk.h"

#define BLOCK_SIZE 512
#define TOTAL_BLOCKS 16
#define FREE_MAP_BLOCK 1
#define METADATA_START_BLOCK 2
#define ROOT_DIR_BLOCK 3
#define DATA_START_BLOCK 4
#define DIRECTORY_ENTRY_NONE -1
#define ROOT_DIRECTORY_SIGNATURE "DIRLS2"

const char *disk_path = "./src/resources/virtual_disk.bin";

typedef struct {
    char signature[8];
    int block_size;
    int total_blocks;
    int free_map_block;
    int metadata_start_block;
    int root_dir_block;
    int data_start_block;
} RootBlock;

typedef struct {
    char signature[8];
    int first_entry;
    int free_entry;
    int entry_count;
} DirectoryBlockHeader;

typedef struct {
    char name[9];
    char extension[4];
    int size;
    int first_block;
    int next_entry;
    int used;
} DirectoryEntry;

/**
 * Preenche uma regiao de memoria com bytes zerados.
 *
 * @param buffer ponteiro para a regiao que sera zerada.
 * @param length quantidade de bytes que devem ser zerados.
 */
static void zero_bytes(void *buffer, size_t length)
{
    unsigned char *bytes = buffer;

    for (size_t i = 0; i < length; i++) {
        bytes[i] = 0;
    }
}

/**
 * Copia uma quantidade fixa de bytes entre duas regioes de memoria.
 *
 * @param destination regiao de destino.
 * @param source regiao de origem.
 * @param length quantidade de bytes que devem ser copiados.
 */
static void copy_bytes(void *destination, const void *source, size_t length)
{
    unsigned char *destination_bytes = destination;
    const unsigned char *source_bytes = source;

    for (size_t i = 0; i < length; i++) {
        destination_bytes[i] = source_bytes[i];
    }
}

/**
 * Calcula o tamanho de um texto terminado por caractere nulo.
 *
 * @param text texto que sera medido.
 * @return quantidade de caracteres antes do terminador nulo.
 */
static size_t text_length(const char *text)
{
    size_t length = 0;

    while (text[length] != '\0') {
        length++;
    }

    return length;
}

/**
 * Copia texto para um destino com tamanho maximo conhecido.
 *
 * @param destination buffer de destino.
 * @param source texto de origem.
 * @param destination_size tamanho total disponivel no destino.
 */
static void copy_text(char *destination, const char *source, size_t destination_size)
{
    size_t i = 0;

    if (destination_size == 0) {
        return;
    }

    while (i < destination_size - 1 && source[i] != '\0') {
        destination[i] = source[i];
        i++;
    }

    destination[i] = '\0';
}

/**
 * Compara dois textos terminados por caractere nulo.
 *
 * @param left primeiro texto.
 * @param right segundo texto.
 * @return 1 se os textos forem iguais; 0 caso contrario.
 */
static int text_equals(const char *left, const char *right)
{
    size_t i = 0;

    while (left[i] != '\0' && right[i] != '\0') {
        if (left[i] != right[i]) {
            return 0;
        }

        i++;
    }

    return left[i] == '\0' && right[i] == '\0';
}

/**
 * Verifica se um caractere pode aparecer em nome ou extensao 8.3.
 *
 * @param character caractere que sera validado.
 * @return 1 se o caractere for permitido; 0 caso contrario.
 */
static int is_valid_filename_character(char character)
{
    if (character >= 'A' && character <= 'Z') {
        return 1;
    }

    if (character >= 'a' && character <= 'z') {
        return 1;
    }

    if (character >= '0' && character <= '9') {
        return 1;
    }

    if (character == '_' || character == '-') {
        return 1;
    }

    return 0;
}

/**
 * Retorna a quantidade maxima de entradas que cabem no bloco de diretorio raiz.
 *
 * @return total de entradas de diretorio suportadas em um bloco.
 */
static int root_directory_capacity()
{
    return (BLOCK_SIZE - (int)sizeof(DirectoryBlockHeader)) / (int)sizeof(DirectoryEntry);
}

/**
 * Escreve um bloco de dados vazio.
 *
 * @param disk arquivo do disco virtual.
 * @param blockNumber numero do bloco de dados que sera zerado.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
static int write_empty_data_block(FILE *disk, int blockNumber)
{
    unsigned char block[BLOCK_SIZE];

    if (blockNumber < DATA_START_BLOCK || blockNumber >= TOTAL_BLOCKS) {
        return -1;
    }

    zero_bytes(block, BLOCK_SIZE);

    if (fseek(disk, blockNumber * BLOCK_SIZE, SEEK_SET) != 0) {
        perror("Erro ao posicionar no bloco de dados");
        return -1;
    }

    if (fwrite(block, sizeof(unsigned char), BLOCK_SIZE, disk) != BLOCK_SIZE) {
        perror("Erro ao inicializar bloco de dados");
        return -1;
    }

    return 0;
}

/**
 * Valida e separa um nome no formato 8.3.
 *
 * @param filename texto no formato NOME.EXT.
 * @param name destino para a parte do nome, com ate 8 caracteres.
 * @param extension destino para a extensao, com ate 3 caracteres.
 * @return 0 se o nome for valido; -1 caso contrario.
 */
static int split_filename_8_3(const char *filename, char *name, char *extension)
{
    int nameIndex = 0;
    int extensionIndex = 0;
    int readingExtension = 0;

    zero_bytes(name, 9);
    zero_bytes(extension, 4);

    for (int i = 0; filename[i] != '\0'; i++) {
        if (filename[i] == '.') {
            if (readingExtension || nameIndex == 0) {
                return -1;
            }

            readingExtension = 1;
            continue;
        }

        if (!is_valid_filename_character(filename[i])) {
            return -1;
        }

        if (readingExtension) {
            if (extensionIndex >= 3) {
                return -1;
            }

            extension[extensionIndex] = filename[i];
            extensionIndex++;
        } else {
            if (nameIndex >= 8) {
                return -1;
            }

            name[nameIndex] = filename[i];
            nameIndex++;
        }
    }

    if (nameIndex == 0 || extensionIndex == 0) {
        return -1;
    }

    return 0;
}

/**
 * Cria a representacao em memoria do superbloco esperado pelo simulador.
 *
 * @return estrutura RootBlock preenchida com assinatura e layout atual.
 */
static RootBlock create_root_block()
{
    RootBlock rootBlock;

    zero_bytes(&rootBlock, sizeof(RootBlock));

    copy_text(rootBlock.signature, "IFXFS", sizeof(rootBlock.signature));
    rootBlock.block_size = BLOCK_SIZE;
    rootBlock.total_blocks = TOTAL_BLOCKS;
    rootBlock.free_map_block = FREE_MAP_BLOCK;
    rootBlock.metadata_start_block = METADATA_START_BLOCK;
    rootBlock.root_dir_block = ROOT_DIR_BLOCK;
    rootBlock.data_start_block = DATA_START_BLOCK;

    return rootBlock;
}

/**
 * Verifica se um superbloco lido do disco corresponde ao layout esperado.
 *
 * @param rootBlock superbloco que sera validado.
 * @return 1 se o superbloco for valido; 0 caso contrario.
 */
static int is_valid_root_block(RootBlock *rootBlock)
{
    if (!text_equals(rootBlock->signature, "IFXFS")) {
        return 0;
    }

    if (rootBlock->block_size != BLOCK_SIZE) {
        return 0;
    }

    if (rootBlock->total_blocks != TOTAL_BLOCKS) {
        return 0;
    }

    if (rootBlock->free_map_block != FREE_MAP_BLOCK) {
        return 0;
    }

    if (rootBlock->metadata_start_block != METADATA_START_BLOCK) {
        return 0;
    }

    if (rootBlock->root_dir_block != ROOT_DIR_BLOCK) {
        return 0;
    }

    if (rootBlock->data_start_block != DATA_START_BLOCK) {
        return 0;
    }

    return 1;
}

/**
 * Abre o disco virtual existente ou cria um novo arquivo de disco.
 *
 * @return ponteiro para o arquivo do disco virtual; NULL em caso de erro.
 */
FILE* init_disk(void)
{
    FILE *disk = fopen(disk_path, "r+b");

    if (disk == NULL) 
        disk = fopen(disk_path, "w+b");

    if (disk == NULL) {
        perror("Erro ao abrir ou criar arquivo.");
        return NULL;
    }

    return disk;
}

/**
 * Zera todos os blocos do disco virtual.
 *
 * Esta funcao e destrutiva e deve ser usada apenas durante formatacao.
 *
 * @param disk arquivo do disco virtual.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
int initialize_virtual_disk(FILE *disk)
{
    unsigned char block[BLOCK_SIZE];

    zero_bytes(block, BLOCK_SIZE);

    if (fseek(disk, 0, SEEK_SET) != 0) {
        perror("Erro ao mover ponteiro para o início do disco");
        return -1;
    }

    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        size_t written = fwrite(block, sizeof(unsigned char), BLOCK_SIZE, disk);

        if (written != BLOCK_SIZE) {
            perror("Erro ao escrever bloco no disco virtual");
            return -1;
        }
    }

    if (fseek(disk, 0, SEEK_SET) != 0) {
        perror("Erro ao retornar ponteiro para o início do disco");
        return -1;
    }

    return 0;
}

/**
 * Le o superbloco armazenado no bloco 0 do disco virtual.
 *
 * @param disk arquivo do disco virtual.
 * @param rootBlock destino onde o superbloco lido sera armazenado.
 * @return 0 se a leitura foi concluida; 1 se nao havia dados suficientes; -1 em erro.
 */
int read_root_block(FILE *disk, RootBlock *rootBlock)
{
    if (rootBlock == NULL) {
        return -1;
    }

    zero_bytes(rootBlock, sizeof(RootBlock));

    if (fseek(disk, 0, SEEK_SET) != 0) {
        perror("Erro ao posicionar no superbloco");
        return -1;
    }

    size_t read = fread(rootBlock, sizeof(RootBlock), 1, disk);

    if (read != 1) {
        if (ferror(disk)) {
            perror("Erro ao ler superbloco");
            return -1;
        }

        clearerr(disk);
        return 1;
    }

    return 0;
}

/**
 * Monta em memoria um bloco de diretorio raiz vazio.
 *
 * @param block bloco de 512 bytes que recebera cabecalho e entradas livres.
 */
static void create_empty_root_directory_block(unsigned char *block)
{
    DirectoryBlockHeader header;
    DirectoryEntry entry;
    int capacity = root_directory_capacity();

    zero_bytes(block, BLOCK_SIZE);
    zero_bytes(&header, sizeof(DirectoryBlockHeader));

    copy_text(header.signature, ROOT_DIRECTORY_SIGNATURE, sizeof(header.signature));
    header.first_entry = DIRECTORY_ENTRY_NONE;
    header.free_entry = capacity > 0 ? 0 : DIRECTORY_ENTRY_NONE;
    header.entry_count = 0;

    copy_bytes(block, &header, sizeof(DirectoryBlockHeader));

    for (int i = 0; i < capacity; i++) {
        zero_bytes(&entry, sizeof(DirectoryEntry));

        entry.size = 0;
        entry.first_block = DIRECTORY_ENTRY_NONE;
        entry.next_entry = i + 1 < capacity ? i + 1 : DIRECTORY_ENTRY_NONE;
        entry.used = 0;

        copy_bytes(
            block + sizeof(DirectoryBlockHeader) + (i * sizeof(DirectoryEntry)),
            &entry,
            sizeof(DirectoryEntry)
        );
    }
}

/**
 * Le o bloco de diretorio raiz.
 *
 * @param disk arquivo do disco virtual.
 * @param block destino com 512 bytes para receber o bloco.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
int read_root_directory_block(FILE *disk, unsigned char *block)
{
    zero_bytes(block, BLOCK_SIZE);

    if (fseek(disk, ROOT_DIR_BLOCK * BLOCK_SIZE, SEEK_SET) != 0) {
        perror("Erro ao posicionar no diretorio raiz");
        return -1;
    }

    if (fread(block, sizeof(unsigned char), BLOCK_SIZE, disk) != BLOCK_SIZE) {
        if (ferror(disk)) {
            perror("Erro ao ler diretorio raiz");
            return -1;
        }

        clearerr(disk);
        return -1;
    }

    return 0;
}

/**
 * Escreve o bloco de diretorio raiz.
 *
 * @param disk arquivo do disco virtual.
 * @param block bloco de diretorio que sera persistido.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
int write_root_directory_block(FILE *disk, unsigned char *block)
{
    if (fseek(disk, ROOT_DIR_BLOCK * BLOCK_SIZE, SEEK_SET) != 0) {
        perror("Erro ao posicionar no diretorio raiz");
        return -1;
    }

    if (fwrite(block, sizeof(unsigned char), BLOCK_SIZE, disk) != BLOCK_SIZE) {
        perror("Erro ao escrever diretorio raiz");
        return -1;
    }

    return 0;
}

/**
 * Garante que o bloco de diretorio raiz esteja no formato de lista ligada.
 *
 * @param disk arquivo do disco virtual.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
int ensure_root_directory_block(FILE *disk)
{
    unsigned char block[BLOCK_SIZE];
    DirectoryBlockHeader header;

    if (read_root_directory_block(disk, block) != 0) {
        create_empty_root_directory_block(block);
        return write_root_directory_block(disk, block);
    }

    zero_bytes(&header, sizeof(DirectoryBlockHeader));
    copy_bytes(&header, block, sizeof(DirectoryBlockHeader));

    if (text_equals(header.signature, ROOT_DIRECTORY_SIGNATURE)) {
        return 0;
    }

    create_empty_root_directory_block(block);
    return write_root_directory_block(disk, block);
}

/**
 * Adiciona uma entrada 8.3 ao diretorio raiz.
 *
 * A entrada guarda nome, tamanho logico e primeiro bloco do arquivo. Os demais
 * blocos devem ser encontrados pela estrategia de alocacao em etapa posterior.
 *
 * @param disk arquivo do disco virtual.
 * @param filename nome no formato 8.3.
 * @param firstBlock primeiro bloco de dados do arquivo.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
int add_root_directory_entry(FILE *disk, const char *filename, int firstBlock)
{
    unsigned char block[BLOCK_SIZE];
    DirectoryBlockHeader header;
    DirectoryEntry entry;
    char name[9];
    char extension[4];
    int allocatedEntry;
    int currentEntry;
    int entryOffset;

    if (firstBlock != DIRECTORY_ENTRY_NONE &&
        (firstBlock < DATA_START_BLOCK || firstBlock >= TOTAL_BLOCKS)) {
        return -1;
    }

    if (split_filename_8_3(filename, name, extension) != 0) {
        return -1;
    }

    if (ensure_root_directory_block(disk) != 0) {
        return -1;
    }

    if (read_root_directory_block(disk, block) != 0) {
        return -1;
    }

    copy_bytes(&header, block, sizeof(DirectoryBlockHeader));

    currentEntry = header.first_entry;

    while (currentEntry != DIRECTORY_ENTRY_NONE) {
        entryOffset = sizeof(DirectoryBlockHeader) + (currentEntry * sizeof(DirectoryEntry));

        zero_bytes(&entry, sizeof(DirectoryEntry));
        copy_bytes(&entry, block + entryOffset, sizeof(DirectoryEntry));

        if (entry.used &&
            text_equals(entry.name, name) &&
            text_equals(entry.extension, extension)) {
            return -1;
        }

        currentEntry = entry.next_entry;
    }

    if (header.free_entry == DIRECTORY_ENTRY_NONE) {
        return -1;
    }

    allocatedEntry = header.free_entry;
    entryOffset = sizeof(DirectoryBlockHeader) + (allocatedEntry * sizeof(DirectoryEntry));

    zero_bytes(&entry, sizeof(DirectoryEntry));
    copy_bytes(&entry, block + entryOffset, sizeof(DirectoryEntry));

    header.free_entry = entry.next_entry;

    zero_bytes(&entry, sizeof(DirectoryEntry));
    copy_text(entry.name, name, sizeof(entry.name));
    copy_text(entry.extension, extension, sizeof(entry.extension));
    entry.size = 0;
    entry.first_block = firstBlock;
    entry.next_entry = header.first_entry;
    entry.used = 1;

    header.first_entry = allocatedEntry;
    header.entry_count++;

    copy_bytes(block, &header, sizeof(DirectoryBlockHeader));
    copy_bytes(block + entryOffset, &entry, sizeof(DirectoryEntry));

    return write_root_directory_block(disk, block);
}

/**
 * Cria um arquivo vazio no diretorio raiz.
 *
 * A entrada ainda nao recebe bloco de dados. A alocacao real de blocos pertence
 * a etapa de implementacao de arquivos.
 *
 * @param disk arquivo do disco virtual.
 * @param filename nome do arquivo no formato 8.3.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
int create_root_directory_file(FILE *disk, const char *filename)
{
    return add_root_directory_entry(disk, filename, DIRECTORY_ENTRY_NONE);
}

/**
 * Associa um arquivo do diretorio raiz a um bloco de dados inicial.
 *
 * Esta etapa usa alocacao direta de um unico bloco: a entrada do diretorio aponta
 * para o primeiro bloco de dados do arquivo. O bloco e escolhido procurando um
 * bloco de dados ainda nao referenciado por outra entrada ativa.
 *
 * @param disk arquivo do disco virtual.
 * @param filename nome do arquivo no formato 8.3.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
static int allocate_root_directory_file_storage(FILE *disk, const char *filename)
{
    unsigned char block[BLOCK_SIZE];
    DirectoryBlockHeader header;
    DirectoryEntry entry;
    char name[9];
    char extension[4];
    int referencedBlocks[TOTAL_BLOCKS];
    int currentEntry;
    int entryOffset;
    int targetEntryOffset = -1;
    int allocatedBlock = DIRECTORY_ENTRY_NONE;

    if (split_filename_8_3(filename, name, extension) != 0) {
        return -1;
    }

    if (ensure_root_directory_block(disk) != 0) {
        return -1;
    }

    if (read_root_directory_block(disk, block) != 0) {
        return -1;
    }

    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        referencedBlocks[i] = 0;
    }

    copy_bytes(&header, block, sizeof(DirectoryBlockHeader));
    currentEntry = header.first_entry;

    while (currentEntry != DIRECTORY_ENTRY_NONE) {
        entryOffset = sizeof(DirectoryBlockHeader) + (currentEntry * sizeof(DirectoryEntry));

        zero_bytes(&entry, sizeof(DirectoryEntry));
        copy_bytes(&entry, block + entryOffset, sizeof(DirectoryEntry));

        if (entry.used &&
            entry.first_block >= DATA_START_BLOCK &&
            entry.first_block < TOTAL_BLOCKS) {
            referencedBlocks[entry.first_block] = 1;
        }

        if (entry.used &&
            text_equals(entry.name, name) &&
            text_equals(entry.extension, extension)) {
            targetEntryOffset = entryOffset;

            if (entry.first_block != DIRECTORY_ENTRY_NONE) {
                return 0;
            }
        }

        currentEntry = entry.next_entry;
    }

    if (targetEntryOffset == -1) {
        return -1;
    }

    for (int i = DATA_START_BLOCK; i < TOTAL_BLOCKS; i++) {
        if (!referencedBlocks[i]) {
            allocatedBlock = i;
            break;
        }
    }

    if (allocatedBlock == DIRECTORY_ENTRY_NONE) {
        return -1;
    }

    if (write_empty_data_block(disk, allocatedBlock) != 0) {
        return -1;
    }

    zero_bytes(&entry, sizeof(DirectoryEntry));
    copy_bytes(&entry, block + targetEntryOffset, sizeof(DirectoryEntry));
    entry.first_block = allocatedBlock;

    copy_bytes(block + targetEntryOffset, &entry, sizeof(DirectoryEntry));

    return write_root_directory_block(disk, block);
}

/**
 * Escreve bytes no bloco inicial de um arquivo do diretorio raiz.
 *
 * A escrita atual usa apenas um bloco de dados. Se o arquivo ainda nao possui
 * bloco inicial, a alocacao direta de `x_12` e aplicada internamente.
 *
 * @param disk arquivo do disco virtual.
 * @param filename nome do arquivo no formato 8.3.
 * @param data bytes que serao gravados.
 * @param size quantidade de bytes logicos que serao gravados.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
int write_root_directory_file(FILE *disk, const char *filename, const unsigned char *data, int size)
{
    unsigned char directoryBlock[BLOCK_SIZE];
    unsigned char dataBlock[BLOCK_SIZE];
    DirectoryBlockHeader header;
    DirectoryEntry entry;
    char name[9];
    char extension[4];
    int currentEntry;
    int entryOffset;

    if (size < 0 || size > BLOCK_SIZE) {
        return -1;
    }

    if (size > 0 && data == NULL) {
        return -1;
    }

    if (split_filename_8_3(filename, name, extension) != 0) {
        return -1;
    }

    if (allocate_root_directory_file_storage(disk, filename) != 0) {
        return -1;
    }

    if (read_root_directory_block(disk, directoryBlock) != 0) {
        return -1;
    }

    copy_bytes(&header, directoryBlock, sizeof(DirectoryBlockHeader));
    currentEntry = header.first_entry;

    while (currentEntry != DIRECTORY_ENTRY_NONE) {
        entryOffset = sizeof(DirectoryBlockHeader) + (currentEntry * sizeof(DirectoryEntry));

        zero_bytes(&entry, sizeof(DirectoryEntry));
        copy_bytes(&entry, directoryBlock + entryOffset, sizeof(DirectoryEntry));

        if (entry.used &&
            text_equals(entry.name, name) &&
            text_equals(entry.extension, extension)) {
            if (entry.first_block < DATA_START_BLOCK || entry.first_block >= TOTAL_BLOCKS) {
                return -1;
            }

            zero_bytes(dataBlock, BLOCK_SIZE);

            if (size > 0) {
                copy_bytes(dataBlock, data, (size_t)size);
            }

            if (fseek(disk, entry.first_block * BLOCK_SIZE, SEEK_SET) != 0) {
                perror("Erro ao posicionar no bloco inicial do arquivo");
                return -1;
            }

            if (fwrite(dataBlock, sizeof(unsigned char), BLOCK_SIZE, disk) != BLOCK_SIZE) {
                perror("Erro ao escrever conteudo do arquivo");
                return -1;
            }

            entry.size = size;
            copy_bytes(directoryBlock + entryOffset, &entry, sizeof(DirectoryEntry));

            return write_root_directory_block(disk, directoryBlock);
        }

        currentEntry = entry.next_entry;
    }

    return -1;
}

/**
 * Escreve labels textuais em todos os blocos para depuracao visual.
 *
 * Esta funcao e destrutiva e nao deve ser chamada durante montagem normal.
 *
 * @param disk arquivo do disco virtual.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
int debug_write_block_labels(FILE *disk)
{
    unsigned char block[BLOCK_SIZE];
    char label[32];

    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        zero_bytes(block, BLOCK_SIZE);
        zero_bytes(label, sizeof(label));

        snprintf(label, sizeof(label), "BLOCK %02d", i);

        copy_bytes(block, label, text_length(label));

        long offset = i * BLOCK_SIZE;

        if (fseek(disk, offset, SEEK_SET) != 0) {
            perror("Erro ao posicionar no início do bloco");
            return -1;
        }

        size_t written = fwrite(block, sizeof(unsigned char), BLOCK_SIZE, disk);

        if (written != BLOCK_SIZE) {
            perror("Erro ao escrever cabeçalho do bloco");
            return -1;
        }
    }

    if (fseek(disk, 0, SEEK_SET) != 0) {
        perror("Erro ao retornar ponteiro para o início do disco");
        return -1;
    }

    return 0;
}

/**
 * Escreve o superbloco no bloco 0 do disco virtual.
 *
 * @param disk arquivo do disco virtual.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
int write_root_block(FILE *disk)
{
    unsigned char block[BLOCK_SIZE];

    zero_bytes(block, BLOCK_SIZE);

    RootBlock rootBlock = create_root_block();

    copy_bytes(block, &rootBlock, sizeof(RootBlock));

    if (fseek(disk, 0, SEEK_SET) != 0) {
        perror("Erro ao posicionar no bloco 0");
        return -1;
    }

    size_t written = fwrite(block, sizeof(unsigned char), BLOCK_SIZE, disk);

    if (written != BLOCK_SIZE) {
        perror("Erro ao escrever superbloco");
        return -1;
    }

    return 0;
}

/**
 * Escreve marcadores iniciais nas regioes estruturais do layout.
 *
 * O bloco FREE_MAP_BLOCK permanece apenas reservado; metadados e diretorio raiz recebem marcadores.
 *
 * @param disk arquivo do disco virtual.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
int write_layout_markers(FILE *disk)
{
    unsigned char block[BLOCK_SIZE];

    zero_bytes(block, BLOCK_SIZE);
    copy_text((char *)block, "FREE_MAP", BLOCK_SIZE);

    if (fseek(disk, FREE_MAP_BLOCK * BLOCK_SIZE, SEEK_SET) != 0) {
        perror("Erro ao posicionar no bloco reservado para espaco livre");
        return -1;
    }

    if (fwrite(block, sizeof(unsigned char), BLOCK_SIZE, disk) != BLOCK_SIZE) {
        perror("Erro ao escrever marcador de espaco livre");
        return -1;
    }

    zero_bytes(block, BLOCK_SIZE);
    copy_text((char *)block, "METADATA", BLOCK_SIZE);

    if (fseek(disk, METADATA_START_BLOCK * BLOCK_SIZE, SEEK_SET) != 0) {
        perror("Erro ao posicionar na regiao de metadados");
        return -1;
    }

    if (fwrite(block, sizeof(unsigned char), BLOCK_SIZE, disk) != BLOCK_SIZE) {
        perror("Erro ao escrever regiao de metadados");
        return -1;
    }

    create_empty_root_directory_block(block);

    if (write_root_directory_block(disk, block) != 0) {
        return -1;
    }

    return 0;
}

/**
 * Garante que o bloco FREE_MAP_BLOCK esteja apenas como marcador reservado.
 *
 * Esta etapa ainda nao implementa mapa livre, lista livre ou FAT.
 *
 * @param disk arquivo do disco virtual.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
int ensure_free_map_placeholder(FILE *disk)
{
    unsigned char block[BLOCK_SIZE];

    zero_bytes(block, BLOCK_SIZE);

    if (fseek(disk, FREE_MAP_BLOCK * BLOCK_SIZE, SEEK_SET) != 0) {
        perror("Erro ao posicionar no bloco reservado para espaco livre");
        return -1;
    }

    if (fread(block, sizeof(unsigned char), BLOCK_SIZE, disk) != BLOCK_SIZE) {
        if (ferror(disk)) {
            perror("Erro ao ler bloco reservado para espaco livre");
            return -1;
        }

        clearerr(disk);
    }

    if (text_equals((char *)block, "FREE_MAP")) {
        return 0;
    }

    zero_bytes(block, BLOCK_SIZE);
    copy_text((char *)block, "FREE_MAP", BLOCK_SIZE);

    if (fseek(disk, FREE_MAP_BLOCK * BLOCK_SIZE, SEEK_SET) != 0) {
        perror("Erro ao reposicionar no bloco reservado para espaco livre");
        return -1;
    }

    if (fwrite(block, sizeof(unsigned char), BLOCK_SIZE, disk) != BLOCK_SIZE) {
        perror("Erro ao restaurar marcador de espaco livre");
        return -1;
    }

    return 0;
}

/**
 * Formata o disco virtual com o layout estrutural inicial.
 *
 * A formatacao zera todos os blocos, escreve marcadores de layout e grava o superbloco.
 *
 * @param disk arquivo do disco virtual.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
int format_virtual_disk(FILE *disk)
{
    if (initialize_virtual_disk(disk) != 0) {
        return -1;
    }

    if (write_layout_markers(disk) != 0) {
        return -1;
    }

    if (write_root_block(disk) != 0) {
        return -1;
    }

    if (fflush(disk) != 0) {
        perror("Erro ao sincronizar disco virtual");
        return -1;
    }

    return 0;
}

/**
 * Monta o disco virtual se o superbloco for valido ou formata caso esteja vazio/invalido.
 *
 * @param disk arquivo do disco virtual.
 * @return 0 em caso de sucesso; -1 em caso de erro.
 */
int mount_or_format_virtual_disk(FILE *disk)
{
    RootBlock rootBlock;
    int read_status = read_root_block(disk, &rootBlock);

    if (read_status < 0) {
        return -1;
    }

    if (read_status == 0 && is_valid_root_block(&rootBlock)) {
        if (ensure_free_map_placeholder(disk) != 0) {
            return -1;
        }

        if (ensure_root_directory_block(disk) != 0) {
            return -1;
        }

        printf("Disco virtual montado sem formatar.\n");
        return 0;
    }

    printf("Disco virtual invalido ou vazio. Formatando layout inicial.\n");

    return format_virtual_disk(disk);
}
