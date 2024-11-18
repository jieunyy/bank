// include/report.h
#ifndef REPORT_H
#define REPORT_H

#include <mysql/mysql.h>

// 월별 거래 리포트 생성 함수
void generateMonthlyTransactionReport(MYSQL *conn, int year, int month);

// 고객 요약 리포트 생성 함수
void generateCustomerSummaryReport(MYSQL *conn);

#endif // REPORT_H
