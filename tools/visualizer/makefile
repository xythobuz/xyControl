CC = gcc
SRC = main.c
SRC += serial.c

TARGET = visualizer

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -lncurses -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) -std=c99 -x c -c $< -o $@

clean:
	$(RM) *.o
	$(RM) $(TARGET)
