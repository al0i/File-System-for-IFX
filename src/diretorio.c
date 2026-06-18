#include "diretorio.h"

void CopiarCaracteresLimitado(unsigned char *destino, const char *origem, int max_tamanho)
{
    int i = 0;
    for (; origem[i] != '\0' && i < max_tamanho; i++) destino[i] = (unsigned char)origem[i];
    for (; i < max_tamanho; i++) destino[i] = '\0';
}

void InicializaDiretorioRoot(EntradaDiretorio *root)
{
    for (int i = 0; i < MAX_ENTRADAS_ROOT; i++)
    {
        root[i].status = 0x00;
        root[i].tamanho_arquivo = 0;
        root[i].bloco_inicio = 0;

        for (int j = 0; j < TAMANHO_NOME_ARQUIVO; j++) root[i].nome_arquivo[j] = '\0';
        for (int k = 0; k < TAMANHO_EXTENSAO_ARQUIVO; k++) root[i].extensao_arquivo[k] = '\0';
    }
    
}

int CriaEntradaArquivo(
    EntradaDiretorio *root, 
    char *nome_arquivo, 
    char *extensao_arquivo,
    int bloco_inicio
)
{
    for (int i = 0; i < MAX_ENTRADAS_ROOT; i++)
    {
        if (root[i].status == 0x00)
        {
            root[i].status = 0x01;
            CopiarCaracteresLimitado(root[i].nome_arquivo, nome_arquivo, TAMANHO_NOME_ARQUIVO);
            CopiarCaracteresLimitado(root[i].extensao_arquivo, extensao_arquivo, TAMANHO_EXTENSAO_ARQUIVO);
            root[i].tamanho_arquivo = 0;
            root[i].bloco_inicio = bloco_inicio;
            
            return i;
        }
    }
    
    printf("Erro: Diretório /root está cheio!\n");

    return -1;
}

void DIR(EntradaDiretorio *root)
{
    printf("\nDiretório /root:\n\n");

    for (int i = 0; i < MAX_ENTRADAS_ROOT; i++)
    {
        if (root[i].status == 0x01)
        {
            printf("SLOT [%02d]\tArquivo: ", i);

            for (int j = 0; j < TAMANHO_NOME_ARQUIVO && root[i].nome_arquivo[j] != '\0'; j++)
            {
                printf("%c", root[i].nome_arquivo[j]);
            }
            
            printf(".");

            for (int k = 0; k < TAMANHO_EXTENSAO_ARQUIVO && root[i].extensao_arquivo[k] != '\0'; k++)
            {
                printf("%c", root[i].extensao_arquivo[k]);
            }

            printf("\tInicio: Bloco %u\tTamanho: %u bytes\n", root[i].bloco_inicio, root[i].tamanho_arquivo);            
        }
    }
}

void GravarDiretorioNoBloco(unsigned char *data, EntradaDiretorio *root)
{
    int deslocamento_nome_diretorio = 12; 

    for (int i = 0; i < MAX_ENTRADAS_ROOT; i++)
    {
        for (int j = 0; j < TAMANHO_NOME_ARQUIVO; j++)
            data[deslocamento_nome_diretorio++] = (unsigned char)root[i].nome_arquivo[j];

        for (int j = 0; j < TAMANHO_EXTENSAO_ARQUIVO; j++)
            data[deslocamento_nome_diretorio++] = (unsigned char)root[i].extensao_arquivo[j];

        data[deslocamento_nome_diretorio++] = (unsigned char)root[i].status;

        data[deslocamento_nome_diretorio++] = (unsigned char)(root[i].tamanho_arquivo & 0xFF);
        data[deslocamento_nome_diretorio++] = (unsigned char)((root[i].tamanho_arquivo >> 8) & 0xFF);
        data[deslocamento_nome_diretorio++] = (unsigned char)((root[i].tamanho_arquivo >> 16) & 0xFF);
        data[deslocamento_nome_diretorio++] = (unsigned char)((root[i].tamanho_arquivo >> 24) & 0xFF);

        data[deslocamento_nome_diretorio++] = (unsigned char)(root[i].bloco_inicio & 0xFF);
        data[deslocamento_nome_diretorio++] = (unsigned char)((root[i].bloco_inicio >> 8) & 0xFF);
        data[deslocamento_nome_diretorio++] = (unsigned char)((root[i].bloco_inicio >> 16) & 0xFF);
        data[deslocamento_nome_diretorio++] = (unsigned char)((root[i].bloco_inicio >> 24) & 0xFF);
    }
}