# 🛸 Fora no Espaço

## 👩‍💻 Autores

- Larissa Böhm Araújo

## 🎯 Objetivo do Trabalho

Simular o funcionamento de um restaurante espacial, utilizando múltiplas threads com pthreads para gerenciar pedidos, tripulantes e cozinhas. O projeto envolve conceitos de programação concorrente e sincronização de recursos com mutex.

## ⚙️ Descrição do Funcionamento

O programa realiza as seguintes tarefas:

1. **Geração de pedidos**: os pratos são adicionados ao mural com tempos específicos de preparo e cozimento.
2. **Exibição do mural**: periodicamente são exibidos no terminal os pratos disponíveis.
3. **Chefe de cozinha**: distribui os pedidos do mural para os tripulantes disponíveis.
4. **Tripulantes**: cada tripulante realiza o preparo e o cozimento dos pratos, utilizando bancadas e cozinhas conforme a disponibilidade.
5. **Finalização**: após todos os pratos serem preparados e cozidos, o sistema é finalizado com uma mensagem no terminal.

## 🛠️ Descrição da Implementação

A solução foi desenvolvida com uma arquitetura multithread que separa as responsabilidades de lógica e interface para garantir a sincronização e evitar condições de corrida.

- **Threads criadas**:

  - `gerenciadorDeTela`: Única thread responsável por toda a renderização na tela com `ncurses`. Ela lê o estado do jogo (pedidos, recursos, logs) de forma segura e atualiza a interface periodicamente. 
  - `gerenciadorDeInput`: Thread dedicada a capturar as entradas do teclado do usuário (`getch`) de forma não bloqueante para o resto do sistema. 
  - `geradorDePedidos`: Simula o "Mural de Pedidos", criando novas ordens de pratos em intervalos de tempo para manter a simulação dinâmica. 
  - `chefeDeCozinha`: Atua como o "cérebro" da lógica, consumindo os comandos do usuário (capturados pelo `gerenciadorDeInput`) e atribuindo pedidos aos tripulantes disponíveis, validando também a disponibilidade de recursos. 
  - `executarTripulante` (múltiplas): Cada uma representa um tripulante, executando as tarefas de preparo e cozimento e gerenciando o uso dos recursos (bancadas e cozinhas). 

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

```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```

## 🧪 Instruções de Compilação e Execução

### Pré-requisitos:

- `gcc` instalado
- `make` instalado (opcional, mas recomendado)

### Compilação com Make:

```bash
make
```

### Execução:

```bash
./fora-no-espaco
```

### Para limpar os arquivos gerados:

```bash
make clean
```

## 🎮 Como Jogar

1.  **Fase de Setup:** Ao iniciar o programa, você pode opcionalmente adicionar pedidos customizados na fila.

    - Digite `a` para adicionar um novo prato (informe nome, tempo de preparo e cozimento).
    - Digite `s` para iniciar a simulação.

2.  **Durante a Simulação:** A tela principal será exibida.
    - **Seu Papel:** Você atua como o **Chefe da Cozinha**. 
    - **Comandos:** Para atribuir o primeiro pedido da fila a um tripulante, pressione a tecla numérica correspondente ao tripulante (de 1 a 4). 
    - **Objetivo:** Atenda a todos os pedidos. O jogo termina quando todos os pratos forem entregues ou se a fila de pedidos pendentes ficar muito grande. 
