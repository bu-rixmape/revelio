/*
 *  Filename:
 *      stegano.c
 *
 *  Purpose:
 *      To define functions for decoding and encoding messages
 *      into and from an image, respectively.
 *
 *  Modifications:
 *      08 April 2022 - created
 *      09 April 2022 - modified identifiers and functions
 *      10 April 2022 - modified encodeText() to skip padding
 *      11 April 2022 - used memcpy() in obtaining image properties
 *      12 April 2022 - modified decodeText() to write file for secret message
 */

#include "stegano.h"

// Displays ASCII-art for terminal background. Returns none.
void showBackground(const char *fname)
{
    printf("%s", "\033[H\033[J");  // Clears terminal screen
    fflush(stdout);                // Flushes stdout's buffer
    FILE *art = fopen(fname, "r"); // Opens text file containing ASCII art

    // Terminates program if opening file failed
    if (art == NULL)
    {
        fprintf(stderr,
                "fopen() failed: %s could not be opened in showBackground()",
                fname);
        exit(EXIT_FAILURE); // Return control to operating system
    }

    int ch;                         // Next character in ASCII art
    while ((ch = getc(art)) != EOF) // Obtains character in ASCII art until EOF
    {
        putchar(ch); // Displays character
    }

    fclose(art); // Closes text file containing ASCII art
}

// Opens a bitmap image indicated by fname. Returns pointer to
// BMP structure of the image.
BMP *loadImage(const char *fname)
{
    void storeProperties(BMP * imgPtr); // Function prototype

    const char *extension = strrchr(fname, '.'); // Obatins file extension

    // Terminates program if file format is not bitmap
    if (strcmp(extension, ".bmp"))
    {
        fprintf(stderr,
                "invalid file format: %s is not a bitmap file in loadImage()",
                fname);
        exit(EXIT_FAILURE);
    }

    BMP *imgPtr = malloc(sizeof(BMP));   // Allocates memory for cover image
    imgPtr->filePtr = fopen(fname, "r"); // Opens image indicated by fname

    // Terminates program if opening the image failed
    if (imgPtr->filePtr == NULL)
    {
        fprintf(stderr,
                "fopen() failed: %s could not be opened in loadImage()",
                fname);
        free(imgPtr); // Free allocated memory for cover image
        exit(EXIT_FAILURE);
    }

    storeProperties(imgPtr); // Initialize structure members
    return imgPtr;
}

// Stores properties of the image represented by the BMP structure
// pointed to by imgPtr. Returns none.
void storeProperties(BMP *imgPtr)
{
    // Obtains DIB header size measured in bytes
    fseek(imgPtr->filePtr, FILEHEADER_SIZE, SEEK_SET);
    fread(&imgPtr->headerSize, sizeof(imgPtr->headerSize), 1, imgPtr->filePtr);

    // Sets total size of image header measured in bytes
    imgPtr->headerSize += FILEHEADER_SIZE;

    // Allocates memory for image header
    imgPtr->header = malloc(imgPtr->headerSize);

    // Obtains content of image header
    rewind(imgPtr->filePtr);
    fread(imgPtr->header, sizeof(*imgPtr->header),
          imgPtr->headerSize, imgPtr->filePtr);

    // Obtains color depth measured in bits
    memcpy(&imgPtr->bitDepth, &imgPtr->header[28], sizeof(imgPtr->bitDepth));

    // Computes total number of entries in color pallete
    imgPtr->colorCount = pow(2, imgPtr->bitDepth);

    if (imgPtr->bitDepth >= 8) // Checks if color table exists
    {
        // Allocates memory for color table
        imgPtr->colorTable = malloc(imgPtr->colorCount *
                                    sizeof(*imgPtr->colorTable));

        // Note: sizeof(*imgPtr->colorTable) evaluates to the size (in bytes)
        // of the data type being pointed to by imgPtr->colorTable.

        // Obtains color table
        fread(imgPtr->colorTable, sizeof(*imgPtr->colorTable),
              imgPtr->colorCount, imgPtr->filePtr);
    }

    // Obtains image width and height measured in pixels
    memcpy(&imgPtr->width, &imgPtr->header[18], sizeof(imgPtr->width));
    memcpy(&imgPtr->height, &imgPtr->header[22], sizeof(imgPtr->height));

    // Computes size of pixel array measured in bytes
    DWORD pxRowSize = ceil((float)(imgPtr->bitDepth * imgPtr->width) / 32) * 4;
    imgPtr->pxArrSize = (pxRowSize * imgPtr->height);

    // Allocates memory for pixel array
    imgPtr->pxArr = malloc(imgPtr->pxArrSize);

    // Obtains file offset of pixel array
    fseek(imgPtr->filePtr, 10, SEEK_SET);
    DWORD pxArrOffset;
    fread(&pxArrOffset, sizeof(pxArrOffset), 1, imgPtr->filePtr);

    // Obtains content of pixel array
    fseek(imgPtr->filePtr, pxArrOffset, SEEK_SET);
    fread(imgPtr->pxArr, sizeof(*imgPtr->pxArr),
          imgPtr->pxArrSize, imgPtr->filePtr);

    // Creates copy of pixel array to be used for encoding secret text
    imgPtr->pxArrMod = malloc(imgPtr->pxArrSize);
    memcpy(imgPtr->pxArrMod, imgPtr->pxArr, imgPtr->pxArrSize);

    // Computes padding in pixel array
    imgPtr->padding = pxRowSize - imgPtr->width;
}

