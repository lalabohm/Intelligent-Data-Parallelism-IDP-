#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "mural.h"
#include "structs.h"

// Número de segundos para simulação do jogo
#define TEMPO_DE_JOGO 30

int main()
{
    pthread_t threadMural;

    // Inicializa o mutex do mural de pedidos
    pthread_mutex_init(&mutexPedidos, NULL);

    // Cria a thread do mural de pedidos
    if (pthread_create(&threadMural, NULL, muralDePedidos, NULL) != 0)
    {
        perror("Erro ao criar a thread do mural de pedidos");
        exit(1);
    }

    printf("🚀 Iniciando o jogo Fora no Espaço! 🌌\n");
    printf("⌛ Tempo de jogo: %d segundos\n", TEMPO_DE_JOGO);
    printf("🔄 Pressione Ctrl+C para encerrar a qualquer momento.\n\n");

    // Simula o tempo de execução do jogo
    for (int i = 0; i < TEMPO_DE_JOGO; i++)
    {
        sleep(5);        // atualiza a cada 5s (ajustável)
        listarPedidos(); // exibe os pedidos atuais (teste)
    }

    printf("\n⏹️ Tempo de jogo encerrado!\n");

    // Encerra a thread do mural (força parada)
    pthread_cancel(threadMural);
    pthread_join(threadMural, NULL);

    // Destroi o mutex
    pthread_mutex_destroy(&mutexPedidos);

    return 0;
}
