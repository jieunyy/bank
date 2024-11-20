// customer.c

#include "customer.h"
#include "utils.h" // utils.h 포함
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mysql/mysql.h>

// OpenSSL EVP 헤더 포함
#include <openssl/evp.h>
#include <openssl/err.h>

// 기존 코드에서 SHA 관련 함수 제거 및 EVP API 사용

// 고객 추가 함수
void addCustomer(MYSQL *conn) {
    char name[100], phone[20], email[100], address[255];
    
    printf("\n--- 고객 추가 ---\n");
    
    printf("이름: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;
    
    printf("전화번호: ");
    fgets(phone, sizeof(phone), stdin);
    phone[strcspn(phone, "\n")] = 0;
    
    printf("이메일: ");
    fgets(email, sizeof(email), stdin);
    email[strcspn(email, "\n")] = 0;
    
    printf("주소: ");
    fgets(address, sizeof(address), stdin);
    address[strcspn(address, "\n")] = 0;

    
    // 준비된 문장 초기화
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() 실패\n");
        return;
    }
    
    const char *query = "INSERT INTO Customer (Name, PhoneNumber, Email, Address, RegistrationDate) VALUES (?, ?, ?, ?, NOW())";
    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "mysql_stmt_prepare() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }
    
    // 바인딩 초기화
    MYSQL_BIND bind[5];
    memset(bind, 0, sizeof(bind));
    
    // 이름 바인딩
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = name;
    bind[0].buffer_length = strlen(name);
    
    // 전화번호 바인딩
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = phone;
    bind[1].buffer_length = strlen(phone);
    
    // 이메일 바인딩
    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = email;
    bind[2].buffer_length = strlen(email);
    
    // 주소 바인딩
    bind[3].buffer_type = MYSQL_TYPE_STRING;
    bind[3].buffer = address;
    bind[3].buffer_length = strlen(address);
    
    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "mysql_stmt_bind_param() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }
    
    // 쿼리 실행
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "mysql_stmt_execute() 실패: %s\n", mysql_stmt_error(stmt));
    } else {
        printf("고객이 성공적으로 추가되었습니다.\n");
    }
    
    mysql_stmt_close(stmt);
}

// 고객 조회 함수
void customerLookup(MYSQL *conn) {
    int customerID;
    printf("\n--- 고객 조회 ---\n");
    printf("조회할 고객의 ID를 입력하세요: ");
    if (scanf("%d", &customerID) != 1) {
        printf("잘못된 입력입니다.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    MYSQL_STMT *stmt;
    MYSQL_BIND bind[1];
    MYSQL_BIND result_bind[5];
    char name[100], phone[20], email[100], address[255];
    char registrationDate[20];
    unsigned long length[5];
    bool is_null[5];

    stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() 실패\n");
        return;
    }

    const char *query = "SELECT Name, PhoneNumber, Email, Address, RegistrationDate FROM Customer WHERE CustomerID = ?";
    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "mysql_stmt_prepare() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }

    memset(bind, 0, sizeof(bind));

    // CustomerID 바인딩
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &customerID;
    bind[0].is_null = 0;
    bind[0].length = 0;

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

    // 결과 바인딩
    result_bind[0].buffer_type = MYSQL_TYPE_STRING;
    result_bind[0].buffer = name;
    result_bind[0].buffer_length = sizeof(name);
    result_bind[0].is_null = &is_null[0];
    result_bind[0].length = &length[0];

    result_bind[1].buffer_type = MYSQL_TYPE_STRING;
    result_bind[1].buffer = phone;
    result_bind[1].buffer_length = sizeof(phone);
    result_bind[1].is_null = &is_null[1];
    result_bind[1].length = &length[1];

    result_bind[2].buffer_type = MYSQL_TYPE_STRING;
    result_bind[2].buffer = email;
    result_bind[2].buffer_length = sizeof(email);
    result_bind[2].is_null = &is_null[2];
    result_bind[2].length = &length[2];

    result_bind[3].buffer_type = MYSQL_TYPE_STRING;
    result_bind[3].buffer = address;
    result_bind[3].buffer_length = sizeof(address);
    result_bind[3].is_null = &is_null[3];
    result_bind[3].length = &length[3];

    result_bind[4].buffer_type = MYSQL_TYPE_STRING;
    result_bind[4].buffer = registrationDate;
    result_bind[4].buffer_length = sizeof(registrationDate);
    result_bind[4].is_null = &is_null[4];
    result_bind[4].length = &length[4];

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
        printf("\n--- 고객 정보 ---\n");
        printf("이름: %s\n", is_null[0] ? "NULL" : name);
        printf("전화번호: %s\n", is_null[1] ? "NULL" : phone);
        printf("이메일: %s\n", is_null[2] ? "NULL" : email);
        printf("주소: %s\n", is_null[3] ? "NULL" : address);
        printf("등록일: %s\n", is_null[4] ? "NULL" : registrationDate);
    } else {
        printf("해당 ID의 고객을 찾을 수 없습니다.\n");
    }

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);
}

// 고객의 계좌 조회 함수
void customerAccountLookup(MYSQL *conn) {
    int customerID;
    printf("\n--- 고객 계좌 조회 ---\n");
    printf("고객 ID를 입력하세요: ");
    if (scanf("%d", &customerID) != 1) {
        printf("잘못된 입력입니다.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    MYSQL_STMT *stmt;
    MYSQL_BIND bind[1];
    MYSQL_BIND result_bind[3];
    char accountNumber[20], accountType[50];
    double balance;
    unsigned long length[3];
    bool is_null[3];

    stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() 실패\n");
        return;
    }

    const char *query = "SELECT AccountNumber, AccountType, Balance FROM Account WHERE CustomerID = ?";
    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "mysql_stmt_prepare() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }

    memset(bind, 0, sizeof(bind));

    // CustomerID 바인딩
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &customerID;
    bind[0].is_null = 0;
    bind[0].length = 0;

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

    // 결과 바인딩
    result_bind[0].buffer_type = MYSQL_TYPE_STRING;
    result_bind[0].buffer = accountNumber;
    result_bind[0].buffer_length = sizeof(accountNumber);
    result_bind[0].is_null = &is_null[0];
    result_bind[0].length = &length[0];

    result_bind[1].buffer_type = MYSQL_TYPE_STRING;
    result_bind[1].buffer = accountType;
    result_bind[1].buffer_length = sizeof(accountType);
    result_bind[1].is_null = &is_null[1];
    result_bind[1].length = &length[1];

    result_bind[2].buffer_type = MYSQL_TYPE_DOUBLE;
    result_bind[2].buffer = &balance;
    result_bind[2].is_null = &is_null[2];
    result_bind[2].length = &length[2];

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

    printf("\n--- 계좌 정보 ---\n");
    printf("AccountNumber | AccountType | Balance\n");
    printf("--------------------------------------\n");

    bool hasResults = false;

    while (mysql_stmt_fetch(stmt) == 0) {
        hasResults = true;
        printf("%-13s | %-11s | %.2f\n", accountNumber, accountType, balance);
    }

    if (!hasResults) {
        printf("해당 고객의 계좌 정보가 없습니다.\n");
    }

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);
}
