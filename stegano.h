/*
 *  Filename:
 *      stegano.h
 *
 *  Purpose:
 *      To declare data structures and function prototypes for decoding and
 *      encoding messages into and from an image, respectively.
 *
 *  Modifications:
 *      08 April 2022 - created
 *      09 April 2022 - modified function prototypes
 *                    - added new data type name
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <wchar.h>

#define FILEHEADER_SIZE 14
#define CHANGE_VALUE 1
#define ASCII_MIN 0
#define ASCII_MAX 127

typedef unsigned char BYTE;  // 1 byte
typedef unsigned int DWORD;  // 4 bytes
typedef unsigned short WORD; // 2 bytes
typedef int LONG;            // 4 bytes

struct bitmap
{
    FILE *filePtr;     // File pointer for the image

    DWORD headerSize;  // Size of the image header in bytes
    LONG width;        // Width of image in pixels
    LONG height;       // Height of image in pixels
    WORD bitDepth;     // Number of bits per pixel
    DWORD colorCount;  // Number of colors in color pallete
    DWORD pxArrSize;   // Size of pixel array in bytes
    DWORD padding;     // Padding for pixel array in bytes
    
    BYTE *header;      // Content of image header
    DWORD *colorTable; // Content of color table
    BYTE *pxArr;       // Original pixel array
    BYTE *pxArrMod;    // Modified pixel array
};

typedef struct bitmap BMP;

BMP *loadImage(const char *fname);
void storeProperties(BMP *imgPtr);
void printProperties(BMP img);

void encodeText(const char *fname, BMP *imgPtr);
FILE *openText(const char *fname, BMP img);

void createStego(const char *fname, BMP img);
void freeImage(BMP *imgPtr);

void decodeText(BMP origImg, BMP stegImg, const char *fname);