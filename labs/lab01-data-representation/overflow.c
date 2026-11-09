#include <stdio.h>
#include <stdint.h>

int main(void)
{
    uint8_t value = 254;

    for (int i = 0; i < 4; i++) {
        printf("value = %u\n", value);
        value++;
    }

    return 0;
}
