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

- **Threads criadas**:

  - `muralDePedidos`: adiciona pratos no mural.
  - `exibirMuralPeriodicamente`: imprime o mural a cada 2 segundos.
  - `chefeDeCozinha`: verifica se há pedidos e tripulantes livres e os associa.
  - `executarTripulante`: realiza o preparo e o cozimento do prato atribuído.

- **Sincronização**:

  - `pthread_mutex_t` para controle do mural, bancadas e cozinhas.

- **Estruturas**:

  - `Pedido`: nome, tempo de preparo e tempo de cozimento.
  - `Tripulante`: id, estado de ocupação, prato atual.
  - `Bancada` e `Cozinha`: controle de disponibilidade.

- **Modularização**:
  - `main.c`: inicializa e gerencia threads.
  - `mural.c`: gerenciamento do mural de pedidos.
  - `tripulante.c`: lógica dos tripulantes.
  - `chefe.c`: lógica do chefe de cozinha.
  - `structs.h`: definição das estruturas de dados.

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
