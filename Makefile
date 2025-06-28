CC = gcc

SRC_DIR = src
INCLUDE_DIR = include

CFLAGS = -Wall -I$(INCLUDE_DIR)

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/mural.c $(SRC_DIR)/tripulante.c $(SRC_DIR)/chefe.c

EXEC = fora-no-espaco

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC) -lpthread

clean:
	rm -f $(EXEC)
