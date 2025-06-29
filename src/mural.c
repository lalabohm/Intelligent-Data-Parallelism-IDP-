#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include "mural.h"
#include "structs.h"

#define LOG_MAX_MESSAGES 4
#define NUM_TRIPULANTES 4
#define NUM_BANCADAS 3
#define NUM_COZINHAS 3

extern pthread_mutex_t mutexPedidos;
extern pthread_mutex_t mutexTela;
extern pthread_mutex_t mutexLog;
extern pthread_mutex_t mutexBancadas;
extern pthread_mutex_t mutexCozinhas;

extern Pedido *inicio;
extern int muralAtivo;
extern bool ja_teve_pedidos;

extern Bancada bancadas[];
extern Cozinha cozinhas[];
extern Tripulante tripulantes[];

extern char mensagens_log[][128];
extern int num_logs;

void adicionarPedido(const char *nome, int preparo, int cozimento)
{
    pthread_mutex_lock(&mutexPedidos);
    ja_teve_pedidos = true;
    Pedido *novo = malloc(sizeof(Pedido));
    if (novo)
    {
        strcpy(novo->nome, nome);
        novo->tempoPreparoIngredientes = preparo;
        novo->tempoCozimento = cozimento;
        novo->proximo = NULL;

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
    }
    pthread_mutex_unlock(&mutexPedidos);
}

void *gerenciadorDeTela(void *arg)
{
    while (muralAtivo)
    {
        // Etapa 1: Bloqueia TODOS os mutexes no início do quadro.
        // A ordem é importante para evitar deadlocks (tela primeiro, depois os dados).
        pthread_mutex_lock(&mutexTela);
        pthread_mutex_lock(&mutexPedidos);
        pthread_mutex_lock(&mutexBancadas);
        pthread_mutex_lock(&mutexCozinhas);
        pthread_mutex_lock(&mutexLog);

        erase();

        attron(COLOR_PAIR(2));
        mvprintw(0, 1, "BEM-VINDO AO RESTAURANTE FORA NO ESPAÇO!");
        attroff(COLOR_PAIR(2));

        mvprintw(2, 0, "--- CARDÁPIO DE PEDIDOS PENDENTES ---");
        int linha_mural = 3;
        int prato_idx = 0;
        Pedido *temp = inicio;
        while (temp != NULL && prato_idx < 5)
        {
            attron(COLOR_PAIR(6));
            mvprintw(linha_mural, 1, "[%c]", 'a' + prato_idx);
            attroff(COLOR_PAIR(6));
            attron(COLOR_PAIR(1));
            mvprintw(linha_mural++, 5, "Prato: %s", temp->nome);
            attroff(COLOR_PAIR(1));
            temp = temp->proximo;
            prato_idx++;
        }

        int linha_recursos = 9;
        mvprintw(linha_recursos++, 0, "--- STATUS DOS RECURSOS ---");
        for (int i = 0; i < NUM_BANCADAS; i++)
        {
            int cor = bancadas[i].ocupado ? 3 : 5;
            attron(COLOR_PAIR(cor));
            mvprintw(linha_recursos, 1 + (i * 22), "Bancada %d: %s", bancadas[i].id, bancadas[i].ocupado ? "Ocupada" : "Livre");
            attroff(COLOR_PAIR(cor));
        }
        linha_recursos++;
        for (int i = 0; i < NUM_COZINHAS; i++)
        {
            int cor = cozinhas[i].ocupado ? 3 : 5;
            attron(COLOR_PAIR(cor));
            mvprintw(linha_recursos, 1 + (i * 22), "Cozinha %d: %s", cozinhas[i].id, cozinhas[i].ocupado ? "Ocupada" : "Livre");
            attroff(COLOR_PAIR(cor));
        }
        linha_recursos += 2;

        mvprintw(linha_recursos++, 0, "--- STATUS DOS TRIPULANTES ---");
        for (int i = 0; i < NUM_TRIPULANTES; i++)
        {
            if (tripulantes[i].ocupado && tripulantes[i].pedidoAtual != NULL)
            {
                mvprintw(linha_recursos + i, 1, "Tripulante %d: Trabalhando no prato '%s'", tripulantes[i].id, tripulantes[i].pedidoAtual->nome);
            }
            else
            {
                mvprintw(linha_recursos + i, 1, "Tripulante %d: Ocioso", tripulantes[i].id);
            }
        }

        int linha_log = 20;
        mvprintw(linha_log++, 0, "--- LOG DE ATIVIDADES ---");
        for (int i = 0; i < num_logs; i++)
        {
            mvprintw(linha_log + i, 1, "%s", mensagens_log[i]);
        }

        attron(COLOR_PAIR(6));
        mvprintw(29, 0, "COMANDOS: Pressione (Tripulante 1-4) e depois (Prato a,b,c...) para atribuir um pedido.");
        attroff(COLOR_PAIR(6));

        refresh();

        // Etapa 2: Libera TODOS os mutexes no final, na ordem inversa.
        pthread_mutex_unlock(&mutexLog);
        pthread_mutex_unlock(&mutexCozinhas);
        pthread_mutex_unlock(&mutexBancadas);
        pthread_mutex_unlock(&mutexPedidos);
        pthread_mutex_unlock(&mutexTela);

        usleep(100000);
    }
    return NULL;
}