#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <locale.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include "mural.h"
#include "structs.h"
#include "tripulante.h"
#include "chefe.h"

#define NUM_TRIPULANTES 4
#define NUM_BANCADAS 3
#define NUM_COZINHAS 3
#define LOG_MAX_MESSAGES 8

pthread_mutex_t mutexPedidos;
pthread_mutex_t mutexBancadas;
pthread_mutex_t mutexCozinhas;
pthread_mutex_t mutexTela;
pthread_mutex_t mutexComando;
pthread_mutex_t mutexLog;

pthread_cond_t condBancadas;
pthread_cond_t condCozinhas;

Pedido *inicio = NULL;
int muralAtivo = 1;
Comando comando_atual = {-1, 0};

// Recursos da cozinha
Bancada bancadas[NUM_BANCADAS];
Cozinha cozinhas[NUM_COZINHAS];
Tripulante tripulantes[NUM_TRIPULANTES];

// Cardápio de pratos que podem ser gerados
const Pedido pratos_possiveis[] = {
    {"Nebulosa de Sabores", 5, 3},
    {"Anéis de Saturno", 4, 4},
    {"Buraco Negro Grelhado", 7, 5},
    {"Cometa Halley Picante", 3, 2}};
const int num_pratos_possiveis = sizeof(pratos_possiveis) / sizeof(Pedido);

char mensagens_log[LOG_MAX_MESSAGES][128];
int num_logs = 0;

/**
 * Adiciona uma mensagem ao painel de log de forma segura (thread-safe).
 * A string de mensagem a ser adicionada.
 */
void adicionar_log(const char *mensagem)
{
    pthread_mutex_lock(&mutexLog);
    if (num_logs < LOG_MAX_MESSAGES)
    {
        strcpy(mensagens_log[num_logs], mensagem);
        num_logs++;
    }
    else
    {
        for (int i = 0; i < LOG_MAX_MESSAGES - 1; i++)
        {
            strcpy(mensagens_log[i], mensagens_log[i + 1]);
        }
        strcpy(mensagens_log[LOG_MAX_MESSAGES - 1], mensagem);
    }
    pthread_mutex_unlock(&mutexLog);
}

/**
 * Thread que gera novos pedidos automaticamente em intervalos de tempo.
 * Argumentos da thread (não utilizado).
 */
void *geradorDePedidos(void *arg)
{
    adicionar_log("Gerador de pedidos iniciado.");
    while (muralAtivo)
    {
        sleep(10 + rand() % 10);
        if (!muralAtivo)
            break;
        int prato_idx = rand() % num_pratos_possiveis;
        const Pedido *prato_gerado = &pratos_possiveis[prato_idx];
        adicionarPedido(prato_gerado->nome, prato_gerado->tempoPreparoIngredientes, prato_gerado->tempoCozimento);
        char buffer_log[100];
        sprintf(buffer_log, "NOVO PEDIDO: '%s' chegou!", prato_gerado->nome);
        adicionar_log(buffer_log);
    }
    adicionar_log("Gerador de pedidos encerrado.");
    return NULL;
}

// Thread dedicada a capturar os inputs do teclado do usuário.
void *gerenciadorDeInput(void *arg)
{
    int trip_ch, prato_ch;
    while (muralAtivo)
    {
        trip_ch = getch();
        if (!muralAtivo)
            break;

        if (trip_ch >= '1' && trip_ch <= '0' + NUM_TRIPULANTES)
        {
            char buffer[100];
            sprintf(buffer, "Tripulante %c selecionado. Escolha o prato...", trip_ch);
            adicionar_log(buffer);

            prato_ch = getch();
            if (!muralAtivo)
                break;

            if (prato_ch >= 'a' && prato_ch <= 'z')
            {
                pthread_mutex_lock(&mutexComando);
                comando_atual.tripulante_id = trip_ch - '0';
                comando_atual.prato_id = prato_ch;
                pthread_mutex_unlock(&mutexComando);
            }
        }
    }
    return NULL;
}

// Define o estado inicial dos recursos (bancadas e cozinhas).
void inicializarRecursos()
{
    for (int i = 0; i < NUM_BANCADAS; i++)
    {
        bancadas[i] = (Bancada){.id = i + 1, .ocupado = 0};
    }
    for (int i = 0; i < NUM_COZINHAS; i++)
    {
        cozinhas[i] = (Cozinha){.id = i + 1, .ocupado = 0};
    }
}

