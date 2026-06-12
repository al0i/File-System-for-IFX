#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// Apenas para fins de DEBUG, iremos importar neste primeiro momento: stdio.h
#include <stdio.h>

#define TAMANHO_BLOCO 512

typedef struct BlocoAlocado {
	unsigned int id;
	unsigned char data[TAMANHO_BLOCO];
	struct BlocoAlocado *anterior;
	struct BlocoAlocado *proximo;
} BlocoAlocado;

typedef struct BlocosVirtuais {
	BlocoAlocado *inicio;
	BlocoAlocado *fim;
	unsigned int size;
} BlocosVirtuais;

BlocoAlocado* CriaBloco(unsigned int id);

int InserirBloco(BlocoAlocado **inicio, BlocoAlocado **fim, unsigned int id);

void EscreverAssinaturaBloco(unsigned char *assinatura_grupo, const char* titulo_secao);

void EscreverListaBlocos(BlocoAlocado *atual, int arquivo_id);

int GravaListaBlocos(int arquivo_id, BlocoAlocado *atual);

BlocosVirtuais* IniciaBlocosVirtuais(int arquivo_id);

void LimpaBlocos(BlocosVirtuais *lista_blocos);