#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief The tool options
 * 
 */
struct Options {
    unsigned char verbose;
    unsigned char ignore_case;   
};

/**
 * @brief Entry point of cmp
 *
 * @return int error code
 */
int main(int argc, char *argv[])
{
    struct Options options;
    options.verbose = 0;
    options.ignore_case = 0;

    char* fileName1 = NULL;
    char* fileName2 = NULL;

    for (size_t i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0) {
            options.verbose = 1;
        } else if (strcmp(argv[i], "-i") == 0) {
            options.ignore_case = 1;
        } else if (fileName1 == NULL) {
            fileName1 = argv[i];
        } else if (fileName2 == NULL) {
            fileName2 = argv[i];
        } else {
            printf("Invalid arguments!\n");
            return 2;
        }
    }

    if (fileName1 == NULL || fileName2 == NULL) {
        printf("Missing at last on of the files!\n");
        return 2;
    }

    FILE *file1 = fopen(fileName1, "rb");
    FILE *file2 = fopen(fileName2, "rb");

    if (file1 == NULL || file2 == NULL) {
        if (file1 != NULL) {
            fclose(file1);
        } else {
            printf("Cannot read \"%s\"!\n", fileName1);
        }

        if (file2 != NULL) {
            fclose(file2);
        } else {
            printf("Cannot read \"%s\"!\n", fileName2);
        }
        return 2;
    }

    char byte1 = (char)fgetc(file1);
    char byte2 = (char)fgetc(file2);
    while(byte1 != EOF && byte2 != EOF) 
    {
        if (options.ignore_case == 1) {
            if (byte1 >= 'A' && byte1 <= 'Z') {
                byte1 -= 'A';
            }
            if (byte2 >= 'A' && byte2 <= 'Z') {
                byte2 -= 'A';
            }
        }

        if (byte1 != byte2) {
            byte1 = EOF;
            byte2 = 'A';
        } else {
            byte1 = (char)fgetc(file1);
            byte2 = (char)fgetc(file2);
        }
    }

    fclose(file1);
    fclose(file2);

    if (!(byte1 == EOF && byte2 == EOF)) {
        if (options.verbose == 1) {
            printf("distinct\n");
        }
        return 1;
    }

    if (options.verbose == 1) {
        printf("equal\n");
    }    
    return 0;
}
