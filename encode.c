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

int main(void)
{
    const char *asciiArt = "assets\\computerArt.txt"; // Filename of ASCII art
    showBackground(asciiArt);                         // Displays screen background

    setCursorPos(24, 16); // Moves terminal cursor
    puts("Enter filename of the following files.");

    setCursorPos(14, 18);
    printf("%s", "Cover image (.bmp): "); // Displays user prompt
    char cover[FNAME_MAX];                // Filename of cover image
    scanf("%s", cover);                   // Obtains filename of cover image

    setCursorPos(14, 19);
    printf("%s", "Secret text (.txt): ");
    char secret[FNAME_MAX];               // Filename of secret text
    scanf("%s", secret);                  // Obtains filename of secret text

    setCursorPos(14, 20);
    printf("%s", "Stego image (.bmp): ");
    char stego[FNAME_MAX];                // Filename of stego image
    scanf("%s", stego);                   // Obtains filename of stego image

    char *cover = "assets\\rizal.bmp";
    char *secret = "assets\\novels.txt";
    char *stego = "stegoNew.bmp";

    BMP *imagePtr = loadImage(cover);     // Creates BMP structure for cover image
    encodeText(secret, imagePtr);         // Hides secret text into the cover image
    createStego(stego, *imagePtr);        // Creates stego image
    freeImage(imagePtr);                  // Closes cover image

    showBackground(asciiArt);             // Update screen background

    setCursorPos(22, 16);
    puts(">>> STEGO IMAGE SUCCESSFULLY CREATED! <<<\n");
    setCursorPos(14, 18);
    printf("Cover image found at %s", cover);
    setCursorPos(14, 19);
    printf("Secret text found at %s", secret);
    setCursorPos(14, 20);
    printf("Stego image saved at %s", stego);

    setCursorPos(0, 36); // Move cursor to the last line of screen
}