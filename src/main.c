// main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "login.h"
#include "customer.h"
#include "account.h"
#include "transaction.h"
#include "expense.h"
#include "report.h"
#include "utils.h"

// 데이터베이스 접속 정보 정의
#define HOST "localhost"
#define USER "root"
#define PASS "123456"
#define DB "BankManagementSystem"

int main() {
    MYSQL *conn;

    // MySQL 연결 초기화 및 설정
    conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() 실패\n");
        exit(EXIT_FAILURE);
    }

    // 데이터베이스에 연결
    if (mysql_real_connect(conn, HOST, USER, PASS, DB, 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() 실패: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }

    // 로그인 절차
    int userID = login(conn);
    if (userID == -1) {
        printf("로그인 실패: 사용자 이름 또는 비밀번호가 올바르지 않습니다.\n");
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }

    // 로그인 성공 시 사용자 ID를 사용할 수 있습니다.
    // 예: printf("로그인 성공! 사용자 ID: %d\n", userID);

    int choice;
    while (1) {
        printf("\n--- 은행 관리 시스템 ---\n");
        printf("1. 고객 조회\n");
        printf("2. 고객 추가\n");
        printf("3. 고객 계좌 조회\n");
        printf("4. 계좌 개설\n");
        printf("5. 계좌 삭제\n");
        printf("6. 거래 내역 추가\n");
        printf("7. 월간 고객 거래 리포트 생성\n");
        printf("8. 고객 요약 리포트 생성\n");
        printf("0. 종료\n");
        printf("선택: ");
        if (scanf("%d", &choice) != 1) {
            printf("입력 오류. 숫자를 입력하세요.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();  // 입력 버퍼 클리어

        switch (choice) {
            case 1:
                customerLookup(conn);
                break;
            case 2:
                addCustomer(conn);
                break;
            case 3:
                customerAccountLookup(conn);
                break;
            case 4:
                openAccount(conn);
                break;
            case 5:
                deleteAccount(conn);
                break;
            case 6:
                addExpense(conn);
                break;
            case 7: {
                int year, month;
                printf("리포트를 생성할 연도와 월을 입력하세요 (예: 2024 11): ");
                if (scanf("%d %d", &year, &month) != 2) {
                    printf("입력 오류. 연도와 월을 숫자로 입력하세요.\n");
                    clearInputBuffer();
                    break;
                }
                clearInputBuffer();
                generateMonthlyTransactionReport(conn, year, month);
                break;
            }
            case 8:
                generateCustomerSummaryReport(conn);
                break;
            case 0:
                printf("프로그램을 종료합니다. 안녕히 가세요!\n");
                mysql_close(conn);
                exit(EXIT_SUCCESS);
            default:
                printf("잘못된 선택입니다. 다시 시도하세요.\n");
        }
    }

    mysql_close(conn);
    return 0;
}
