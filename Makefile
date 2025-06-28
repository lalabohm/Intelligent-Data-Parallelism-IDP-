EXEC    = fora-no-espaco
CC      = gcc

SRC_DIR = src
OBJ_DIR = obj

CFLAGS  = -Wall -Iinclude -MMD -MP
LIBS    = -lpthread -lncurses

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

.PHONY: all clean run

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(EXEC) $(OBJ_DIR)

run: all
	./$(EXEC)

-include $(DEPS)