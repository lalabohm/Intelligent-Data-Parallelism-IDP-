#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "mural.h"

extern pthread_mutex_t mutexPedidos;
extern Pedido *inicio;
extern int muralAtivo;

int totalPedidos = 0;

// Função que adiciona um novo pedido ao mural
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
    totalPedidos++;
    pthread_mutex_unlock(&mutexPedidos);
}

// Thread responsável por adicionar os pedidos ao mural de forma sequencial com atraso
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
// Thread que exibe o conteúdo atual do mural periodicamente.
void *exibirMuralPeriodicamente(void *arg)
{
    while (muralAtivo)
    {
        pthread_mutex_lock(&mutexPedidos);
        Pedido *temp = inicio;

        if (temp != NULL)
        {
            printf("\n---Mural Atual---\n");
            while (temp != NULL)
            {
                printf("Prato: %s | Preparo: %d | Cozimento: %d\n", temp->nome, temp->tempoPreparoIngredientes, temp->tempoCozimento);
                temp = temp->proximo;
            }
        }
        pthread_mutex_unlock(&mutexPedidos);
        sleep(2);
    }
    return NULL;
}