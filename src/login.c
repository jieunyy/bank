#include "login.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 최대 입력 길이 정의
#define MAX_USERNAME_LEN 50
#define MAX_PASSWORD_LEN 50

// 입력 버퍼 클리어 함수 선언 (utils.c)
extern void clearInputBuffer();

int login(MYSQL *conn) {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    MYSQL_RES *res;
    MYSQL_ROW row;

    printf("\n--- 로그인 ---\n");
    printf("아이디: ");
    if (fgets(username, sizeof(username), stdin) == NULL) {
        return 0;
    }
    username[strcspn(username, "\n")] = 0; // 개행 문자 제거

    printf("비밀번호: ");
    if (fgets(password, sizeof(password), stdin) == NULL) {
        return 0;
    }
    password[strcspn(password, "\n")] = 0; // 개행 문자 제거

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT * FROM Users WHERE Username='%s' AND Password='%s'",
             username, password);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "로그인 쿼리 실패: %s\n", mysql_error(conn));
        return 0;
    }

    res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "결과 저장 실패: %s\n", mysql_error(conn));
        return 0;
    }

    int num_rows = mysql_num_rows(res);
    mysql_free_result(res);

    if (num_rows == 1) {
        printf("로그인 성공!\n");
        return 1;
    } else {
        printf("아이디 또는 비밀번호가 올바르지 않습니다.\n");
        return 0;
    }
}
