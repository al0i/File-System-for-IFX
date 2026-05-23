# CODEX.md

## 1) Papel deste documento

Este arquivo orienta a evolucao do simulador de gerenciamento de arquivos com base no capitulo 4 do PDF:

`docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf`

O simulador deve evoluir em etapas pequenas, sempre relacionando uma implementacao `x_n` com uma base bibliografica `y_n`.

Formato padrao de pedido:

- `code x_n based on y_n from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Este documento e a referencia operacional principal da IA neste repositorio, mas nao substitui `bib_steps.md`. O `bib_steps.md` continua sendo o mapa bibliografico detalhado. Este arquivo registra o estado atual, as decisoes arquiteturais e a ordem pratica de trabalho.

---

## 2) Hierarquia das instrucoes do projeto

As solicitacoes do usuario podem orientar a arquitetura e a experiencia de uso do simulador, mas devem ser encaixadas no roadmap `x_n/y_n`.

Regra de precedencia:

1. `bib_steps.md` define a estrutura ideal de ensino e a relacao com Tanenbaum.
2. `CODEX.md` adapta essa estrutura ao estado atual do codigo.
3. pedidos do usuario definem foco e prioridade, desde que nao quebrem a segmentacao didatica.

Consequencia:

- uma solicitacao arquitetural, como "criar uma biblioteca", nao vira uma etapa paralela com o mesmo peso de `x_n/y_n`
- a arquitetura deve facilitar as etapas, nao substituir as etapas
- operacoes completas de arquivo so devem ser consideradas completas quando a etapa bibliografica correspondente estiver implementada

---

## 3) Regras tecnicas fixas

O projeto deve usar somente estas bibliotecas padrao:

- `stdio.h`
- `stdlib.h`

Nao adicionar outras bibliotecas sem autorizacao explicita.

Consequencias praticas:

- comparacao, copia e tamanho de texto devem ser feitos por funcoes locais
- manipulacao do disco virtual deve usar `FILE`, `fopen`, `fread`, `fwrite`, `fseek`, `fflush`, `fclose` e funcoes equivalentes de `stdio.h`
- alocacao dinamica, quando necessaria, deve usar apenas recursos de `stdlib.h`
- nao usar `string.h`, `stdint.h`, `stdbool.h`, `sys/stat.h` ou bibliotecas externas

---

## 4) Regra de uso do PDF

Ao implementar uma etapa, o modelo deve:

1. localizar a secao `y_n` no PDF
2. extrair o conceito principal
3. adaptar o conceito para um simulador didatico
4. implementar apenas o escopo de `x_n`
5. preservar o layout persistente ja existente, salvo quando a etapa for explicitamente de formatacao ou migracao
6. explicar brevemente quais invariantes foram introduzidas

---

## 5) Direcao arquitetural atual: biblioteca IFX

O projeto deve caminhar para uma biblioteca simples de sistema de arquivos. A `main` deve ficar fina e servir como cliente da biblioteca, nao como lugar onde regras internas do disco sao implementadas.

Forma desejada de uso:

```c
IFXFileSystem fs;

