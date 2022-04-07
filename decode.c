#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define HEADER_SIZE 54
#define CHANGE_VALUE 3

typedef unsigned char BYTE; // sizeof(unsigned char) == 1

typedef struct
{
    FILE *filePtr;
    int width;
    int height;
    int area;
    int bitDepth;
    BYTE header[HEADER_SIZE];
    BYTE *colorTable;
    BYTE *pixels;
} BMP;

void openImage(char *filename, BMP *img);
void decodeMessage(BMP origImg, BMP newImg);
void closeImage(BMP *img);

int main(void)
{
    BMP origImage;                       // BMP structure for input image
    BMP *origImagePtr = &origImage;      // Pointer to BMP structure
    openImage("orig.bmp", origImagePtr); // Open input image

    BMP newImage;                      // BMP structure for input image
    BMP *newImagePtr = &newImage;      // Pointer to BMP structure
    openImage("new.bmp", newImagePtr); // Open input image

    decodeMessage(origImage, newImage);

    closeImage(newImagePtr);
    closeImage(origImagePtr);
}

// Opens an image indicated by filename and stores the image properties to
// the BMP structure pointed to by img. Returns none.
void openImage(char *filename, BMP *img)
{
    void getProperties(BMP * img);

    img->filePtr = fopen(filename, "r"); // Open input image

    // Exit program if opening the image failed
    if (img->filePtr == NULL)
    {
        fprintf(stderr,
                "error opening input file %s in function openImage()\n",
                img->filePtr);
        exit(1);
    }

    getProperties(img);
}

// Stores image properties to the BMP structure pointed to by img. Returns none.
void getProperties(BMP *img)
{
    // Obtain content of the image header
    fread(img->header, sizeof(BYTE), HEADER_SIZE, img->filePtr);

    // Initialize the structure members of the pointed BMP structure
    img->width = *(int *)&img->header[18];
    img->height = *(int *)&img->header[22];
    img->area = img->width * img->height;
    img->bitDepth = *(int *)&img->header[28];

    if (img->bitDepth >= 8) // Check if color table exists
    {
        // Allocate the size of array for color table
        int entries = pow(2, img->bitDepth);
        img->colorTable = malloc(entries * sizeof(BYTE));

        // Obtain color table
        fread(img->colorTable, sizeof(BYTE), entries, img->filePtr);
    }

    // Allocate the size of array for pixel values
    img->pixels = malloc(img->area * sizeof(BYTE));

    // Obtain pixel values
    fread(img->pixels, sizeof(BYTE), img->width * img->height, img->filePtr);
}

// Closes the input image. Returns none.
void closeImage(BMP *img)
{
    // Deallocates the space previously allocated by malloc().
    free(img->colorTable);
    free(img->pixels);

    // Closes the file pointer of the input image
    fclose(img->filePtr);
}

void decodeMessage(BMP origImg, BMP newImg)
{

    int diffs[newImg.area];

    // Loop through each pixel
    for (size_t i = 0; i < newImg.area; i++)
    {
        // Determine the change in pixel value
        int diff = origImg.pixels[i] - newImg.pixels[i];

        // printf("origPx: %-3dnewPx: %-3dchangePx: %-3d\n", origImg.pixels[i],
        //        newImg.pixels[i], diff);

        if (diff < 0)
        {
            // Negative difference indicates that the original pixel value is
            // increased suggesting 1 bit.
            diffs[i] = 1;
        }
        else if (diff >= 0)
        {
            diffs[i] = 0;
        }
    }

    for (size_t i = 0; i < newImg.area; i += 8)
    {
        // Obtain corresponding ASCII decimal value
        int character = (pow(2, 7) * diffs[0]) +
                        (pow(2, 6) * diffs[i + 1]) +
                        (pow(2, 5) * diffs[i + 2]) +
                        (pow(2, 4) * diffs[i + 3]) +
                        (pow(2, 3) * diffs[i + 4]) +
                        (pow(2, 2) * diffs[i + 5]) +
                        (pow(2, 1) * diffs[i + 6]) +
                        (pow(2, 0) * diffs[i + 7]);

        printf("%c", character);
    }
}