// Prints image properties of BMP structure img. Returns none.
void printProperties(BMP img)
{
    puts("IMAGE PROPERTIES");
    printf("%-14s: %d bytes\n", "Header size", img.headerSize);
    printf("%-14s: %d pixels\n", "Image width", img.width);
    printf("%-14s: %d pixels\n", "Image height", img.height);
    printf("%-14s: %d bits\n", "Bit depth", img.bitDepth);
    printf("%-14s: %d colors\n", "Color count", img.colorCount);
    printf("%-14s: %d bytes\n", "Pixel array", img.pxArrSize);
    printf("%-14s: %d byte\n", "Padding", img.padding);
}

// Creates stego image indicated by fname. Uses the modified bitmap file
// structure of cover image that contains the secret text. Returns None.
void createStego(const char *fname, BMP img)
{
    const char *extension = strrchr(fname, '.'); // Obatins file extension

    // Terminates program if file format is not bitmap
    if (strcmp(extension, ".bmp"))
    {
        fprintf(stderr,
                "invalid file format: %s is not a bitmap file in creatStego()",
                fname);
        exit(EXIT_FAILURE);
    }

    FILE *imgOut = fopen(fname, "wb"); // Opens stego image

    // Uses the modified file structure of cover image to create stego image
    fwrite(img.header, sizeof(*img.header), img.headerSize, imgOut);
    fwrite(img.colorTable, sizeof(*img.colorTable), img.colorCount, imgOut);
    fwrite(img.pxArrMod, sizeof(*img.pxArrMod), img.pxArrSize, imgOut);

    fclose(imgOut);
}

// Frees memory allocated for BMP structure pointed to by imgPtr. Returns none.
void freeImage(BMP *imgPtr)
{
    // Deallocates the memory previously allocated by malloc()
    free(imgPtr->header);
    free(imgPtr->pxArr);
    free(imgPtr->pxArrMod);

    // Deallocates memory used by color table if it exists
    if (imgPtr->colorTable != NULL)
    {
        free(imgPtr->colorTable);
    }

    fclose(imgPtr->filePtr); // Closes the file pointer for cover image
    free(imgPtr);            // Deallocates memory for BMP structure
}

// Encodes secret text indicated by fname into the BMP structure pointed
// to by imgPtr. Returns none.
void encodeText(const char *fname, BMP *imgPtr)
{
    FILE *openText(const char *fname, BMP img); // Function prototype

    FILE *textPtr = openText(fname, *imgPtr); // Opens secret text

    // Index of last pixel in first row of pixel array
    size_t lastPx = imgPtr->width - 1;

    size_t px = 0;  // Index of modified pixel
    char character; // Next character in secret text

    // Loops through each character in secret text until eof
    while ((character = getc(textPtr)) != EOF)
    {
        // Skips encoding non-ASCII character
        if (character < ASCII_MIN || character > ASCII_MAX)
        {
            fprintf(stderr,
                    "warning: skipped encoding character '%c' in encodeText()",
                    character);
            exit(EXIT_FAILURE);
        }

        char mask = 1 << (CHAR_BIT - 1); // Bit mask for 8-bit character

        // Loop through each bit of the 8-bit character
        for (unsigned int i = 0; i < CHAR_BIT; i++)
        {
            if (character & mask) // Current bit is 1
            {
                // Increases pixel value by CHANGE_VALUE
                imgPtr->pxArrMod[px] += CHANGE_VALUE;

                // Sets higher limit for pixel value according to color depth
                int maxPxValue = imgPtr->colorCount - 1;
                if (imgPtr->pxArrMod[px] > maxPxValue)
                {
                    imgPtr->pxArrMod[px] = maxPxValue;
                }
            }
            else // Current bit is 0
            {
                // Avoids negative pixel value
                if (imgPtr->pxArrMod[px] < CHANGE_VALUE)
                {
                    // Sets lower limit for pixel value to 0
                    imgPtr->pxArrMod[px] = 0;
                }
                else
                {
                    // Decreases pixel value by CHANGE_VALUE
                    imgPtr->pxArrMod[px] -= CHANGE_VALUE;
                }
            }

            // Skip padding in pixel array
            if (px == lastPx)
            {
                // Index of first pixel in next row
                px += imgPtr->padding + 1;

                // Index of last pixel in next row
                lastPx += imgPtr->width + imgPtr->padding;
            }
            else
            {
                px++; // Set index of next pixel in current row
            }

            character <<= 1; // Shift bits of character by 1 bit to the right
        }
    }

    fclose(textPtr); // Closes secret text
}

