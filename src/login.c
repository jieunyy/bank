// login.c

#include "login.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>

// 로그인 함수
int login(MYSQL *conn) {
    char username[100], password[50];
    int userID = -1; // 로그인 실패를 나타내는 초기 값

    printf("\n--- 로그인 ---\n");

    printf("사용자 이름: ");
    if (fgets(username, sizeof(username), stdin) == NULL) {
        fprintf(stderr, "사용자 이름 입력 실패\n");
        return userID;
    }
    username[strcspn(username, "\n")] = 0; // 개행 문자 제거

    printf("비밀번호: ");
    if (fgets(password, sizeof(password), stdin) == NULL) {
        fprintf(stderr, "비밀번호 입력 실패\n");
        return userID;
    }
    password[strcspn(password, "\n")] = 0; // 개행 문자 제거

    // 준비된 문장 초기화
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() 실패\n");
        return userID;
    }

    // SQL 쿼리 수정: PasswordHash 대신 Password 사용
    const char *query = "SELECT UserID FROM Users WHERE UserName = ? AND Password = ?";
    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "mysql_stmt_prepare() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return userID;
    }

    // 바인딩 초기화
    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    // 사용자 이름 바인딩
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = username;
    bind[0].buffer_length = strlen(username);

    // 비밀번호 바인딩
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = password;
    bind[1].buffer_length = strlen(password);

    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "mysql_stmt_bind_param() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return userID;
    }

    // 쿼리 실행
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "mysql_stmt_execute() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return userID;
    }

    // 결과 바인딩
    MYSQL_BIND result_bind[1];
    memset(result_bind, 0, sizeof(result_bind));

    result_bind[0].buffer_type = MYSQL_TYPE_LONG;
    result_bind[0].buffer = &userID;
    result_bind[0].is_null = 0;
    result_bind[0].length = 0;

    if (mysql_stmt_bind_result(stmt, result_bind)) {
        fprintf(stderr, "mysql_stmt_bind_result() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return userID;
    }

    if (mysql_stmt_store_result(stmt)) {
        fprintf(stderr, "mysql_stmt_store_result() 실패: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return userID;
    }

    // 결과 가져오기
    if (mysql_stmt_fetch(stmt) == 0) {
        printf("로그인 성공! 사용자 ID: %d\n", userID);
    } else {
        printf("로그인 실패: 사용자 이름 또는 비밀번호가 올바르지 않습니다.\n");
    }

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);
    return userID;
}
