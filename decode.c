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

    setCursorPos(26, 16);                             // Moves position of terminal cursor
    puts("Ready to DECODE your secret text?");        // Displays user prompt
    setCursorPos(21, 17);
    puts("Enter the filename of the following files:");

    char cover[FNAME_MAX];   // Filename of cover image
    char stego[FNAME_MAX];   // Filename of stego image
    char decoded[FNAME_MAX]; // Filename of decoded text

    // Obtains filename of cover image
    setCursorPos(14, 19);
    printf("%s", "Cover image (.bmp): ");
    scanf("%s", cover);

    // Obtains filename of stego image
    setCursorPos(14, 20);
    printf("%s", "Stego image (.bmp): ");
    scanf("%s", stego);

    // Obtains filename of decoded text
    setCursorPos(14, 21);
    printf("%s", "Decoded text (.txt): ");
    scanf("%s", decoded);

    BMP *coverImagePtr = loadImage(cover); // Opens cover image
    BMP *stegoImagePtr = loadImage(stego); // Opens stego image

    // Dereferences pointer to prepare for passing by value
    BMP coverImage = *coverImagePtr;
    BMP stegoImage = *stegoImagePtr;

    // Compares cover and stego image to decode secret text
    decodeText(coverImage, stegoImage, decoded);

    // Releases memory allocated to cover and stego image
    freeImage(coverImagePtr);
    freeImage(stegoImagePtr);

    showBackground(asciiArt); // Displays terminal background

    // Displays success message
    setCursorPos(22, 16);
    puts(">>> STEGO IMAGE SUCCESSFULLY DECODED! <<<\n");
    setCursorPos(14, 18);
    printf("Cover image found at %s", cover);
    setCursorPos(14, 19);
    printf("Stego image found at %s", stego);
    setCursorPos(14, 20);
    printf("Decoded text saved at %s", decoded);

    setCursorPos(0, 36); // Moves cursor to the last line of screen
}