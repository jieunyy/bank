// account.h
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <mysql/mysql.h>

// 계좌 개설 함수
void openAccount(MYSQL *conn);

// 계좌 삭제 함수
void deleteAccount(MYSQL *conn);

#endif // ACCOUNT_H
