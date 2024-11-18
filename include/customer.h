// include/customer.h
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <mysql/mysql.h>

void addCustomer(MYSQL *conn);
void customerLookup(MYSQL *conn);
void customerAccountLookup(MYSQL *conn);  // 함수 선언 추가

#endif // CUSTOMER_H
