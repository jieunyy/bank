#include "utils.h"
#include <stdio.h>

// 입력 버퍼 클리어 함수 구현
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
