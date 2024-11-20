// include/customer.h
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <mysql/mysql.h>

// 고객 추가 함수
void addCustomer(MYSQL *conn);

// 고객 조회 함수
void customerLookup(MYSQL *conn);

// 고객의 계좌 조회 함수
void customerAccountLookup(MYSQL *conn);

#endif // CUSTOMER_H
