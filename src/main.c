#include "blocos_virtuais.h"
#include "controle_arquivo_id.h"

int main()
{
	const char *caminho_arquivo_disco = "./src/resources/storage.bin";
	
	int arquivo_id = AbreArquivo(caminho_arquivo_disco);
	long arquivo_deslocamento = ReposicionaPonteiroArquivo(arquivo_id, 0);

	BlocosVirtuais *lista_blocos = IniciaBlocosVirtuais(arquivo_id);
	
	LimpaBlocos(lista_blocos);

	close(arquivo_id);
	return 0;	
}
