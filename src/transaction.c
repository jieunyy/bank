#include "transaction.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// 입력 버퍼 클리어 함수 선언 (utils.c)
extern void clearInputBuffer();

void transactionLookup(MYSQL *conn) {
    char input[100];
    printf("\n--- 거래 내역 조회 ---\n");
    printf("계좌 번호 입력: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("입력 오류.\n");
        return;
    }
    input[strcspn(input, "\n")] = 0; // 개행 문자 제거

    // 입력이 숫자인지 확인
    int isNumeric = 1;
    for (size_t i = 0; i < strlen(input); i++) {
        if (!isdigit(input[i])) {
            isNumeric = 0;
            break;
        }
    }

    if (!isNumeric) {
        printf("잘못된 계좌 번호입니다. 숫자만 입력해 주세요.\n");
        return;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT TransactionID, TransactionType, Amount, TransactionDate, TransactionStatus FROM Transaction WHERE AccountNumber = %s AND TransactionType IN ('입금', '출금')",
             input);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "거래 내역 조회 실패: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "결과 저장 실패: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_ROW row;
    printf("\n%-15s %-15s %-15s %-25s %-15s\n",
           "거래 ID", "유형", "금액", "날짜 및 시간", "상태");
    printf("-------------------------------------------------------------------------------\n");

    while ((row = mysql_fetch_row(res))) {
        printf("%-15s %-15s %-15s %-25s %-15s\n",
               row[0], row[1], row[2], row[3], row[4]);
    }

    mysql_free_result(res);
}
