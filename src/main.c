#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mural.h"
#include "structs.h"
#include "tripulante.h"

#define NUM_TRIPULANTES 4
#define NUM_BANCADAS 3
#define NUM_COZINHAS 3

pthread_mutex_t mutexPedidos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexBancadas = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCozinhas = PTHREAD_MUTEX_INITIALIZER;

Pedido *inicio = NULL;

Bancada bancadas[NUM_BANCADAS];
Cozinha cozinhas[NUM_COZINHAS];

int main()
{
    pthread_t threadMural;
    Tripulante tripulantes[NUM_TRIPULANTES];
    pthread_t threadsTripulantes[NUM_TRIPULANTES];

    printf("****************************\n");
    printf("BEM-VINDO AO FORA NO ESPAÇO!\n");
    printf("****************************\n");

    for (int i = 0; i < NUM_BANCADAS; i++)
    {
        bancadas[i].id = i + 1;
        bancadas[i].ocupado = 0;
    }

    for (int i = 0; i < NUM_COZINHAS; i++)
    {
        cozinhas[i].id = i + 1;
        cozinhas[i].ocupado = 0;
    }

    if (pthread_create(&threadMural, NULL, muralDePedidos, NULL))
    {
        fprintf(stderr, "Erro ao criar a thread do nural...\n");
        return 1;
    }

    for (int i = 0; i < 4; i++)
    {
        tripulantes[i].id = i + 1;
        tripulantes[i].ocupado = 0;
        tripulantes[i].pedidoAtual = NULL;

        pthread_create(&threadsTripulantes[i], NULL, executarTripulante, &tripulantes[i]);
    }

    pthread_join(threadMural, NULL);

    for (int i = 0; i < NUM_TRIPULANTES; i++)
    {
        pthread_join(threadsTripulantes[i], NULL);
    }

    printf("\nTodos os pedidos foram processados!\n");

    listarPedidos();

    return 0;
}