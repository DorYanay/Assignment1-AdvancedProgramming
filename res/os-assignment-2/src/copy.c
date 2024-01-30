#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief The tool options
 * 
 */
struct Options {
    unsigned char verbose;
    unsigned char force;   
};

/**
 * @brief Entry point of copy
 *
 * @return int error code
 */
int main(int argc, char *argv[])
{
    struct Options options;
    options.verbose = 0;
    options.force = 0;

    char* srcName = NULL;
    char* targetName = NULL;

    for (size_t i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0) {
            options.verbose = 1;
        } else if (strcmp(argv[i], "-f") == 0) {
            options.force = 1;
        } else if (srcName == NULL) {
            srcName = argv[i];
        } else if (targetName == NULL) {
            targetName = argv[i];
        } else {
            printf("Invalid arguments!\n");
            return 2;
        }
    }

    if (srcName == NULL || targetName == NULL) {
        printf("Missing at last on of the files!\n");
        return 2;
    }

    FILE *srcFile = fopen(srcName, "rb");
    if (srcFile == NULL) {
        if (options.verbose == 1) {
            printf("general failure\n");
        }
        return 1;
    }

    FILE *targetFile = fopen(targetName, "rb");
    if (targetFile != NULL) {
        if (options.force != 1) {
            if (options.verbose) {
                printf("target file exist\n");
            }

            fclose(srcFile);
            fclose(targetFile);
            return 1;
        }
        fclose(targetFile);
    }

    targetFile = fopen(targetName, "wb");
    if (targetFile == NULL) {
        if (options.verbose == 1) {
            printf("general failure\n");
        }
        fclose(srcFile);
        return 1;
    }

    int byte1 = fgetc(srcFile);
    while(byte1 != EOF) 
    {
        fputc(byte1, targetFile);
        byte1 = fgetc(srcFile);
    }

    fclose(srcFile);
    fclose(targetFile);

    if (options.verbose == 1) {
        printf("success\n");
    }    
    return 0;
}
