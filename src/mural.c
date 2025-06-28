#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include "mural.h"
#include "structs.h"

extern pthread_mutex_t mutexPedidos;
extern pthread_mutex_t mutexTela;
extern Pedido *inicio;
extern int muralAtivo;
extern int linhaSaida;

int totalPedidos = 0;

// Função para adicionar novo pedido ao final da lista
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

// Função que permite ao chefe inserir pratos manualmente
void *muralDePedidos(void *arg)
{
    char *opcoes[] = {
        "Meteorito Saboroso",
        "Eclipse Energético",
        "Carne Cósmica",
        "Nugget de Supernova"};

    int temposPreparo[] = {5, 4, 6, 3};
    int temposCozimento[] = {3, 2, 3, 2};
    int escolha;

    while (1)
    {
        pthread_mutex_lock(&mutexTela);
        mvprintw(linhaSaida++, 0, "Escolha um prato para adicionar ao mural:");
        for (int j = 0; j < 4; j++)
        {
            mvprintw(linhaSaida++, 0, "%d. %s", j + 1, opcoes[j]);
        }
        mvprintw(linhaSaida++, 0, "5. Sair");
        refresh();
        pthread_mutex_unlock(&mutexTela);

        echo();
        scanw("%d", &escolha);
        noecho();

        if (escolha >= 1 && escolha <= 4)
        {
            adicionarPedido(opcoes[escolha - 1], temposPreparo[escolha - 1], temposCozimento[escolha - 1]);
        }
        else if (escolha == 5)
        {
            break; // Sai do loop
        }

        sleep(1);
    }

    return NULL;
}

// Exibe os pedidos atuais do mural na tela a cada 2 segundos
void *exibirMuralPeriodicamente(void *arg)
{
    while (muralAtivo)
    {
        pthread_mutex_lock(&mutexPedidos);
        Pedido *temp = inicio;

        pthread_mutex_lock(&mutexTela);

        // Limpa as linhas do mural
        for (int i = 0; i < 15; i++)
            mvprintw(4 + i, 0, "                                         ");

        attron(COLOR_PAIR(3)); // Título amarelo
        mvprintw(4, 0, "--- Mural Atual ---");
        attroff(COLOR_PAIR(3));

        int linha = 5;
        while (temp != NULL)
        {
            attron(COLOR_PAIR(2)); // Pratos em rosa (magenta)
            mvprintw(linha++, 0, "Prato: %s | Preparo: %d | Cozimento: %d",
                     temp->nome, temp->tempoPreparoIngredientes, temp->tempoCozimento);
            attroff(COLOR_PAIR(2));
            temp = temp->proximo;
        }

        refresh();
        pthread_mutex_unlock(&mutexTela);
        pthread_mutex_unlock(&mutexPedidos);

        sleep(2);
    }

    return NULL;
}
