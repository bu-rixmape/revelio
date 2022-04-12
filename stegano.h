/*
 *  fname:
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

#define FNAME_LIMIT 100
#define CHANGE_VALUE 1
#define FILEHEADER_SIZE 14

typedef unsigned char BYTE;  // 1 byte
typedef unsigned int DWORD;  // 4 bytes
typedef unsigned short WORD; // 2 bytes
typedef int LONG;            // 4 bytes (signed)

typedef struct
{
    FILE *filePtr;

    DWORD headerSize;
    LONG width;
    LONG height;
    WORD bitDepth;
    DWORD colorCount;
    DWORD pxArrSize;
    DWORD padding;

    BYTE *header;
    DWORD *colorTable;
    BYTE *pxArr;
    BYTE *pxArrMod;
} BMP;

BMP *loadImage(const char *fname);
void storeProperties(BMP *imgPtr);
void printProperties(BMP img);

void encodeText(const char *fname, BMP *imgPtr);
FILE *openText(const char *fname, BMP img);

void createStego(const char *fname, BMP img);
void freeImage(BMP *imgPtr);

void decodeText(BMP origImg, BMP stegImg, const char *fname);