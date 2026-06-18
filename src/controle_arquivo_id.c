#include "controle_arquivo_id.h"

int AbreArquivo(const char *caminho_arquivo_disco)
{
    int arquivo_id; // FileDescriptor::IdentificadorArquivo
	// printf("Caminho do disco: %s", caminho_arquivo_disco);

	arquivo_id = open(
		caminho_arquivo_disco,
		O_RDWR | O_CREAT, // O_RDWR = 2/LEITURA&ESCRITA | O_CREAT = 64/CRIA ARQUIVO CASO NÃO EXISTA
		0666			  // PERMISSÕES DE LEITURA E ESCRITA PARA TODOS > OCTAL:USUARIO:GRUPO_USUARIO:OUTROS > 4/r+2/w=6
	);

	if (arquivo_id < 0)
	{
		printf("Erro: Não foi possível abrir ou criar o arquivo %s.\n", caminho_arquivo_disco);
		close(arquivo_id);
		return 1;
	}

	// printf("Id do arquivo: %i\nO_RDWR: %i\nO_CREAT: %i", arquivo_id, O_RDWR, O_CREAT);

    return arquivo_id;
}

int ReposicionaPonteiroArquivo(int arquivo_id, long deslocamento)
{
	if ((long)lseek(arquivo_id, 0, SEEK_SET) < 0)
	{
		printf("Erro: Não foi possível resposicionar o ponteiro do arquivo.\n");
		close(arquivo_id);
		return 1;
	}
    // printf("[DEBUG]: Arquivo aberto e reposicionado no byte: %ld.\n", arquivo_deslocamento);

    return 0;
}