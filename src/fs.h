#include "disk.h"
#define FAT_BLOCKS (NUM_BLOCKS * sizeof(int)) / BLOCK_SIZE

int root_folder_block = -1;

struct DirEntry {
    char nome[8];
    char ext[3];
    int type; //0=FREE - 1=ARQUIVO - 2=DIRETÓRIO
    int first_block;
    //int size;
    //int author_id;
    //int permissoes;
    //int isOculto;
};

int init_fat(); // Inicia a File Allocation Table, que controle os blocos como livres, ocupados e as continuações dos arquivos
int next_free_block(void); // Retorna o próximo bloco livre
int set_block(int block, int value_set); // Define o valor de um bloco na FAT
int create_root_folder(void); // Por enquanto, cria apenas o root folder
int new_dir_entry(char nome[8], char ext[3], int type, int first_block); // Cria uma entrada de diretório e salva no bloco do diretório

int read_file(char filename[8], char ext[3], void *file_data);
int write_file(char filename[8], char ext[3], void *data);
struct DirEntry get_dir_entry(char filename[8], char ext[3]);


int read_archive(int fd, void *buffer, size_t size); // Irá ler um arquivo guardado no computador, apenas para testes