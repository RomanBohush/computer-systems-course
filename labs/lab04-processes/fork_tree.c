#include <stdio.h>
#include <unistd.h>

int main(void)
{
    fork();
    fork();

    sleep(1);
    printf("PID=%d PPID=%d\n", getpid(), getppid());

    return 0;
}
