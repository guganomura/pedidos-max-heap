#ifndef VISUALIZACAO_H
#define VISUALIZACAO_H

#include "heap.h"

/* [8, 5, 3, 2, 1] */
void imprimir_prioridades(const Pedido *vetor, int n, FILE *saida);

/* Estado interno: tamanho, capacidade, vetor, indices e arvore. */
void exibir_estado(const Heap *heap, FILE *saida);

#endif