// Opens secret text indicated by fname. Returns file pointer for secret text.
FILE *openText(const char *fname, BMP img)
{
    FILE *filePtr = fopen(fname, "r"); // Open input file

    // Terminates program if opening file failed
    if (filePtr == NULL)
    {
        fprintf(stderr,
                "error opening secret text %s in openText()\n",
                fname);
        exit(EXIT_FAILURE);
    }

    unsigned int charCount = 0; // Character counter
    int character;              // Character in secret text

    // Loop through each character in secret text
    while ((character = getc(filePtr)) != EOF)
    {
        // Terminates program if text contains non-ASCII character
        if (character < ASCII_MIN || character > ASCII_MAX)
        {
            fprintf(stderr,
                    "file error: %s contains non-ASCII character\n",
                    fname);
            exit(EXIT_FAILURE);
        }

        charCount++; // Increment character counter
    }
    rewind(filePtr); // Sets file pointer at the beginning of file stream

    // Computes number of pixels needed to represent a binary digit
    // of each 8-bit (1 byte) character in secret text
    unsigned int pxNeed = charCount * 8;

    // Computes total number of pixels used by the image
    DWORD pxTotal = img.width * img.height;

    // Terminates program if secret text cannot fit in cover image
    if (pxNeed > pxTotal)
    {
        fprintf(stderr,
                "secret text %s has too many characters\n"
                "%u pixels is needed but cover image only has %u pixels.\n",
                fname, pxNeed, pxTotal);
        exit(EXIT_FAILURE);
    }

    return filePtr;
}

// Writes secret text into file indicated by fname based from stego image
// stegImg and cover image origImg. Returns none.
void decodeText(BMP origImg, BMP stegImg, const char *fname)
{
    // Terminates program if pixel array size of cover and stego image
    // are not equal. Suggests that the images are not related to each other.
    if (origImg.pxArrSize != stegImg.pxArrSize)
    {
        fprintf(stderr,
                "%s",
                "different cover image and stego image in decodeText()\n");
        exit(EXIT_FAILURE);
    }

    FILE *decodedTxt = fopen(fname, "w"); // Opens file for decoded text

    // Terminates program if file could not be created
    if (decodedTxt == NULL)
    {
        fprintf(stderr,
                "decoded text %s could not be created in decodeText()\n",
                fname);
        exit(EXIT_FAILURE);
    }

    // Initializes the index of last pixel in first row of pixel array
    size_t lastPx = stegImg.width - 1;

    size_t decodedBit = 0; // Counter for decoded bit of 8-bit character
    char character = 0;    // ASCII decimal value of decoded character

    int diff; // Difference of corresponding pixels in cover and stego image
    int bit;  // Bit secretly stored in a pixel

    // Loop through each pixel of cover and stego image
    for (size_t px = 0; px < stegImg.pxArrSize; px++)
    {
        // Computes difference of corresponding pixels
        diff = stegImg.pxArr[px] - origImg.pxArr[px];

        // Dettermines the bit represented by the difference
        if (diff >= CHANGE_VALUE) // Positive difference
        {
            bit = 1; // Current pixel stores a bit value of 1
        }
        else if (diff <= 0) // Positive or zero difference
        {
            bit = 0; // Current pixel stores a bit value of 0
        }

        // Updates value of current character
        character += (char)pow(2, 8 - (decodedBit + 1)) * bit;
        decodedBit++; // Increments number of decoded bit for current character

        // Checks if current character is fully decoded
        if ((decodedBit != 0) && (decodedBit % CHAR_BIT == 0))
        {
            // Skips decoding null or non-ASCII character
            if (character <= ASCII_MIN || character > ASCII_MAX)
            {
                break;
            }

            fputc(character, decodedTxt); // Display character
            decodedBit = 0;               // Reset counter for decoded bit
            character = 0;                // Reset ASCII decimal value
        }

        // Skip padding in pixel array
        if (px == lastPx)
        {
            // Index of first pixel in next row
            px += stegImg.padding;

            // Index of last pixel in next row
            lastPx += stegImg.width + stegImg.padding;
        }
    }

    fclose(decodedTxt);
}