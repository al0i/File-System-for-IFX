#include "blocos_virtuais.h"

BlocoAlocado* CriaBloco(unsigned int id)
{
	BlocoAlocado *novo_bloco = (BlocoAlocado*) malloc(sizeof(BlocoAlocado));
	if (novo_bloco == NULL)
	{
		printf("Erro: Falha na alocação de memória para o bloco %u.\n", id);
		exit(1);
	}

	novo_bloco->id = id;
	novo_bloco->anterior = NULL;
	novo_bloco->proximo = NULL;

	for (int i = 0; i < TAMANHO_BLOCO; i++)
	{
		novo_bloco->data[i] = 0;
	}

	return novo_bloco;
}

int InserirBloco(
	BlocoAlocado **inicio,
	BlocoAlocado **fim,
	unsigned int id
)
{
	BlocoAlocado *novo_bloco = CriaBloco(id);

	if (*inicio == NULL)
	{
		*inicio = novo_bloco;
		*fim = novo_bloco;
	}
	else
	{
		(*fim)->proximo = novo_bloco;
		novo_bloco->anterior = *fim;
		*fim = novo_bloco;
	}
}

void EscreverAssinaturaBloco(unsigned char *assinatura_grupo, const char* titulo_secao)
{
    int i = 0;

    while (titulo_secao[i] != '\0' && i < TAMANHO_BLOCO)
    {
        assinatura_grupo[i] = (unsigned char) titulo_secao[i];
        i++;
    }
    
}

void EscreverListaBlocos(BlocoAlocado *atual, int arquivo_id)
{
    while (atual != NULL)
	{
		// printf("Bloco carregado [id]:%u", atual->id);

		switch (atual->id)
		{
			case 0:
				// printf(">>> Reservado a MBR <<<\n");
                EscreverAssinaturaBloco(atual->data, "mbr");
				break;
			case 1:
				// printf(">>> [Reservado para Tabela FAT] <<<\n");
                EscreverAssinaturaBloco(atual->data, "fat");
				break;
			case 2:
				// printf(">>> [Reservado para Root Directory] <<<\n");
                EscreverAssinaturaBloco(atual->data, "root");
				break;
			case 3:
				// printf(">>> [Area de Dados Gerais] <<<\n");
                EscreverAssinaturaBloco(atual->data, "free_blocks");
				break;
			default:
				break;
		}

        if (GravaListaBlocos(arquivo_id, atual) < 0)
		{
			printf("Erro: Falha ao gravar o bloco %u no arquivo.\n", atual->id);
			exit(1);
		}
		atual = atual->proximo;
	}
}

int GravaListaBlocos(int arquivo_id, BlocoAlocado *atual)
{
    if (atual != NULL && write(arquivo_id, atual->data, TAMANHO_BLOCO) != TAMANHO_BLOCO)
    {
        printf("Erro: Falha ao gravar bloco %u.\n", atual->id);
        return -1;
    }

    return 0;
}

BlocosVirtuais *IniciaBlocosVirtuais(int arquivo_id)
{
    BlocosVirtuais *lista_blocos = (BlocosVirtuais*) malloc(sizeof(BlocosVirtuais));
    if (lista_blocos == NULL)
    {
        printf("Erro: Falha na alocação de memória para BlocosVirtuais.\n");
        exit(1);
    }

	lista_blocos->inicio = NULL;
	lista_blocos->fim = NULL;
	lista_blocos->size = 0;

	for (unsigned i = 0; i < 4; i++)
	{
		InserirBloco(
			&lista_blocos->inicio,
			&lista_blocos->fim,
			i
		);
	}

	EscreverListaBlocos(lista_blocos->inicio, arquivo_id);

    return lista_blocos;
}

void LimpaBlocos(BlocosVirtuais *lista_blocos)
{
    BlocoAlocado *atual = lista_blocos->inicio;
    while (atual != NULL)
    {
        BlocoAlocado *temporario = atual;
        atual = atual->proximo;
        free(temporario);
    }
    free(lista_blocos);
}