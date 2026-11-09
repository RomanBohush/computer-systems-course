#include <stdio.h>

int main(void)
{
    char chars[] = {'A', 'B', 'a', '0', '9'};

    for (int i = 0; i < 5; i++) {
        printf("Character: %c  Decimal: %d  Hex: 0x%X\n",
               chars[i], chars[i], (unsigned char)chars[i]);
    }

    return 0;
}
