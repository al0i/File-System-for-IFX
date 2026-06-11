#include "disk.h"
#include <unistd.h> // read, write, lseek, close
#include <fcntl.h> // Definições de open()
#include <stdio.h> // perror
#include <string.h> // memset

static int disk = -1;

// Cria o arquivo .bin, como se fosse nosso disco
int create_disk(){
    int disk = creat("disk.bin", 0666);
    if (disk < 0) {
        perror("Erro ao criar o disco.");
        return -1;
    }
    close(disk);
    return 0;
}

// Coloca 0 em todos os bytes do arquivo .bin, como se o disco estivesse vazio
int init_disk(){
    int disk = open("disk.bin", O_WRONLY);
    if (disk < 0){
        perror("Erro ao acessar o disco!\n");
        return -1;
    }

    char buffer[BLOCK_SIZE];
    memset(buffer, 0, BLOCK_SIZE);

    for (int i=0; i<NUM_BLOCKS; i++){
        if (write(disk, buffer, BLOCK_SIZE) != BLOCK_SIZE){
            perror("Erro durante a escrita no disco");
            close(disk);
            return -1;
        }
    }
    close(disk);
    return 0;
}

// Não entendi exatamente a finalidade dessa função:
// Define o disco como aberto
int open_disk(){
    disk = open("disk.bin", O_RDWR);
    if (disk < 0){
        perror("Erro ao abrir o disco!\n");
        return -1;
    }
    return 0;
}

// Não entendi exatamente a finalidade dessa função:
// Define o disco como fechado
int close_disk(){
    if (disk < 0){
        return -1;
    }

    if (close(disk) < 0){
        perror("Erro ao fechar o disco!\n");
        return -1;
    }

    disk = -1;
    return 0;
}

// Escreva no bloco k
int write_block(int block_number, const void *buffer) {
    //Se o disco estiver fechado, erro!
    if (disk < 0) {
        perror("Disco fechado!");
        return -1;
    }

    // Se o número do bloco foi inválido, erro!
    if (block_number < 0 || block_number >= NUM_BLOCKS) {
        fprintf(stderr, "Bloco inválido!\n");
        return -1;
    }

    // Define o deslocamento a partir do começo do disco para saber onde escreve
    off_t offset = (off_t) block_number * (off_t) BLOCK_SIZE;

    // Se não conseguir definir o offset, erro!
    if (lseek(disk, offset, SEEK_SET) < 0){
        perror("Erro ao definir ponteiro no bloco!");
        return -1;
    }

    // Se a quantidade de bytes escritos for diferente do BLOCK_SIZE, erro!
    ssize_t bytes_escritos = write(disk, buffer, BLOCK_SIZE);
    if (bytes_escritos != BLOCK_SIZE) {
        perror("Erro ao escrever no bloco!");
        return -1;
    }

    return 0;
}

// Leia o bloco k
int read_block(int block_number, void *buffer){
    // Se o disco estiver fechado, erro!
    if (disk < 0){
        perror("Disco fechado!");
        return -1;
    }
    
    // Se o número do bloco foi inválido, erro!
    if (block_number < 0 || block_number >= NUM_BLOCKS) {
        fprintf(stderr, "Bloco inválido!\n");
    }

    // Define o deslocamento a partir do começo do disco para saber onde ler
    off_t offset = (off_t) block_number * BLOCK_SIZE;

    // Se o lseek não definir o ponteiro corretamente, erro!
    if (lseek(disk, offset, SEEK_SET) < 0) {
        perror("Erro ao definir ponteiro no bloco!");
        return -1;
    }

    // Se a quantidade de blocos lidos for diferente do tamanho do bloco, erro!
    ssize_t bytes_lidos = read(disk, buffer, BLOCK_SIZE);
    if (bytes_lidos != BLOCK_SIZE ) {
        perror("Erro ao ler o bloco!");
        return -1;
    }
    
    return 0;
}