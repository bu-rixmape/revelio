#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    // Obtain filename of text message
    char fileName[100];
    printf("%s", "Enter filename of text message: ");
    fgets(fileName, 100, stdin);
    fileName[strlen(fileName) - 1] = 0;

    // Open input file
    FILE *filePtr;
    filePtr = fopen(fileName, "r");

    // Exit program if opening file failed
    if (filePtr == NULL)
    {
        fprintf(stderr, "file %s could not be opened", fileName);
        exit(1);
    }

    // Count character in text message
    int count = 0;
    while(!feof(filePtr))
    {
        char character = getc(filePtr);
        count++;
    }
    fseek(filePtr, 0, SEEK_SET);
    printf("Character count: %d\n", count);

    // Check if message can fit in the image
    
}