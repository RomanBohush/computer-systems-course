#include <stdio.h>

int process(int a, int b)
{
    int result = 0;

    if (a > b) {
        result = a - b;
    } else {
        result = a + b;
    }

    for (int i = 0; i < 3; i++) {
        result = result * 2;
    }

    return result;
}

int main(void)
{
    printf("%d\n", process(10, 4));

    return 0;
}
