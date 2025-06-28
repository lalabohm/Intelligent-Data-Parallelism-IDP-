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

        // Verifica se há pedidos no mural
        if (inicio == NULL)
        {
            pthread_mutex_unlock(&mutexPedidos);
            sleep(1);
            continue;
        }

        Pedido *pedido = inicio;

        // Procura um tripulante disponível
        int tripulanteLivre = -1;
        for (int i = 0; i < 4; i++)
        {
            if (tripulantes[i].ocupado == 0)
            {
                tripulanteLivre = i;
                break;
            }
        }

        // Se não houver nenhum disponível, espera
        if (tripulanteLivre == -1)
        {
            pthread_mutex_unlock(&mutexPedidos);
            sleep(1);
            continue;
        }

        // Atribui o pedido ao tripulante
        inicio = inicio->proximo;
        tripulantes[tripulanteLivre].pedidoAtual = pedido;
        tripulantes[tripulanteLivre].ocupado = 1;

        // Exibe a atribuição na tela
        pthread_mutex_lock(&mutexTela);
        mvprintw(linhaSaida++, 0, "O chefe atribuiu o prato %s para Tripulante %d", pedido->nome, tripulantes[tripulanteLivre].id);
        refresh();
        pthread_mutex_unlock(&mutexTela);

        pthread_mutex_unlock(&mutexPedidos);
        sleep(1);
    }

    return NULL;
}
