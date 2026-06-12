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

BlocosVirtuais *IniciaBlocosVirtuais()
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

    /**
     * DEBUG: Exibição dos blocos alocados
     */
	// BlocoAlocado *atual = lista_blocos->inicio;
	// while (atual != NULL)
	// {
	// 	printf("Bloco carregado [id]:%u", atual->id);

	// 	switch (atual->id)
	// 	{
	// 		case 0:
	// 			printf(">>> Reservado a MBR <<<\n");
	// 			break;
	// 		case 1:
	// 			printf(">>> [Reservado para Tabela FAT] <<<\n");
	// 			break;
	// 		case 2:
	// 			printf(">>> [Reservado para Root Directory] <<<\n");
	// 			break;
	// 		case 3:
	// 			printf(">>> [Area de Dados Gerais] <<<\n");
	// 			break;
	// 		default:
	// 			break;
	// 	}

	// 	atual = atual->proximo;
	// }

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