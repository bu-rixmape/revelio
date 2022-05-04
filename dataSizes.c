/*
 *  Filename:
 *      dataTypes.c
 *
 *  Purpose:
 *      To display the sizes of C data types for reference purposes.
 *
 *  Modifications:
 *      07 April 2022 - created
 */

#include <stdio.h>
#include <limits.h>

int main(int argc, char *argv[])
{
    puts("SIZES OF INTEGER DATA TYPES");
    printf("int: %zu bytes\n", sizeof(int));
    printf("unsigned int: %zu bytes\n\n", sizeof(unsigned int));

    printf("long: %zu bytes\n", sizeof(long));
    printf("unsigned long: %zu bytes\n\n", sizeof(unsigned long));

    printf("char: %zu bytes\n", sizeof(char));
    printf("unsigned char: %zu bytes\n\n", sizeof(unsigned char));

    printf("short: %zu bytes\n", sizeof(short));
    printf("unsigned short: %zu bytes\n\n", sizeof(unsigned short));

    printf("CHAR_BIT: %d bits\n", CHAR_BIT);
}