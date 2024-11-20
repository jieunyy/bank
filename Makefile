# Makefile

CC = gcc
CFLAGS = -I./include -I/usr/local/opt/mysql@8.0/include -I/usr/local/opt/openssl@3/include `mysql_config --cflags` -Wall -Wextra -std=c99
LIBS = `mysql_config --libs` -lssl -lcrypto

SRC = src/main.c src/login.c src/customer.c src/account.c src/transaction.c src/expense.c src/report.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = bank

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
