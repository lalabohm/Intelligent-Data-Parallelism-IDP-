#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct pedido
{
    char nome[50];
    int tempoPreparoIngredientes;
    int tempoCozimento;
    struct pedido *proximo;
} Pedido;

typedef struct
{
    int id;
    int ocupado;
} Bancada;

typedef struct
{
    int id;
    int ocupado;
} Cozinha;

typedef struct
{
    int id;
    int ocupado;
    Pedido *pedidoAtual;
} Tripulante;

#endif
