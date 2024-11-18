# Makefile

# 컴파일러 및 플래그 설정
CC = gcc
CFLAGS = -I./include -I/usr/local/opt/mysql@8.0/include `mysql_config --cflags` -Wall -Wextra -std=c99
# LDFLAGS는 링크 시 필요한 라이브러리를 지정
LDFLAGS = `mysql_config --libs`

# 소스 파일 목록 (login.c 포함)
SRC = src/main.c src/login.c src/customer.c src/account.c src/transaction.c src/expense.c src/report.c src/utils.c

# 오브젝트 파일 목록
OBJ = $(SRC:.c=.o)

# 타겟 실행 파일
TARGET = bank

# 기본 타겟
all: $(TARGET)

# 타겟 생성 규칙
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# 오브젝트 파일 생성 규칙
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 클린 규칙
clean:
	rm -f $(OBJ) $(TARGET)
