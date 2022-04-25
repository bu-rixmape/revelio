/*
 *  Filename:
 *      encode.c
 *
 *  Purpose:
 *      To create a steganographic image by encoding secret text
 *      into a cover image.
 *
 *  Modifications:
 *      07 April 2022 - created
 *      08 April 2022 - added stegano.h header
 *                    - file renamed from pixel_array.c to encode.c
 *      09 April 2022 - added argument parsing
 *      10 April 2022 - modified argument parsing
 */

#include "stegano.h"

int main(int argc, char *argv[])
{
    char *cover = "assets\\rizal.bmp";        // Filename of cover image
    char *message = "assets\\rizalbooks.txt"; // Filename of secret message
    char *stego = "stegoRizal.bmp";           // Filename of stego image

    BMP *imagePtr = loadImage(cover); // Creates BMP structure for cover image
    encodeText(message, imagePtr);    // Hides secret message
    createStego(stego, *imagePtr);    // Creates new file for stego image
    freeImage(imagePtr);              // Closes cover image
}