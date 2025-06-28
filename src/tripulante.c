#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include "mural.h"
#include "structs.h"

extern pthread_mutex_t mutexPedidos;
extern pthread_mutex_t mutexBancadas;
extern pthread_mutex_t mutexCozinhas;
extern pthread_mutex_t mutexTela;
extern int linhaSaida;
extern Bancada bancadas[];
extern Cozinha cozinhas[];
extern Pedido *inicio;

void *executarTripulante(void *arg)
{
    Tripulante *trip = (Tripulante *)arg;

    while (1)
    {
        if (trip->pedidoAtual == NULL)
        {
            sleep(1);
            continue;
        }

        Pedido *pedidoAtual = trip->pedidoAtual;

        // Busca por bancada livre
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
            pthread_mutex_lock(&mutexTela);
            attron(COLOR_PAIR(2));
            mvprintw(linhaSaida++, 0, "Tripulante %d começou o preparo do prato %s na bancada %d",
                     trip->id, pedidoAtual->nome, bancadaUsada);
            attroff(COLOR_PAIR(2));
            refresh();
            pthread_mutex_unlock(&mutexTela);

            sleep(pedidoAtual->tempoPreparoIngredientes);

            pthread_mutex_lock(&mutexBancadas);
            bancadas[bancadaUsada - 1].ocupado = 0;
            pthread_mutex_unlock(&mutexBancadas);
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
            pthread_mutex_lock(&mutexTela);
            attron(COLOR_PAIR(4));
            mvprintw(linhaSaida++, 0, "Tripulante %d está cozinhando o prato %s na cozinha %d",
                     trip->id, pedidoAtual->nome, cozinhaUsada);
            attroff(COLOR_PAIR(4));
            refresh();
            pthread_mutex_unlock(&mutexTela);

            sleep(pedidoAtual->tempoCozimento);

            pthread_mutex_lock(&mutexCozinhas);
            cozinhas[cozinhaUsada - 1].ocupado = 0;
            pthread_mutex_unlock(&mutexCozinhas);
        }

        pthread_mutex_lock(&mutexTela);
        mvprintw(linhaSaida++, 0, "Tripulante %d finalizou o prato %s",
                 trip->id, pedidoAtual->nome);
        refresh();
        pthread_mutex_unlock(&mutexTela);

        free(pedidoAtual);
        trip->pedidoAtual = NULL;
        trip->ocupado = 0;

        // Verifica se há mais pedidos no mural
        pthread_mutex_lock(&mutexPedidos);
        if (inicio == NULL)
        {
            pthread_mutex_unlock(&mutexPedidos);
            break;
        }
        pthread_mutex_unlock(&mutexPedidos);
    }

    pthread_mutex_lock(&mutexTela);
    mvprintw(linhaSaida++, 0, "Tripulante %d concluiu todos os pedidos e está descansando!", trip->id);
    refresh();
    pthread_mutex_unlock(&mutexTela);

    return NULL;
}
