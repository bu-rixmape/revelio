/*
 *  fname:
 *      stegano.c
 *
 *  Purpose:
 *      To define functions for decoding and encoding messages
 *      into and from an image, respectively.
 *
 *  Modifications:
 *      08 April 2022 - created
 *      09 April 2022 - modified identifiers and functions
 */

#include "stegano.h"

// Loads an image indicated by fname into memory. Returns pointer to
// BMP structure of the image.
BMP *loadImage(char *fname)
{
    BMP *imgPtr = malloc(sizeof(BMP));   // Allocate memory for input image
    imgPtr->filePtr = fopen(fname, "r"); // Open input image

    // Exit program if opening the image failed
    if (imgPtr->filePtr == NULL)
    {
        fprintf(stderr,
                "error opening input file %s in function loadImage()\n",
                imgPtr->filePtr);
        free(imgPtr); // Free allocated memory for input image
        exit(1);
    }

    setProperties(imgPtr);
    return imgPtr;
}

// Initializes structure members of the pointed BMP structure. Returns none.
void setProperties(BMP *imgPtr)
{
    // Obtain enitre bitmap header size
    fseek(imgPtr->filePtr, 14, SEEK_SET);
    fread(&(imgPtr->headerSize), sizeof(DWORD), 1, imgPtr->filePtr); // DIB header
    imgPtr->headerSize += 14; // File header

    // Allocate memory for image header
    imgPtr->header = malloc(imgPtr->headerSize * sizeof(BYTE));

    // Obtain content of the image header
    fseek(imgPtr->filePtr, 0, SEEK_SET);
    fread(imgPtr->header, sizeof(BYTE), imgPtr->headerSize, imgPtr->filePtr);

    // Initialize the structure members of the pointed BMP structure
    imgPtr->width = *(int *)&imgPtr->header[18];
    imgPtr->height = *(int *)&imgPtr->header[22];
    imgPtr->pxCount = imgPtr->width * imgPtr->height;

    imgPtr->bitDepth = *(int *)&imgPtr->header[28];
    imgPtr->colorCount = pow(2, imgPtr->bitDepth);

    if (imgPtr->bitDepth >= 8) // Check if color table exists
    {
        // Allocate the size of array for color table
        imgPtr->colorTable = malloc(imgPtr->colorCount * sizeof(BYTE));

        // Obtain color table
        fread(imgPtr->colorTable, sizeof(BYTE), imgPtr->colorCount, imgPtr->filePtr);
    }

    // Determine size of entire pixel array
    unsigned int pxRowSize = ceil((imgPtr->bitDepth * imgPtr->width) / 32) * 4;
    unsigned int pxArraySize = pxRowSize * imgPtr->height;

    // Allocate the size of pixel array
    imgPtr->pxArray = malloc(pxArraySize * sizeof(BYTE));

    // Obtain pixel array
    fread(imgPtr->pxArray, sizeof(BYTE), pxArraySize, imgPtr->filePtr);
}

// Print properties of input image to stream
void printProperties(BMP img)
{
    fprintf(stdout, "img.headerSize: %d\n", img.headerSize);
    fprintf(stdout, "img.width: %d\n", img.width);
    fprintf(stdout, "img.height: %d\n", img.height);
    fprintf(stdout, "img.pxCount: %d\n", img.pxCount);
    fprintf(stdout, "img.bitDepth: %d\n", img.bitDepth);
    fprintf(stdout, "img.colorCount: %d\n", img.colorCount);
}

// Opens an image indicated by fname and writes the bitmap file structure
// of the edited input image. Returns none.
void saveNewImage(char *fname, BMP img)
{
    FILE *imgOut = fopen(fname, "wb");

    // Write the edited bitmap file structure containing the hidden message
    fwrite(img.header, sizeof(BYTE), img.headerSize, imgOut);
    fwrite(img.colorTable, sizeof(BYTE), img.colorCount, imgOut);
    fwrite(img.pxArray, sizeof(BYTE), img.pxCount, imgOut);

    fclose(imgOut);
}

