#ifndef STRUCTS_H
#define STRUCTS_H

#include <pthread.h>

typedef struct
{
    int tripulante_id; // Qual tripulante (1-4)
    char prato_id;     // Qual prato ('a', 'b', 'c'...)
} Comando;

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

tro lugar(em main.c).

    // Mutexes e Variáveis de Condição
    extern pthread_mutex_t mutexPedidos;
extern pthread_mutex_t mutexBancadas;
extern pthread_mutex_t mutexCozinhas;
extern pthread_mutex_t mutexTela;
extern pthread_mutex_t mutexComando;
extern pthread_mutex_t mutexLog;
extern pthread_cond_t condBancadas;
extern pthread_cond_t condCozinhas;

// Estado da Simulação
extern Pedido *inicio;
extern int muralAtivo;
extern Comando comando_atual;

// Arrays de Recursos
extern Bancada bancadas[];
extern Cozinha cozinhas[];
extern Tripulante tripulantes[];

// Sistema de Log
extern char mensagens_log[][128];
extern int num_logs;

void adicionar_log(const char *mensagem);

#endif