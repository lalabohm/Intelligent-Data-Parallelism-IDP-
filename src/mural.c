#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "mural.h"

void adicionarPedido(const char *nome, int preparo, int cozimento)
{
    Pedido *novo = (Pedido *)malloc(sizeof(Pedido));
    strcpy(novo->nome, nome);
    novo->tempoPreparoIngredientes = preparo;
    novo->tempoCozimento = cozimento;
    novo->proximo = NULL;

    pthread_mutex_lock(&mutexPedidos);

    if (inicio == NULL)
    {
        inicio = novo;
    }
    else
    {
        Pedido *temp = inicio;
        while (temp->proximo != NULL)
            temp = temp->proximo;
        temp->proximo = novo;
    }
    pthread_mutex_unlock(&mutexPedidos);
}

void listarPedidos()
{
    pthread_mutex_lock(&mutexPedidos);
    Pedido *temp = inicio;
    printf("***Pedidos no mural***:\n");
    while (temp != NULL)
    {
        printf("Nome do prato: %s - Preparo: %d - Cozimento: %d\n", temp->nome, temp->tempoPreparoIngredientes, temp->tempoCozimento);
        temp = temp->proximo;
    }
    pthread_mutex_unlock(&mutexPedidos);
}

void *muralDePedidos(void *arg)
{
    adicionarPedido("Meteorito Saboroso", 5, 3);
    sleep(1);
    adicionarPedido("Eclipse Energético", 4, 2);
    sleep(1);
    adicionarPedido("Carne Cósmica", 6, 3);
    sleep(1);
    adicionarPedido("Nugget de Supernova", 3, 2);
    return NULL;
}