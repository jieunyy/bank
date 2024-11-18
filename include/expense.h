#ifndef EXPENSE_H
#define EXPENSE_H

#include <mysql/mysql.h>

// 지출 내역 조회 함수
void expenseLookup(MYSQL *conn);

#endif // EXPENSE_H
