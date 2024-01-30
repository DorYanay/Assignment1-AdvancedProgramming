#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "codec.h"

/**
 * @brief Load dynamic library like dlopen. Include woking directory into the PATH.
 * 
 * @param libraryPath Library path
 * @return void* Pointer to library handler or NULL if has error
 */
void* loadLibrary(char* libraryPath) {
    void* handle = dlopen(libraryPath, RTLD_LAZY);

    if (!handle) {
        size_t libraryLength = strlen(libraryPath);
        char* newLibraryPath = (char*)malloc(2 + libraryLength + 1);
        memcpy(newLibraryPath, "./", 2);
        memcpy(newLibraryPath + 2, libraryPath, libraryLength + 1);

		handle = dlopen(newLibraryPath, RTLD_LAZY);

        free(newLibraryPath);
	}

    return handle;
}

/**
 * @brief Entry point of encode
 *
 * @return int error code
 */
int main(int argc, char *argv[])
{
    if (argc != 2 && argc != 3) {
        printf("ecoude <library>\n");
        printf("ecoude <library> <text>\n");
        printf("Invalid arguments!\n");
        return 1;
    }

    char* library = argv[1];

    void* handle = loadLibrary(library);
    
    if (!handle) {
		printf("Cannot open the library!\n");
		return 1;
	}

    encode_function encodeFunc = dlsym(handle, "encode");

    if (!handle) {
		printf("Cannot load the function encode!\n");
		return 1;
	}
    
    if (argc == 3) {
        // load from arguments
        char* str = argv[2];

        encodeFunc(str);
        printf("%s\n", str);
    } else {
        // load from standart input
        char bf[2];
        bf[1] = '\0';
        *bf = (char)getc(stdin);
        while (*bf != -1) {
            encodeFunc(bf);
            printf("%c", *bf);
            *bf = (char)getc(stdin);
        }
    }

    dlclose(handle);
    return 0;
}
