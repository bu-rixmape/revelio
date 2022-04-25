/*
 *  Filename:
 *      decode.c
 *
 *  Purpose:
 *      To obtain secret text hidden in a stego image.
 *
 *  Modifications:
 *      07 April 2022 - created
 *      12 April 2022 - added argument parsing
 */

#include "stegano.h"

int main(int argc, char *argv[])
{
    char *cover = "assets\\rizal.bmp"; // Filename of cover image
    char *message = "decodedtext.txt"; // Filename of decoded text
    char *stego = "stegoRizal.bmp";    // Filename of stego image

    BMP *coverImage = loadImage(cover); // Opens cover image
    BMP *stegoImage = loadImage(stego); // Opens stego image

    decodeText(*coverImage, *stegoImage, message);
    freeImage(coverImage);
    freeImage(stegoImage);
}