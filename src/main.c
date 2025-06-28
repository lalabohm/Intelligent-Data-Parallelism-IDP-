#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mural.h"
#include "structs.h"
#include "tripulante.h"
#include "chefe.h"

#define NUM_TRIPULANTES 4
#define NUM_BANCADAS 3
#define NUM_COZINHAS 3

pthread_mutex_t mutexPedidos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexBancadas = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCozinhas = PTHREAD_MUTEX_INITIALIZER;

Pedido *inicio = NULL;
int muralAtivo = 1;

Bancada bancadas[NUM_BANCADAS];
Cozinha cozinhas[NUM_COZINHAS];

int main()
{
    pthread_t threadMural;
    pthread_t threadMuralExibicao;
    pthread_t threadChefe;
    pthread_t threadsTripulantes[NUM_TRIPULANTES];
    Tripulante tripulantes[NUM_TRIPULANTES];

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

    for (int i = 0; i < NUM_TRIPULANTES; i++)
    {
        tripulantes[i].id = i + 1;
        tripulantes[i].ocupado = 0;
        tripulantes[i].pedidoAtual = NULL;
    }

    // Criação da thread do chefe, que distribui os pedidos aos tripulantes
    pthread_create(&threadChefe, NULL, chefeDeCozinha, tripulantes);

    // Criação da thread que exibe periodicamente o mural de pedidos
    pthread_create(&threadMuralExibicao, NULL, exibirMuralPeriodicamente, NULL);

    // Criação da thread que gera e adiciona os pedidos no mural
    if (pthread_create(&threadMural, NULL, muralDePedidos, NULL))
    {
        fprintf(stderr, "Erro ao criar a thread do mural...\n");
        return 1;
    }

    for (int i = 0; i < NUM_TRIPULANTES; i++)
    {
        pthread_create(&threadsTripulantes[i], NULL, executarTripulante, &tripulantes[i]);
    }

    pthread_join(threadMural, NULL);

    for (int i = 0; i < NUM_TRIPULANTES; i++)
    {
        pthread_join(threadsTripulantes[i], NULL);
    }

    muralAtivo = 0;
    pthread_join(threadMuralExibicao, NULL);
    pthread_join(threadChefe, NULL);

    printf("\nTodos os pedidos foram processados!\n");

    return 0;
}
