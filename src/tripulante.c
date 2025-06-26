#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "mural.h"
#include "structs.h"

#define TEMPO_BANCADA 2
#define TEMPO_COZINHA 2

extern pthread_mutex_t mutexPedidos;
extern Pedido *inicio;

void *exexutarTripulante(void *arg)
{
    Tripulante *trip = (Tripulante *)arg;

    while (1)
    {
        pthread_mutex_lock(&mutexPedidos);
        if (inicio != NULL)
        {
            Pedido *pedidoAtual = inicio;
            inicio = inicio->proximo;

            trip->pedidoAtual = pedidoAtual;
            trip->ocupado = 1;
            pthread_mutex_unlock(&mutexPedidos);

            printf("Tripulante %d começou o preparo de %s\n", trip->id, pedidoAtual->nome);
            sleep(pedidoAtual->tempoPreparoIngredientes);

            printf("Tripulante %d está cozinhando %s\n", trip->id, pedidoAtual->nome);
            sleep(pedidoAtual->tempoCozimento);

            printf("Tripulante %d finalzou o prato %s\n", trip->id, pedidoAtual->nome);

            free(pedidoAtual);

            trip->pedidoAtual = NULL;
            trip->ocupado = 0;
        }
        else
        {
            pthread_mutex_unlock(&mutexPedidos);
            sleep(1);
        }
    }
    return NULL;
}