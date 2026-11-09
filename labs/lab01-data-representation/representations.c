#include <stdio.h>

int main(void)
{
    unsigned int values[] = {10, 65, 127, 255, 256, 1024};

    for (int i = 0; i < 6; i++) {
        printf("Decimal: %4u   Hex: 0x%X\n", values[i], values[i]);
    }

    return 0;
}
