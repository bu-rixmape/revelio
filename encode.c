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
    puts("Ready to ENCODE your secret text?");        // Displays user prompt
    setCursorPos(21, 17);
    puts("Enter the filename of the following files:");

    char cover[FNAME_MAX];  // Filename of cover image
    char secret[FNAME_MAX]; // Filename of secret text
    char stego[FNAME_MAX];  // Filename of stego image

    // Obtains filename of cover image
    setCursorPos(14, 19);
    printf("%s", "Cover image (.bmp): ");
    scanf("%s", cover);

    BMP *imagePtr = loadImage(cover); // Creates BMP structure for cover image

    // Computes maximum number of characters for secret text
    unsigned int maxChar = (imagePtr->width * imagePtr->height) / CHAR_BIT;
    setCursorPos(14, 20);
    printf("Note: Secret text must have at most %u characters", maxChar);

    // Obtains filename of secret text
    setCursorPos(14, 21);
    printf("%s", "Secret text (.txt): ");
    scanf("%s", secret);

    encodeText(secret, imagePtr);     // Hides secret text into the cover image

    // Obtains filename of stego image
    setCursorPos(14, 22);
    printf("%s", "Stego image (.bmp): ");
    scanf("%s", stego);

    BMP image = *imagePtr;     // Dereferences pointer for passing by value     
    createStego(stego, image); // Creates stego image from modified cover image
    freeImage(imagePtr);       // Releases memory allocated for cover image

    showBackground(asciiArt);  // Displays terminal background

    // Display success message
    setCursorPos(22, 16);
    puts(">>> STEGO IMAGE SUCCESSFULLY CREATED! <<<\n");
    setCursorPos(14, 18);
    printf("Cover image found at %s", cover);
    setCursorPos(14, 19);
    printf("Secret text found at %s", secret);
    setCursorPos(14, 20);
    printf("Stego image saved at %s", stego);

    setCursorPos(0, 36); // Moves cursor to the last line of terminal
}