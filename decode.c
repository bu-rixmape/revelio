/*
 *  fname:
 *      decode.c
 *
 *  Purpose:
 *      To obtain secret message hidden in a stego image.
 *
 *  Modifications:
 *      07 April 2022 - created
 */

#include "stegano.h"

int main(void)
{
    BMP *origImage = loadImage("assets\\rizal.bmp"); // Open original image
    BMP *newImage = loadImage("assets\\stego003.bmp"); // Open stego image

    decodeMessage(*origImage, *newImage);

    freeImage(origImage);
    freeImage(newImage);
}