#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <locale.h>
#include <ncurses.h>
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
pthread_mutex_t mutexTela = PTHREAD_MUTEX_INITIALIZER;

Pedido *inicio = NULL;
int muralAtivo = 1;
int linhaSaida = 15;

Bancada bancadas[NUM_BANCADAS];
Cozinha cozinhas[NUM_COZINHAS];

int main()
{
    setlocale(LC_ALL, "pt_BR.UTF-8");

    // Threads e tripulantes
    pthread_t threadMural;
    pthread_t threadMuralExibicao;
    pthread_t threadChefe;
    pthread_t threadsTripulantes[NUM_TRIPULANTES];
    Tripulante tripulantes[NUM_TRIPULANTES];

    // Inicialização do ncurses com cores
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);

    noecho();
    curs_set(FALSE);
    clear();
    refresh();

    pthread_mutex_lock(&mutexTela);

    attron(COLOR_PAIR(1));
    mvprintw(0, 0, "****************************");
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    mvprintw(1, 0, "BEM-VINDO AO FORA NO ESPAÇO!");
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(1));
    mvprintw(2, 0, "****************************");
    attroff(COLOR_PAIR(1));

    refresh();
    pthread_mutex_unlock(&mutexTela);

    // Inicializa bancadas e cozinhas
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

    // Cria threads principais
    pthread_create(&threadChefe, NULL, chefeDeCozinha, tripulantes);
    pthread_create(&threadMuralExibicao, NULL, exibirMuralPeriodicamente, NULL);
    pthread_create(&threadMural, NULL, muralDePedidos, NULL);

    // Cria threads dos tripulantes
    for (int i = 0; i < NUM_TRIPULANTES; i++)
    {
        pthread_create(&threadsTripulantes[i], NULL, executarTripulante, &tripulantes[i]);
    }

    // Aguarda término das threads
    pthread_join(threadMural, NULL);
    for (int i = 0; i < NUM_TRIPULANTES; i++)
    {
        pthread_join(threadsTripulantes[i], NULL);
    }

    muralAtivo = 0;
    pthread_join(threadMuralExibicao, NULL);
    pthread_join(threadChefe, NULL);

    pthread_mutex_lock(&mutexTela);
    attron(COLOR_PAIR(3));
    mvprintw(linhaSaida++, 0, "Todos os pedidos foram processados!");
    attroff(COLOR_PAIR(3));
    refresh();
    pthread_mutex_unlock(&mutexTela);

    getch();
    endwin();

    return 0;
}
