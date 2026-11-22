#ifndef PEDIDO_H
#define PEDIDO_H

#define TAM_DESCRICAO 128

typedef struct {
    int id;
    char descricao[TAM_DESCRICAO];
    int prioridade; /* quanto maior, mais urgente */
} Pedido;

#endif
