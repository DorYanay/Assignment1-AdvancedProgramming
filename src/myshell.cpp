#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "myshell.h"

// globals
struct Options options;

int s_status = -1; // -1 - for running, 0 - stoped, elsewize - error code
pid_t s_childPid = 0; // 0 - can be closed, elsewize - cannot be closed
std::string s_promptValue = "hello";

/**
 * @brief Entry point of myshell
 *
 * @return int error code
 */
int main(int argc, char **argv)
{
    options.verbose = 0;
    if (argc > 1 && strcmp("-v", argv[1]) == 0) {
        options.verbose = 1;
    }

    // command
    array command;
    array_create(&command);

    int argsCount;
    int outputType;
    char* outputFilePath;

    // Main Loop
    while (s_status == -1)
    {
        signal(SIGINT, signalMainHandler);

        // cleanup
        char* ptr = array_pull(&command);
        while (ptr != NULL)
        {
            free(ptr);
            ptr = array_pull(&command);
        }
        
        array_free(&command);

        // read input
        std::cout << s_promptValue << ": ";
        fflush(stdout);

        int errorCode = getCommand(&command, &argsCount, &outputType, &outputFilePath);
        if (errorCode == 2) {
            printf("\n");
            return 0;
        }

        if (errorCode == 0) {
			if (handleFlowCommands(&command, argsCount)) {
				continue;
			}
			
            s_childPid = fork();
            if (s_childPid < 0) {
                printf("Error: cannot create a fork!\n");
                continue;
            } else if (s_childPid == 0) {
                signal(SIGINT, signalChildHandler);

                // execute the command
                FILE *fOutput = NULL;
                int fdOut = STDOUT_FILENO;
        
                if (outputFilePath != NULL) {
                    if (outputType == 1 || outputType == 3) {
                        fOutput = fopen(outputFilePath, "w");
                    } else if (outputType == 1) {
                        fOutput = fopen(outputFilePath, "a");
                    }

                    if (fOutput == NULL) {
                        printf("Error: cannot accses output file!\n");
                        return 1;
                    }

                    fdOut = fileno(fOutput);

                    array_set(&command, argsCount, 0);
                    array_set(&command, argsCount + 1, 0);
                } else {
                    array_push(&command, 0);
                    array_push(&command, 0);
                }

                for (size_t i = 0; i < argsCount; i++)
                {
                    if (strcmp(array_get(&command, i), "|") == 0) {
                        array_set(&command, i, 0);
                    }
                }

                s_status = executeFullCommand(array_data(&command), STDIN_FILENO, fdOut, outputType == 3);

                if (outputFilePath != NULL) {
                    fclose(fOutput);
                }

                return s_status;
            }
            int childStatus = 0;
            waitpid(s_childPid, &childStatus, 0);
            if (childStatus == 0) {
                s_status = childStatus;
            }
            s_childPid = 0;
        }
    }

    // cleanup
    char* ptr = array_pull(&command);
    while (ptr != NULL)
    {
        free(ptr);
        ptr = array_pull(&command);
    }
        
    array_free(&command);
    
    return s_status;
}

// handlers
void signalMainHandler(int signalNumber)
{
    if (signalNumber != SIGINT) {
        return;
    }

    signal(SIGINT, SIG_IGN);
}

void signalChildHandler(int signalNumber)
{

}

// read command
char getCharFromUser() {
    char tv = (char)getchar();

    if (options.verbose == 1) {
        if (tv != -1 || tv == '\n') {
            printf("%c", tv);
            fflush(stdout);
        }
    }
    
    return tv;
}

int getCommand(array* command, int* argsCount, int* outputType, char** outputFilePath) {
    array_create(command);

    char* word = (char*)calloc(STRING_MAXSIZE, 1);

    char* wordPtr = word;

    char inString = 0;
    char tv = getCharFromUser();

    if (tv == -1) {
        return 2;
    }

    while (tv != '\n' && tv != '\0')
    {
        if (inString == 0) {
            if (tv == '\"') {
                if (*word != '\0') { // " in middle of word
                    printf("Error: cannot set \" in middle argument!\n");
                    
                    // cleanup
                    free(word);

                    return 1;
                }
                inString = 1;
            } else {
                if (tv == ' ') {
                    array_push(command, word);
                    word = (char*)calloc(STRING_MAXSIZE, 1);
                    wordPtr = word;
                } else {
                    *wordPtr = tv;
                    wordPtr++;
                }
            }
        } else {
            if (tv == '\"') {
                tv = getCharFromUser();
                if (tv == -1) {
                    return 2;
                }

                if (tv != '\n' && tv != '\0' && tv != ' ') { // " in middle of word
                    printf("Error: cannot set \" in middle argument!\n");

                    // cleanup
                    free(word);
                    
                    return 1;
                }

                if (*word != '\0') { // not empty ""
                    array_push(command, word);
                    word = (char*)calloc(STRING_MAXSIZE, 1);
                    wordPtr = word;
                }
                inString = 0;
            } else {
                *wordPtr = tv;
                wordPtr++;
            }
        }
        
        if (tv != '\n' && tv != '\0') {
            tv = getCharFromUser();
            if (tv == -1) {
                return 2;
            }
        }
    }

    if (inString == 1) {
        printf("Error: Unclosed \" in last argument!\n");

        // cleanup
        free(word);

        return 1;
    }

    if (*word != '\0') {
        array_push(command, word);
    } else if (options.verbose == 0) {
        printf("\n");
    }

    return validCommand(command, argsCount, outputType, outputFilePath);
}

