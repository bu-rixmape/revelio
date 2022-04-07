/* Filename: pixel_array.c
 * Purpose: Simple manipulation of pixel array.
 */

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
void printProperties(BMP img, FILE *stream);

void hideMessage(BMP *img);

void saveNewImage(char *filename, BMP img);
void closeImage(BMP *img);

int main(void)
{
    BMP image;              // BMP structure for input image
    BMP *imagePtr = &image; // Pointer to BMP structure

    // Obtain filename of input image
    char input[100];
    printf("%s", "Enter filename of input image: ");
    fgets(input, 100, stdin);
    input[strlen(input) - 1] = 0;

    openImage(input, imagePtr);     // Open input image
    printProperties(image, stdout); // Display image properties

    hideMessage(imagePtr);

    // Obtain filename of output image
    char output[100];
    printf("%s", "Enter filename of output file: ");
    fgets(output, 100, stdin);
    output[strlen(output) - 1] = 0;

    saveNewImage(output, image); // Create new image

    closeImage(imagePtr); // Close input image
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

// Print properties of input image to stream
void printProperties(BMP img, FILE *stream)
{
    // ???: Different printing style for stdout and text file
    if (stream == stdout)
    {
        puts("\nIMAGE PROPERTIES");
    }

    fprintf(stream, "bitmapWidth: %u\n", img.width);
    fprintf(stream, "bitmapHeight: %u\n", img.height);
    fprintf(stream, "bitDepth: %u\n\n", img.bitDepth);
}

// Opens an image indicated by filename and writes the bitmap file structure
// of the edited input image. Returns none.
void saveNewImage(char *filename, BMP img)
{
    FILE *imgOut = fopen(filename, "wb"); // Open output file

    // Write the edited bitmap file structure
    fwrite(img.header, sizeof(BYTE), HEADER_SIZE, imgOut);
    fwrite(img.colorTable, sizeof(BYTE), pow(2, img.bitDepth), imgOut);
    fwrite(img.pixels, sizeof(BYTE), img.area, imgOut);

    fclose(imgOut); // Close output file
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

void hideMessage(BMP *img)
{
    FILE *openMessage(void);
    FILE *message = openMessage();

    // Count character in text message
    int count = 0;
    while (!feof(message))
    {
        char character = getc(message); // Obtain character
        count++;
    }
    fseek(message, 0, SEEK_SET);
    printf("Character count: %d\n", count);

    // Each pixel in the image will hold a single binary digit of each
    // 4-byte (8 bits) character in the text message.
    int neededPixels = count * 8;
    printf("Needed pixels: %d\n\n", neededPixels);

    // Check if message can fit in the image
    if (neededPixels <= img->area)
    {
        int pxIndex = 0;

        // Loop through each character in the text message
        while (!feof(message))
        {
            char character = getc(message);

            char mask = 1 << 7; // Mask for 8-bit character

            // Loop through each binary place value of the character
            for (unsigned int i = 1; i <= 8; i++)
            {
                if (character & mask)
                {
                    // Increase pixel value by CHANGE_VALUE if
                    // current binary place value contains 1
                    img->pixels[pxIndex] += CHANGE_VALUE;

                    // Set higher limit for pixel value according to bitDepth
                    // int pxUpLim = pow(2, img->bitDepth) - 1;
                    if (img->pixels[pxIndex] > 255)
                    {
                        img->pixels[pxIndex] = 255;
                    }
                }
                else
                {
                    // Set lower limit for pixel value to 0
                    if (img->pixels[pxIndex] < 3)
                    {
                        img->pixels[pxIndex] = 0;
                    }
                    else
                    {
                        // Decrease pixel value by CHANGE_VALUE if
                        // current binary place value contains 1
                        img->pixels[pxIndex] -= CHANGE_VALUE;
                    }
                }

                pxIndex++;       // Increment index to refer to next pixel
                character <<= 1; // Shift bits of character 1 bit to the right
            }
        }
    }
}

FILE *openMessage(void)
{
    // Obtain filename of text message
    char filename[100];
    printf("%s", "Enter filename of text message: ");
    fgets(filename, 100, stdin);
    filename[strlen(filename) - 1] = 0;

    // Open input file
    FILE *filePtr;
    filePtr = fopen(filename, "r");

    // Exit program if opening file failed
    if (filePtr == NULL)
    {
        fprintf(stderr,
                "error opening input file %s in function openMessage()\n",
                filename);
        exit(1);
    }
}