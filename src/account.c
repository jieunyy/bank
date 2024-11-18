// src/account.c
#include "account.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void clearInputBuffer();

// 계좌 개설 함수
void openAccount(MYSQL *conn) {
    int customerID;
    char accountType[50];
    double initialDeposit;
    
    printf("\n--- 계좌 개설 ---\n");
    printf("고객 ID: ");
    if (scanf("%d", &customerID) != 1) {
        printf("잘못된 입력입니다.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    printf("계좌 유형 (예: 저축, 당좌): ");
    if (fgets(accountType, sizeof(accountType), stdin) == NULL) {
        printf("입력 오류.\n");
        return;
    }
    accountType[strcspn(accountType, "\n")] = 0;
    
    printf("초기 입금액: ");
    if (scanf("%lf", &initialDeposit) != 1) {
        printf("잘못된 입력입니다.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    // 준비된 문장 초기화
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() 실패\n");
        return;
    }
    
    const char *query = "INSERT INTO Account (CustomerID, AccountType, Balance, OpeningDate) VALUES (?, ?, ?, NOW())";
    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "mysql_stmt_prepare() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }
    
    // 바인딩 초기화
    MYSQL_BIND bind[3];
    memset(bind, 0, sizeof(bind));
    
    // 고객 ID 바인딩
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &customerID;
    
    // 계좌 유형 바인딩
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = accountType;
    bind[1].buffer_length = strlen(accountType);
    
    // 초기 입금액 바인딩
    bind[2].buffer_type = MYSQL_TYPE_DOUBLE;
    bind[2].buffer = &initialDeposit;
    
    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "mysql_stmt_bind_param() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }
    
    // 쿼리 실행
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "mysql_stmt_execute() 실패: %s\n", mysql_stmt_error(stmt));
    } else {
        printf("계좌가 성공적으로 개설되었습니다.\n");
    }
    
    mysql_stmt_close(stmt);
}

// 계좌 삭제 함수
void deleteAccount(MYSQL *conn) {
    int accountID;
    
    printf("\n--- 계좌 삭제 ---\n");
    printf("삭제할 계좌 ID를 입력하세요: ");
    if (scanf("%d", &accountID) != 1) {
        printf("잘못된 입력입니다.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    // 계좌 존재 여부 확인
    char checkQuery[256];
    snprintf(checkQuery, sizeof(checkQuery), "SELECT COUNT(*) FROM Account WHERE AccountID = %d", accountID);
    
    if (mysql_query(conn, checkQuery)) {
        fprintf(stderr, "계좌 존재 확인 쿼리 실패: %s\n", mysql_error(conn));
        return;
    }
    
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "결과 저장 실패: %s\n", mysql_error(conn));
        return;
    }
    
    MYSQL_ROW row = mysql_fetch_row(res);
    int count = atoi(row[0]);
    mysql_free_result(res);
    
    if (count == 0) {
        printf("계좌 ID %d는 존재하지 않습니다.\n", accountID);
        return;
    }
    
    // 계좌 삭제
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() 실패\n");
        return;
    }
    
    const char *deleteQuery = "DELETE FROM Account WHERE AccountID = ?";
    if (mysql_stmt_prepare(stmt, deleteQuery, strlen(deleteQuery))) {
        fprintf(stderr, "mysql_stmt_prepare() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }
    
    // 바인딩 초기화
    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));
    
    // 계좌 ID 바인딩
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &accountID;
    
    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "mysql_stmt_bind_param() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }
    
    // 쿼리 실행
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "mysql_stmt_execute() 실패: %s\n", mysql_stmt_error(stmt));
    } else {
        printf("계좌 ID %d가 성공적으로 삭제되었습니다.\n", accountID);
    }
    
    mysql_stmt_close(stmt);
}
