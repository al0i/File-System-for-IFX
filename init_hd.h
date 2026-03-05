#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * Inicializa o HD virtual.
 * Retorna o file descriptor (int > 0) em caso de sucesso, ou -1 em caso de erro.
 */
int init_hd(const char *filename, long size_in_bytes) {
    // Abre o arquivo: Leitura/Escrita (O_RDWR) e Criação (O_CREAT)
    int fd = open(filename, O_RDWR | O_CREAT, 0644);

    if (fd == -1) {
        perror("Erro ao abrir ou criar o arquivo do HD");
        return -1; 
    }

    // Define o tamanho físico do HD no disco real
    if (ftruncate(fd, size_in_bytes) == -1) {
        perror("Erro ao truncar/definir o tamanho do HD");
        close(fd);
        return -1;
    }

    return fd;
}