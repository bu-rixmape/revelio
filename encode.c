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
    // Displays program description
    FILE *prompt = fopen("prompt.txt", "r");
    char line[FNAME_MAX];
    for (unsigned int i = 0; i < 12; i++)
    {
        fgets(line, FNAME_MAX, prompt);
        printf("%s", line);
    }
    putchar('\n');

    // Opens cover image
    char cover[FNAME_MAX];
    printf("|=^_^=| Input a cover Image (.bmp file format):");
    scanf("%s", cover);

    // Opens secret text
    char secret[FNAME_MAX];
    printf("|=0_0=| Input a Secret Text (.txt file format):");
    scanf("%s", secret);

    // Opens stego name
    char stego[FNAME_MAX];
    printf("|=*_*=| Input a Stego Image (.bmp file format):");
    scanf("%s", stego);

    BMP *imagePtr = loadImage(cover); // Creates BMP structure for cover image
    encodeText(secret, imagePtr);     // Hides secret message
    createStego(stego, *imagePtr);    // Creates new file for stego image
    freeImage(imagePtr);              // Closes cover image
}