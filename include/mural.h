#ifndef MURAL_H
#define MURAL_H

#include "structs.h"

void *muralDePedidos(void *arg);
void *exibirMuralPeriodicamente(void *arg);
void adicionarPedido(const char *nome, int preparo, int cozimento);
void listarPedidos();

extern pthread_mutex_t mutexPedidos;
extern Pedido *inicio;
extern int totalPedidos;

#endif
