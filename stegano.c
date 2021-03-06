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

// Checks validity of filename according to its file extension.
// Returns 0 on success.
int verifyFilename(const char *fname, const char *extension, const char *caller)
{
    const char *suffix = strrchr(fname, '.'); // Obtains file extension

    // Terminates program if filename has no file extension
    if (suffix == NULL)
    {
        clearTerminal();
        fprintf(stderr,
                "invalid filename: %s has no file extension in %s\n",
                fname, caller);
        exit(EXIT_FAILURE);
    }

    // Terminates program if file format is not bitmap
    else if (strcmp(suffix, extension))
    {
        clearTerminal();
        fprintf(stderr,
                "invalid file format: %s is not a %s file in %s\n",
                fname, extension, caller);
        exit(EXIT_FAILURE);
    }

    return 0; // Filename is valid
}

// Deletes content of terminal. Returns none.
void clearTerminal(void)
{
    printf("%s", "\033[H\033[J"); // Clears content of terminal
    fflush(stdout);               // Flushes buffer of stdout
}

// Displays ASCII-art, which is stored in text file indicated by fname,
// in terminal. Returns 0 on success.
int showBackground(const char *fname)
{
    verifyFilename(fname, ".txt", "showBackground");

    clearTerminal();
    FILE *art = fopen(fname, "r"); // Opens text file containing ASCII art

    // Terminates program if opening file failed
    if (art == NULL)
    {
        fprintf(stderr,
                "fopen() failed: %s could not be opened in showBackground()\n",
                fname);
        exit(EXIT_FAILURE); // Returns control to operating system
    }

    int ch;                         // Next character in ASCII art
    while ((ch = getc(art)) != EOF) // Obtains character in ASCII art until EOF
    {
        putchar(ch); // Displays character
    }

    fclose(art); // Closes text file containing ASCII art

    return 0;
}

// Opens a bitmap image indicated by fname. Returns pointer to
// BMP structure of the image.
BMP *loadImage(const char *fname)
{
    int storeProperties(BMP * imgPtr); // Function prototype

    verifyFilename(fname, ".bmp", "loadImage()");

    BMP *imgPtr = malloc(sizeof(BMP));   // Allocates memory for cover image
    imgPtr->filePtr = fopen(fname, "r"); // Opens image indicated by fname

    // Terminates program if opening the image failed
    if (imgPtr->filePtr == NULL)
    {
        clearTerminal();
        fprintf(stderr,
                "fopen() failed: %s could not be opened in loadImage()\n",
                fname);
        free(imgPtr); // Free allocated memory for cover image
        exit(EXIT_FAILURE);
    }

    storeProperties(imgPtr); // Initialize structure members
    return imgPtr;
}

// Initializes structure members representing image properties of BMP structure
// pointed to by imgPtr. Returns 0 on success.
int storeProperties(BMP *imgPtr)
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

    return 0; // Structure members for image properties successfully initialized
}

// Prints image properties of img in stdin. Returns 0 on success.
int printProperties(BMP img)
{
    clearTerminal();
    puts("IMAGE PROPERTIES");
    printf("%-14s: %d bytes\n", "Header size", img.headerSize);
    printf("%-14s: %d pixels\n", "Image width", img.width);
    printf("%-14s: %d pixels\n", "Image height", img.height);
    printf("%-14s: %d bits\n", "Bit depth", img.bitDepth);
    printf("%-14s: %d colors\n", "Color count", img.colorCount);
    printf("%-14s: %d bytes\n", "Pixel array", img.pxArrSize);
    printf("%-14s: %d byte\n", "Padding", img.padding);

    return 0;
}

// Creates stego image indicated by fname from the modified bitmap file
// structure of cover image indicated by img. Returns 0 on success.
int createStego(const char *fname, BMP img)
{
    verifyFilename(fname, ".bmp", "createStego()");

    FILE *imgOut = fopen(fname, "wb"); // Opens stego image

    // Uses the modified file structure of cover image to create stego image
    fwrite(img.header, sizeof(*img.header), img.headerSize, imgOut);
    fwrite(img.colorTable, sizeof(*img.colorTable), img.colorCount, imgOut);
    fwrite(img.pxArrMod, sizeof(*img.pxArrMod), img.pxArrSize, imgOut);

    fclose(imgOut);

    return 0; // Stego image is successfully created
}

