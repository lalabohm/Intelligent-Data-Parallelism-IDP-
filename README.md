# 🛸 Fora no Espaço

## 👩‍💻 Autores

- Larissa Böhm Araújo

## 🎯 Objetivo do Trabalho

Simular o funcionamento de um restaurante espacial, utilizando múltiplas threads com pthreads para gerenciar pedidos, tripulantes e cozinhas. O projeto envolve conceitos de programação concorrente e sincronização de recursos com mutex e variáveis de condição.

## ⚙️ Descrição do Funcionamento

O programa realiza as seguintes tarefas:

1.  **Geração de pedidos**: Novos pratos são adicionados dinamicamente ao mural durante a simulação.
2.  **Exibição de informações**: Um dashboard em tempo real exibe o status dos pedidos, tripulantes e recursos da cozinha.
3.  **Chefe de cozinha**: O jogador atua como o chefe, consumindo os inputs do teclado para distribuir os pedidos do mural aos tripulantes disponíveis.
4.  **Tripulantes**: Cada tripulante (thread) realiza o preparo e o cozimento dos pratos, disputando e aguardando por recursos compartilhados (bancadas e cozinhas).
5.  **Finalização**: O sistema é finalizado quando todos os pratos são entregues ou quando a fila de pedidos atinge um limite crítico.

## 🛠️ Descrição da Implementação

A solução foi desenvolvida com uma arquitetura multithread que separa as responsabilidades de lógica e interface para garantir a sincronização e evitar condições de corrida.

- **Threads criadas**:

  - `gerenciadorDeTela`: Única thread responsável por toda a renderização na tela com `ncurses`. Ela lê o estado do jogo (pedidos, recursos, logs) de forma segura e atualiza a interface periodicamente.
  - `gerenciadorDeInput`: Thread dedicada a capturar as entradas do teclado do usuário (`getch`) de forma não bloqueante para o resto do sistema.
  - `geradorDePedidos`: Simula o "Mural de Pedidos", criando novas ordens de pratos em intervalos de tempo para manter a simulação dinâmica.
  - `chefeDeCozinha`: Atua como o "cérebro" da lógica, consumindo os comandos do usuário (capturados pelo `gerenciadorDeInput`) e atribuindo pedidos aos tripulantes disponíveis.
  - `executarTripulante` (múltiplas): Cada uma representa um tripulante, executando as tarefas de preparo e cozimento e gerenciando o uso dos recursos.

- **Sincronização**:

  - `pthread_mutex_t`: Utilizado para garantir acesso exclusivo a todos os recursos compartilhados: a lista de pedidos, os arrays de bancadas, cozinhas e tripulantes, o buffer de comando do usuário e o painel de logs.
  - `pthread_cond_t`: Essencial para a eficiência do sistema. Os tripulantes usam variáveis de condição para esperar (dormir) por bancadas e cozinhas livres, evitando o desperdício de CPU com _busy-waiting_ e implementando um sistema de fila natural.

- **Modularização**:
  - `main.c`: Orquestra a simulação, inicializando os recursos, as threads e gerenciando o ciclo de vida do programa. Também contém as implementações das threads de input e do gerador de pedidos.
  - `mural.c`: Contém a implementação da thread de tela (`gerenciadorDeTela`) e a função para adicionar pedidos.
  - `tripulante.c`: Contém a lógica de trabalho dos tripulantes.
  - `chefe.c`: Contém a lógica de atribuição de tarefas do chefe.
  - `structs.h`: Centraliza a definição de todas as estruturas de dados e a declaração de variáveis globais compartilhadas.

## 📺 Interface com `ncurses`

Este projeto utiliza a biblioteca `ncurses` para criar uma interface visual interativa no terminal.

- Exibição dinâmica com `mvprintw` e `refresh`.
- Atualização limpa das seções com controle de linhas.
- Uso de `setlocale(LC_ALL, "")` para permitir caracteres acentuados.

> 📦 **Dependência**: é necessário ter a biblioteca `ncurses` instalada no sistema.

### Instalação no Ubuntu/Debian:

````bash
sudo apt-get install libncurses5-dev libncursesw5-dev

## 🧪 Instruções de Compilação e Execução

### Pré-requisitos:

- `gcc` instalado
- `make` instalado (opcional, mas recomendado)

### Compilação com Make:

```bash
make
````

### Execução:

```bash
./fora-no-espaco
```

### Para limpar os arquivos gerados:

```bash
make clean
```

## 🎮 Como Jogar

1.  **Fase de Setup:** Ao iniciar o programa, um menu interativo aparece para permitir uma configuração inicial opcional:

    - Digite `a` para ver um cardápio e adicionar um pedido inicial à fila (escolhendo pelo número).
    - Digite `s` para pular esta etapa e iniciar a simulação diretamente.

2.  **Durante a Simulação:** A tela principal do jogo será exibida com o "Cardápio de Pedidos Pendentes". - **Seu Papel:** Você atua como o **Chefe da Cozinha**, e sua função é coletar os inputs para gerenciar a equipe[cite: 52]. - **Comandos:** O sistema de comando funciona com **duas teclas**, como sugerido pelo enunciado[cite: 56]: 1. Primeiro, pressione a tecla numérica do **Tripulante** que você quer usar (de 1 a 4). 2. Em seguida, pressione a tecla de letra do **Prato** que você quer atribuir (ex: `a`, `b`, `c`...). - _Exemplo de comando: Pressionar `2` e depois `a` dá a ordem: "Tripulante 2, prepare o prato [a] do cardápio"_ - **Objetivo:** Atenda a todos os pedidos, gerenciando sua equipe para que a fila de pratos não cresça demais.O jogo termina quando todos os pedidos tiverem sido atendidos ou por acúmulo de pedidos não atendidos[cite: 28].

## 📸 Imagens do Projeto

<p align="center">
  <strong>1. Tela de Início (Setup)</strong><br>
  <img src="Captura de tela 2025-06-29 141155.png" alt="Menu Inicial de Setup" width="80%"/>
  <br><em>Menu principal no terminal, onde o usuário pode adicionar pedidos iniciais ou começar a simulação.</em>
</p>

<p align="center">
  <strong>2. Cardápio de Setup</strong><br>
  <img src="Captura de tela 2025-06-29 141207.png" alt="Cardápio para Adicionar Pedidos" width="80%"/>
  <br><em>Tela exibida ao escolher 'adicionar pedido', mostrando os pratos pré-definidos para a configuração inicial.</em>
</p>

<p align="center">
  <strong>3. Início da Simulação</strong><br>
  <img src="Captura de tela 2025-06-29 141227.png" alt="Dashboard no Início da Simulação" width="80%"/>
  <br><em>O dashboard principal do jogo no "momento zero", com os recursos livres e o primeiro pedido aguardando na fila.</em>
</p>

<p align="center">
  <strong>4. Simulação em Ação</strong><br>
  <img src="Captura de tela 2025-06-29 141257.png" alt="Simulação em Andamento" width="80%"/>
  <br><em>O jogo em andamento, mostrando um tripulante trabalhando, recursos ocupados e o log de atividades sendo preenchido em tempo real.</em>
</p>