// Closes the input image. Returns none.
void freeImage(BMP *imgPtr)
{
    // Deallocates the space previously allocated by malloc().
    free(imgPtr->header);
    free(imgPtr->colorTable);
    free(imgPtr->pxArray);

    // Closes the file pointer of the input image
    fclose(imgPtr->filePtr);

    // Frees the memory allocated to the BMP structure
    free(imgPtr);
}

// Encodes a text message indicated by fname into the BMP structure.
void encodeMessage(char *fname, BMP *imgPtr)
{
    FILE *filePtr = openMessage(fname, imgPtr);

    int px = 0; // Counter for pixels in pixel array

    // Loop through each character in the text message
    while (!feof(filePtr))
    {
        char character = getc(filePtr); // Obtain character

        char mask = 1 << 7; // Bit mask for 8-bit character

        // Loop through each binary place value of the character
        for (unsigned int i = 1; i <= 8; i++)
        {
            if (character & mask)
            {
                // Increase pixel value by CHANGE_VALUE if
                // current binary place value contains 1
                imgPtr->pxArray[px] += CHANGE_VALUE;

                // Set higher limit for pixel value according to bitDepth
                int maxPxValue = imgPtr->colorCount - 1;
                if (imgPtr->pxArray[px] > maxPxValue)
                {
                    imgPtr->pxArray[px] = maxPxValue;
                }
            }
            else
            {
                // Set lower limit for pixel value to 0
                if (imgPtr->pxArray[px] < CHANGE_VALUE)
                {
                    imgPtr->pxArray[px] = 0;
                }
                else
                {
                    // Decrease pixel value by CHANGE_VALUE if
                    // current binary place value contains 1
                    imgPtr->pxArray[px] -= CHANGE_VALUE;
                }
            }

            px++;            // Increment index to refer to next pixel
            character <<= 1; // Shift bits of character 1 bit to the right
        }
    }
}

FILE *openMessage(char *fname, BMP *imgPtr)
{
    FILE *filePtr = fopen(fname, "r"); // Open input file

    // Exit program if opening file failed
    if (filePtr == NULL)
    {
        fprintf(stderr,
                "error opening output file %s in function encodeMessage()\n",
                fname);
        exit(1);
    }

    // Count character in text message
    unsigned int count = 0;
    while (!feof(filePtr))
    {
        char character = getc(filePtr); // Obtain character
        count++;
    }
    fseek(filePtr, 0, SEEK_SET);

    // Each pixel in the image will hold a single binary digit of each
    // 4-byte (8 bits) character in the text message.
    unsigned int requiredPxCount = count * 8;

    // Check if message can fit in the image
    if (requiredPxCount > imgPtr->pxCount)
    {
        fprintf(stderr, "Secret message %s is too large", fname);
        exit(1);
    }
}

void decodeMessage(BMP origImg, BMP stegImg)
{
    if (origImg.pxCount != stegImg.pxCount)
    {
        fprintf(stderr, "cover and steg images are different");
        exit(1);
    }

    // int diffs[stegImg.pxCount]; (causes error)
    int *diffs = malloc(stegImg.pxCount * sizeof(int));
    
    // Loop through each pixel
    for (size_t i = 0; i < stegImg.pxCount; i++)
    {
        // Compute the change between values of corresponding
        // pixel in cover and stego image 
        int diff = origImg.pxArray[i] - stegImg.pxArray[i];

        // Dettermines the bit value represented by the change
        if (diff < 0)
        {
            // Negative change in value indicates that the original pixel
            // was increased that suggests a bit value of 1 based from the
            // original encoding algorithm.
            diffs[i] = 1;
        }
        else if (diff >= 0)
        {
            diffs[i] = 0;
        }
    }

    for (size_t i = 0; i < stegImg.pxCount; i += 8)
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

    free(diffs);
} 