#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define TOTAL_BLOCKS 16

const char *disk_path = "./src/resources/virtual_disk.bin";

typedef struct {
    char signature[8];      // identificação do sistema
    int block_size;         // 512
    int total_blocks;       // 16
    int data_start_block;   // primeiro bloco de dados
} RootBlock;

/**
 * Faz o buffer do arquivo .bin com verificação de exitência 
 * e caso no final, ainda dê erro, retornamos uma mensagem de erro.
 * @author imheldermartins
 */
FILE* init_disk()
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
 * Inicializa e aloca o arquivo para simulação do sistema de arquivos
 * @param disk variavel com ponteiro para manipular o arquivo root 
 * @author imheldermartins
 */
int initialize_virtual_disk(FILE *disk)
{
    unsigned char block[BLOCK_SIZE];

    memset(block, 0, BLOCK_SIZE);

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
 * 1. Que o arquivo foi dividido logicamente em blocos de 512 bytes.\n
 * 2. Que conseguimos posicionar o cursor exatamente no início de cada bloco.\n
 * 3. Que o hexdump mostra as escritas nos offsets corretos: 0x0000, 0x0200, 0x0400, 0x0600...\n
 * 
 * @author: imheldermartins
 */
int debug_write_block_labels(FILE *disk)
{
    unsigned char block[BLOCK_SIZE];
    char label[32];

    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        memset(block, 0, BLOCK_SIZE);
        memset(label, 0, sizeof(label));

        snprintf(label, sizeof(label), "BLOCK %02d", i);

        memcpy(block, label, strlen(label));

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

int write_root_block(FILE *disk)
{
    unsigned char block[BLOCK_SIZE];

    memset(block, 0, BLOCK_SIZE);

    RootBlock rootBlock;

    memset(&rootBlock, 0, sizeof(RootBlock));

    strcpy(rootBlock.signature, "IFXFS");
    rootBlock.block_size = BLOCK_SIZE;
    rootBlock.total_blocks = TOTAL_BLOCKS;
    rootBlock.data_start_block = 1;

    memcpy(block, &rootBlock, sizeof(RootBlock));

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