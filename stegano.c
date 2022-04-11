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
 *      11 April 2022 - used memcpy() in obtaining image properties
 */

#include "stegano.h"

// Loads an image indicated by fname into memory. Returns pointer to
// BMP structure of the image.
BMP *loadImage(char *fname)
{
    BMP *imgPtr = malloc(sizeof(BMP));   // Allocate memory for cover image
    imgPtr->filePtr = fopen(fname, "r"); // Open cover image

    // Exit program if opening the image failed
    if (imgPtr->filePtr == NULL)
    {
        fprintf(stderr,
                "error opening input file %s in function loadImage()\n",
                imgPtr->filePtr);
        free(imgPtr); // Free allocated memory for cover image
        exit(1);
    }

    setProperties(imgPtr);
    return imgPtr;
}

// Initializes structure members of the pointed BMP structure.
void setProperties(BMP *imgPtr)
{
    // Obtains DIB header size
    fseek(imgPtr->filePtr, FILEHEADER_SIZE, SEEK_SET);
    fread(&(imgPtr->headerSize), sizeof(DWORD), 1, imgPtr->filePtr);

    imgPtr->headerSize += FILEHEADER_SIZE; // Total size of image header

    // Allocate memory for image header
    imgPtr->header = malloc(imgPtr->headerSize * sizeof(BYTE));

    // Obtain content of the image header
    fseek(imgPtr->filePtr, 0, SEEK_SET);
    fread(imgPtr->header, sizeof(BYTE), imgPtr->headerSize, imgPtr->filePtr);

    memcpy(&imgPtr->width, &imgPtr->header[18], sizeof(DWORD));  // Bitmap width in pixels
    memcpy(&imgPtr->height, &imgPtr->header[22], sizeof(DWORD)); // Bitmap height in pixels
    imgPtr->pxCount = imgPtr->width * imgPtr->height;            // Number of visible pixels

    memcpy(&imgPtr->bitDepth, &imgPtr->header[28], sizeof(DWORD)); // Color depth of image
    imgPtr->colorCount = pow(2, imgPtr->bitDepth);                 // Entries in color pallete

    if (imgPtr->bitDepth >= 8) // Check if color table exists
    {
        // Allocate the size of array for color table
        imgPtr->colorTable = malloc(imgPtr->colorCount * sizeof(BYTE));

        // Obtain color table
        fread(imgPtr->colorTable, sizeof(BYTE),
              imgPtr->colorCount, imgPtr->filePtr);
    }

    // Determines size of entire pixel array in bytes
    DWORD pxRowSize = ceil((float)(imgPtr->bitDepth * imgPtr->width) / 32) * 4;
    DWORD pxArraySize = pxRowSize * imgPtr->height;
    imgPtr->pxArraySize = pxArraySize;

    // Allocate the size of pixel array
    imgPtr->pxArray = malloc(pxArraySize * sizeof(BYTE));

    // Obtain pixel array
    fread(imgPtr->pxArray, sizeof(BYTE), pxArraySize, imgPtr->filePtr);

    imgPtr->padding = pxRowSize - imgPtr->width; // Padding in pixel array
}

// Prints image properties of cover image.
void printProperties(BMP img)
{
    puts("IMAGE PROPERTIES");
    printf("%-14s: %d bytes\n", "Header size", img.headerSize);
    printf("%-14s: %d pixels\n", "Image width", img.width);
    printf("%-14s: %d pixels\n", "Image height", img.height);
    printf("%-14s: %d pixels\n", "Pixel count", img.pxCount);
    printf("%-14s: %d bits\n", "Bit depth", img.bitDepth);
    printf("%-14s: %d colors\n", "Color count", img.colorCount);
    printf("%-14s: %d bytes\n", "Pixel array", img.pxArraySize);
    printf("%-14s: %d byte\n", "Padding", img.padding);
}

