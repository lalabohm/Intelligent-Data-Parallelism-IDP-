#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include "mural.h"
#include "structs.h"

#define LOG_MAX_MESSAGES 8

// --- Variáveis Globais Externas ---
extern pthread_mutex_t mutexPedidos;
extern pthread_mutex_t mutexTela;
extern pthread_mutex_t mutexLog;

extern Pedido *inicio;
extern int muralAtivo;
extern char mensagens_log[LOG_MAX_MESSAGES][128];
extern int num_logs;

// Adiciona um novo pedido ao final da lista
void adicionarPedido(const char *nome, int preparo, int cozimento)
{
    Pedido *novo = malloc(sizeof(Pedido));
    if (!novo)
        return;

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

// THREAD: GERENCIADOR DE TELA
// Única thread responsável por todas as operações de desenho.
void *gerenciadorDeTela(void *arg)
{
    while (muralAtivo)
    {
        pthread_mutex_lock(&mutexTela);
        erase(); // Limpa a tela inteira para redesenhar

        // --- 1. Desenha o Cabeçalho ---
        attron(COLOR_PAIR(2));
        mvprintw(0, 1, "BEM-VINDO AO RESTAURANTE FORA NO ESPAÇO!");
        attroff(COLOR_PAIR(2));

        // --- 2. Desenha o Mural de Pedidos ---
        mvprintw(2, 0, "--- MURAL DE PEDIDOS ---");
        pthread_mutex_lock(&mutexPedidos);
        int linha = 3;
        Pedido *temp = inicio;
        while (temp != NULL && linha < 10)
        {
            attron(COLOR_PAIR(1));
            mvprintw(linha++, 1, "Prato: %-20s | Preparo: %d | Cozimento: %d",
                     temp->nome, temp->tempoPreparoIngredientes, temp->tempoCozimento);
            attroff(COLOR_PAIR(1));
            temp = temp->proximo;
        }
        pthread_mutex_unlock(&mutexPedidos);

        // --- 3. Desenha o Painel de Status/Log ---
        mvprintw(12, 0, "--- LOG DE ATIVIDADES ---");
        pthread_mutex_lock(&mutexLog);
        for (int i = 0; i < num_logs; i++)
        {
            mvprintw(13 + i, 1, "%s", mensagens_log[i]);
        }
        pthread_mutex_unlock(&mutexLog);

        // --- 4. Desenha as Instruções ---
        attron(COLOR_PAIR(6));
        mvprintw(22, 0, "COMANDOS: Pressione (1-4) para atribuir o próximo pedido ao tripulante correspondente.");
        attroff(COLOR_PAIR(6));

        // Atualiza a tela física UMA VEZ
        refresh();
        pthread_mutex_unlock(&mutexTela);

        usleep(100000);
    }
    return NULL;
}