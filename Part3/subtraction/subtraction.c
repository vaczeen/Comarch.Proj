#include <stdio.h>

int subtract(int minuend, int subtrahend) {
    int result = minuend;
    while (subtrahend != 0) {
        result--;
        subtrahend--;
    }
    return result;
}

int main() {
    int minuend = 32766;
    int subtrahend = 10383;

    int result = subtract(minuend, subtrahend);
    printf("The result of %d - %d is %d\n", minuend, subtrahend, result);

    return 0;
}