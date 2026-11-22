#include "visualizacao.h"

#define TAM_PREFIXO 160 /* 4 colunas por nivel, e um int tem ate 31 niveis */

void imprimir_prioridades(const Pedido *vetor, int n, FILE *saida) {
    int i;

    fprintf(saida, "[");
    for (i = 0; i < n; i++) fprintf(saida, "%s%d", i > 0 ? ", " : "", vetor[i].prioridade);
    fprintf(saida, "]");
}

/* Desenha os filhos de i. E = esquerdo, D = direito. */
static void desenhar_filhos(const Heap *heap, int i, const char *prefixo, FILE *saida) {
    char seguinte[TAM_PREFIXO];
    int filho, ultimo;

    for (filho = esquerdo(i); filho <= direito(i) && filho < heap->tamanho; filho++) {
        ultimo = filho == direito(i) || filho + 1 == heap->tamanho;
        fprintf(saida, "%s%s %c: [%d]\n", prefixo, ultimo ? "`--" : "|--",
                filho == esquerdo(i) ? 'E' : 'D', heap->dados[filho].prioridade);
        snprintf(seguinte, sizeof seguinte, "%s%s", prefixo, ultimo ? "    " : "|   ");
        desenhar_filhos(heap, filho, seguinte, saida);
    }
}

void exibir_estado(const Heap *heap, FILE *saida) {
    int i;

    fprintf(saida, "Estado do heap:\n");
    fprintf(saida, "Tamanho: %d | Capacidade: %d\n", heap->tamanho, heap->capacidade);
    fprintf(saida, "Vetor: ");
    imprimir_prioridades(heap->dados, heap->tamanho, saida);
    fprintf(saida, "\n");

    for (i = 0; i < heap->tamanho; i++) {
        fprintf(saida, "  i=%d", i);
        if (i > 0) fprintf(saida, " (pai %d)", pai(i));
        else fprintf(saida, " (raiz) ");
        fprintf(saida, " | ID %d | Prioridade %d | %s\n",
                heap->dados[i].id, heap->dados[i].prioridade, heap->dados[i].descricao);
    }

    fprintf(saida, "\nÁrvore (E = esquerdo, D = direito):\n");
    if (heap->tamanho == 0) {
        fprintf(saida, "(vazia)\n");
    } else {
        fprintf(saida, "[%d]\n", heap->dados[0].prioridade);
        desenhar_filhos(heap, 0, "", saida);
    }
}
