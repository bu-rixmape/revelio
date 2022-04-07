/* Filename: duplicate.c
 * Purpose: To duplicate a BMP file
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define HEADERSIZE 54
#define TABLESIZE 1024
#define FNAMELIMIT 100

typedef unsigned char BYTE; // sizeof(unsigned char) == 1

typedef struct
{
    FILE *fileHandle;
    BYTE header[HEADERSIZE];
    BYTE colorTable[TABLESIZE];
    BYTE *pixels;
    int width;
    int height;
    int bitDepth;
} BMP;

void setProperties(BMP *image);
void showProperties(BMP image);
void duplicate(BMP image);

int main(void)
{
    char *fname;                                    // Define character pointer for filename
    printf("%s", "Enter filename of input file: "); // Display user prompt
    fgets(fname, FNAMELIMIT, stdin);                // Obtain filename of input file'
    fname[strlen(fname) - 1] = 0;                   // Remove trailing whitespace

    BMP image;                            // Define BMP variable for input file
    image.fileHandle = fopen(fname, "r"); // Open input file

    // Exit program if opening the file failed
    if (image.fileHandle == NULL)
    {
        fprintf(stderr, "error opening file %s in function main()\n", fname);
        exit(1);
    }

    setProperties(&image); // Pass image by reference to modify member values
    showProperties(image);
    duplicate(image);

    fclose(image.fileHandle);
}

// Read the properties of the image
void setProperties(BMP *img)
{
    // Store content of the image header
    fread(img->header,
          sizeof(BYTE),
          HEADERSIZE,
          img->fileHandle);

    // Initialize the structure members of the pointed BMP variable
    img->width = *(int *)&img->header[18];
    img->height = *(int *)&img->header[22];
    img->bitDepth = *(int *)&img->header[28];

    // Store content of the color table, if it exists
    if (img->bitDepth >= 8)
    {
        fread(img->colorTable,
              sizeof(BYTE),
              pow(2, img->bitDepth),
              img->fileHandle);
    }

    // Allocate the size of pixel array
    int size = img->width * img->height;
    img->pixels = malloc(size * sizeof(BYTE));

    // Store pixel array
    fread(img->pixels,
          sizeof(BYTE),
          img->width * img->height,
          img->fileHandle);
}

void showProperties(BMP img)
{
    puts("\nIMAGE PROPERTIES");
    printf(" | Bitmap Width: %u\n", img.width);
    printf(" | Bitmap Height: %u\n", img.height);
    printf(" | Bit Depth: %u\n", img.bitDepth);
}

void duplicate(BMP img)
{
    FILE *imgOut = fopen("out2.bmp", "wb");
    fwrite(img.header, sizeof(BYTE), HEADERSIZE, imgOut);
    fwrite(img.colorTable, sizeof(BYTE), pow(2, img.bitDepth), imgOut);
    fwrite(img.pixels, sizeof(BYTE), img.width * img.height, imgOut);

    fclose(imgOut);
}