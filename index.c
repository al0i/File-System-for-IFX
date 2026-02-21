#include <stdlib.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024

char *virtual_disk = NULL;

int init_disk(){
    if (virtual_disk == NULL){
        virtual_disk = (char *)calloc(NUM_BLOCKS, BLOCK_SIZE);

        if (virtual_disk == NULL){
            return -1; //Error!
        }
    }
    return 0; //Success!
}

int read_block(int block_num, char *buffer) {
    if (block_num < 0 || block_num >= NUM_BLOCKS || virtual_disk == NULL || buffer == NULL){
        return -1;
    }

    int offset = block_num * BLOCK_SIZE;
    
    for (int i=0; i<BLOCK_SIZE; i++){
        buffer[i] = virtual_disk[offset + i];
    }

    return 0; //Success!
}

int write_block(int block_num, const char *buffer){
    if (block_num < 0 || block_num >= NUM_BLOCKS || virtual_disk == NULL || buffer == NULL){
        return -1;
    }

    int offset = block_num * BLOCK_SIZE;

    for (int i=0;i<BLOCK_SIZE;i++){
        virtual_disk[offset+i] = buffer[i];
    }

    return 0; //Success!
}

void close_disk(){
    if (virtual_disk != NULL) {
        free(virtual_disk);
        virtual_disk = NULL;
    }
}