CC=gcc
CFLAGS=-Wall -Wno-format -pthread

# Цели
all: parallel_sum

# Правило для сборки parallel_sum
parallel_sum: parallel_sum.c sum_lib.o utils.o
	$(CC) -o parallel_sum parallel_sum.c sum_lib.o utils.o $(CFLAGS)

# Правило для сборки sum_lib.o
sum_lib.o: sum_lib.c sum_lib.h
	$(CC) -o sum_lib.o -c sum_lib.c $(CFLAGS)

# Правило для сборки utils.o
utils.o: utils.c utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

# Правило для очистки
clean:
	rm -f parallel_sum sum_lib.o utils.o