ifx_init(&fs);
ifx_mount(&fs);
ifx_create_file(&fs, "TESTE.TXT");
ifx_write_file(&fs, "TESTE.TXT", data, size);
ifx_close(&fs);
```

Estado implementado:

- `src/ifx_fs.h` expoe a API publica inicial
- `src/ifx_fs.c` implementa a fachada da biblioteca
- `src/virtual_disk.h` expoe somente as operacoes de disco que a fachada precisa
- `src/virtual_disk.c` continua concentrando o layout persistente e as estruturas internas
- `src/main.c` usa `ifx_init`, `ifx_mount` e `ifx_close`

API publica atual:

- `ifx_init`: abre ou cria o disco virtual
- `ifx_mount`: monta o disco existente ou formata se estiver vazio/invalido
- `ifx_create_file`: cria uma entrada vazia no diretorio raiz
- `ifx_write_file`: escreve bytes em um arquivo existente
- `ifx_close`: fecha o disco virtual e limpa o estado da estrutura

Limite importante:

- `ifx_create_file` ainda nao escreve conteudo
- o arquivo criado inicialmente e uma entrada nomeada no diretorio raiz, com `first_block = -1`
- `ifx_write_file` aloca o bloco inicial internamente se o arquivo ainda nao tiver bloco
- `ifx_write_file` substitui o conteudo logico do arquivo a partir do inicio
- a escrita atual fica limitada a um bloco de dados
- o controle real de blocos livres pertence a `x_14 based on y_14`
- metadados mais completos, alem do tamanho logico atual, pertencem a evolucoes futuras de `x_5` ou etapas posteriores

Essa decisao permite que a biblioteca seja usada desde cedo sem fingir que as etapas futuras ja estao completas.

Decisao de abstracao para operacoes:

- `mount` nao deve criar, escrever, ler ou remover arquivos
- operacoes de arquivo devem acontecer depois de `mount`
- alocacao de blocos e detalhe interno da implementacao de escrita, nao operacao publica principal
- a `main` pode demonstrar um fluxo de uso, mas deve continuar chamando a API da biblioteca

Decisao implicita para `x_3`:

- o conteudo de arquivo e tratado como sequencia de bytes
- `size` representa a quantidade logica de bytes
- registros estruturados ficam fora do escopo atual
- essa decisao sustenta `x_12`, `x_4` e `x_6`

Decisao de simplificacao para `x_5`:

- os metadados serao mantidos dentro da entrada do diretorio raiz enquanto o simulador tiver apenas um diretorio simples
- nao criar uma tabela separada de metadados antes de haver necessidade real
- nao adicionar `type` enquanto so existir arquivo regular
- nao adicionar permissoes, dono ou carimbos temporais nesta fase

---

## 6) Mapa bibliografico

### y_1 -> 4.1 Arquivos

Base para tratar arquivo como unidade logica persistente.

### y_2 -> 4.1.1 Nomeacao de arquivos

Base para regras de nomes validos, colisao e comparacao.

### y_3 -> 4.1.2 Estrutura de arquivos

Base para decidir se o conteudo sera sequencia de bytes ou registros.

### y_4 -> 4.1.4 Acesso aos arquivos

Base para acesso sequencial, acesso direto e deslocamento.

### y_5 -> 4.1.5 Atributos de arquivos

Base para metadados: tamanho, tipo, localizacao e estado.

### y_6 -> 4.1.6 Operacoes com arquivos

Base para criar, abrir, fechar, ler, escrever, remover e truncar.

### y_7 -> 4.2 Diretorios

Base para diretorio raiz, entradas e associacao entre nomes e objetos.

### y_8 -> 4.2.2 Sistemas de diretorios hierarquicos

Base para subdiretorios e relacao pai-filho.

### y_9 -> 4.2.3 Nomes de caminhos

Base para resolver caminhos absolutos e relativos.

### y_10 -> 4.2.4 Operacoes com diretorios

Base para criar, listar, renomear e remover diretorios.

### y_11 -> 4.3.1 Esquema do sistema de arquivos

Base para layout interno do disco virtual.

No simulador, esta etapa deve definir:

- bloco de controle principal
- regiao de controle de espaco livre
- regiao de metadados
- bloco ou regiao do diretorio raiz
- regiao de dados
- diferenca entre formatar e montar um disco existente

### y_12 -> 4.3.2 Implementando arquivos

Base para escolher como blocos sao associados a arquivos.

### y_13 -> 4.3.3 Implementando diretorios

Base para formato interno das entradas de diretorio.

### y_14 -> 4.4.1 Gerenciamento de espaco em disco

Base para bitmap ou lista de blocos livres.

### y_15 -> 4.4.3 Consistencia do sistema de arquivos

Base para validacao de referencias, duplicidade e divergencia entre estruturas.

### y_16 -> 4.4.4 Desempenho do sistema de arquivos

Base para cache e reducao de leituras desnecessarias.

---

## 7) Roadmap atual

### Base de biblioteca IFX

Status: implementado como camada arquitetural de suporte.

Natureza:

- nao e uma etapa bibliografica autonoma
- serve para organizar o codigo e preparar uma API escalavel
- deve permanecer subordinada ao roadmap `x_n/y_n`

Invariantes:

- a `main` deve chamar a API publica da biblioteca
- detalhes de disco virtual devem ficar fora da `main`
- novas operacoes publicas so devem expor comportamento sustentado por uma etapa bibliografica

### x_11 based on y_11

Status: implementado como primeira etapa estrutural.

Objetivo:

- separar formatacao de montagem
- impedir sobrescrita automatica do disco a cada execucao
- registrar layout basico no superbloco

Layout adotado:

- bloco 0: superbloco
- bloco 1: mapa de espaco livre reservado
- bloco 2: regiao inicial de metadados
- bloco 3: diretorio raiz reservado
- bloco 4 em diante: dados

Invariantes:

- se o superbloco existente for valido, o disco e montado sem zerar blocos
- se o superbloco estiver ausente ou invalido, o disco e formatado
- a formatacao inicializa o layout base uma unica vez

### x_2 based on y_2

Status: suporte inicial implementado.

Implementado:

- nomes no formato 8.3
- caracteres permitidos: letras, numeros, `_` e `-`
- rejeicao de nome vazio, extensao vazia e mais de um ponto
- rejeicao de duplicidade no mesmo diretorio raiz

Pendente:

- decidir se havera normalizacao de caixa
- expor validacao de nome como operacao publica, se necessario

### x_7 based on y_7

Status: implementado como lista ligada no bloco `ROOT_DIR_BLOCK`.

Objetivo:

- usar um bloco reservado para representar o diretorio raiz
- armazenar entradas de arquivo no formato 8.3
- associar nome a um objeto persistido
- deixar a cadeia dos blocos para a estrategia de alocacao futura

Representacao adotada:

- cabecalho com assinatura `DIRLS2`
- ponteiro para primeira entrada ativa
- ponteiro para primeira entrada livre
- entradas ligadas por indice interno no bloco
- cada entrada guarda `name[8]`, `extension[3]`, `size`, `first_block`, `next_entry` e estado de uso

Invariantes:

- o diretorio raiz fica persistido no bloco 3
- nomes seguem o formato `8.3`
- entradas duplicadas no mesmo diretorio nao devem ser aceitas
- arquivo vazio pode ter `first_block = -1`
- blocos de dados reais sao associados pela politica direta minima de `x_12`

### x_5 based on y_5

Status: implementado de forma minima.

Objetivo:

- representar atributos basicos de arquivo sem transformar o simulador em um sistema de arquivos completo antes da hora
- preparar a escrita/leitura futura para saber o tamanho logico do arquivo

Atributos adotados:

- `name` e `extension`: identificacao no formato 8.3
- `size`: tamanho logico em bytes
- `first_block`: localizacao inicial, usando `-1` para arquivo vazio
- `used`: estado de ocupacao da entrada

Decisoes de simplificacao:

- `type` foi adiado porque ainda nao existem subdiretorios nem outros tipos de entrada
- permissoes, dono e timestamps ficaram fora do escopo
- os metadados ficam na propria entrada de diretorio raiz

Impacto no layout:

- a assinatura do bloco de diretorio raiz passou de `DIRLST` para `DIRLS2`
- isso versiona o formato da entrada, porque `DirectoryEntry` passou a incluir `size`
- se um disco antigo tiver o diretorio raiz no formato `DIRLST`, a montagem recria o bloco de diretorio raiz vazio no formato novo

### x_12 based on y_12

Status: implementado de forma minima.

Objetivo:

- definir como uma entrada de arquivo passa a apontar para dados no disco virtual
- preparar escrita e leitura futuras sem implementar ainda as operacoes completas de arquivo

Politica adotada:

- alocacao direta de um unico bloco inicial
- a entrada de diretorio usa `first_block` como referencia para esse bloco
- o bloco escolhido e o primeiro bloco da area de dados que ainda nao aparece como `first_block` de uma entrada ativa
- o bloco reservado e zerado antes de ser associado ao arquivo

Decisoes de simplificacao:

- esta etapa nao implementa FAT
- esta etapa nao implementa arquivo com multiplos blocos
- esta etapa nao implementa escrita nem leitura de conteudo
- esta etapa nao implementa bitmap/lista de espaco livre; isso permanece em `x_14`
- o tamanho maximo pratico de conteudo, ate uma futura evolucao, e um bloco de dados

Invariantes:

- arquivo criado por `ifx_create_file` continua podendo existir vazio com `first_block = -1`
- alocacao de bloco so opera sobre arquivo ja existente
- a chamada publica `ifx_write_file` aciona a alocacao de bloco quando necessario
- dois arquivos ativos nao devem receber o mesmo `first_block`
- `size` passa a refletir a quantidade de bytes escrita no arquivo

### x_4 based on y_4

Status: implementado parcialmente.

Objetivo:

- introduzir uma semantica inicial de acesso ao conteudo do arquivo
- permitir escrita de bytes em um arquivo existente

Semantica adotada:

- arquivo tratado como sequencia de bytes
- escrita substitui o conteudo a partir do inicio do arquivo
- `size` registra a quantidade logica de bytes escritos
- a escrita atual cabe em um unico bloco de dados

Pendente:

- leitura
- acesso por deslocamento arbitrario
- ponteiro de leitura/escrita
- escrita que ultrapassa um bloco

### x_6 based on y_6

Status: implementado parcialmente.

Operacoes publicas atuais:

- `ifx_create_file`
- `ifx_write_file`
- `ifx_close`

Pendente:

- abrir arquivo individual
- fechar arquivo individual
- ler
- remover
- truncar
- consultar metadados por API publica

### x_14 based on y_14

Status: pendente.

Objetivo:

- escolher posteriormente uma estrategia propria de controle de espaco livre
- nao confundir gerenciamento de espaco livre com FAT de encadeamento de arquivos
- manter o bloco `FREE_MAP_BLOCK` apenas como reservado enquanto a decisao nao for tomada

Decisao atual:

- nenhuma estrutura de mapa livre foi adotada ainda
- o bloco 1 guarda apenas o marcador `FREE_MAP`
- a FAT nao deve ser implementada dentro de `FREE_MAP_BLOCK`
- FAT/lista encadeada pertence a etapa de implementacao de arquivos, nao a esta etapa

Invariantes:

- o marcador `FREE_MAP` pode ser substituido depois por bitmap, lista livre ou outra estrutura
- a decisao deve ser tomada com base na etapa bibliografica correta
- se a estrategia escolhida for FAT, ela deve ser tratada em `x_12 based on y_12`

---

## 8) Ordem pratica recomendada

1. `x_11 based on y_11`: layout persistente, formatacao e montagem
2. base de biblioteca IFX: API publica minima para `init`, `mount`, `create_file` e `close`
3. `x_2 based on y_2`: validacao de nomes
4. `x_7 based on y_7`: diretorio raiz e entradas nomeadas
5. `x_5 based on y_5`: metadados de arquivos
6. `x_12 based on y_12`: estrategia de alocacao de blocos por arquivo
7. `x_14 based on y_14`: controle real de blocos livres, se necessario separadamente
8. `x_6 based on y_6`: operacoes completas de arquivo

Esta ordem prioriza uma base persistente correta antes de expor operacoes completas de arquivo.

Proxima etapa recomendada apos esta correcao:

- completar leitura em `x_4` e expor `ifx_read_file`, mantendo alocacao como detalhe interno.
