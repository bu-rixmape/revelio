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
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <limits.h>

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
    DWORD pxCount;
    WORD bitDepth;
    DWORD colorCount;
    DWORD pxArraySize;
    DWORD padding;
    BYTE *header;
    BYTE *colorTable;
    BYTE *pxArray;
} BMP;

BMP *loadImage(char *fname);
void setProperties(BMP *imgPtr);
void printProperties(BMP img);

void encodeMessage(char *fname, BMP *imgPtr);
FILE *openMessage(char *fname, BMP *imgPtr);

void createStego(char *fname, BMP img);
void freeImage(BMP *imgPtr);
void decodeMessage(BMP origImg, BMP stegImg);