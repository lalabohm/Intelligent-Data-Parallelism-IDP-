#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "mural.h"
#include "structs.h"

extern pthread_mutex_t mutexPedidos;
extern Pedido *inicio;

void *executarTripulante(void *arg)
{
    Tripulante *trip = (Tripulante *)arg;

    while (1)
    {
        pthread_mutex_lock(&mutexPedidos);

        if (inicio == NULL)
        {
            pthread_mutex_unlock(&mutexPedidos);
            break;
        }

        Pedido *pedidoAtual = inicio;
        inicio = inicio->proximo;

        pthread_mutex_unlock(&mutexPedidos);

        trip->pedidoAtual = pedidoAtual;
        trip->ocupado = 1;

        printf("Tripulante %d começou o preparo de %s\n", trip->id, pedidoAtual->nome);
        sleep(pedidoAtual->tempoPreparoIngredientes);

        printf("Tripulante %d está cozinhando %s\n", trip->id, pedidoAtual->nome);
        sleep(pedidoAtual->tempoCozimento);

        printf("Tripulante %d finalzou o prato %s\n", trip->id, pedidoAtual->nome);

        free(pedidoAtual);

        trip->pedidoAtual = NULL;
        trip->ocupado = 0;
    }

    printf("Tripulante %d concluiu todos os pedidos e está descansando!\n", trip->id);
    return NULL;
}