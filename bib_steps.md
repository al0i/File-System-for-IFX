## 1) Objetivo deste arquivo

Este `CODEX.md` define como o modelo deve conduzir a evolucao de um simulador de gerenciamento de arquivos usando como referencia primaria o capitulo 4 do PDF:

`docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf`

O objetivo nao e descrever o estado atual do codigo. O objetivo e transformar o capitulo em um instrucional de desenvolvimento incremental para um simulador de sistema de arquivos.

Este arquivo deve guiar pedidos no formato:

- `code x_n based on y_n from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Onde:

- `x_n` representa uma etapa concreta de implementacao no simulador.
- `y_n` representa a secao conceitual do capitulo 4 que fundamenta a etapa.

Restricao desta versao:

- nao usar linguagem C como foco da instrucao
- priorizar etapas de desenvolvimento, responsabilidades de modulo, regras de consistencia e sequenciamento de implementacao

---

## 2) Regra principal de uso pelo modelo

Ao receber uma solicitacao de implementacao, o modelo deve:

1. localizar a secao conceitual `y_n` no PDF
2. extrair dela os requisitos funcionais e estruturais
3. traduzir esses requisitos para o contexto de um simulador de gerenciamento de arquivos
4. implementar apenas o escopo da etapa `x_n`
5. preservar compatibilidade com as etapas anteriores
6. explicitar quais estruturas, operacoes e invariantes foram introduzidas

O modelo nao deve pular diretamente para uma implementacao completa de sistema de arquivos se o pedido estiver segmentado. Cada entrega deve corresponder a uma etapa isolavel do simulador.

---

## 3) Modelo de segmentacao obrigatoria

Cada pedido ou tarefa futura deve ser interpretado dentro do seguinte padrao:

- `code x_1 based on y_1 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`
- `code x_2 based on y_2 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`
- `code x_3 based on y_3 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Cada `x_n` deve mapear para uma unidade implementavel do simulador. Cada `y_n` deve mapear para uma secao do capitulo 4.

Estrutura esperada para cada entrega:

- objetivo da etapa
- secao de origem no PDF
- conceito extraido
- impacto no simulador
- estruturas ou modulos afetados
- regras de validacao
- dependencias sobre etapas anteriores

---

## 4) Mapa-base do capitulo 4 para o simulador

### y_1 -> 4.1 Arquivos

Usar para introduzir a abstracao de arquivo como unidade logica persistente.

Aplicacoes no simulador:

- entidade arquivo
- identificacao por nome
- persistencia independente do processo
- leitura e escrita como operacoes centrais

### y_2 -> 4.1.1 Nomeacao de arquivos

Usar para definir regras de nomes validos.

Aplicacoes no simulador:

- validacao de nome
- normalizacao de comparacao
- regra para duplicidade no mesmo diretorio

### y_3 -> 4.1.2 Estrutura de arquivos

Usar para decidir o modelo logico do conteudo.

Aplicacoes no simulador:

- arquivo como sequencia de bytes
- ou arquivo como registros, se o escopo exigir
- definicao explicita do formato interno suportado

### y_4 -> 4.1.4 Acesso aos arquivos

Usar para modelar como o simulador le e escreve dados.

Aplicacoes no simulador:

- acesso sequencial
- acesso direto por deslocamento
- ponteiro de leitura e escrita

### y_5 -> 4.1.5 Atributos de arquivos

Usar para criar metadados.

Aplicacoes no simulador:

- nome
- tipo
- tamanho
- localizacao inicial
- estado de uso
- permissao simplificada
- carimbos temporais se o projeto exigir

### y_6 -> 4.1.6 Operacoes com arquivos

Usar para definir a API funcional do simulador.

Aplicacoes no simulador:

- criar
- abrir
- fechar
- ler
- escrever
- remover
- truncar
- consultar metadados

### y_7 -> 4.2 Diretórios

Usar para introduzir agrupamento e organizacao de arquivos.

Aplicacoes no simulador:

- diretorio raiz
- entradas de diretorio
- associacao nome -> objeto

