#include <stdio.h>

void print_binary(unsigned char value)
{
    for (int i = 7; i >= 0; i--) {
        printf("%u", (value >> i) & 1u);
    }
}

int main(void)
{
    unsigned char values[] = {0, 1, 10, 65, 127, 128, 255};

    for (int i = 0; i < 7; i++) {
        printf("Decimal: %3u  Hex: 0x%02X  Binary: ", values[i], values[i]);
        print_binary(values[i]);
        printf("\n");
    }

    return 0;
}
