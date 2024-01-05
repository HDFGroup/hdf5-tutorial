#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Hello, World!\n");

    if (argc > 1)
    {
        printf("Arguments:\n");
        for (int i = 0; i < argc; ++i)
            printf("argv[%d] = %s\n", i, argv[i]);
    }
    else
        printf("No arguments\n");

    return 0;
}