### y_8 -> 4.2.2 Sistemas de diretorios hierarquicos

Usar para evoluir do diretorio unico para arvores.

Aplicacoes no simulador:

- subdiretorios
- pai e filhos
- navegacao na hierarquia

### y_9 -> 4.2.3 Nomes de caminhos

Usar para resolver caminhos absolutos e relativos.

Aplicacoes no simulador:

- parser de caminho
- resolucao por componentes
- tratamento de erro em caminhos invalidos

### y_10 -> 4.2.4 Operacoes com diretorios

Usar para ampliar a API do simulador.

Aplicacoes no simulador:

- criar diretorio
- listar conteudo
- renomear entrada
- remover diretorio vazio

### y_11 -> 4.3.1 Esquema do sistema de arquivos

Usar para definir o layout interno do disco simulado.

Aplicacoes no simulador:

- bloco de controle principal
- mapa de espaco livre
- area de diretorios
- area de dados
- convencoes de enderecamento

### y_12 -> 4.3.2 Implementando arquivos

Usar para decidir como blocos sao associados a arquivos.

Aplicacoes no simulador:

- alocacao contigua
- lista encadeada
- tabela de indices
- escolha de politica para o projeto

### y_13 -> 4.3.3 Implementando diretorios

Usar para definir o formato de entradas de diretorio.

Aplicacoes no simulador:

- entrada com nome e referencia
- diferenciacao entre arquivo e diretorio
- busca linear ou indexada

### y_14 -> 4.4.1 Gerenciamento de espaco em disco

Usar para controlar espaco livre e ocupado.

Aplicacoes no simulador:

- bitmap de blocos
- lista de blocos livres
- estrategia de alocacao

### y_15 -> 4.4.3 Consistencia do sistema de arquivos

Usar para criar regras de verificacao e reparo basico.

Aplicacoes no simulador:

- detectar bloco marcado e nao referenciado
- detectar referencia duplicada
- validar contagens e encadeamentos

### y_16 -> 4.4.4 Desempenho do sistema de arquivos

Usar para otimizar acesso sem alterar o modelo conceitual.

Aplicacoes no simulador:

- cache simplificado
- reducao de leituras redundantes
- organizacao mais eficiente das buscas

---

## 5) Roadmap instrucional em etapas `x_n`

### x_1 based on y_1

`code x_1 based on y_1 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- introduzir a abstracao de arquivo persistente no simulador

Entregaveis:

- definicao conceitual de arquivo
- identificador basico
- armazenamento persistente no disco virtual

Nao incluir ainda:

- hierarquia de diretorios
- politica completa de alocacao

### x_2 based on y_2

`code x_2 based on y_2 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- implementar regras de nomeacao e unicidade

Entregaveis:

- validacao de nomes
- regra para colisao de nomes
- padrao de comparacao no mesmo diretorio

### x_3 based on y_3

`code x_3 based on y_3 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- definir o modelo do conteudo do arquivo

Entregaveis:

- arquivo como sequencia de bytes ou registros
- criterio explicito de escolha
- reflexo no layout logico do simulador

### x_4 based on y_4

`code x_4 based on y_4 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- implementar leitura e escrita com semantica clara de acesso

Entregaveis:

- acesso sequencial
- acesso direto
- controle de deslocamento

### x_5 based on y_5

`code x_5 based on y_5 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- adicionar atributos e metadados de arquivo

Entregaveis:

- tamanho logico
- tipo da entrada
- localizacao
- estado de ocupacao

### x_6 based on y_6

`code x_6 based on y_6 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- consolidar o conjunto minimo de operacoes de arquivo

Entregaveis:

- criar
- abrir
- fechar
- ler
- escrever
- remover

### x_7 based on y_7

`code x_7 based on y_7 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- introduzir o diretorio raiz como estrutura organizadora

Entregaveis:

- tabela de entradas
- associacao entre nomes e arquivos
- regra de busca por nome

### x_8 based on y_8

`code x_8 based on y_8 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- evoluir para diretorios hierarquicos

Entregaveis:

- subdiretorios
- relacao pai-filho
- navegacao de arvore

