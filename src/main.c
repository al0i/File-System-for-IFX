#include "blocos_virtuais.h"
#include "controle_arquivo_id.h"
#include "diretorio.h"

int main()
{
	const char *caminho_arquivo_disco = "./src/resources/storage.bin";
	
	int arquivo_id = AbreArquivo(caminho_arquivo_disco);
	if (ReposicionaPonteiroArquivo(arquivo_id, 0))
	{
		printf("Erro: Não foi possível reposicionar o ponteiro do arquivo.\n");
		return 1;
	}

	BlocosVirtuais *lista_blocos = IniciaBlocosVirtuais(arquivo_id);

	EntradaDiretorio diretorio_root[MAX_ENTRADAS_ROOT];
	InicializaDiretorioRoot(diretorio_root);

	CriaEntradaArquivo(diretorio_root, "README", "md", 3);
	CriaEntradaArquivo(diretorio_root, "texto", "txt", 4);

	DIR(diretorio_root);

	BlocoAlocado *atual = lista_blocos->inicio;
    while (atual != NULL)
    {
        if (atual->id == 2)
        {
            GravarDiretorioNoBloco(atual->data, diretorio_root);
            
            lseek(arquivo_id, 2 * TAMANHO_BLOCO, SEEK_SET);
            
            GravaListaBlocos(arquivo_id, atual);
            break;
        }
        atual = atual->proximo;
    }
	
	LimpaBlocos(lista_blocos);

	close(arquivo_id);
	return 0;	
}
