// src/customer.c
#include "customer.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>  // bool 타입을 사용하기 위해 추가

// 고객 등록 함수
void addCustomer(MYSQL *conn) {
    char name[100], phone[20], email[100], address[255];
    
    printf("\n--- 고객 등록 ---\n");
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
    MYSQL_BIND bind[4];
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
        printf("고객이 성공적으로 등록되었습니다.\n");
    }
    
    mysql_stmt_close(stmt);
}

// 고객 조회 함수
void customerLookup(MYSQL *conn) {
    char input[100];
    printf("\n--- 고객 조회 ---\n");
    printf("고객 ID 또는 이름을 입력하세요: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    MYSQL_STMT *stmt;
    MYSQL_BIND bind[1];
    MYSQL_BIND result_bind[6];
    int customerID;
    char name[100];
    char phoneNumber[20];
    char email[100];
    char address[255];
    char registrationDate[20];
    unsigned long length[6];
    bool is_null[6];  // my_bool을 bool로 변경

    stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() 실패\n");
        return;
    }

    const char *query = "SELECT CustomerID, Name, PhoneNumber, Email, Address, RegistrationDate FROM Customer WHERE CustomerID = ? OR Name LIKE ?";
    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "mysql_stmt_prepare() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return;
    }

    memset(bind, 0, sizeof(bind));

    // 고객 ID 또는 이름 바인딩
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = input;
    bind[0].buffer_length = strlen(input);

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
    result_bind[0].buffer_type = MYSQL_TYPE_LONG;
    result_bind[0].buffer = &customerID;
    result_bind[0].is_null = &is_null[0];
    result_bind[0].length = &length[0];

    result_bind[1].buffer_type = MYSQL_TYPE_STRING;
    result_bind[1].buffer = name;
    result_bind[1].buffer_length = sizeof(name);
    result_bind[1].is_null = &is_null[1];
    result_bind[1].length = &length[1];

    result_bind[2].buffer_type = MYSQL_TYPE_STRING;
    result_bind[2].buffer = phoneNumber;
    result_bind[2].buffer_length = sizeof(phoneNumber);
    result_bind[2].is_null = &is_null[2];
    result_bind[2].length = &length[2];

    result_bind[3].buffer_type = MYSQL_TYPE_STRING;
    result_bind[3].buffer = email;
    result_bind[3].buffer_length = sizeof(email);
    result_bind[3].is_null = &is_null[3];
    result_bind[3].length = &length[3];

    result_bind[4].buffer_type = MYSQL_TYPE_STRING;
    result_bind[4].buffer = address;
    result_bind[4].buffer_length = sizeof(address);
    result_bind[4].is_null = &is_null[4];
    result_bind[4].length = &length[4];

    result_bind[5].buffer_type = MYSQL_TYPE_STRING;
    result_bind[5].buffer = registrationDate;
    result_bind[5].buffer_length = sizeof(registrationDate);
    result_bind[5].is_null = &is_null[5];
    result_bind[5].length = &length[5];

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

    printf("\n--- 고객 정보 ---\n");
    printf("CustomerID | Name       | PhoneNumber   | Email             | Address             | RegistrationDate\n");
    printf("---------------------------------------------------------------------------------------------\n");

    while (mysql_stmt_fetch(stmt) == 0) {
        printf("%-10d | %-10s | %-13s | %-18s | %-20s | %-15s\n",
               customerID,
               name,
               phoneNumber,
               email,
               address,
               registrationDate);
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
    int accountNumber;
    char accountType[50];
    double balance;
    unsigned long length[3];
    bool is_null[3];  // my_bool을 bool로 변경

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
    result_bind[0].buffer_type = MYSQL_TYPE_LONG;
    result_bind[0].buffer = &accountNumber;
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

    while (mysql_stmt_fetch(stmt) == 0) {
        printf("%-13d | %-11s | %.2f\n", accountNumber, accountType, balance);
    }

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);
}
