#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// Apenas para fins de DEBUG, iremos importar neste primeiro momento: stdio.h
#include <stdio.h>

int AbreArquivo(const char *caminho_arquivo_disco);

long ReposicionaPonteiroArquivo(int arquivo_id, long deslocamento);