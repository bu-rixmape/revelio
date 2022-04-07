#include <stdio.h>

int main(void)
{
    printf("%15s %5zu bytes\n", "int", sizeof(int));
    printf("%15s %5zu bytes\n\n", "unsigned int", sizeof(unsigned int));

    printf("%15s %5zu bytes\n", "long", sizeof(long));
    printf("%15s %5zu bytes\n\n", "unsigned long", sizeof(unsigned long));

    printf("%15s %5zu bytes\n", "char", sizeof(char));
    printf("%15s %5zu bytes\n\n", "unsigned char", sizeof(unsigned char));
}