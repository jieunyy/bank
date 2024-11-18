// src/report.c
#include "report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 월별 거래 리포트 생성 함수
void generateMonthlyTransactionReport(MYSQL *conn, int year, int month) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind[2];
    MYSQL_BIND result_bind[1];
    int transaction_count = 0;

    stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() 실패\n");
        return;
    }

    const char *query = "SELECT COUNT(*) FROM Transaction WHERE YEAR(TransactionDate) = ? AND MONTH(TransactionDate) = ?";
    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "mysql_stmt_prepare() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }

    memset(bind, 0, sizeof(bind));

    // Year 바인딩
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &year;

    // Month 바인딩
    bind[1].buffer_type = MYSQL_TYPE_LONG;
    bind[1].buffer = &month;

    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "mysql_stmt_bind_param() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "mysql_stmt_execute() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }

    memset(result_bind, 0, sizeof(result_bind));

    result_bind[0].buffer_type = MYSQL_TYPE_LONG;
    result_bind[0].buffer = &transaction_count;

    if (mysql_stmt_bind_result(stmt, result_bind)) {
        fprintf(stderr, "mysql_stmt_bind_result() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_store_result(stmt)) {
        fprintf(stderr, "mysql_stmt_store_result() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_fetch(stmt) == 0) {
        printf("\n--- %d년 %02d월 거래 리포트 ---\n", year, month);
        printf("거래 건수: %d\n", transaction_count);
    } else {
        printf("리포트 생성 중 오류가 발생했습니다.\n");
    }

    mysql_stmt_close(stmt);
}

// 고객 요약 리포트 생성 함수
void generateCustomerSummaryReport(MYSQL *conn) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    const char *query = "SELECT Customer.Name, COUNT(Account.AccountNumber) AS AccountCount, IFNULL(SUM(Account.Balance), 0) AS TotalBalance FROM Customer LEFT JOIN Account ON Customer.CustomerID = Account.CustomerID GROUP BY Customer.CustomerID";

    if (mysql_query(conn, query)) {
        fprintf(stderr, "리포트 쿼리 실패: %s\n", mysql_error(conn));
        return;
    }

    res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "결과 저장 실패: %s\n", mysql_error(conn));
        return;
    }

    printf("\n--- 고객 요약 리포트 ---\n");
    printf("%-20s %-15s %-15s\n", "이름", "계좌 수", "총 잔액");
    printf("-------------------------------------------------\n");

    while ((row = mysql_fetch_row(res))) {
        printf("%-20s %-15s %-15s\n", row[0], row[1], row[2] ? row[2] : "0.00");
    }

    mysql_free_result(res);
}