// Inicializa a tela, cores e modos da biblioteca ncurses.
void inicializarNcurses()
{
    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);
}

int main()
{
    // Semente para o gerador de números aleatórios
    srand(time(NULL));
    setlocale(LC_ALL, "pt_BR.UTF-8");

    printf("--- Restaurante Fora no Espaço ---\n");
    char escolha;
    do
    {
        printf("\nO que você deseja fazer?\n");
        printf("  [a] Adicionar um pedido do cardápio\n");
        printf("  [s] Sair e iniciar a simulação\n");
        printf("Escolha uma opção: ");

        scanf(" %c", &escolha);

        if (escolha == 'a')
        {
            printf("\n--- Escolha um Prato para Adicionar ---\n");
            for (int i = 0; i < num_pratos_possiveis; i++)
            {
                printf("  [%d] %s\n", i + 1, pratos_possiveis[i].nome);
            }
            printf("Escolha uma opção (ou 0 para voltar): ");

            int escolha_prato = 0;
            if (scanf("%d", &escolha_prato) != 1)
            {
                while (getchar() != '\n')
                    ;
                escolha_prato = -1;
            }

            if (escolha_prato > 0 && escolha_prato <= num_pratos_possiveis)
            {
                const Pedido *prato_escolhido = &pratos_possiveis[escolha_prato - 1];
                adicionarPedido(prato_escolhido->nome, prato_escolhido->tempoPreparoIngredientes, prato_escolhido->tempoCozimento);
                printf("  -> Pedido '%s' adicionado com sucesso!\n", prato_escolhido->nome);
            }
            else if (escolha_prato != 0)
            {
                printf("  -> Opção de prato inválida.\n");
            }
        }
        else if (escolha != 's')
        {
            printf("  Opção inválida. Tente novamente.\n");
        }
    } while (escolha != 's');

    printf("\nIniciando a simulação...\n");
    sleep(1);

    inicializarNcurses();
    inicializarRecursos();

    pthread_mutex_init(&mutexPedidos, NULL);
    pthread_mutex_init(&mutexBancadas, NULL);
    pthread_mutex_init(&mutexCozinhas, NULL);
    pthread_mutex_init(&mutexTela, NULL);
    pthread_mutex_init(&mutexComando, NULL);
    pthread_mutex_init(&mutexLog, NULL);
    pthread_cond_init(&condBancadas, NULL);
    pthread_cond_init(&condCozinhas, NULL);

    for (int i = 0; i < NUM_TRIPULANTES; i++)
    {
        tripulantes[i] = (Tripulante){.id = i + 1, .ocupado = 0, .pedidoAtual = NULL};
    }

    pthread_t threadTela, threadInput, threadChefe, threadGerador;
    pthread_t threadsTripulantes[NUM_TRIPULANTES];

    adicionar_log("Simulação iniciada! Use (Tripulante+Prato) para jogar.");

    pthread_create(&threadTela, NULL, gerenciadorDeTela, NULL);
    pthread_create(&threadInput, NULL, gerenciadorDeInput, NULL);
    pthread_create(&threadChefe, NULL, chefeDeCozinha, tripulantes);
    pthread_create(&threadGerador, NULL, geradorDePedidos, NULL);

    for (int i = 0; i < NUM_TRIPULANTES; i++)
    {
        pthread_create(&threadsTripulantes[i], NULL, executarTripulante, &tripulantes[i]);
    }

    pthread_join(threadChefe, NULL);
    for (int i = 0; i < NUM_TRIPULANTES; i++)
    {
        pthread_join(threadsTripulantes[i], NULL);
    }

    muralAtivo = 0;
    ungetch('\n');

    pthread_join(threadInput, NULL);
    pthread_join(threadTela, NULL);
    pthread_join(threadGerador, NULL);

    endwin();

    pthread_mutex_destroy(&mutexPedidos);
    pthread_mutex_destroy(&mutexBancadas);
    pthread_mutex_destroy(&mutexCozinhas);
    pthread_mutex_destroy(&mutexTela);
    pthread_mutex_destroy(&mutexComando);
    pthread_mutex_destroy(&mutexLog);
    pthread_cond_destroy(&condBancadas);
    pthread_cond_destroy(&condCozinhas);

    printf("Simulação encerrada.\n");
    return 0;
}