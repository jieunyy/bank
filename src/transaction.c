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

// 거래 내역 추가 함수
void addExpense(MYSQL *conn) {
    char accountNumber[20];
    char category[50];
    double amount;
    char paymentMethod[20];
    char date[20]; // YYYY-MM-DD 형식

    printf("\n--- 거래 내역 추가 ---\n");

    printf("계좌 번호 입력: ");
    if (fgets(accountNumber, sizeof(accountNumber), stdin) == NULL) {
        printf("입력 오류.\n");
        return;
    }
    accountNumber[strcspn(accountNumber, "\n")] = 0; // 개행 문자 제거

    printf("카테고리 입력: ");
    if (fgets(category, sizeof(category), stdin) == NULL) {
        printf("입력 오류.\n");
        return;
    }
    category[strcspn(category, "\n")] = 0;

    printf("금액 입력: ");
    if (scanf("%lf", &amount) != 1) {
        printf("입력 오류. 금액을 올바르게 입력하세요.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer(); // 입력 버퍼 클리어

    printf("결제 수단 입력: ");
    if (fgets(paymentMethod, sizeof(paymentMethod), stdin) == NULL) {
        printf("입력 오류.\n");
        return;
    }
    paymentMethod[strcspn(paymentMethod, "\n")] = 0;

    printf("날짜 입력 (YYYY-MM-DD): ");
    if (fgets(date, sizeof(date), stdin) == NULL) {
        printf("입력 오류.\n");
        return;
    }
    date[strcspn(date, "\n")] = 0;

    // 준비된 문장 초기화
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() 실패\n");
        return;
    }

    const char *query = "INSERT INTO Expense (AccountNumber, ExpenseCategory, ExpenseAmount, ExpenseDate, PaymentMethod) VALUES (?, ?, ?, ?, ?)";
    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "mysql_stmt_prepare() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }

    // 바인딩 초기화
    MYSQL_BIND bind_param[5];
    memset(bind_param, 0, sizeof(bind_param));

    // AccountNumber
    bind_param[0].buffer_type = MYSQL_TYPE_STRING;
    bind_param[0].buffer = accountNumber;
    bind_param[0].buffer_length = strlen(accountNumber);

    // ExpenseCategory
    bind_param[1].buffer_type = MYSQL_TYPE_STRING;
    bind_param[1].buffer = category;
    bind_param[1].buffer_length = strlen(category);

    // ExpenseAmount
    bind_param[2].buffer_type = MYSQL_TYPE_DOUBLE;
    bind_param[2].buffer = &amount;
    bind_param[2].is_null = 0;
    bind_param[2].length = 0;

    // ExpenseDate
    bind_param[3].buffer_type = MYSQL_TYPE_STRING;
    bind_param[3].buffer = date;
    bind_param[3].buffer_length = strlen(date);

    // PaymentMethod
    bind_param[4].buffer_type = MYSQL_TYPE_STRING;
    bind_param[4].buffer = paymentMethod;
    bind_param[4].buffer_length = strlen(paymentMethod);

    if (mysql_stmt_bind_param(stmt, bind_param)) {
        fprintf(stderr, "mysql_stmt_bind_param() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }

    // 쿼리 실행
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "mysql_stmt_execute() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }

    printf("거래 내역이 성공적으로 추가되었습니다.\n");

    mysql_stmt_close(stmt);
}