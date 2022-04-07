/* Filename: pixel_array.c
 * Purpose: Simple manipulation of pixel array.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define HEADERSIZE 54

typedef unsigned char BYTE; // sizeof(unsigned char) == 1

typedef struct
{
    FILE *filePtr;
    int width;
    int height;
    int bitDepth;
    BYTE header[HEADERSIZE];
    BYTE *colorTable;
    BYTE *pixels;
} BMP;

void openImage(BMP *img);
void getProperties(BMP *img);
void printProperties(BMP img, FILE *stream);
void saveNewImage(BMP img);
void closeImage(BMP *img);

int main(void)
{
    BMP image;              // BMP structure for input image
    BMP *imagePtr = &image; // Pointer to BMP structure

    openImage(imagePtr); // Obtain file pointer

    getProperties(imagePtr);        // Pass BMP pointer to modify member values
    printProperties(image, stdout); // Print image properties to stdout
    saveNewImage(image);            // Create new image

    closeImage(imagePtr); // Close input image
}

void openImage(BMP *img)
{
    // Obtain filename of input file
    char imgName[100];
    printf("%s", "Enter filename of input file: ");
    fgets(imgName, 100, stdin);
    imgName[strlen(imgName) - 1] = 0;

    img->filePtr = fopen(imgName, "r"); // Open input file

    // Exit program if opening the file failed
    if (img->filePtr == NULL)
    {
        fprintf(stderr, "error opening input file %s\n", img->filePtr);
        exit(1);
    }
}

void getProperties(BMP *img)
{
    // Obtain content of the image header
    fread(img->header, sizeof(BYTE), HEADERSIZE, img->filePtr);

    // Initialize the structure members of the pointed BMP variable
    img->width = *(int *)&img->header[18];
    img->height = *(int *)&img->header[22];
    img->bitDepth = *(int *)&img->header[28];

    if (img->bitDepth >= 8) // Check if color table exists
    {
        // Allocate the size of pixel array
        int entries = pow(2, img->bitDepth);
        img->colorTable = malloc(entries * sizeof(BYTE));

        // Obtain color table
        fread(img->colorTable, sizeof(BYTE), entries, img->filePtr);
    }

    // Allocate the size of pixel array
    int size = img->width * img->height;
    img->pixels = malloc(size * sizeof(BYTE));

    // Obtain pixel array
    fread(img->pixels, sizeof(BYTE), img->width * img->height, img->filePtr);
}

void printProperties(BMP img, FILE *stream)
{
    // ???: Different printing style for stdout and text file
    if (stream == stdout) {
        puts("\nIMAGE PROPERTIES");
    }

    fprintf(stream, "bitmapWidth: %u\n", img.width);
    fprintf(stream, "bitmapHeight: %u\n", img.height);
    fprintf(stream, "bitDepth: %u\n\n", img.bitDepth);
}

void saveNewImage(BMP img)
{
    // Obtain filename of output file
    char imgName[100];
    printf("%s", "Enter filename of output file: ");
    fgets(imgName, 100, stdin);
    imgName[strlen(imgName) - 1] = 0;

    FILE *imgOut = fopen(imgName, "wb"); // Open output file

    fwrite(img.header, sizeof(BYTE), HEADERSIZE, imgOut);
    fwrite(img.colorTable, sizeof(BYTE), pow(2, img.bitDepth), imgOut);
    fwrite(img.pixels, sizeof(BYTE), img.width * img.height, imgOut);

    fclose(imgOut); // Close output file
}

void closeImage(BMP *img)
{
    free(img->colorTable);
    free(img->pixels);
    fclose(img->filePtr);
}