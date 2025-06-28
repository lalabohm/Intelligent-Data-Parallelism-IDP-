#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
int linhaSaida = 6;

Bancada bancadas[NUM_BANCADAS];
Cozinha cozinhas[NUM_COZINHAS];

int main()
{
    setlocale(LC_ALL, "pt_BR.UTF-8");

    pthread_t threadMuralExibicao, threadChefe;
    pthread_t threadsTripulantes[NUM_TRIPULANTES];
    Tripulante tripulantes[NUM_TRIPULANTES];

    // Inicializa ncurses com suporte a cores
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);   // branco
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK); // rosa
    init_pair(3, COLOR_RED, COLOR_BLACK);     // vermelho
    init_pair(4, COLOR_CYAN, COLOR_BLACK);    // azul

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

    attron(COLOR_PAIR(4));
    mvprintw(4, 0, "Digite os pedidos no terminal (fora do ncurses). Escreva 'sair' para encerrar.");
    attroff(COLOR_PAIR(4));
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

    // Cria threads
    pthread_create(&threadChefe, NULL, chefeDeCozinha, tripulantes);
    pthread_create(&threadMuralExibicao, NULL, exibirMuralPeriodicamente, NULL);

    // Recebe pedidos no terminal (fora do ncurses)
    char nome[50];
    int preparo, cozimento;
    while (1)
    {
        printf("Nome do prato (ou 'sair'): ");
        scanf("%s", nome);
        if (strcmp(nome, "sair") == 0)
            break;
        printf("Tempo de preparo: ");
        scanf("%d", &preparo);
        printf("Tempo de cozimento: ");
        scanf("%d", &cozimento);
        adicionarPedido(nome, preparo, cozimento);
    }

    muralAtivo = 0;

    // Tripulantes
    for (int i = 0; i < NUM_TRIPULANTES; i++)
    {
        pthread_create(&threadsTripulantes[i], NULL, executarTripulante, &tripulantes[i]);
    }

    for (int i = 0; i < NUM_TRIPULANTES; i++)
    {
        pthread_join(threadsTripulantes[i], NULL);
    }

    pthread_join(threadChefe, NULL);
    pthread_join(threadMuralExibicao, NULL);

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