// Creates stego image indicated by fname from the modified bitmap file
// structure of cover image containing the secret message.
void createStego(char *fname, BMP img)
{
    FILE *imgOut = fopen(fname, "wb");

    // Writes the bitmap file structure into the stegoImg
    fwrite(img.header, sizeof(BYTE), img.headerSize, imgOut);
    fwrite(img.colorTable, sizeof(BYTE), img.colorCount, imgOut);
    fwrite(img.pxArray, sizeof(BYTE), img.pxCount, imgOut);

    fclose(imgOut);
}

// Frees the memory allocated to BMP structure pointed to by imgPtr.
void freeImage(BMP *imgPtr)
{
    // Deallocates the space previously allocated by malloc().
    free(imgPtr->header);
    free(imgPtr->colorTable);
    free(imgPtr->pxArray);

    fclose(imgPtr->filePtr); // Closes the file pointer of the cover image
    free(imgPtr);            // Frees the memory allocated to the BMP structure
}

// Encodes secret message indicated by fname into the BMP structure pointed
// to by imgPtr.
void encodeMessage(char *fname, BMP *imgPtr)
{
    FILE *msgPtr = openMessage(fname, imgPtr);

    int px = 0; // Counter for pixels

    // Index of last pixel in first row of pixel array
    int lastPx = imgPtr->width - 1;

    // Loops through each character in secret message
    while (!feof(msgPtr))
    {
        char character = getc(msgPtr);   // Obtains character
        char mask = 1 << (CHAR_BIT - 1); // Bit mask for 8-bit character

        // Loop through each binary digit of the 8-bit character
        for (unsigned int i = 1; i <= CHAR_BIT; i++)
        {
            // Determines value of bit
            if (character & mask) // Bit is 1
            {
                // Increases pixel value by CHANGE_VALUE
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
                // Avoids negative pixel value
                if (imgPtr->pxArray[px] < CHANGE_VALUE)
                {
                    // Sets lower limit for pixel value to 0
                    imgPtr->pxArray[px] = 0;
                }
                else
                {
                    // Decreases pixel value by CHANGE_VALUE
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

            character <<= 1; // Shift bits of character by 1 bit to the right
        }
    }
}

// Opens secret message indicated by fname.
FILE *openMessage(char *fname, BMP *imgPtr)
{
    FILE *filePtr = fopen(fname, "r"); // Open input file

    // Exit program if opening file failed
    if (filePtr == NULL)
    {
        fprintf(stderr,
                "error opening secret message %s in encodeMessage()\n",
                fname);
        exit(1);
    }

    // Count character in secret message
    unsigned int charCount = 0;
    while (!feof(filePtr))
    {
        char character = getc(filePtr); // Obtain character
        charCount++;
    }
    fseek(filePtr, 0, SEEK_SET);

    // Each pixel in the image represents a binary digit from the
    // 8-bit (1 byte) character in the secret message.
    unsigned int pxNeed = charCount * 8;

    // Check if secret message can fit in the image
    if (pxNeed > imgPtr->pxCount)
    {
        fprintf(stderr,
                "Secret message %s has too many characters\n"
                "%u pixels is needed but cover image only has %u pixels.\n",
                fname, pxNeed, imgPtr->pxCount);
        exit(1);
    }

    return filePtr;
}

// Decodes stego image stegImg based from cover image origImg.
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

    int bitChar = 0;   // Counter for decoded bit of 8-bit character
    int character = 0; // ASCII decimal value of decoded character

    // Loop through each pixel in pixel Array
    for (size_t px = 0; px < stegImg.pxArraySize; px++)
    {
        // Compute the change between values of corresponding
        // pixel in cover and stego image
        int diff = stegImg.pxArray[px] - origImg.pxArray[px];

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

        // Update ASCII value of current character
        character += pow(2, 8 - (bitChar + 1)) * diffs[px];
        bitChar++; // Increment number of decoded bit for current character

        // Check if a character is fully decoded
        if (bitChar != 0 && bitChar % CHAR_BIT == 0)
        {
            // Terminate loop if last character is reached
            if (character == 0)
            {
                break;
            }

            printf("%c", character); // Display character
            bitChar = 0;             // Reset counter for decoded bit
            character = 0;           // Reset ASCII value for next character
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