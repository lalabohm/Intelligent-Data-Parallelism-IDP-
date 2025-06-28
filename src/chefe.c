#include "chefe.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include "structs.h"

extern Pedido *inicio;
extern pthread_mutex_t mutexPedidos;
extern pthread_mutex_t mutexBancadas;
extern pthread_mutex_t mutexTela;
extern int muralAtivo;
extern int linhaSaida;

void *chefeDeCozinha(void *arg)
{
    Tripulante *tripulantes = (Tripulante *)arg;

    while (muralAtivo)
    {
        pthread_mutex_lock(&mutexPedidos);

        if (inicio == NULL)
        {
            pthread_mutex_unlock(&mutexPedidos);
            sleep(1);
            continue;
        }

        Pedido *pedido = inicio;

        pthread_mutex_lock(&mutexTela);
        attron(COLOR_PAIR(3));
        mvprintw(linhaSaida++, 0, "Digite o número do tripulante (1-4) para o prato %s:", pedido->nome);
        refresh();
        attroff(COLOR_PAIR(3));
        pthread_mutex_unlock(&mutexTela);

        int ch = getch();
        int tripulanteID = ch - '0'; // Converte '1' em 1, etc.

        if (tripulanteID < 1 || tripulanteID > 4 || tripulantes[tripulanteID - 1].ocupado)
        {
            pthread_mutex_lock(&mutexTela);
            mvprintw(linhaSaida++, 0, "Tripulante inválido ou ocupado.");
            refresh();
            pthread_mutex_unlock(&mutexTela);
            pthread_mutex_unlock(&mutexPedidos);
            sleep(1);
            continue;
        }

        // Atribui o pedido ao tripulante escolhido
        inicio = inicio->proximo;
        tripulantes[tripulanteID - 1].pedidoAtual = pedido;
        tripulantes[tripulanteID - 1].ocupado = 1;

        pthread_mutex_lock(&mutexTela);
        attron(COLOR_PAIR(2));
        mvprintw(linhaSaida++, 0, "Chefe atribuiu %s ao Tripulante %d", pedido->nome, tripulanteID);
        attroff(COLOR_PAIR(2));
        refresh();
        pthread_mutex_unlock(&mutexTela);

        pthread_mutex_unlock(&mutexPedidos);
        sleep(1);
    }

    return NULL;
}
