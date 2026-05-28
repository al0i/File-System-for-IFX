# Manual da biblioteca IFX

Este manual descreve como programar usando a biblioteca IFX do simulador de
sistema de arquivos.

A biblioteca esta exposta principalmente por:

```c
#include "ifx_fs.h"
```

O objetivo atual e representar, de forma didatica, as operacoes basicas de
arquivos descritas no capitulo 4 de Tanenbaum: criar, escrever, abrir, ler,
fechar e manter metadados minimos associados ao arquivo.

## Modelo mental

A biblioteca separa tres ideias:

- `IFXFileSystem`: representa o sistema de arquivos montado pelo programa.
- Diretorio raiz: estrutura persistente que associa nome de arquivo a seus
  metadados.
- Descritor de arquivo (`fd`): indice pequeno usado depois de abrir um arquivo.

O fluxo de leitura segue a ideia apresentada em operacoes com arquivos:

1. `open` localiza o arquivo no diretorio.
2. os metadados minimos sao carregados para uma tabela em memoria.
3. `read` usa um descritor, um buffer e uma quantidade de bytes.
4. cada leitura avanca o cursor do descritor.
5. `close` libera o descritor.

## Estruturas publicas

### `IFXFileSystem`

```c
typedef struct {
    FILE *disk;
    int mounted;
    IFXFileDescriptor fd_table[IFX_MAX_OPEN_FILES];
} IFXFileSystem;
```

Propriedades:

- `disk`: ponteiro para o arquivo que representa o disco virtual.
- `mounted`: indica se o disco virtual ja foi montado.
- `fd_table`: tabela simples de descritores de arquivos abertos.

Codigo cliente deve inicializar essa estrutura com `ifx_init` e montar com
`ifx_mount`. O acesso direto aos campos existe para fins didaticos, mas o uso
normal deve acontecer pelas funcoes publicas.

### `IFXFileDescriptor`

```c
typedef struct {
    int used;
    int cursor;
    int size;
    int first_block;
} IFXFileDescriptor;
```

Propriedades:

- `used`: indica se a entrada da tabela esta em uso.
- `cursor`: posicao logica atual para leitura.
- `size`: tamanho logico do arquivo em bytes.
- `first_block`: primeiro bloco de dados associado ao arquivo.

Essas propriedades sao carregadas durante `ifx_open_file`. Elas representam uma
visao em memoria dos metadados persistidos no diretorio raiz.

## Ciclo basico

```c
IFXFileSystem fs;

if (ifx_init(&fs) != 0) {
    return -1;
}

if (ifx_mount(&fs) != 0) {
    ifx_close(&fs);
    return -1;
}

/* operacoes de arquivo aqui */

ifx_close(&fs);
```

## Operacoes

### `ifx_init`

```c
int ifx_init(IFXFileSystem *fs);
```

Abre ou cria o disco virtual e limpa a tabela de descritores.

Retorno:

- `0`: sucesso.
- `-1`: erro.

### `ifx_mount`

```c
int ifx_mount(IFXFileSystem *fs);
```

Monta o disco virtual se o layout existente for valido. Se o disco estiver vazio
ou invalido, formata o layout inicial.

Retorno:

- `0`: sucesso.
- `-1`: erro.

### `ifx_create_file`

```c
int ifx_create_file(IFXFileSystem *fs, const char *filename);
```

Cria uma entrada vazia no diretorio raiz.

Regras atuais de nome:

- formato `NOME.EXT`;
- ate 8 caracteres no nome;
- ate 3 caracteres na extensao;
- letras, numeros, `_` e `-`;
- nao aceita nome duplicado no diretorio raiz.

Retorno:

- `0`: arquivo criado.
- `-1`: erro, nome invalido, duplicidade ou falta de entrada livre.

### `ifx_write_file`

```c
int ifx_write_file(
    IFXFileSystem *fs,
    const char *filename,
    const unsigned char *data,
    int size
);
```

Escreve bytes em um arquivo existente. Se o arquivo ainda nao tiver bloco de
dados, a biblioteca aloca internamente o primeiro bloco livre disponivel.

Semantica atual:

- substitui o conteudo desde o inicio do arquivo;
- atualiza o tamanho logico (`size`);
- limita a escrita a um bloco de dados.

