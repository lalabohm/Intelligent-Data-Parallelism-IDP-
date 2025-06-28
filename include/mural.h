#ifndef MURAL_H
#define MURAL_H

#include "structs.h"
#include <pthread.h>

void *muralDePedidos(void *arg);
void *exibirMuralPeriodicamente(void *arg);
void adicionarPedido(const char *nome, int preparo, int cozimento);

extern pthread_mutex_t mutexPedidos;
extern pthread_mutex_t mutexTela;
extern Pedido *inicio;
extern int totalPedidos;
extern int muralAtivo;
extern int linhaSaida;

#endif
