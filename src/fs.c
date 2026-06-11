#include "fs.h"

int init_fat(){
    int oc_blocks = 1; //O valor começa em 1 pois o bloco 0 é definido como ocupado por padrão
    oc_blocks = oc_blocks + FAT_BLOCKS; //Define quantos blocos terá nossa FAT e deixa os blocos 0 + FAT ocupados
    //ATENÇÃO! A FÓRMULA ACIMA IRÁ NOS TRAZER PROBLEMAS SE O NUM_BLOCKS * SIZEOF(INT) NÃO FOR MÚLTIPLO DO BLOCK_SIZE

    unsigned char fat[BLOCK_SIZE]; // Cria um buffer referente ao bloco da FAT
    memset(fat, 0, BLOCK_SIZE); // Zera o buffer, para não ter lixo no meio
    
    for (int i=0;i<oc_blocks;i++){
        fat[i] = -1; // Define os oc_blocks primeiros blocos como ocupados (-1)
    }

    open_disk(); // Abre o disco
    write_block(1,fat); // Escreve no bloco 1 o nosso buffer da FAT
    close_disk(); // Fecha o disco

    return 0; // Success!
}

int next_free_block(void){ // Retorna o próximo bloco livre
    open_disk();
    char buffer[BLOCK_SIZE];
    int free_block = 0; // Contador de blocos livres

    for (int i=1;i<FAT_BLOCKS;i++){
        read_block(i, buffer);
        
        int j = 0;
        while (j < BLOCK_SIZE/sizeof(int)){
            if (buffer[j] == 0){
                close_disk();
                return free_block;
            }
            free_block++;
            j++;
        }
    }
    
    close_disk();
    return -1;
}

int set_block(int block, int value_set){
    int fat_block = block / (BLOCK_SIZE/sizeof(int));

    if (block % (BLOCK_SIZE/sizeof(int)) != 0){ //Caso não esteja na primeira página da FAT
        fat_block++;
    } //NÃO TESTADO

    while(block / (BLOCK_SIZE/sizeof(int)) > 1){ //Caso não esteja na primeira página da FAT
        block = block/(BLOCK_SIZE/sizeof(int));
    } //NÃO TESTADO

    char buffer[BLOCK_SIZE];
    memset(buffer, 0, BLOCK_SIZE);

    open_disk();
    read_block(fat_block, buffer);
    buffer[block] = value_set;
    write_block(fat_block, buffer);
    close_disk();

    return 0;
}

int create_folder(){
    root_folder_block = next_free_block();
    set_block(root_folder_block, -1);
}

int new_dir_entry(char nome[8], char ext[3], int type, int first_block){
    if (first_block == -1){
        first_block = next_free_block();
    }

    struct DirEntry arquivo;
    memcpy(arquivo.nome, nome, 8);
    memcpy(arquivo.ext, ext, 3);
    arquivo.type = type;
    arquivo.first_block = first_block;
    
    set_block(first_block, -1);
    
    char buffer[BLOCK_SIZE];
    struct DirEntry *entries = (struct DirEntry *) buffer;

    open_disk();

    read_block(root_folder_block, buffer);
    for (int i=0;i<BLOCK_SIZE/sizeof(struct DirEntry);i++){
        if (entries[i].type == 0) {
            entries[i] = arquivo;
            write_block(root_folder_block, buffer);
            close_disk();
            return 0;
        }
    }

    close_disk();
    return -1; // Provavalmente todas as entradas estão ocupadas
}

/*int read_archive(int fd, void buffer, size_t size){
    read(fd, buffer, size);
    buffer[size] = '\0';
    return 0;
}*/