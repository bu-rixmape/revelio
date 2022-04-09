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
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define HEADER_SIZE 54
#define CHANGE_VALUE 1

typedef unsigned char BYTE;

typedef struct
{
    FILE *filePtr;
    int dibHeaderSize;
    int width;
    int height;
    int pxCount;
    int bitDepth;
    int colorCount;
    BYTE header[HEADER_SIZE];
    BYTE *colorTable;
    BYTE *pxArray;
} BMP;

BMP *loadImage(char *fname);
void freeImage(BMP *imgPtr);

void setProperties(BMP *imgPtr);
void printProperties(BMP img);

void encodeMessage(char *fname, BMP *imgPtr);
FILE *openMessage(void);

void saveNewImage(char *fname, BMP img);