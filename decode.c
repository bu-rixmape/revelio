/*
 *  Filename:
 *      decode.c
 *
 *  Purpose:
 *      To obtain secret text hidden in a stego image.
 *
 *  Modifications:
 *      07 April 2022 - created
 *      08 April 2022 - added stegano.h header
 *                    - file renamed from pixel_array.c to encode.c
 *      09 April 2022 - added command line argument parsing
 *      10 April 2022 - modified command line argument parsing
 *                    - modified encoding function
 *      11 April 2022 - deleted command line argumnet parsing
 *      12 April 2022 - modified encoding and decoding function
 *      25 April 2022 - ignored non-ASCII characters
 *      01 May 2022   - added text file for user prompt
 *      04 May 2022   - fixed rizal.bmp
 */

#include "stegano.h"

int main(void)
{
    const char *asciiArt = "assets\\computerArt.txt"; // Filename of ASCII art
    showBackground(asciiArt);                         // Displays terminal background
    setCursorPos(24, 16);                             // Moves position of cursor in terminal
    puts("Enter filename of the following files.");   // Displays user prompt

    char cover[FNAME_MAX];   // Filename of cover image
    char stego[FNAME_MAX];   // Filename of stego image
    char decoded[FNAME_MAX]; // Filename of decoded text

    // Obtains filename of cover image
    setCursorPos(14, 18);
    printf("%s", "Cover image (.bmp): ");
    scanf("%s", cover);

    // Obtains filename of stego image
    setCursorPos(14, 19);
    printf("%s", "Stego image (.bmp): ");
    scanf("%s", stego);

    // Obtains filename of decoded text
    setCursorPos(14, 20);
    printf("%s", "Decoded text (.txt): ");
    scanf("%s", decoded);

    BMP *coverImage = loadImage(cover); // Opens cover image
    BMP *stegoImage = loadImage(stego); // Opens stego image

    // Compares cover and stego image to decode secret text
    decodeText(*coverImage, *stegoImage, decoded);

    // Releases memory allocated to cover and stego image
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