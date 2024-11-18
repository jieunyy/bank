#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <mysql/mysql.h>

// 거래 내역 조회 함수
void transactionLookup(MYSQL *conn);

#endif // TRANSACTION_H
