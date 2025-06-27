#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "mural.h"
#include "structs.h"

extern pthread_mutex_t mutexPedidos;
extern pthread_mutex_t mutexBancadas;
extern pthread_mutex_t mutexCozinhas;
extern Pedido *inicio;
extern Bancada bancadas[];
extern Cozinha cozinhas[];

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

        int bancadaUsada = -1;
        pthread_mutex_lock(&mutexBancadas);
        for (int i = 0; i < 3; i++)
        {
            if (bancadas[i].ocupado == 0)
            {
                bancadas[i].ocupado = 1;
                bancadaUsada = bancadas[i].id;
                break;
            }
        }
        pthread_mutex_unlock(&mutexBancadas);

        if (bancadaUsada != -1)
        {
            printf("Tripulante %d começou o preparo do prato %s na bancada %d\n", trip->id, pedidoAtual->nome, bancadaUsada);
            sleep(pedidoAtual->tempoPreparoIngredientes);

            pthread_mutex_lock(&mutexBancadas);
            bancadas[bancadaUsada - 1].ocupado = 0;
            pthread_mutex_unlock(&mutexBancadas);
        }
        else
        {
            printf("Tripulante %d não encontrou bancada disponivel, e está esperando...\n", trip->id);
            sleep(1);
            continue;
        }

        int cozinhaUsada = -1;
        pthread_mutex_lock(&mutexCozinhas);
        for (int i = 0; i < 3; i++)
        {
            if (cozinhas[i].ocupado == 0)
            {
                cozinhas[i].ocupado = 1;
                cozinhaUsada = cozinhas[i].id;
                break;
            }
        }

        pthread_mutex_unlock(&mutexCozinhas);

        if (cozinhaUsada != -1)
        {
            printf("Tripulante %d está cozinhando o prato %s na cozinha %d\n", trip->id, pedidoAtual->nome, cozinhaUsada);
            sleep(pedidoAtual->tempoCozimento);

            pthread_mutex_lock(&mutexCozinhas);
            cozinhas[cozinhaUsada - 1].ocupado = 0;
            pthread_mutex_unlock(&mutexCozinhas);
        }
        else
        {
            printf("Tripulante %d não encontrou cozinha disponivel, e está esperando...\n", trip->id);
            sleep(1);
            continue;
        }

        printf("Tripulante %d finalizou o prato %s\n", trip->id, pedidoAtual->nome);

        free(pedidoAtual);

        trip->pedidoAtual = NULL;
        trip->ocupado = 0;
    }

    printf("Tripulante %d concluiu todos os pedidos e está descansando!\n", trip->id);
    return NULL;
}