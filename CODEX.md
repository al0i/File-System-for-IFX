# Guia do Codex

Este projeto implementa uma simulacao de sistema de arquivos inspirada em conceitos de sistemas operacionais, usando um arquivo binario como disco virtual.

## Contexto atual

- Branch de trabalho: `file-block-control`.
- Disco virtual: `src/resources/virtual_disk.bin`.
- Tamanho de bloco: 512 bytes.
- Total atual: 16 blocos, resultando em 8192 bytes.
- Bloco 0: reservado para metadados do sistema de arquivos.
- Primeiro bloco de dados: bloco 1.
- Referencia conceitual: Sistemas Operacionais Modernos, Tanenbaum.

## Restricoes

- O codigo C deve usar apenas `stdio.h` e `stdlib.h`.
- Evitar dependencias externas.
- Manter a simulacao baseada em escrita/leitura direta no arquivo binario.
- Nao escrever dados de usuario no bloco 0.

## Fluxo atual

1. `scripts/run.sh` cria o diretorio `build/`.
2. Compila `src/main.c` para `build/main`.
3. Executa o binario.
4. Mostra o conteudo do disco virtual com `hexdump -C`.

## Estado da implementacao

- `init_disk()` abre ou cria o disco virtual.
- `initialize_virtual_disk()` zera todos os blocos do arquivo.
- `debug_write_block_labels()` escreve labels de depuracao nos blocos para validar offsets.
- `write_root_block()` grava o superbloco no bloco 0 com:
  - assinatura `IFXFS`;
  - tamanho de bloco `512`;
  - total de blocos `16`;
  - primeiro bloco de dados `1`.

## Resultado observado

Ao rodar `bash scripts/run.sh`, o `hexdump` mostra:

- offset `0x0000`: assinatura `IFXFS` e metadados do bloco raiz;
- offset `0x0200`: inicio do bloco 1;
- offset `0x0400`: inicio do bloco 2;
- offsets seguintes avancando de 512 em 512 bytes ate o bloco 15;
- tamanho final `0x2000`, equivalente a 8192 bytes.

## Observacoes para proximas alteracoes

- O disco virtual e reinicializado a cada execucao, entao dados anteriores sao perdidos.
- Se o objetivo passar a persistir dados, separar inicializacao/formattacao de operacoes normais.
- Antes de implementar alocacao real, definir a estrutura de controle de blocos livres.
- Manter o bloco 0 reservado para metadados e iniciar dados comuns no bloco 1.
