#include "chefe.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "structs.h"

#define LIMITE_PEDIDOS 10
#define NUM_TRIPULANTES 4
#define NUM_BANCADAS 3

extern Pedido *inicio;
extern Tripulante tripulantes[];
extern Bancada bancadas[];

extern pthread_mutex_t mutexPedidos;
extern pthread_mutex_t mutexBancadas;
extern pthread_mutex_t mutexComando;

extern int muralAtivo;
extern int comando_tripulante_id;

extern void adicionar_log(const char *mensagem);

void *chefeDeCozinha(void *arg)
{
    while (muralAtivo)
    {
        int tripulante_id_local = -1;

        pthread_mutex_lock(&mutexComando);
        if (comando_tripulante_id != -1)
        {
            tripulante_id_local = comando_tripulante_id;
            comando_tripulante_id = -1;
        }
        pthread_mutex_unlock(&mutexComando);

        pthread_mutex_lock(&mutexPedidos);

        int contador_pedidos = 0;
        Pedido *temp = inicio;
        while (temp)
        {
            contador_pedidos++;
            temp = temp->proximo;
        }

        if (contador_pedidos > LIMITE_PEDIDOS)
        {
            adicionar_log("ACÚMULO DE PEDIDOS! Fim de jogo!");
            muralAtivo = 0;
            pthread_mutex_unlock(&mutexPedidos);
            break;
        }

        if (inicio == NULL)
        {
            bool todos_ociosos = true;
            for (int i = 0; i < NUM_TRIPULANTES; i++)
            {
                if (tripulantes[i].ocupado)
                {
                    todos_ociosos = false;
                    break;
                }
            }
            if (todos_ociosos)
            {
                adicionar_log("Todos os pedidos concluídos! Encerrando...");
                muralAtivo = 0;
                pthread_mutex_unlock(&mutexPedidos);
                break;
            }
        }

        if (tripulante_id_local == -1)
        {
            pthread_mutex_unlock(&mutexPedidos);
            usleep(500000);
            continue;
        }

        if (inicio == NULL)
        {
            adicionar_log("Chefe: Sem pedidos na fila para atribuir.");
            pthread_mutex_unlock(&mutexPedidos);
            continue;
        }

        if (tripulante_id_local < 1 || tripulante_id_local > NUM_TRIPULANTES || tripulantes[tripulante_id_local - 1].ocupado)
        {
            adicionar_log("Chefe: Tripulante inválido ou ocupado.");
            pthread_mutex_unlock(&mutexPedidos);
            continue;
        }

        bool bancada_livre = false;
        pthread_mutex_lock(&mutexBancadas);
        for (int i = 0; i < NUM_BANCADAS; i++)
        {
            if (!bancadas[i].ocupado)
            {
                bancada_livre = true;
                break;
            }
        }
        pthread_mutex_unlock(&mutexBancadas);

        if (!bancada_livre)
        {
            adicionar_log("Chefe: Sem bancadas livres para iniciar preparo.");
            pthread_mutex_unlock(&mutexPedidos);
            continue;
        }

        Pedido *pedido = inicio;
        inicio = inicio->proximo;

        tripulantes[tripulante_id_local - 1].pedidoAtual = pedido;
        tripulantes[tripulante_id_local - 1].ocupado = 1;

        char buffer[128];
        sprintf(buffer, "Chefe atribuiu '%s' ao Tripulante %d", pedido->nome, tripulante_id_local);
        adicionar_log(buffer);

        pthread_mutex_unlock(&mutexPedidos);
    }

    return NULL;
}