#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>

#include "pedido.h"

#define CAPACIDADE_INICIAL 4

/* Max-Heap em vetor: dados[0] e sempre um pedido de maior prioridade. */
typedef struct {
    Pedido *dados;
    int tamanho;
    int capacidade;
} Heap;

/* Relacoes entre os indices. pai(i) so tem sentido para i > 0. */
int pai(int i);
int esquerdo(int i);
int direito(int i);

/*
 * Cria o heap com a capacidade dada. Se vetor nao for NULL, copia os n pedidos
 * (aumentando a capacidade, se preciso) e chama construir. Para um heap vazio,
 * use vetor = NULL e n = 0. Retorna 1, ou 0 em caso de erro.
 * Se o heap ja estiver em uso, chame liberar_heap antes.
 */
int inicializar_heap(Heap *heap, int capacidade, const Pedido *vetor, int n);

/* Retorna 1, ou 0 se faltar memoria (o heap fica como estava). */
int inserir(Heap *heap, Pedido pedido);

/* Retira o de maior prioridade. Retorna 1, ou 0 se o heap estiver vazio.
 * Se removido nao for NULL, recebe o pedido retirado. */
int remover(Heap *heap, Pedido *removido);

/* Faz o elemento i descer, supondo que as duas subarvores dele ja sao heaps. */
void max_heapfy(Heap *heap, int i);

/* Transforma o vetor atual, em qualquer ordem, em um Max-Heap. */
void construir(Heap *heap);

/* Escreve os pedidos na ordem do vetor. */
void print_heap(const Heap *heap, FILE *saida);

/* Auxiliar exigida pelo C: devolve a memoria do vetor. */
void liberar_heap(Heap *heap);

#endif
