#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mural.h"
#include "structs.h"

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
    pthread_join(threadMural, NULL);

    listarPedidos();

    return 0;
}