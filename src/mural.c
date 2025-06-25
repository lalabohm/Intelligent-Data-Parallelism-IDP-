#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "mural.h"

Pedido *inicio = NULL;
pthread_mutex_t mutexPedidos;

void adicionarPedido(const char *nome, int preparo, int cozimento)
{
    Pedido *novo = malloc(sizeof(Pedido));
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
        {
            temp = temp->proximo;
        }
        temp->proximo = novo;
    }
    pthread_mutex_unlock(&mutexPedidos);
}

void *muralDePedidos(void *arg)
{
    char *nomesPratos[] = {"Hamburguer", "Pizza", "Suco", "Salada"};
    int temposPreparo[] = {5, 6, 2, 3};
    int temposCozimento[] = {5, 7, 2, 3};

    while (1)
    {
        int r = rand() % 4;
        adicionarPedido(nomesPratos[r], temposPreparo[r], temposCozimento[r]);
        printf("🛸 Novo pedido: %s 🍽️\n", nomesPratos[r]);
        sleep(5);
    }
    pthread_exit(NULL);
}

void listarPedidos()
{
    pthread_mutex_lock(&mutexPedidos);
    Pedido *temp = inicio;
    printf("\n📋 Pedidos Atuais:\n");
    while (temp != NULL)
    {
        printf("➡️ %s (Preparo: %ds | Cozimento: %ds)\n",
               temp->nome, temp->tempoPreparoIngredientes, temp->tempoCozimento);
        temp = temp->proximo;
    }
    pthread_mutex_unlock(&mutexPedidos);
}
