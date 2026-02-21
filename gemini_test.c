#include "index.c"
#include <stdio.h>

// Função auxiliar para visualizar o conteúdo de um bloco
void print_block_dump(const char *buffer, int size_to_print) {
    printf("\n--- Hex Dump do Bloco ---\n");
    for (int i = 0; i < size_to_print; i++) {
        // Imprime o valor em hexadecimal e o caractere (se for imprimível)
        printf("[%03d] Hex: %02X | Char: %c\n", i, (unsigned char)buffer[i], 
               (buffer[i] >= 32 && buffer[i] <= 126) ? buffer[i] : '.');
    }
    printf("-------------------------\n\n");
}

int main(){
    printf("Iniciando...\n");

    char write_buffer[BLOCK_SIZE];
    char read_buffer[BLOCK_SIZE];

    if(init_disk() != 0){
        printf("Error: init_disk().\n");
        return 1;
    }
    
    write_buffer[0] = 'i';
    write_buffer[1] = 'f';
    write_buffer[2] = 'x';
    for (int i = 3; i<BLOCK_SIZE; i++){
        write_buffer[i] = 0;
    }

    if(write_block(10, write_buffer) != 0){
        printf("Error: write_error.\n");
        return 1;
    }

    if (read_block(10, read_buffer) != 0){
        printf("Error: read_error.\n");
        return 1;
    }

    // Visualizando os primeiros 10 bytes do que foi lido do disco
    printf("Conteudo lido do bloco 10:\n");
    print_block_dump(read_buffer, 10);

    if(read_buffer[0] != 'i'){
        printf("Error: integrity error.\n");
        return 1;
    } else {
        printf("Sucesso! Integridade validada.\n");
    }

    close_disk();

    return 0;
}