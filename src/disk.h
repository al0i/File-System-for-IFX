#define BLOCK_SIZE 512 //Tamanho do bloco = 512 bytes
#define NUM_BLOCKS (1*1024*1024)/512 // 1MB = 2048 BLOCKS

int read_block(int block_number, void *buffer); //Leia o bloco k; e
int write_block(int block_number, const void *buffer); //Escreva no bloco k.

// As funções abaixo não são funções primordiais de um disco.
// São necessárias por estarmos simulando um disco em um arquivo .bin.

int create_disk(); // Cria o arquivo .bin do disco, com o nome "disk.bin"
int init_disk(); // Inicializa o disco, colocando 0 em todo seu conteúdo.

int disk_open(); // Abre o arquivo .bin que simula o disco.
int disk_close(); // Fecha o arquivo .bin que simula o disco.