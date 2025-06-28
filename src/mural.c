#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include "mural.h"

extern pthread_mutex_t mutexPedidos;
extern pthread_mutex_t mutexTela;
extern Pedido *inicio;
extern int muralAtivo;

int totalPedidos = 0;

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
            temp = temp->proximo;
        temp->proximo = novo;
    }

    totalPedidos++;

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

void *exibirMuralPeriodicamente(void *arg)
{
    while (muralAtivo)
    {
        pthread_mutex_lock(&mutexPedidos);
        Pedido *temp = inicio;

        pthread_mutex_lock(&mutexTela);
        clear();
        mvprintw(0, 0, "---Mural Atual---");

        int linha = 1;
        while (temp != NULL)
        {
            mvprintw(linha++, 0, "Prato: %s | Preparo: %d | Cozimento: %d",
                     temp->nome, temp->tempoPreparoIngredientes, temp->tempoCozimento);
            temp = temp->proximo;
        }

        refresh();
        pthread_mutex_unlock(&mutexTela);
        pthread_mutex_unlock(&mutexPedidos);

        sleep(2);
    }

    return NULL;
}
