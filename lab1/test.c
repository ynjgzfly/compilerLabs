#include <stdio.h>
#include <sys/types.h>
#include <sys/unistd.h>

int main()
{

    while (1)
    {
        fork();
    }

    return 0;
}
