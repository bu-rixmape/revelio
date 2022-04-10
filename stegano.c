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
 *      10 April 2022 - modified encodeMessage() to skip padding
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
    imgPtr->headerSize += 14;                                        // File header

    // Allocate memory for image header
    imgPtr->header = malloc(imgPtr->headerSize * sizeof(BYTE));

    // Obtain content of the image header
    fseek(imgPtr->filePtr, 0, SEEK_SET);
    fread(imgPtr->header, sizeof(BYTE), imgPtr->headerSize, imgPtr->filePtr);

    imgPtr->width = *(int *)&imgPtr->header[18];      // Bitmap width in pixels
    imgPtr->height = *(int *)&imgPtr->header[22];     // Bitmap height in pixels
    imgPtr->pxCount = imgPtr->width * imgPtr->height; // Number of visible pixels
    imgPtr->bitDepth = *(int *)&imgPtr->header[28];   // Color depth of image
    imgPtr->colorCount = pow(2, imgPtr->bitDepth);    // Colors in the color pallete

    if (imgPtr->bitDepth >= 8) // Check if color table exists
    {
        // Allocate the size of array for color table
        imgPtr->colorTable = malloc(imgPtr->colorCount * sizeof(BYTE));

        // Obtain color table
        fread(imgPtr->colorTable, sizeof(BYTE), imgPtr->colorCount, imgPtr->filePtr);
    }

    // Determines size of entire pixel array
    DWORD pxRowSize = ceil((float)(imgPtr->bitDepth * imgPtr->width) / 32) * 4;
    DWORD pxArraySize = pxRowSize * imgPtr->height;
    imgPtr->pxArraySize = pxArraySize;


    // Allocate the size of pixel array
    imgPtr->pxArray = malloc(pxArraySize * sizeof(BYTE));

    // Obtain pixel array
    fread(imgPtr->pxArray, sizeof(BYTE), pxArraySize, imgPtr->filePtr);

    imgPtr->padding = pxRowSize - imgPtr->width; // Padding in pixel array
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
    fprintf(stdout, "img.padding: %d\n", img.padding);
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
    FILE *msgPtr = openMessage(fname, imgPtr);

    int px = 0; // Counter for pixels in pixel array

    // Initializes the index of last pixel in first row of pixel array
    int lastPx = imgPtr->width - 1;

    // Loop through each character in text message
    while (!feof(msgPtr))
    {
        char character = getc(msgPtr); // Obtain character
        char mask = 1 << 7;            // Bit mask for 8-bit character

        // Loop through each binary digit of the 8-bit character
        for (unsigned int i = 1; i <= 8; i++)
        {
            // Determines value of bit
            if (character & mask) // Bit is 1
            {
                // Increase pixel value by CHANGE_VALUE
                imgPtr->pxArray[px] += CHANGE_VALUE;
    
                // Set higher limit for pixel value according to color depth
                int maxPxValue = imgPtr->colorCount - 1;
                if (imgPtr->pxArray[px] > maxPxValue)
                {
                    imgPtr->pxArray[px] = maxPxValue;
                }
            }
            else // Bit is 0
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

            // Skip padding in pixel array
            if (px == lastPx)
            {
                px += imgPtr->padding + 1; // Index of next pixel in next row
                
                // Index of the last pixel in next row
                lastPx += imgPtr->width + imgPtr->padding;
            }
            else
            {
                px++; // Set index of next pixel in the same row
            }

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
    unsigned int pxNeed = count * 8;

    // Check if message can fit in the image
    if (pxNeed > imgPtr->pxCount)
    {
        fprintf(stderr, "Secret message %s is too large", fname);
        exit(1);
    }

    return filePtr;
}

void decodeMessage(BMP origImg, BMP stegImg)
{
    if (origImg.pxArraySize != stegImg.pxArraySize)
    {
        fprintf(stderr,
                "%s",
                "pixel array for cover and stego images are different");
        exit(1);
    }

    // Allocate memory for array of pixel value differences
    int *diffs = malloc(stegImg.pxArraySize * sizeof(int));

    // Initializes the index of last pixel in first row of pixel array
    int lastPx = stegImg.width - 1;

    int bitChar = 0; // Counter for decoded bit of 8-bit character
    int character = 0; // ASCII decimal value of decoded character

    // Loop through each pixel in pixel Array
    for (size_t px = 0; px < stegImg.pxArraySize; px++)
    {
        // Compute the change between values of corresponding
        // pixel in cover and stego image
        int diff = stegImg.pxArray[px] - origImg.pxArray[px];

        // if (px < 32)
        // {
        //     printf("(%4d) origPx - newPx = %3d - %3d = %3d   ", px,
        //             origImg.pxArray[px], stegImg.pxArray[px], diff);
        // }

        // Dettermines the bit value represented by the change
        if (diff >= CHANGE_VALUE) // Negative change
        {
            // Negative change in value indicates that the original pixel
            // was increased that suggests a bit value of 1 based from the
            // original encoding algorithm.
            diffs[px] = 1;
        }
        else if (diff <= 0) // Positive or zero change
        {
            diffs[px] = 0;
        }
    
        // if (px < 32)
        // {
        //     printf("diff: %d  ", diffs[px]);
        //     printf("bitChar: %d  ", bitChar);
        //     printf("x: %f\n", pow(2, 8 - (bitChar + 1)) * diffs[px]);
        // }

        // Update ASCII value of current character
        character += pow(2, 8 - (bitChar + 1)) * diffs[px];
        bitChar++; // Increment number of decoded bit for current character

        // Check if a character is fully decoded 
        if (bitChar != 0 && bitChar % 8 == 0)
        {
            printf("%c", character); // Display character
            bitChar = 0; // Reset counter for decoded bit
            character = 0; // Reset ASCII value for next character
        }

        // Skip padding in pixel array
        if (px == lastPx)
        {
            px += stegImg.padding; // Index of next pixel in next row
            // Index of the last pixel in next row
            lastPx += stegImg.width + stegImg.padding;
        }
    }

    free(diffs);
}