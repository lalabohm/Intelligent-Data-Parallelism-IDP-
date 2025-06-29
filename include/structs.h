#ifndef STRUCTS_H
#define STRUCTS_H

#include <pthread.h>

// --- Estruturas de Dados ---

typedef struct
{
    int tripulante_id;
    char prato_id;
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

// --- Declarações Globais (extern) ---

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
extern Comando comando_atual;

extern Bancada bancadas[];
extern Cozinha cozinhas[];
extern Tripulante tripulantes[];

extern char mensagens_log[][128];
extern int num_logs;

// --- Protótipos de Funções Globais ---

void adicionar_log(const char *mensagem);

#endif