### x_9 based on y_9

`code x_9 based on y_9 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- suportar caminhos absolutos e relativos

Entregaveis:

- tokenizacao do caminho
- resolucao incremental
- retorno de erro em componentes ausentes

### x_10 based on y_10

`code x_10 based on y_10 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- completar operacoes de diretorio

Entregaveis:

- criar diretorio
- listar
- remover diretorio vazio
- renomear entrada

### x_11 based on y_11

`code x_11 based on y_11 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- definir o layout estrutural do disco virtual

Entregaveis:

- bloco de controle principal
- regiao de alocacao
- regiao de metadados
- regiao de dados

### x_12 based on y_12

`code x_12 based on y_12 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- escolher e implementar a estrategia de alocacao de blocos para arquivos

Entregaveis:

- politica de alocacao definida
- encadeamento ou indexacao de blocos
- leitura consistente do conteudo armazenado

### x_13 based on y_13

`code x_13 based on y_13 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- formalizar a implementacao interna dos diretorios

Entregaveis:

- formato da entrada de diretorio
- estrategia de busca
- integracao com arquivos e subdiretorios

### x_14 based on y_14

`code x_14 based on y_14 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- implementar o controle de espaco livre

Entregaveis:

- bitmap ou estrutura equivalente
- reserva e liberacao de blocos
- validacao de exaustao de espaco

### x_15 based on y_15

`code x_15 based on y_15 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- adicionar verificacao de consistencia do sistema de arquivos

Entregaveis:

- checagem de referencias invalidas
- checagem de blocos duplicados
- checagem entre bitmap e estruturas ativas

### x_16 based on y_16

`code x_16 based on y_16 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Objetivo:

- otimizar o simulador apos a corretude basica estar estavel

Entregaveis:

- cache simplificado
- melhoria de localidade
- reducao de leituras desnecessarias

---

## 6) Ordem obrigatoria de desenvolvimento

A ordem recomendada para pedidos e implementacoes e:

1. `x_1` ate `x_6` para formar o modelo basico de arquivo
2. `x_7` ate `x_10` para introduzir e consolidar diretorios
3. `x_11` ate `x_14` para estruturar layout interno e alocacao
4. `x_15` para consistencia
5. `x_16` para otimizacao

O modelo deve tratar qualquer pedido fora de ordem como uma extensao com dependencia. Se uma etapa depender de outra ainda inexistente, isso deve ser explicitado antes da implementacao.

---

## 7) Regras de traducao do PDF para implementacao

Ao usar o PDF como base, o modelo deve seguir estas regras:

- traduzir conceitos, nao copiar texto do livro como especificacao de codigo
- adaptar a teoria para um simulador didatico de gerenciamento de arquivos
- distinguir claramente interface logica, metadados, alocacao e consistencia
- preferir implementacoes incrementais e verificaveis
- manter compatibilidade entre layout persistente e operacoes expostas

Quando houver mais de uma alternativa conceitual no capitulo, o modelo deve:

- escolher uma estrategia
- justificar a escolha de forma curta
- indicar o que ficou fora da etapa atual

---

## 8) Formato esperado para futuras solicitacoes

Exemplos validos:

- `code x_4 based on y_4 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`
- `code x_11 based on y_11 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`
- `code x_15 based on y_15 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf"`

Exemplos validos com contexto adicional:

- `code x_8 based on y_8 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf" using the current root directory model`
- `code x_12 based on y_12 from "docs/Sistemas Operacionais Modernos (Andrew S. Tanenbaum, Herbert Bos)-200-250.pdf" choosing indexed allocation`

---

## 9) Regra final para este repositorio

Este `CODEX.md` deve ser lido como um roteiro de engenharia orientado por bibliografia. O modelo deve usar o PDF como referencia conceitual principal e converter cada secao relevante em uma etapa objetiva de construcao do simulador de gerenciamento de arquivos.

Se a solicitacao do usuario mencionar `x_n` e `y_n`, o modelo deve assumir que essa segmentacao e deliberada e responder implementando somente o recorte pedido.
