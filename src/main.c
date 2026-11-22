/* Interface de terminal do sistema de pedidos. */
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "heap.h"
#include "visualizacao.h"

#define TAM_LINHA 256
#define MAX_CONJUNTO 1000 /* so limita o que se digita de uma vez, nao o heap */

static Heap heap;

/* Conjunto que esta sendo digitado, guardado aqui para ser liberado ao sair. */
static Pedido *conjunto;

static void ao_sair(void) {
    liberar_heap(&heap);
    free(conjunto);
}

/* ---------- Entrada ---------- */

/* Le uma linha, sem espacos nas pontas. O fim da entrada encerra o sistema. */
static void ler_linha(const char *pergunta, char linha[TAM_LINHA]) {
    size_t inicio, fim;
    int c;

    for (;;) {
        printf("%s", pergunta);
        fflush(stdout);
        if (fgets(linha, TAM_LINHA, stdin) == NULL) {
            printf("\nFim da entrada.\nSistema encerrado.\n");
            exit(0); /* ao_sair e chamada pelo atexit */
        }
        if (strchr(linha, '\n') != NULL || feof(stdin)) break;

        while ((c = getchar()) != '\n' && c != EOF) {
            /* descarta o restante da linha */
        }
        printf("Entrada muito longa.\n");
    }
    inicio = strspn(linha, " \t\r\n");
    fim = strlen(linha);
    while (fim > inicio && strchr(" \t\r\n", linha[fim - 1]) != NULL) fim--;
    memmove(linha, linha + inicio, fim - inicio);
    linha[fim - inicio] = '\0';
}

static int ler_inteiro(const char *pergunta, int minimo, int maximo) {
    char linha[TAM_LINHA], *fim;
    long valor;

    for (;;) {
        ler_linha(pergunta, linha);
        errno = 0;
        valor = strtol(linha, &fim, 10);
        if (errno == 0 && fim != linha && *fim == '\0' && valor >= minimo && valor <= maximo)
            return (int)valor;
        printf("Digite um número inteiro entre %d e %d.\n", minimo, maximo);
    }
}

static int confirmar(const char *pergunta) {
    char linha[TAM_LINHA];

    for (;;) {
        ler_linha(pergunta, linha);
        if (strcmp(linha, "s") == 0 || strcmp(linha, "S") == 0) return 1;
        if (strcmp(linha, "n") == 0 || strcmp(linha, "N") == 0) return 0;
        printf("Responda com s ou n.\n");
    }
}

/* Um heap nao e ordenado por ID, entao a busca e linear. */
static int id_existe(const Pedido *vetor, int n, int id) {
    int i;

    for (i = 0; i < n; i++) {
        if (vetor[i].id == id) return 1;
    }
    return 0;
}

/* Le um pedido com ID que ainda nao esta entre os n pedidos do vetor. */
static Pedido ler_pedido(const Pedido *vetor, int n) {
    Pedido pedido;
    char linha[TAM_LINHA];

    for (;;) {
        pedido.id = ler_inteiro("ID: ", 1, INT_MAX);
        if (!id_existe(vetor, n, pedido.id)) break;
        printf("Já existe um pedido com ID %d.\n", pedido.id);
    }
    for (;;) {
        ler_linha("Descrição: ", linha);
        if (linha[0] == '\0') {
            printf("A descrição não pode ficar vazia.\n");
        } else if (strlen(linha) >= TAM_DESCRICAO) {
            printf("Descrição muito longa (máximo de %d bytes).\n", TAM_DESCRICAO - 1);
        } else {
            strcpy(pedido.descricao, linha);
            break;
        }
    }
    pedido.prioridade = ler_inteiro("Prioridade: ", INT_MIN, INT_MAX);
    return pedido;
}

/* ---------- Opcoes do menu ---------- */

static void cadastrar(void) {
    Pedido pedido = ler_pedido(heap.dados, heap.tamanho);

    if (!inserir(&heap, pedido)) printf("Erro: memória insuficiente.\n");
}

