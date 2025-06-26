#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mural.h"
#include "structs.h"
#include "tripulante.h"

pthread_mutex_t mutexPedidos = PTHREAD_MUTEX_INITIALIZER;
Pedido *inicio = NULL;

int main()
{
    pthread_t threadMural;

    printf("****************************\n");
    printf("BEM-VINDO AO FORA NO ESPAÇO!\n");
    printf("****************************\n");

    if (pthread_create(&threadMural, NULL, muralDePedidos, NULL))
    {
        fprintf(stderr, "Erro ao criar a thread do nural...\n");
        return 1;
    }

    tripulante tripulantes[4];
    pthread_t threadsTripulantes[4];

    for (int i = 0; i < 4; i++)
    {
        tripulantes[i].id = i + 1;
        tripulantes[i].ocupado = 0;
        tripulantes[i].pedidoAtual = NULL;

        pthread_create(&threadsTripulantes[i], NULL, executarTripulante, &tripulantes[i]);
    }

    pthread_join(threadMural, NULL);

    listarPedidos();

    return 0;
}