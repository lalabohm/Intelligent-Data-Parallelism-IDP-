#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "structs.h"

extern pthread_mutex_t mutexBancadas;
extern pthread_mutex_t mutexCozinhas;
extern pthread_cond_t condBancadas;
extern pthread_cond_t condCozinhas;

extern Bancada bancadas[];
extern Cozinha cozinhas[];
extern int muralAtivo;
extern void adicionar_log(const char *mensagem);

void *executarTripulante(void *arg)
{
    Tripulante *trip = (Tripulante *)arg;
    char buffer_log[128];

    while (muralAtivo)
    {
        // Espera por um pedido ser atribuído pelo chefe
        if (trip->pedidoAtual == NULL)
        {
            sleep(1);
            continue;
        }

        Pedido *pedidoAtual = trip->pedidoAtual;

        int bancadaIdx = -1;

        pthread_mutex_lock(&mutexBancadas);
        while (muralAtivo) // Loop para esperar pela condição
        {
            // Procura por uma bancada livre
            for (int i = 0; i < 3; i++)
            { // NUM_BANCADAS
                if (!bancadas[i].ocupado)
                {
                    bancadas[i].ocupado = 1;
                    bancadaIdx = i;
                    break;
                }
            }

            if (bancadaIdx != -1)
            {
                break; // Conseguiu a bancada, sai do loop de espera
            }

            // Se não encontrou, espera ser sinalizado (acordado)
            pthread_cond_wait(&condBancadas, &mutexBancadas);
        }
        pthread_mutex_unlock(&mutexBancadas);

        if (!muralAtivo)
            break; // Verifica se a simulação acabou enquanto esperava

        // Prepara o prato
        sprintf(buffer_log, "Trip %d prepara '%s' na bancada %d", trip->id, pedidoAtual->nome, bancadas[bancadaIdx].id);
        adicionar_log(buffer_log);
        sleep(pedidoAtual->tempoPreparoIngredientes);

        // Libera a bancada e acorda uma thread que esteja esperando
        pthread_mutex_lock(&mutexBancadas);
        bancadas[bancadaIdx].ocupado = 0;
        pthread_cond_signal(&condBancadas);
        pthread_mutex_unlock(&mutexBancadas);

        int cozinhaIdx = -1;

        pthread_mutex_lock(&mutexCozinhas);
        while (muralAtivo) // Loop para esperar
        {
            for (int i = 0; i < 3; i++)
            {
                if (!cozinhas[i].ocupado)
                {
                    cozinhas[i].ocupado = 1;
                    cozinhaIdx = i;
                    break;
                }
            }
            if (cozinhaIdx != -1)
            {
                break;
            }
            pthread_cond_wait(&condCozinhas, &mutexCozinhas);
        }
        pthread_mutex_unlock(&mutexCozinhas);

        if (!muralAtivo)
            break;

        sprintf(buffer_log, "Trip %d cozinha '%s' na cozinha %d", trip->id, pedidoAtual->nome, cozinhas[cozinhaIdx].id);
        adicionar_log(buffer_log);
        sleep(pedidoAtual->tempoCozimento);

        // Libera a cozinha e acorda outra thread
        pthread_mutex_lock(&mutexCozinhas);
        cozinhas[cozinhaIdx].ocupado = 0;
        pthread_cond_signal(&condCozinhas); // Acorda UMA thread
        pthread_mutex_unlock(&mutexCozinhas);

        sprintf(buffer_log, "Trip %d FINALIZOU o prato '%s'!", trip->id, pedidoAtual->nome);
        adicionar_log(buffer_log);

        free(pedidoAtual);
        trip->pedidoAtual = NULL;
        trip->ocupado = 0;
    }

    sprintf(buffer_log, "Tripulante %d encerrou o expediente.", trip->id);
    adicionar_log(buffer_log);

    return NULL;
}