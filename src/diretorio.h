#ifndef DIRETORIO_H
#define DIRETORIO_H

#include <stdlib.h>
#include <unistd.h>

// Somente para fins de DEBUG, iremos importar neste primeiro momento: stdio.h
#include <stdio.h>

#define TAMANHO_NOME_ARQUIVO 8
#define TAMANHO_EXTENSAO_ARQUIVO 3

/**
 * DIRETÓRIO/PASTA
 * Total de bytes esperados: 8: nome do arquivo + 3: extensão do arquivo + 1: status + 4: tamanho do arquivo + 4: ponteiro para o bloco de dados = 20 bytes
*/
#define MAX_ENTRADAS_ROOT 25

typedef struct EntradaDiretorio
{
    char nome_arquivo[TAMANHO_NOME_ARQUIVO];
    char extensao_arquivo[TAMANHO_EXTENSAO_ARQUIVO];
    unsigned int status; // 0 = livre, 1 = ocupado
    unsigned int tamanho_arquivo; // Tamanho do arquivo em bytes
    unsigned int bloco_inicio; // Ponteiro para o bloco de dados do
} EntradaDiretorio;

void InicializaDiretorioRoot(EntradaDiretorio *root);

int CriaEntradaArquivo(
    EntradaDiretorio *root, 
    char *nome_arquivo, 
    char *extensao_arquivo,
    int bloco_inicio
);

void DIR(EntradaDiretorio *root);

void GravarDiretorioNoBloco(unsigned char *data, EntradaDiretorio *root);

#endif