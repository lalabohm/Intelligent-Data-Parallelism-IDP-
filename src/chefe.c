#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "chefe.h"
#include "structs.h"

#define LIMITE_PEDIDOS 8
#define NUM_TRIPULANTES 4

void *chefeDeCozinha(void *arg)
{
    Tripulante *tripulantes = (Tripulante *)arg;

    while (muralAtivo)
    {
        pthread_mutex_lock(&mutexPedidos);

        int contador_pedidos = 0;
        Pedido *temp_count = inicio;
        while (temp_count)
        {
            contador_pedidos++;
            temp_count = temp_count->proximo;
        }

        if (contador_pedidos > LIMITE_PEDIDOS)
        {
            adicionar_log("ACÚMULO DE PEDIDOS! Fim de jogo!");
            muralAtivo = 0;
        }
        else if (inicio == NULL && contador_pedidos == 0)
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
            }
        }

        pthread_mutex_unlock(&mutexPedidos);

        if (!muralAtivo)
        {
            break;
        }

        Comando cmd = {-1, 0};
        pthread_mutex_lock(&mutexComando);
        if (comando_atual.tripulante_id != -1)
        {
            cmd = comando_atual;
            comando_atual.tripulante_id = -1;
        }
        pthread_mutex_unlock(&mutexComando);

        if (cmd.tripulante_id == -1)
        {
            usleep(200000);
            continue;
        }

        pthread_mutex_lock(&mutexPedidos);

        if (tripulantes[cmd.tripulante_id - 1].ocupado)
        {
            adicionar_log("Chefe: Tripulante escolhido está ocupado.");
            pthread_mutex_unlock(&mutexPedidos);
            continue;
        }

        Pedido *pedido_alvo = NULL;
        Pedido *anterior = NULL;
        Pedido *atual = inicio;
        int prato_idx_alvo = cmd.prato_id - 'a';
        int idx_atual = 0;

        while (atual != NULL)
        {
            if (idx_atual == prato_idx_alvo)
            {
                pedido_alvo = atual;
                if (anterior == NULL)
                {
                    inicio = atual->proximo;
                }
                else
                {
                    anterior->proximo = atual->proximo;
                }
                break;
            }
            anterior = atual;
            atual = atual->proximo;
            idx_atual++;
        }

        if (pedido_alvo == NULL)
        {
            adicionar_log("Chefe: Prato não encontrado ou inválido.");
            pthread_mutex_unlock(&mutexPedidos);
            continue;
        }

        tripulantes[cmd.tripulante_id - 1].pedidoAtual = pedido_alvo;
        tripulantes[cmd.tripulante_id - 1].ocupado = 1;

        char buffer[128];
        sprintf(buffer, "Chefe atribuiu '%s' ao Tripulante %d", pedido_alvo->nome, cmd.tripulante_id);
        adicionar_log(buffer);

        pthread_mutex_unlock(&mutexPedidos);
    }

    return NULL;
}