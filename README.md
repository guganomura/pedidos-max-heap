# Sistema de pedidos com Max-Heap

Sistema em C que gerencia pedidos com um Max-Heap armazenado em vetor dinâmico, implementado
manualmente. O pedido de maior prioridade fica sempre na posição 0 e é o primeiro a ser atendido.
Usa apenas a biblioteca padrão do C.

- [Compilação e execução](#compilação-e-execução)
- [Como usar](#como-usar)
- [A estrutura de dados](#a-estrutura-de-dados)
- [Operações do Heap](#operações-do-heap)
- [Funcionalidades](#funcionalidades)
- [Exemplos de execução](#exemplos-de-execução)
- [Testes realizados](#testes-realizados)
- [Resultados dos testes](#resultados-dos-testes)

## Compilação e execução

É necessário um compilador C99 (GCC ou Clang).

Com Make (macOS e Linux):

```bash
make        # compila
make run    # executa
make test   # roda todos os testes
make clean  # apaga os binários
```

Sem Make:

```bash
mkdir -p build
cc -std=c99 src/main.c src/heap.c src/visualizacao.c -o build/pedidos
./build/pedidos

cc -std=c99 tests/test_unitarios.c src/heap.c src/visualizacao.c -o build/testes
./build/testes
sh tests/test_sistema.sh
```

No Windows, com MinGW:

```bat
gcc -std=c99 src/main.c src/heap.c src/visualizacao.c -o pedidos.exe
pedidos.exe
```

## Como usar

| Opção | Ação |
|:-:|---|
| 1 | Cadastrar pedido (ID, descrição e prioridade) |
| 2 | Atender o pedido de maior prioridade |
| 3 | Exibir pedidos |
| 4 | Exibir quantidade de pedidos |
| 5 | Sair |
| 6 | Exibir o estado do heap (vetor, índices e árvore) |
| 7 | Construir o heap a partir de um conjunto |
| 8 | Inicializar um heap vazio |

As opções 1 a 5 são as do enunciado, com os mesmos números. As opções 6 a 8 cobrem as demais
funcionalidades pedidas. Como no exemplo do enunciado, o menu aparece uma vez. Enter vazio o mostra
de novo.

## Organização

```text
src/pedido.h                          dados de um pedido
src/heap.h, heap.c                    Max-Heap
src/visualizacao.h, visualizacao.c    desenho do vetor e da árvore
src/main.c                            menu e validação da entrada
tests/test_unitarios.c                testes do heap
tests/test_sistema.sh                 testes do sistema completo
```

## A estrutura de dados

```c
typedef struct {
    int id;
    char descricao[TAM_DESCRICAO];
    int prioridade;
} Pedido;

typedef struct {
    Pedido *dados;
    int tamanho;
    int capacidade;
} Heap;
```

A árvore não tem ponteiros. Ela está implícita nos índices do vetor:

```text
índice     0   1   2   3   4   5   6   7
         +---+---+---+---+---+---+---+---+
dados    | 8 | 5 | 3 | 2 | 1 |   |   |   |      tamanho = 5, capacidade = 8
         +---+---+---+---+---+---+---+---+

                 [8] i=0
                /   \
          i=1 [5]   [3] i=2
              / \
      i=3 [2]   [1] i=4

pai       = (i - 1) / 2
esquerdo  = 2 * i + 1
direito   = 2 * i + 2
```

**Propriedade do Max-Heap:** todo pai tem prioridade maior ou igual à dos filhos. Isso garante o maior
na raiz, mas **não deixa o vetor ordenado**: `[5, 2, 3]` é um heap válido.

## Operações do Heap

| Operação | O que faz | Tempo |
|---|---|:-:|
| `inicializar_heap` | aloca o vetor e, se receber pedidos, constrói o heap | O(n) |
| `inserir` | põe no final e sobe | O(log n) amortizado |
| `remover` | tira a raiz, põe o último no lugar e desce | O(log n) |
| `max_heapfy` | faz um elemento descer até o lugar certo | O(log n) |
| `construir` | transforma um vetor qualquer em heap | O(n) |
| `print_heap` | exibe os pedidos na ordem do vetor | O(n) |

### `inicializar_heap`

`inicializar_heap(&heap, capacidade, vetor, n)` tem dois usos:

- **heap vazio:** `inicializar_heap(&heap, 4, NULL, 0)`;
- **a partir de um vetor:** copia os `n` pedidos, aumentando a capacidade se preciso, e chama
  `construir`. O vetor de origem não é alterado.

### `inserir`

Coloca o pedido no final e troca com o pai enquanto for maior que ele. Inserindo 8 em `[5, 2, 3]`:

```text
final do vetor               [5, 2, 3, 8]     i = 3, pai = 1
8 > 2, troca                 [5, 8, 3, 2]     i = 1, pai = 0
8 > 5, troca                 [8, 5, 3, 2]     i = 0, chegou à raiz
```

Se o vetor estiver cheio, a capacidade dobra com `realloc`. Dobrar, em vez de somar 1, faz o custo
médio do crescimento ser O(1) por inserção. Se o `realloc` falhar, o heap fica como estava.

### `remover`

Retorna a raiz, coloca o último elemento no lugar dela e chama `max_heapfy(0)`:

```text
início                       [8, 5, 3, 2]
retira 8, o 2 vira raiz      [2, 5, 3]        filhos 5 e 3
troca com o maior filho      [5, 2, 3]
```

### `max_heapfy`

Compara o elemento `i` com os dois filhos. Se algum for maior, troca com o **maior deles** e repete
na posição para onde ele desceu. Trocar com o menor filho deixaria um pai menor que um filho.
Pressupõe que as subárvores de `i` já são heaps. Só considera índices menores que `tamanho`.

### `construir`

Chama `max_heapfy` de `tamanho / 2 - 1` até 0. Os índices seguintes são folhas, que já são heaps.
A ordem é de trás para frente para que as subárvores estejam prontas quando o pai for processado.

```text
entrada                      [2, 5, 3, 8, 1]
max_heapfy(1): 5 x 8         [2, 8, 3, 5, 1]
max_heapfy(0): 2 x 8         [8, 2, 3, 5, 1]
continua descendo: 2 x 5     [8, 5, 3, 2, 1]
```

É O(n), melhor que inserir um a um, que é O(n log n) no pior caso.

### `print_heap`

Exibe `ID: 104 | Prioridade: 8` para cada pedido, na ordem do vetor e sem remover nada. Por isso,
após atender o 104 no exemplo do enunciado, a lista é 102, 101, 103.

`pai`, `esquerdo`, `direito` e `liberar_heap` são funções auxiliares.

## Funcionalidades

| Requisito | Opção | Função | Testes |
|---|:-:|---|---|
| Inicializar um heap vazio | 8 e ao abrir | `inicializar_heap` | INI-01, SIS-04, SIS-28 |
| Cadastrar um pedido | 1 | `inserir` | INS-01, SIS-06 |
| Informar ID, descrição e prioridade | 1 | `ler_pedido` | SIS-07, SIS-32 a SIS-39 |
| Exibir os pedidos | 3 | `print_heap` | PRI-03, SIS-02 |
| Atender o de maior prioridade | 2 | `remover` | REM-03, SIS-03 |
| Informar qual foi atendido | 2 | `atender` | SIS-03, SIS-07 |
| Exibir o estado atual do heap | 6 | `exibir_estado` | SIS-15 a SIS-19 |
| Construir de um conjunto inicial | 7 | `construir` | CON-01, SIS-20 a SIS-22 |
| Cadastrar após construir | 1 | `inserir` | CON-03, SIS-24 |
| Atender até esvaziar | 2 | `remover` | REM-07, SIS-12, SIS-13 |
| Informar a quantidade | 4 | `heap.tamanho` | SIS-08, SIS-09 |
| Aumentar a capacidade | automático | `realloc` em `inserir` | INS-06, INS-10, SIS-17 |

### Decisões de projeto

- **Prioridades iguais** não são atendidas necessariamente por ordem de chegada. Um heap não é
  estável e o enunciado não define critério de desempate.
- **Prioridade** aceita qualquer `int`, inclusive negativo.
- **ID** é positivo e único entre os pedidos aguardando. Conferir isso no cadastro custa O(n) e é
  feito pelo sistema, fora de `inserir`, que continua O(log n).
- **Capacidade** começa em 4 (ou no tamanho do conjunto inicial, se for maior), dobra quando
  necessário e só volta ao valor inicial quando o heap é reinicializado.
- **Construir e inicializar** pedem confirmação se houver pedidos a descartar.

## Exemplos de execução

### Exemplo 1: execução do enunciado

```text
===== SISTEMA DE PEDIDOS =====

1 - Cadastrar pedido
2 - Atender pedido
3 - Exibir pedidos
4 - Exibir quantidade de pedidos
5 - Sair
6 - Exibir estado do heap
7 - Construir heap a partir de um conjunto
8 - Inicializar heap vazio

Escolha: 1

ID: 101
Descrição: Pedido de 10 produtos
Prioridade: 2

Escolha: 1

ID: 102
Descrição: Pedido de 5 produtos
Prioridade: 5

Escolha: 1

ID: 103
Descrição: Pedido de 20 produtos
Prioridade: 3

Escolha: 1

ID: 104
Descrição: Pedido urgente
Prioridade: 8

Escolha: 3

Pedidos:
ID: 104 | Prioridade: 8
ID: 102 | Prioridade: 5
ID: 103 | Prioridade: 3
ID: 101 | Prioridade: 2

Escolha: 2

Pedido atendido:
ID: 104
Descrição: Pedido urgente
Prioridade: 8

Pedidos restantes:
ID: 102 | Prioridade: 5
ID: 101 | Prioridade: 2
ID: 103 | Prioridade: 3

Escolha: 5

Sistema encerrado.
```

### Exemplo 2: construir, cadastrar, atender até esvaziar

```text
===== SISTEMA DE PEDIDOS =====

1 - Cadastrar pedido
2 - Atender pedido
3 - Exibir pedidos
4 - Exibir quantidade de pedidos
5 - Sair
6 - Exibir estado do heap
7 - Construir heap a partir de um conjunto
8 - Inicializar heap vazio

Escolha: 7

Conjunto inicial (1 = exemplo do enunciado, 2 = digitar): 1

Vetor recebido: [2, 5, 3, 8, 1]
Heap construído: [8, 5, 3, 2, 1]

Escolha: 1

ID: 6
Descrição: Pedido F
Prioridade: 9

Escolha: 6

Estado do heap:
Tamanho: 6 | Capacidade: 10
Vetor: [9, 5, 8, 2, 1, 3]
  i=0 (raiz)  | ID 6 | Prioridade 9 | Pedido F
  i=1 (pai 0) | ID 2 | Prioridade 5 | Pedido B
  i=2 (pai 0) | ID 4 | Prioridade 8 | Pedido D
  i=3 (pai 1) | ID 1 | Prioridade 2 | Pedido A
  i=4 (pai 1) | ID 5 | Prioridade 1 | Pedido E
  i=5 (pai 2) | ID 3 | Prioridade 3 | Pedido C

Árvore (E = esquerdo, D = direito):
[9]
|-- E: [5]
|   |-- E: [2]
|   `-- D: [1]
`-- D: [8]
    `-- E: [3]

Escolha: 4

Pedidos aguardando atendimento: 6

Escolha: 2

Pedido atendido:
ID: 6
Descrição: Pedido F
Prioridade: 9

Pedidos restantes:
ID: 4 | Prioridade: 8
ID: 2 | Prioridade: 5
ID: 3 | Prioridade: 3
ID: 1 | Prioridade: 2
ID: 5 | Prioridade: 1

Escolha: 2

Pedido atendido:
ID: 4
Descrição: Pedido D
Prioridade: 8

Pedidos restantes:
ID: 2 | Prioridade: 5
ID: 1 | Prioridade: 2
ID: 3 | Prioridade: 3
ID: 5 | Prioridade: 1

Escolha: 2

Pedido atendido:
ID: 2
Descrição: Pedido B
Prioridade: 5

Pedidos restantes:
ID: 3 | Prioridade: 3
ID: 1 | Prioridade: 2
ID: 5 | Prioridade: 1

Escolha: 2

Pedido atendido:
ID: 3
Descrição: Pedido C
Prioridade: 3

Pedidos restantes:
ID: 1 | Prioridade: 2
ID: 5 | Prioridade: 1

Escolha: 2

Pedido atendido:
ID: 1
Descrição: Pedido A
Prioridade: 2

Pedidos restantes:
ID: 5 | Prioridade: 1

Escolha: 2

Pedido atendido:
ID: 5
Descrição: Pedido E
Prioridade: 1

Pedidos restantes:
Nenhum pedido aguardando atendimento.

Escolha: 2

Não há pedidos para atender.

Escolha: 5

Sistema encerrado.
```

## Testes realizados

São **94 testes automatizados**: 52 unitários e 42 de sistema. Para repetir:

```bash
make test
```

Os testes EXA são exaustivos: usam os 873612 vetores de tamanho n até 7 com valores de 0 a n-1,
inclusive repetidos, e conferem a propriedade, a ordem de atendimento e se nenhum pedido se perdeu.

### Heap

#### Índices

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| IDX-01 | pai dos indices 1, 2, 3 e 4 | `0, 0, 1 e 1` | Passou |
| IDX-02 | filho esquerdo de 0 e 3 | `1 e 7` | Passou |
| IDX-03 | filho direito de 0 e 3 | `2 e 8` | Passou |
| IDX-04 | pai de cada filho, 100000 indices | `volta ao proprio indice` | Passou |

#### `inicializar_heap`

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| INI-01 | heap vazio com capacidade 4 | `tamanho 0, capacidade 4` | Passou |
| INI-02 | capacidade zero | `rejeita` | Passou |
| INI-03 | capacidade negativa | `rejeita` | Passou |
| INI-04 | a partir de um vetor | `[8, 5, 3, 2, 1]` | Passou |
| INI-05 | vetor de origem | `nao e alterado` | Passou |
| INI-06 | vetor maior que a capacidade | `capacidade sobe para 5` | Passou |
| INI-07 | tamanho sem vetor | `rejeita` | Passou |
| INI-08 | tamanho negativo | `rejeita` | Passou |
| INI-09 | liberar duas vezes | `heap zerado, sem falha` | Passou |

#### `inserir`

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| INS-01 | inserir no heap vazio | `fica na raiz` | Passou |
| INS-02 | maior de todos | `sobe ate a raiz` | Passou |
| INS-03 | menor que o pai | `permanece no final` | Passou |
| INS-04 | maior que o pai, menor que a raiz | `sobe um nivel` | Passou |
| INS-05 | exemplo do enunciado | `[8, 5, 3, 2, 1]` | Passou |
| INS-06 | quinto pedido com capacidade 4 | `capacidade dobra para 8` | Passou |
| INS-07 | troca leva o pedido inteiro | `ID e descricao acompanham` | Passou |
| INS-08 | prioridade igual a do pai | `nao troca` | Passou |
| INS-09 | extremos de int e negativos | `compara sem estouro` | Passou |
| INS-10 | 100000 pedidos em capacidade 1 | `cresce e continua heap` | Passou |
| INS-11 | inserir em heap liberado | `aloca de novo` | Passou |

#### `remover`

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| REM-01 | remover de heap vazio | `falha, destino inalterado` | Passou |
| REM-02 | remover o unico | `heap vazio` | Passou |
| REM-03 | remover de [8, 5, 3, 2] | `retorna o 8` | Passou |
| REM-04 | vetor apos a remocao | `[5, 2, 3], como no enunciado` | Passou |
| REM-05 | capacidade apos a remocao | `nao muda` | Passou |
| REM-06 | filho direito e o maior | `desce pela direita` | Passou |
| REM-07 | inserir apos esvaziar | `volta a funcionar` | Passou |

#### `max_heapfy`

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| MAX-01 | esquerdo e o maior | `troca com o esquerdo` | Passou |
| MAX-02 | direito e o maior | `troca com o direito` | Passou |
| MAX-03 | pai ja e o maior | `nao troca` | Passou |
| MAX-04 | precisa descer dois niveis | `[9, 7, 8, 1, 6]` | Passou |
| MAX-05 | apenas o filho esquerdo | `troca` | Passou |
| MAX-06 | filhos fora do tamanho | `sao ignorados` | Passou |
| MAX-07 | prioridades iguais | `nao troca` | Passou |
| MAX-08 | chamar em indice interno | `corrige a subarvore` | Passou |

#### `construir`

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| CON-01 | exemplo do enunciado | `[8, 5, 3, 2, 1]` | Passou |
| CON-02 | vetor que ja e heap | `nao muda` | Passou |
| CON-03 | inserir apos construir | `continua heap` | Passou |
| CON-04 | vetor crescente | `[7, 5, 6, 4, 2, 1, 3]` | Passou |
| CON-05 | vetor vazio | `continua vazio` | Passou |
| CON-06 | um elemento | `nao muda` | Passou |
| CON-07 | todos iguais | `nao troca` | Passou |

#### `print_heap`

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| PRI-01 | heap vazio | `avisa que nao ha pedidos` | Passou |
| PRI-02 | um pedido | `ID e prioridade` | Passou |
| PRI-03 | varios pedidos | `ordem do vetor` | Passou |
| PRI-04 | exibir nao remove | `heap inalterado` | Passou |

#### Exaustivos

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| EXA-01 | construir os 873612 vetores de ate 7 itens | `atende em ordem decrescente` | Passou |
| EXA-02 | inserir um a um os 873612 vetores | `propriedade mantida a cada passo` | Passou |

### Sistema

Enviam entradas ao executável e conferem a saída. SIS-02 e SIS-03 exigem a saída do enunciado.

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| SIS-01 | menu do enunciado | `opcoes 1 a 5 iguais` | Passou |
| SIS-02 | exibir no exemplo do enunciado | `104, 102, 103, 101` | Passou |
| SIS-03 | atender no exemplo do enunciado | `104, restam 102, 101, 103` | Passou |
| SIS-04 | iniciar com heap vazio | `0 pedidos` | Passou |
| SIS-05 | exibir heap vazio | `avisa` | Passou |
| SIS-06 | cadastrar um pedido | `aparece na lista` | Passou |
| SIS-07 | descricao cadastrada | `devolvida ao atender` | Passou |
| SIS-08 | quantidade apos cadastros | `4` | Passou |
| SIS-09 | quantidade apos atender | `3` | Passou |
| SIS-10 | segundo atendimento | `ID 102` | Passou |
| SIS-11 | terceiro atendimento | `ID 103` | Passou |
| SIS-12 | atender ate esvaziar | `ultimo e o 101` | Passou |
| SIS-13 | atender heap vazio | `avisa` | Passou |
| SIS-14 | cadastrar apos esvaziar | `volta a funcionar` | Passou |
| SIS-15 | estado: vetor | `[8, 5, 3, 2]` | Passou |
| SIS-16 | estado: tamanho e capacidade | `4 e 4` | Passou |
| SIS-17 | estado: capacidade apos crescer | `5 e 8` | Passou |
| SIS-18 | estado: arvore | `a do enunciado` | Passou |
| SIS-19 | estado do heap vazio | `arvore vazia` | Passou |
| SIS-20 | conjunto de exemplo: antes | `[2, 5, 3, 8, 1]` | Passou |
| SIS-21 | conjunto de exemplo: depois | `[8, 5, 3, 2, 1]` | Passou |
| SIS-22 | conjunto digitado | `[9, 1, 4]` | Passou |
| SIS-23 | ID repetido no conjunto | `pede outro` | Passou |
| SIS-24 | cadastrar apos construir | `9 vai a raiz` | Passou |
| SIS-25 | atender apos construir | `Pedido D` | Passou |
| SIS-26 | construir sobre pedidos, confirmando | `substitui` | Passou |
| SIS-27 | construir sobre pedidos, cancelando | `mantem` | Passou |
| SIS-28 | inicializar vazio, confirmando | `0 pedidos` | Passou |
| SIS-29 | inicializar vazio, cancelando | `mantem` | Passou |
| SIS-30 | inicializar heap ja vazio | `sem confirmacao` | Passou |
| SIS-31 | confirmacao invalida | `pede de novo` | Passou |
| SIS-32 | ID repetido no cadastro | `pede outro` | Passou |
| SIS-33 | ID de pedido atendido | `pode reutilizar` | Passou |
| SIS-34 | ID zero | `pede de novo` | Passou |
| SIS-35 | descricao vazia | `pede de novo` | Passou |
| SIS-36 | prioridade com texto | `pede de novo` | Passou |
| SIS-37 | prioridade fora de int | `pede de novo` | Passou |
| SIS-38 | prioridade negativa | `aceita` | Passou |
| SIS-39 | descricao com acentos | `preserva` | Passou |
| SIS-40 | opcao invalida | `avisa e mostra o menu` | Passou |
| SIS-41 | sair | `programa termina` | Passou |
| SIS-42 | fim inesperado da entrada | `encerra sem travar` | Passou |

## Resultados dos testes

| Grupo | Executados | Passaram | Falharam |
|---|:-:|:-:|:-:|
| Índices | 4 | 4 | 0 |
| `inicializar_heap` | 9 | 9 | 0 |
| `inserir` | 11 | 11 | 0 |
| `remover` | 7 | 7 | 0 |
| `max_heapfy` | 8 | 8 | 0 |
| `construir` | 7 | 7 | 0 |
| `print_heap` | 4 | 4 | 0 |
| Exaustivos | 2 | 2 | 0 |
| Sistema | 42 | 42 | 0 |
| **Total** | 94 | 94 | 0 |

Ambiente da execução: macOS 26.5.1 (arm64), Apple clang version 21.0.0 (clang-2100.1.1.101).

<details>
<summary>Saída completa de <code>make test</code></summary>

```text
[OK] IDX-01 pai dos indices 1, 2, 3 e 4 -> 0, 0, 1 e 1
[OK] IDX-02 filho esquerdo de 0 e 3 -> 1 e 7
[OK] IDX-03 filho direito de 0 e 3 -> 2 e 8
[OK] IDX-04 pai de cada filho, 100000 indices -> volta ao proprio indice
[OK] INI-01 heap vazio com capacidade 4 -> tamanho 0, capacidade 4
[OK] INI-02 capacidade zero -> rejeita
[OK] INI-03 capacidade negativa -> rejeita
[OK] INI-04 a partir de um vetor -> [8, 5, 3, 2, 1]
[OK] INI-05 vetor de origem -> nao e alterado
[OK] INI-06 vetor maior que a capacidade -> capacidade sobe para 5
[OK] INI-07 tamanho sem vetor -> rejeita
[OK] INI-08 tamanho negativo -> rejeita
[OK] INI-09 liberar duas vezes -> heap zerado, sem falha
[OK] INS-01 inserir no heap vazio -> fica na raiz
[OK] INS-02 maior de todos -> sobe ate a raiz
[OK] INS-03 menor que o pai -> permanece no final
[OK] INS-04 maior que o pai, menor que a raiz -> sobe um nivel
[OK] INS-05 exemplo do enunciado -> [8, 5, 3, 2, 1]
[OK] INS-06 quinto pedido com capacidade 4 -> capacidade dobra para 8
[OK] INS-07 troca leva o pedido inteiro -> ID e descricao acompanham
[OK] INS-08 prioridade igual a do pai -> nao troca
[OK] INS-09 extremos de int e negativos -> compara sem estouro
[OK] INS-10 100000 pedidos em capacidade 1 -> cresce e continua heap
[OK] INS-11 inserir em heap liberado -> aloca de novo
[OK] REM-01 remover de heap vazio -> falha, destino inalterado
[OK] REM-02 remover o unico -> heap vazio
[OK] REM-03 remover de [8, 5, 3, 2] -> retorna o 8
[OK] REM-04 vetor apos a remocao -> [5, 2, 3], como no enunciado
[OK] REM-05 capacidade apos a remocao -> nao muda
[OK] REM-06 filho direito e o maior -> desce pela direita
[OK] REM-07 inserir apos esvaziar -> volta a funcionar
[OK] MAX-01 esquerdo e o maior -> troca com o esquerdo
[OK] MAX-02 direito e o maior -> troca com o direito
[OK] MAX-03 pai ja e o maior -> nao troca
[OK] MAX-04 precisa descer dois niveis -> [9, 7, 8, 1, 6]
[OK] MAX-05 apenas o filho esquerdo -> troca
[OK] MAX-06 filhos fora do tamanho -> sao ignorados
[OK] MAX-07 prioridades iguais -> nao troca
[OK] MAX-08 chamar em indice interno -> corrige a subarvore
[OK] CON-01 exemplo do enunciado -> [8, 5, 3, 2, 1]
[OK] CON-02 vetor que ja e heap -> nao muda
[OK] CON-03 inserir apos construir -> continua heap
[OK] CON-04 vetor crescente -> [7, 5, 6, 4, 2, 1, 3]
[OK] CON-05 vetor vazio -> continua vazio
[OK] CON-06 um elemento -> nao muda
[OK] CON-07 todos iguais -> nao troca
[OK] PRI-01 heap vazio -> avisa que nao ha pedidos
[OK] PRI-02 um pedido -> ID e prioridade
[OK] PRI-03 varios pedidos -> ordem do vetor
[OK] PRI-04 exibir nao remove -> heap inalterado
[OK] EXA-01 construir os 873612 vetores de ate 7 itens -> atende em ordem decrescente
[OK] EXA-02 inserir um a um os 873612 vetores -> propriedade mantida a cada passo

Unitarios: 52 testes, 0 falhas

[OK] SIS-01 menu do enunciado -> opcoes 1 a 5 iguais
[OK] SIS-02 exibir no exemplo do enunciado -> 104, 102, 103, 101
[OK] SIS-03 atender no exemplo do enunciado -> 104, restam 102, 101, 103
[OK] SIS-04 iniciar com heap vazio -> 0 pedidos
[OK] SIS-05 exibir heap vazio -> avisa
[OK] SIS-06 cadastrar um pedido -> aparece na lista
[OK] SIS-07 descricao cadastrada -> devolvida ao atender
[OK] SIS-08 quantidade apos cadastros -> 4
[OK] SIS-09 quantidade apos atender -> 3
[OK] SIS-10 segundo atendimento -> ID 102
[OK] SIS-11 terceiro atendimento -> ID 103
[OK] SIS-12 atender ate esvaziar -> ultimo e o 101
[OK] SIS-13 atender heap vazio -> avisa
[OK] SIS-14 cadastrar apos esvaziar -> volta a funcionar
[OK] SIS-15 estado: vetor -> [8, 5, 3, 2]
[OK] SIS-16 estado: tamanho e capacidade -> 4 e 4
[OK] SIS-17 estado: capacidade apos crescer -> 5 e 8
[OK] SIS-18 estado: arvore -> a do enunciado
[OK] SIS-19 estado do heap vazio -> arvore vazia
[OK] SIS-20 conjunto de exemplo: antes -> [2, 5, 3, 8, 1]
[OK] SIS-21 conjunto de exemplo: depois -> [8, 5, 3, 2, 1]
[OK] SIS-22 conjunto digitado -> [9, 1, 4]
[OK] SIS-23 ID repetido no conjunto -> pede outro
[OK] SIS-24 cadastrar apos construir -> 9 vai a raiz
[OK] SIS-25 atender apos construir -> Pedido D
[OK] SIS-26 construir sobre pedidos, confirmando -> substitui
[OK] SIS-27 construir sobre pedidos, cancelando -> mantem
[OK] SIS-28 inicializar vazio, confirmando -> 0 pedidos
[OK] SIS-29 inicializar vazio, cancelando -> mantem
[OK] SIS-30 inicializar heap ja vazio -> sem confirmacao
[OK] SIS-31 confirmacao invalida -> pede de novo
[OK] SIS-32 ID repetido no cadastro -> pede outro
[OK] SIS-33 ID de pedido atendido -> pode reutilizar
[OK] SIS-34 ID zero -> pede de novo
[OK] SIS-35 descricao vazia -> pede de novo
[OK] SIS-36 prioridade com texto -> pede de novo
[OK] SIS-37 prioridade fora de int -> pede de novo
[OK] SIS-38 prioridade negativa -> aceita
[OK] SIS-39 descricao com acentos -> preserva
[OK] SIS-40 opcao invalida -> avisa e mostra o menu
[OK] SIS-41 sair -> programa termina
[OK] SIS-42 fim inesperado da entrada -> encerra sem travar

Sistema: 42 testes, 0 falhas
```

</details>
