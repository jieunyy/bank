#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <mysql/mysql.h>

// 거래 내역 조회 함수
void transactionLookup(MYSQL *conn);

// 거래 내역 추가 함수
void addExpense(MYSQL *conn);

#endif // TRANSACTION_H
