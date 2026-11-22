#include "heap.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

int pai(int i) {
    return (i - 1) / 2;
}

int esquerdo(int i) {
    return 2 * i + 1;
}

int direito(int i) {
    return 2 * i + 2;
}

static void trocar(Pedido *a, Pedido *b) {
    Pedido temporario = *a;
    *a = *b;
    *b = temporario;
}

/* Dobra a capacidade. Em caso de falha, o vetor antigo continua valido. */
static int aumentar_capacidade(Heap *heap) {
    Pedido *novo;
    int nova;

    if (heap->capacidade > INT_MAX / 2) return 0;
    nova = heap->capacidade > 0 ? heap->capacidade * 2 : CAPACIDADE_INICIAL;
    if ((size_t)nova > SIZE_MAX / sizeof *novo) return 0;

    novo = realloc(heap->dados, (size_t)nova * sizeof *novo);
    if (novo == NULL) return 0;

    heap->dados = novo;
    heap->capacidade = nova;
    return 1;
}

int inicializar_heap(Heap *heap, int capacidade, const Pedido *vetor, int n) {
    int i;

    heap->dados = NULL;
    heap->tamanho = 0;
    heap->capacidade = 0;

    if (capacidade <= 0 || n < 0 || (vetor == NULL && n > 0)) return 0;
    if (capacidade < n) capacidade = n;
    if ((size_t)capacidade > SIZE_MAX / sizeof *heap->dados) return 0;

    heap->dados = malloc((size_t)capacidade * sizeof *heap->dados);
    if (heap->dados == NULL) return 0;
    heap->capacidade = capacidade;

    for (i = 0; i < n; i++) heap->dados[i] = vetor[i];
    heap->tamanho = n;
    construir(heap);
    return 1;
}

int inserir(Heap *heap, Pedido pedido) {
    int i;

    if (heap->tamanho == heap->capacidade && !aumentar_capacidade(heap)) return 0;

    /* 1. entra no final do vetor */
    i = heap->tamanho;
    heap->dados[i] = pedido;
    heap->tamanho++;

    /* 2. sobe enquanto for maior que o pai */
    while (i > 0 && heap->dados[i].prioridade > heap->dados[pai(i)].prioridade) {
        trocar(&heap->dados[i], &heap->dados[pai(i)]);
        i = pai(i);
    }
    return 1;
}

int remover(Heap *heap, Pedido *removido) {
    if (heap->tamanho == 0) return 0;

    if (removido != NULL) *removido = heap->dados[0];  /* 1. guarda a raiz */
    heap->dados[0] = heap->dados[heap->tamanho - 1];   /* 2. ultimo vira raiz */
    heap->tamanho--;
    max_heapfy(heap, 0);                               /* 3. nova raiz desce */
    return 1;
}

void max_heapfy(Heap *heap, int i) {
    int e = esquerdo(i);
    int d = direito(i);
    int maior = i;

    if (e < heap->tamanho && heap->dados[e].prioridade > heap->dados[maior].prioridade)
        maior = e;
    if (d < heap->tamanho && heap->dados[d].prioridade > heap->dados[maior].prioridade)
        maior = d;

    if (maior != i) {
        trocar(&heap->dados[i], &heap->dados[maior]);
        max_heapfy(heap, maior); /* continua descendo */
    }
}

void construir(Heap *heap) {
    int i;

    /* De tamanho / 2 em diante so ha folhas, que ja sao heaps. */
    for (i = heap->tamanho / 2 - 1; i >= 0; i--) {
        max_heapfy(heap, i);
    }
}

void print_heap(const Heap *heap, FILE *saida) {
    int i;

    if (heap->tamanho == 0) fprintf(saida, "Nenhum pedido aguardando atendimento.\n");
    for (i = 0; i < heap->tamanho; i++) {
        fprintf(saida, "ID: %d | Prioridade: %d\n", heap->dados[i].id, heap->dados[i].prioridade);
    }
}

void liberar_heap(Heap *heap) {
    free(heap->dados);
    heap->dados = NULL;
    heap->tamanho = 0;
    heap->capacidade = 0;
}
