#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>   
#include <unistd.h>  

int main() {
    int fd;
    char buffer[256];
    const char *caminho = "./meu_disco_raw.bin";

    // --------------------------------------------------------
    // 1. Abrindo para Leitura e Escrita
    // --------------------------------------------------------
    // O_RDWR: Leitura e Escrita.
    // O_CREAT: Cria se não existir.
    // O_APPEND: As escritas sempre vão para o final, preservando o conteúdo antigo.
    fd = open(caminho, O_RDWR | O_CREAT | O_APPEND, 0644);
    
    if (fd == -1) {
        perror("Erro ao abrir ou criar o arquivo");
        return 1;
    }

    // --------------------------------------------------------
    // 2. Escrevendo no arquivo
    // --------------------------------------------------------
    const char *texto = "Teste 2.\n";

    lseek(fd, 512, SEEK_SET); // Reposiciona o cursor para o início do arquivo (opcional, pois O_APPEND já cuida disso)

    if (write(fd, texto, strlen(texto)) == -1) {
        perror("Erro ao escrever no arquivo");
        close(fd);
        return 1;
    }
    printf("Texto gravado com sucesso!\n\n");

    // --------------------------------------------------------
    // 3. Lendo o arquivo (O segredo do lseek)
    // --------------------------------------------------------
    // Como escrevemos no arquivo, o cursor agora está no FINAL dele.
    // Precisamos rebobinar o cursor de volta para o INÍCIO (byte 0) para ler tudo.
    // SEEK_SET significa "A partir do início do arquivo".
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("Erro ao reposicionar o cursor do arquivo");
        close(fd);
        return 1;
    }

    printf("--- Conteúdo total do arquivo ---\n");
    ssize_t bytes_lidos;
    
    // Lemos em um loop, caso o arquivo seja maior que o nosso buffer de 256 bytes
    while ((bytes_lidos = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_lidos] = '\0'; // Terminador de string em C
        printf("%s", buffer);
    }

    if (bytes_lidos == -1) {
        perror("Erro ao ler o arquivo");
    }
    printf("---------------------------------\n");

    // Fecha o descritor
    close(fd);

    return 0;
}