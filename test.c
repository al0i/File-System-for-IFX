#include "index.c"
#include <stdio.h>

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
        return 1; //Error!
    }

    if (read_block(10, read_buffer) != 0){
        printf("Error: read_error.\n");
        return 1;
    }

    if(read_buffer[0] != 'i'){
        printf("Error: integrity error.\n");
        printf("Disco: %s\n", virtual_disk);
        return 1; //Error!
    }

    

    close_disk();

    return 0;
}