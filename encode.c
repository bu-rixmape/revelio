/*
 *  fname:
 *      encode.c
 *
 *  Purpose:
 *      To demonstrate encoding of message into an image.
 *
 *  Modifications:
 *      07 April 2022 - created
 *      08 April 2022 - added stegano.h header
 *                    - file renamed from pixel_array.c to encode.c
 *      09 April 2022 - commented option parsing
 */

#include "stegano.h"

int main(int argc, char **argv)
{
    // // Exit program if required arguments were not provided
    // if (argc == 5 && argv[1] == "-i" && argv[3] == "-m")
    // {
    //     fprintf(stderr, "%s", "Required arguments not provided.");
    //     exit(1);
    // }

    // // Obtain required arguments
    // // TODO: Check if filenames are valid
    // char *input = argv[2];   // Filename of input image
    // char *message = argv[4]; // Filename of text message

    // // Initialize values for optional arguments
    // char output[100] = "new.bmp";

    // // Obtain optional arguments, if they are provided
    // if (argc > 5)
    // {
    //     for (size_t i = 5; i < argc; i += 2)
    //     {
    //         char option = *(argv[i] + 1); // Obtain option mode

    //         switch (option)
    //         {
    //             case 'o':
    //                 strncpy(output, argv[i + 1], 100 - 1);
    //                 break;

    //             // Add other options here in the future
    //         }
    //     }
    // }

    // printf("i: %s\no: %s\nm: %s\n", input, output, message);

    char *input = "130x200.bmp";
    char *output = "new.bmp";
    char *txt = "message.txt";

    BMP *imagePtr = loadImage(input); // Open input image
    printProperties(*imagePtr);     // Display image properties

    encodeMessage(txt, imagePtr);

    saveNewImage(output, *imagePtr); // Create new image

    freeImage(imagePtr); // Close input image
}