// Releases memory allocated for BMP structure pointed to by imgPtr. 
// Returns 0 on success.
int freeImage(BMP *imgPtr)
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

    return 0;
}

// Encodes secret text in the file indicated by fname into the
// BMP structure pointed to by imgPtr. Returns none.
int encodeText(const char *fname, BMP *imgPtr)
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
            clearTerminal();
            fprintf(stderr,
                    "warning: skipped encoding character '%c' in encodeText()\n",
                    character);
            exit(EXIT_FAILURE);
        }

        char mask = 1 << (CHAR_BIT - 1); // Bit mask for 8-bit character

        // Loop through each bit of the 8-bit character
        for (unsigned int i = 0; i < CHAR_BIT; i++)
        {
            if (character & mask) // Current bit is 1
            {
                // Increases pixel value by 1
                imgPtr->pxArrMod[px] += 1;

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
                if (imgPtr->pxArrMod[px] < 1)
                {
                    // Sets lower limit for pixel value to 0
                    imgPtr->pxArrMod[px] = 0;
                }
                else
                {
                    // Decreases pixel value by 1
                    imgPtr->pxArrMod[px] -= 1;
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

    return 0; // Secret text succesfully encoded
}

// Opens secret text indicated by fname. Returns file pointer for secret text.
FILE *openText(const char *fname, BMP img)
{
    verifyFilename(fname, ".txt", "openText()");

    FILE *filePtr = fopen(fname, "r"); // Open input file

    // Terminates program if opening file failed
    if (filePtr == NULL)
    {
        clearTerminal();
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
            clearTerminal();
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
    unsigned int pxNeed = charCount * CHAR_BIT;

    // Computes total number of pixels used by the image
    DWORD pxTotal = img.width * img.height;

    // Terminates program if secret text cannot fit in cover image
    if (pxNeed > pxTotal)
    {
        clearTerminal();
        fprintf(stderr,
                "secret text %s has too many characters\n"
                "%u pixels is needed but cover image only has %u pixels.\n",
                fname, pxNeed, pxTotal);
        exit(EXIT_FAILURE);
    }

    return filePtr;
}

// Writes the secret text intothe  text file indicated by fname. Secret text is
// decoded from stego image stegImg and cover image covImg. Returns none.
int decodeText(BMP covImg, BMP stegImg, const char *fname)
{
    // Terminates program if pixel array size of cover and stego image
    // are not equal. Suggests that the images are not related to each other.
    if (covImg.pxArrSize != stegImg.pxArrSize)
    {
        clearTerminal();
        fprintf(stderr,
                "%s",
                "incompatible files: different cover image and stego "
                "image in decodeText()\n");
        exit(EXIT_FAILURE);
    }

    verifyFilename(fname, ".txt", "decodeText()");

    FILE *decodedTxt = fopen(fname, "w"); // Opens file for decoded text

    // Terminates program if file could not be created
    if (decodedTxt == NULL)
    {
        clearTerminal();
        fprintf(stderr,
                "fopen error: decoded text %s could not be created in "
                "decodeText()\n",
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
        diff = stegImg.pxArr[px] - covImg.pxArr[px];

        // Determines the bit represented by the difference
        if (diff >= 1) // Positive difference
        {
            bit = 1; // Current pixel stores a bit value of 1
        }
        else if (diff <= 0) // Positive or zero difference
        {
            bit = 0; // Current pixel stores a bit value of 0
        }

        // Computes the ASCII value of the current character
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

            fputc(character, decodedTxt); // Display character into the decoded text file
            decodedBit = 0;               // Reset counter for decoded bit
            character = 0;                // Reset ASCII decimal value
        }

        // Checks if the current pixel is the last pixel of current row
        if (px == lastPx)
        {
            // Skip padding to proceed to the first pixel of next row
            px += stegImg.padding;

            // Update index of last pixel of next row
            lastPx += stegImg.width + stegImg.padding;
        }
    }

    fclose(decodedTxt);

    return 0; // Secret text successfully decoded
}