static void atender(void) {
    Pedido pedido;

    if (!remover(&heap, &pedido)) {
        printf("Não há pedidos para atender.\n");
        return;
    }
    printf("Pedido atendido:\n");
    printf("ID: %d\nDescrição: %s\nPrioridade: %d\n", pedido.id, pedido.descricao, pedido.prioridade);
    printf("\nPedidos restantes:\n");
    print_heap(&heap, stdout);
}

/* O heap atual so e descartado depois que o novo foi criado. */
static void substituir_heap(const Pedido *vetor, int n) {
    Heap novo;

    if (!inicializar_heap(&novo, CAPACIDADE_INICIAL, vetor, n)) {
        printf("Erro: memória insuficiente.\n");
        return;
    }
    liberar_heap(&heap);
    heap = novo;

    if (vetor == NULL) {
        printf("Heap vazio inicializado.\n");
    } else {
        printf("\nVetor recebido: ");
        imprimir_prioridades(vetor, n, stdout);
        printf("\nHeap construído: ");
        imprimir_prioridades(heap.dados, heap.tamanho, stdout);
        printf("\n");
    }
}

static int pode_descartar(void) {
    return heap.tamanho == 0 || confirmar("Os pedidos atuais serão descartados. Continuar? (s/n): ");
}

static void construir_de_conjunto(void) {
    static const Pedido exemplo[] = {
        {1, "Pedido A", 2}, {2, "Pedido B", 5}, {3, "Pedido C", 3},
        {4, "Pedido D", 8}, {5, "Pedido E", 1}
    };
    int n, i;

    if (!pode_descartar()) return;

    if (ler_inteiro("Conjunto inicial (1 = exemplo do enunciado, 2 = digitar): ", 1, 2) == 1) {
        substituir_heap(exemplo, (int)(sizeof exemplo / sizeof exemplo[0]));
        return;
    }
    n = ler_inteiro("Quantidade de pedidos: ", 1, MAX_CONJUNTO);
    conjunto = malloc((size_t)n * sizeof *conjunto);
    if (conjunto == NULL) {
        printf("Erro: memória insuficiente.\n");
        return;
    }
    for (i = 0; i < n; i++) {
        printf("\nPedido %d de %d\n", i + 1, n);
        conjunto[i] = ler_pedido(conjunto, i);
    }
    substituir_heap(conjunto, n);
    free(conjunto);
    conjunto = NULL;
}

static void mostrar_menu(void) {
    printf("===== SISTEMA DE PEDIDOS =====\n\n"
           "1 - Cadastrar pedido\n"
           "2 - Atender pedido\n"
           "3 - Exibir pedidos\n"
           "4 - Exibir quantidade de pedidos\n"
           "5 - Sair\n"
           "6 - Exibir estado do heap\n"
           "7 - Construir heap a partir de um conjunto\n"
           "8 - Inicializar heap vazio\n");
}

int main(void) {
    char linha[TAM_LINHA];
    int opcao;

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    if (!inicializar_heap(&heap, CAPACIDADE_INICIAL, NULL, 0)) {
        printf("Erro: memória insuficiente.\n");
        return 1;
    }
    atexit(ao_sair);
    mostrar_menu();

    /* Como no enunciado, o menu aparece uma vez. Enter vazio mostra de novo. */
    for (;;) {
        ler_linha("\nEscolha: ", linha);
        printf("\n");
        opcao = strlen(linha) == 1 ? linha[0] - '0' : -1;

        switch (opcao) {
            case 1: cadastrar(); break;
            case 2: atender(); break;
            case 3:
                printf("Pedidos:\n");
                print_heap(&heap, stdout);
                break;
            case 4: printf("Pedidos aguardando atendimento: %d\n", heap.tamanho); break;
            case 5: printf("Sistema encerrado.\n"); return 0;
            case 6: exibir_estado(&heap, stdout); break;
            case 7: construir_de_conjunto(); break;
            case 8:
                if (pode_descartar()) substituir_heap(NULL, 0);
                break;
            default:
                if (linha[0] != '\0') printf("Opção inválida.\n\n");
                mostrar_menu();
        }
    }
}