int validCommand(array* command, int* argsCount, int* outputType, char** outputFilePath) {
    if (array_size(command) == 0) {
        return 1;
    }

    int commandLength = array_size(command);

    *argsCount = commandLength;
    *outputType = 0;
    *outputFilePath = NULL;

    // check if has output target (> or >>)
    if (commandLength >= 3) {
        char* outputOptial = array_get(command, commandLength - 2); 
        char* lastArg = array_get(command, commandLength - 1);
        if (strcmp(outputOptial, ">") == 0) {
            *argsCount = commandLength - 2;
            *outputType = 1;
            *outputFilePath = lastArg;
        } else if (strcmp(outputOptial, ">>") == 0) {
            *argsCount = commandLength - 2;
            *outputType = 2;
            *outputFilePath = lastArg;
        } else if (strcmp(outputOptial, "2>") == 0) {
            *argsCount = commandLength - 2;
            *outputType = 3;
            *outputFilePath = lastArg;
        }
    }

    // valid the command
    int lastType = 1; // 0 - command, 1 = pipline (|)

    for (size_t i = 0; i < *argsCount; i++)
    {
        char* arg = array_get(command, i);
        if (strcmp(arg, "|") == 0) {
            if (lastType == 1) {
                printf("Error: Invalid command, pipline must be between commands!\n");
                return 1;
            }
            lastType = 1;
        } else if (strcmp(arg, ">") == 0 || strcmp(arg, ">>") == 0 || strcmp(arg, "2>") == 0) {
            printf("Error: Invalid command!\n");
        } else {
            lastType = 0;
        }
    }

    if (lastType == 1) {
        printf("Error: Invalid command, pipline must be between commands!\n");
        return 1;
    }

    return 0;
}


bool handleFlowCommands(array* command, int argsCount) {
	if (argsCount <= 0) {
		return false;
	}

	if (strcmp(command->data[0], PROMPT_COMMAND) == 0) {
		if (argsCount != 3) {
			printf("Error: Invalid command!\n");
			return true;
		}

		if (strcmp(command->data[1], "=") != 0) {
			printf("Error: Invalid command!\n");
			return true;
		}

		s_promptValue = command->data[2];
		
		return true;
	}

	if (strcmp(command->data[0], CD_COMMAND) == 0) {
		if (argsCount != 2) {
			printf("Error: Invalid command!\n");
			return true;
		}

		char* dirPath = command->data[1];
		
		if (chdir(dirPath) != 0) {
			printf("Error: Cannot change directory!\n");
			return true;
		}
		
		return true;
	}

	return false;
}


// execute full command
int executeFullCommand(char** progs, int fd_in, int fd_out, bool redirect_error) {
    int handlers[3 * MAX_COMMANDS]; // 3k + 0 - fork, 3k + 1 - read, 3k + 2 write

    int progIndex = 0;
    int progIndexNext = progIndex;
    int k = 0;

    while (progs[progIndexNext] != 0 && s_status == -1) {
        while (progs[progIndexNext] != 0) {
            progIndexNext++;
        }
        progIndexNext++;

        // run command part
        int from = 0;
        int to = 0;
		bool lastCommand = progs[progIndexNext] == 0;

        if (progIndex == 0) { // first command
            from = fd_in;
        } else { // not first command
            from = handlers[(3 * (k - 1)) + 1];
        }
        
        if (lastCommand) { // last command
			to = fd_out;
        } else {
            if (pipe(&handlers[3 * k + 1]) == -1) {
                printf("ERROR: Cannot create a pipe!\n");
            
                // Cleanup
                for (size_t i = 0; i < k; i++)
                {
                    kill(handlers[3 * i], SIGKILL);
                    close(handlers[3 * i + 1]);
                    close(handlers[3 * i + 2]);
                }

                kill(handlers[3 * k], SIGKILL);

                return 1;
            }

            to = handlers[3 * k + 2];
        }

        if (strcmp(*(progs + progIndex), EXIT_COMMAND) == 0) {
            // exit command
            s_status = 0;
        } else {
            handlers[3 * k] = fork();
            if (handlers[3 * k] < 0) {
                printf("ERROR: Cannot create a fork!\n");
                
                // Cleanup
                for (size_t i = 0; i < k; i++)
                {
                    kill(handlers[3 * i], SIGKILL);
                    close(handlers[3 * i + 1]);
                    close(handlers[3 * i + 2]);
                }

                return 1;
            } else if (handlers[3 * k] == 0) {
                dup2(from, STDIN_FILENO);
				if (lastCommand && redirect_error) {
					dup2(to, STDERR_FILENO);
				} else {
					dup2(to, STDOUT_FILENO);
				}
                
                for (int i = 0; i < k; i++)
                {
                    close(handlers[3 * i + 1]);
                    close(handlers[3 * i + 2]);
                }

                if (progs[progIndexNext] != 0) { // not last command
                    close(handlers[3 * k + 1]);
                    close(handlers[3 * k + 2]);
                }
                
                execvp(*(progs + progIndex), progs + progIndex);

                // faild
                printf("ERROR: command failed!\n");

                // Cleanup
                for (size_t i = 0; i < k; i++)
                {
                    kill(handlers[3 * i], SIGKILL);
                    close(handlers[3 * i + 1]);
                    close(handlers[3 * i + 2]);
                }

                return 1;
            }

            // nextLoop
            progIndex = progIndexNext;
            k++;
        }
    }

    for (int i = 0; i < k; i++)
    {
        close(handlers[3 * i + 1]);
        close(handlers[3 * i + 2]);
    }

    for (int i = 0; i < k; i++)
    {
        waitpid(handlers[3 * i], &s_childPid, 0);
    }

    return s_status;
}
