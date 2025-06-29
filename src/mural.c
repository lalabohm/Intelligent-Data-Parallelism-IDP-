#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include "mural.h"
#include "structs.h"

#define LOG_MAX_MESSAGES 8
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

extern Bancada bancadas[];
extern Cozinha cozinhas[];
extern Tripulante tripulantes[];

extern char mensagens_log[LOG_MAX_MESSAGES][128];
extern int num_logs;

void adicionarPedido(const char *nome, int preparo, int cozimento)
{
    pthread_mutex_lock(&mutexPedidos);
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

/**
 * Thread que gerencia o desenho de toda a interface do jogo,
 * funcionando como a entidade "Exibição de Informações".
 */
void *gerenciadorDeTela(void *arg)
{
    while (muralAtivo)
    {
        pthread_mutex_lock(&mutexTela);
        erase();

        // Seção 1: Título
        attron(COLOR_PAIR(2));
        mvprintw(0, 1, "BEM-VINDO AO RESTAURANTE FORA NO ESPAÇO!");
        attroff(COLOR_PAIR(2));

        // Seção 2: Cardápio de Pedidos Pendentes
        mvprintw(2, 0, "--- CARDÁPIO DE PEDIDOS PENDENTES ---");
        pthread_mutex_lock(&mutexPedidos);
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
        pthread_mutex_unlock(&mutexPedidos);

        // Seção 3: Status dos Recursos
        int linha_recursos = 9;
        mvprintw(linha_recursos++, 0, "--- STATUS DOS RECURSOS ---");

        pthread_mutex_lock(&mutexBancadas);
        for (int i = 0; i < NUM_BANCADAS; i++)
        {
            int cor = bancadas[i].ocupado ? 3 : 5;
            attron(COLOR_PAIR(cor));
            mvprintw(linha_recursos, 1 + (i * 22), "Bancada %d: %s", bancadas[i].id, bancadas[i].ocupado ? "Ocupada" : "Livre");
            attroff(COLOR_PAIR(cor));
        }
        pthread_mutex_unlock(&mutexBancadas);
        linha_recursos++;

        pthread_mutex_lock(&mutexCozinhas);
        for (int i = 0; i < NUM_COZINHAS; i++)
        {
            int cor = cozinhas[i].ocupado ? 3 : 5;
            attron(COLOR_PAIR(cor));
            mvprintw(linha_recursos, 1 + (i * 22), "Cozinha %d: %s", cozinhas[i].id, cozinhas[i].ocupado ? "Ocupada" : "Livre");
            attroff(COLOR_PAIR(cor));
        }
        pthread_mutex_unlock(&mutexCozinhas);
        linha_recursos += 2;

        // Seção 4: Status dos Tripulantes
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

        // Seção 5: Log de Atividades
        int linha_log = 20;
        mvprintw(linha_log++, 0, "--- LOG DE ATIVIDADES ---");
        pthread_mutex_lock(&mutexLog);
        for (int i = 0; i < num_logs; i++)
        {
            mvprintw(linha_log + i, 1, "%s", mensagens_log[i]);
        }
        pthread_mutex_unlock(&mutexLog);

        // Seção 6: Instruções de Comando
        attron(COLOR_PAIR(6));
        mvprintw(29, 0, "COMANDOS: Pressione (Tripulante 1-4) e depois (Prato a,b,c...) para atribuir um pedido.");
        attroff(COLOR_PAIR(6));

        refresh();
        pthread_mutex_unlock(&mutexTela);
        usleep(100000);
    }
    return NULL;
}