/*  
 *  fname:
 *      properties.c
 *  
 *  Purpose:
 *      To display the properties of a BMP file.
 *  
 *  Modifications:
 *      07 April 2022 - created
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HEADERSIZE 54
#define TABLESIZE 1024
#define FNAMELIMIT 100

typedef unsigned char BYTE;

typedef struct
{
    FILE *fileHandle;
    BYTE header[HEADERSIZE];
    BYTE colorTable[TABLESIZE];
    int width;
    int height;
    int bitDepth;
} BMP;

void setProperties(BMP *image);
void showProperties(BMP image);

int main(void)
{
    BMP image; // Define BMP variable for input file

    char *fname;                                    // Define character pointer for fname
    printf("%s", "Enter fname of input file: "); // Display user prompt
    fgets(fname, FNAMELIMIT, stdin);                // Obtain fname of input file'
    fname[strlen(fname) - 1] = 0;                   // Remove trailing whitespace

    image.fileHandle = fopen(fname, "r"); // Open input file

    // Exit program if opening the file failed
    if (image.fileHandle == NULL)
    {
        fprintf(stderr, "error opening file %s in function main()\n", fname);
        exit(1);
    }

    BMP *imagePtr = &image; // Initialize BMP pointer for input file
    
    setProperties(imagePtr);
    showProperties(image);

    fclose(image.fileHandle);
}

void setProperties(BMP *img)
{
    // Store content of the image header
    fread(img->header,
          sizeof(BYTE),
          HEADERSIZE,
          img->fileHandle);

    // Initialize the structure members of the pointed BMP variable
    memcpy(&img->width, &img->header[18], 4);
    img->height = *(int *)&img->header[22];
    img->bitDepth = *(int *)&img->header[28];

    // Store content of the color table, if it exists
    if (img->bitDepth >= 8)
    {
        fread(img->colorTable,
              sizeof(BYTE),
              1024,
              img->fileHandle);
    }
}

void showProperties(BMP img)
{
    puts("\nIMAGE PROPERTIES");
    printf(" | Bitmap Width: %u\n", img.width);
    printf(" | Bitmap Height: %u\n", img.height);
    printf(" | Bit Depth: %u\n", img.bitDepth);
}