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

int main(void)
{
    const char *asciiArt = "assets\\computerArt.txt"; // Filename of ASCII art
    showBackground(asciiArt);                         // Displays screen background

    setCursorPos(24, 16); // Moves terminal cursor
    puts("Enter filename of the following files.");

    char cover[FNAME_MAX];                 // Filename of cover image
    char decoded[FNAME_MAX];               // Filename of secret text
    char stego[FNAME_MAX];                 // Filename of stego image
    getFnameDecode(cover, stego, decoded); // Obtain filenames

    BMP *coverImage = loadImage(cover); // Opens cover image
    BMP *stegoImage = loadImage(stego); // Opens stego image

    decodeText(*coverImage, *stegoImage, decoded);
    freeImage(coverImage);
    freeImage(stegoImage);

    showBackground(asciiArt); // Update screen background

    setCursorPos(22, 16);
    puts(">>> STEGO IMAGE SUCCESSFULLY DECODED! <<<\n");
    setCursorPos(14, 18);
    printf("Cover image found at %s", cover);
    setCursorPos(14, 19);
    printf("Stego image found at %s", stego);
    setCursorPos(14, 20);
    printf("Decoded text saved at %s", decoded);

    setCursorPos(0, 36); // Move cursor to the last line of screen
}