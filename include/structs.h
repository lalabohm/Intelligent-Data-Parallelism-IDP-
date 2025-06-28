#ifndef STRUCTS_H
#define STRUCTS_H

#include <pthread.h>

typedef struct Pedido
{
    char nome[50];
    int tempoPreparoIngredientes;
    int tempoCozimento;
    struct Pedido *proximo;
} Pedido;

typedef struct
{
    int id;
    int ocupado;
    Pedido *pedidoAtual;
} Tripulante;

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

extern pthread_mutex_t mutexPedidos;
extern pthread_mutex_t mutexBancadas;
extern pthread_mutex_t mutexCozinhas;
extern pthread_mutex_t mutexTela;
extern pthread_mutex_t mutexComando;
extern pthread_mutex_t mutexLog;
extern pthread_cond_t condBancadas;
extern pthread_cond_t condCozinhas;

extern Pedido *inicio;
extern int muralAtivo;
extern int comando_tripulante_id;

void adicionar_log(const char *mensagem);

#endif // STRUCTS_H