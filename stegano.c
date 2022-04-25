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

// void parseArgs(void)
// {
//     char *cover = NULL;   // Filename of cover image
//     char *message = NULL; // Filename of secret message
//     char *stego = NULL;   // Filename of stego image

//     int option; // Next option argument in the argument list

//     // Parses command line arguments
//     while ((option = getopt(argc, argv, ":c:s:m:")) != -1)
//     {
//         switch (option)
//         {
//         case 'c':
//             cover = optarg; // Filename of cover image
//             break;

//         case 's':
//             stego = optarg; // Filename of stego image
//             break;

//         case 'm':
//             message = optarg; // Filename of secret message
//             break;

//         case ':': // Required option with missing option argument
//             printf("missing '%c' command line argument\n", optopt);
//             break;

//         case '?': // Unknown option character
//             printf("unknown '%c' option character\n", optopt);
//             break;
//         }
//     }

//     // Check if required option arguments are obtained
//     if (cover == NULL)
//     {
//         fprintf(stderr, "%s", "no cover image provided in main()\n");
//         exit(1);
//     }

//     if (message == NULL)
//     {
//         fprintf(stderr, "%s", "no secret message provided in main()\n");
//         exit(1);
//     }

//     if (stego == NULL)
//     {
//         stego = "stegoImage.bmp"; // Default filename for stego image
//     }
// }

// Opens a bitmap image indicated by fname. Returns pointer to
// BMP structure of the image.
BMP *loadImage(const char *fname)
{
    // Terminates program if image file format is not bitmap
    const char *extension = strrchr(fname, '.');
    if (strcmp(extension, ".bmp"))
    {
        fprintf(stderr,
                "image %s is not a bitmap file in loadImage()\n",
                fname);
        exit(1);
    }

    BMP *imgPtr = malloc(sizeof(BMP));   // Allocates memory for cover image
    imgPtr->filePtr = fopen(fname, "r"); // Opens image

    // Terminates program if opening the image failed
    if (imgPtr->filePtr == NULL)
    {
        fprintf(stderr,
                "error opening image %s in loadImage()\n",
                imgPtr->filePtr);
        free(imgPtr); // Free allocated memory for cover image
        exit(1);
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
    // Terminates program if image file format is not bitmap
    const char *dot = strrchr(fname, '.');
    if (strcmp(dot, ".bmp"))
    {
        fprintf(stderr,
                "image %s is not a bitmap file in createStego()\n",
                fname);
        exit(1);
    }

    FILE *imgOut = fopen(fname, "wb"); // Opens stego image

    // Writes the modified bitmap file structure into the stego image
    fwrite(img.header, sizeof(*img.header), img.headerSize, imgOut);
    fwrite(img.colorTable, sizeof(*img.colorTable), img.colorCount, imgOut);
    fwrite(img.pxArrMod, sizeof(*img.pxArrMod), img.pxArrSize, imgOut);

    fclose(imgOut);
}

// Frees memory allocated for BMP structure pointed to by imgPtr. Returns none.
void freeImage(BMP *imgPtr)
{
    // Deallocates the space previously allocated by malloc()
    free(imgPtr->header);
    free(imgPtr->pxArr);
    free(imgPtr->pxArrMod);

    if (imgPtr->colorTable != NULL) // Checks if color table exists
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
    FILE *textPtr = openText(fname, *imgPtr);

    int px = 0; // Counter for pixels

    // Index of last pixel in first row of pixel array
    int lastPx = imgPtr->width - 1;

    // Loops through each character in secret text
    while (!feof(textPtr))
    {
        char character = getc(textPtr); // Obtains character

        if (character > ASCII_MAX)
        {
            fprintf(stderr,
                    "character %c is not in ASCII character set\n",
                    character);
            exit(1);
        }

        char mask = 1 << (CHAR_BIT - 1); // Bit mask for 8-bit character

        // Loop through each binary digit of the 8-bit character
        for (unsigned int i = 1; i <= CHAR_BIT; i++)
        {
            // Determines value of bit
            if (character & mask) // Bit is 1
            {
                // Increases pixel value by CHANGE_VALUE
                imgPtr->pxArrMod[px] += CHANGE_VALUE;

                // Set higher limit for pixel value according to color depth
                int maxPxValue = imgPtr->colorCount - 1;
                if (imgPtr->pxArrMod[px] > maxPxValue)
                {
                    imgPtr->pxArrMod[px] = maxPxValue;
                }
            }
            else // Bit is 0
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
        exit(1);
    }

    // Counts character in secret text
    unsigned int charCount = 0;
    while (!feof(filePtr))
    {
        char character = getc(filePtr); // Obtain character

        // Terminates program if character is not representable with 8 bits
        if (character >= pow(2, CHAR_BIT))
        {
            fprintf(stderr,
                    "secret text %s contains invalid character\n",
                    fname);
            exit(1);
        }

        charCount++;
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
        exit(1);
    }

    return filePtr;
}

// Writes secret text into file indicated by fname based from stego image
// stegImg and cover image origImg. Returns none.
void decodeText(BMP origImg, BMP stegImg, const char *fname)
{
    // Checks if pixel array for cover image and stego image are the same
    if (origImg.pxArrSize != stegImg.pxArrSize)
    {
        fprintf(stderr,
                "%s",
                "different cover image and stego image in decodeText()\n");
        exit(1);
    }

    FILE *text = fopen(fname, "w"); // Opens file for secret text

    // Terminates program if file could not be created
    if (text == NULL)
    {
        fprintf(stderr, "%s",
                "secret text could not be created in decodeText()\n");
        exit(1);
    }

    // Allocates memory for array of pixel value differences
    int *diffs = malloc(stegImg.pxArrSize * sizeof(*diffs));

    // Initializes the index of last pixel in first row of pixel array
    int lastPx = stegImg.width - 1;

    int decodedBit = 0; // Counter for decoded bit of 8-bit character
    int character = 0;  // ASCII decimal value of decoded character

    // Loop through each pixel in pixel Array
    for (size_t px = 0; px < stegImg.pxArrSize; px++)
    {
        // Compute the change between values of corresponding
        // pixel in cover and stego image
        int diff = stegImg.pxArr[px] - origImg.pxArr[px];

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

        // Updates value of current character
        character += pow(2, 8 - (decodedBit + 1)) * diffs[px];
        decodedBit++; // Increments number of decoded bit for current character

        // Checks if current character is fully decoded
        if ((decodedBit != 0) && (decodedBit % CHAR_BIT == 0))
        {
            // Stops decoding until non-ASCII character
            if (character < ASCII_MIN && character > ASCII_MAX)
            {
                break;
            }

            fputc(character, text); // Display character
            decodedBit = 0;         // Reset counter for decoded bit
            character = 0;          // Reset ASCII value for next character
        }

        // Skip padding in pixel array
        if (px == lastPx)
        {
            px += stegImg.padding; // Index of first pixel in next row

            // Index of last pixel in next row
            lastPx += stegImg.width + stegImg.padding;
        }
    }

    free(diffs);
    fclose(text);
}