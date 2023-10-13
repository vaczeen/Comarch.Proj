#include <stdio.h>

int multiply(int mcand, int mplier) {
    int result = 0;
    while (mplier > 0) {
        if (mplier % 2 == 1) {
            result += mcand;
        }
        mcand <<= 1;
        mplier >>= 1;
    }
    return result;
}

int main() {
    int mcand = 32766;
    int mplier = 10383;

    int result = multiply(mcand, mplier);
    printf("The result of %d * %d is %d\n", mcand, mplier, result);

    return 0;
}