Retorno:

- `0`: sucesso.
- `-1`: erro.

### `ifx_open_file`

```c
int ifx_open_file(IFXFileSystem *fs, const char *filename);
```

Abre um arquivo existente e retorna um descritor (`fd`).

Durante a abertura, a biblioteca:

- le o diretorio raiz em um buffer;
- procura a entrada pelo nome;
- carrega `size` e `first_block` para a tabela de descritores;
- inicializa o cursor do descritor em `0`.

Retorno:

- `0` ou maior: descritor do arquivo aberto.
- `-1`: erro.

### `ifx_read_file`

```c
int ifx_read_file(
    IFXFileSystem *fs,
    int fd,
    unsigned char *buffer,
    int count
);
```

Le bytes de um arquivo aberto.

Parametros:

- `fd`: descritor retornado por `ifx_open_file`.
- `buffer`: destino dos bytes lidos.
- `count`: quantidade maxima de bytes solicitada.

Semantica:

- a leitura comeca no cursor atual do descritor;
- a funcao nunca le alem do tamanho logico do arquivo;
- o retorno informa a quantidade real de bytes lidos;
- o cursor avanca pela quantidade lida;
- ao chegar no fim logico do arquivo, novas leituras retornam `0`.

Retorno:

- `> 0`: quantidade de bytes lidos.
- `0`: fim do arquivo ou leitura de zero bytes.
- `-1`: erro.

### `ifx_close_file`

```c
int ifx_close_file(IFXFileSystem *fs, int fd);
```

Fecha um descritor de arquivo e libera sua entrada na tabela em memoria.

Retorno:

- `0`: sucesso.
- `-1`: descritor invalido ou erro.

### `ifx_close`

```c
int ifx_close(IFXFileSystem *fs);
```

Fecha o disco virtual e limpa o estado em memoria do sistema de arquivos.

Retorno:

- `0`: sucesso.
- `-1`: erro.

## Exemplo completo

```c
#include <stdio.h>
#include <stdlib.h>

#include "ifx_fs.h"

int main()
{
    IFXFileSystem fs;
    const unsigned char data[] = "IFX";
    unsigned char buffer[4];
    int fd;
    int bytesRead;

    if (ifx_init(&fs) != 0) {
        return -1;
    }

    if (ifx_mount(&fs) != 0) {
        ifx_close(&fs);
        return -1;
    }

    ifx_create_file(&fs, "DEMO.TXT");

    if (ifx_write_file(&fs, "DEMO.TXT", data, 3) != 0) {
        ifx_close(&fs);
        return -1;
    }

    fd = ifx_open_file(&fs, "DEMO.TXT");

    if (fd < 0) {
        ifx_close(&fs);
        return -1;
    }

    bytesRead = ifx_read_file(&fs, fd, buffer, 3);

    if (bytesRead < 0) {
        ifx_close_file(&fs, fd);
        ifx_close(&fs);
        return -1;
    }

    buffer[bytesRead] = '\0';
    printf("%s\n", buffer);

    ifx_close_file(&fs, fd);
    ifx_close(&fs);

    return 0;
}
```

## Leitura em partes

`ifx_read_file` avanca o cursor automaticamente. Por isso, duas chamadas no
mesmo descritor leem partes consecutivas do arquivo:

```c
unsigned char buffer[4];
int fd = ifx_open_file(&fs, "DEMO.TXT");
int firstRead = ifx_read_file(&fs, fd, buffer, 2);
int secondRead = ifx_read_file(&fs, fd, buffer + firstRead, 2);

buffer[firstRead + secondRead] = '\0';
```

Se o arquivo tiver 3 bytes, a primeira chamada retorna `2` e a segunda retorna
`1`.

## Limitacoes atuais

- Existe apenas diretorio raiz.
- Nao ha caminhos hierarquicos.
- Cada arquivo usa no maximo um bloco de dados.
- Nao ha `seek` publico.
- Nao ha remocao, truncamento ou renomeacao.
- Nao ha permissoes, dono ou timestamps.
- O mapa real de blocos livres ainda nao foi implementado.

Essas limitacoes sao intencionais para manter a evolucao alinhada ao roadmap
`x_n/y_n` do projeto.
