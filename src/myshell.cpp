#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <string>

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

std::string s_promptValue = "hello";

std::map<std::string, std::string> s_variables;

// if else support
int s_ifState = 0; // 0 - not in if, 1 - in if, 2 - wait for then, 3 - in if block, 4 in else block
bool s_ifCondition = false;

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
	std::string lastInput;

    array command;
    array_create(&command);

    int argsCount;
    int outputType;
    char* outputFilePath;

	bool mainLoopRunning = true;

    // Main Loop
    while (mainLoopRunning)
    {		
        signal(SIGINT, signalMainHandler);

		// read input
        std::cout << s_promptValue << ": ";
        fflush(stdout);

		std::string input;

		char tv = getCharFromUser();
		input.push_back(tv);
		
		while (tv != -1 && tv != '\n' && tv != '\0')
		{
			tv = getCharFromUser();
			input.push_back(tv);
		}

		if (input.size() >= 3) {
			if (input[0] == '!' && input[1] == '!' && (input[2] == ' ' || input[2] == -1 || input[2] == '\n' || input[2] == '\0')) {
				if (input[2] == ' ') {
					input.erase(input.begin(), input.begin() + 2);
					input.insert(input.begin(), lastInput.begin(), lastInput.end() - 1);
				} else {
					input = lastInput;
				}

				std::cout << s_promptValue << ": ";
				for (size_t i = 0; i < input.size() - 1; i++)
				{
					std::cout << input[i];
				}
				std::cout << "\n";

				fflush(stdout);
			}
		}

		lastInput = input;

		if (input.size() > 4 && input[0] == '$') {
			std::string variableName = "";

			int j = 1;

			while (j < input.size() && validVariableChar(input[j])) {
				variableName += input[j];
				j++;
			}

			if (j != 0 && j + 3 < input.size() && input[j] == ' ' && input[j + 1] == '=' && input[j + 2] == ' ') {

				std::string value = input.substr(j + 3);

				value = value.substr(0, value.size() - 1);

				s_variables[std::string(variableName)] = value;
				continue;
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

		std::string rawInput = "";

		for (size_t i = 0; i < input.size(); i++)
		{
			if (input[i] == '$' && i + 2 < input.size()) {
				if (input[i + 1] == '?' && (input[i + 2] == ' ' | input[i + 2] == '\0' | input[i + 2] == '\n' | input[i + 2] == -1)) {
					rawInput += s_variables["?"];
					i += 1;
				}

				std::string variableName = "";

				i++;

				while (validVariableChar(input[i]) && i < input.size()) {
					variableName += input[i];
					i++;
				}

				if (s_variables.find(variableName) != s_variables.end()) {
					rawInput += s_variables[variableName];
				}

				i--;
			} else {
				rawInput += input[i];
			}
		}

        int errorCode = getCommand(rawInput, &command, &argsCount, &outputType, &outputFilePath);
        if (errorCode == 2) {
            printf("\n");
            return 0;
        }

        if (errorCode == 0) {
			if (handleFlowCommands(&command, argsCount, &mainLoopRunning)) {
				continue;
			}

			array cleanedCommend = command;

			if (s_ifState == 1) {
				cleanedCommend.size = cleanedCommend.size - 1;
				cleanedCommend.data = cleanedCommend.data + 1;
				argsCount -= 1;
			}
			
            pid_t childPid = fork();
            if (childPid < 0) {
                printf("Error: cannot create a fork!\n");
                continue;
            } else if (childPid == 0) {
                signal(SIGINT, signalChildHandler);

                // execute the command
                FILE *fOutput = NULL;

				int fdIn = STDIN_FILENO;
                int fdOut = STDOUT_FILENO;
        
                if (outputFilePath != NULL) {
                    if (outputType == 1 || outputType == 3) {
                        fOutput = fopen(outputFilePath, "w");
                    } else if (outputType == 2) {
                        fOutput = fopen(outputFilePath, "a");
                    } else if (outputType == 4) {
						fOutput = fopen(outputFilePath, "r");
					}

                    if (fOutput == NULL) {
                        printf("Error: cannot accses input/output file!\n");
                        return 1;
                    }

					if (outputType == 4) {
						fdIn = fileno(fOutput);
					} else {
						fdOut = fileno(fOutput);
					}

                    array_set(&cleanedCommend, argsCount, 0);
                    array_set(&cleanedCommend, argsCount + 1, 0);
                } else {
                    array_push(&cleanedCommend, 0);
                    array_push(&cleanedCommend, 0);
                }

                for (size_t i = 0; i < argsCount; i++)
                {
                    if (strcmp(array_get(&cleanedCommend, i), "|") == 0) {
                        array_set(&cleanedCommend, i, 0);
                    }
                }

            	int status = executeFullCommand(array_data(&cleanedCommend), fdIn, fdOut, outputType == 3);

                if (outputFilePath != NULL) {
                    fclose(fOutput);
                }

                return status;
            }
            int childStatus = 0;
            waitpid(childPid, &childStatus, 0);
						
			s_variables["?"] = std::to_string(childStatus % 255);

			if (s_ifState == 1) {
				s_ifCondition = (childStatus % 255) == 0;
			}
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
    
    return 0;
}

// handlers
void signalMainHandler(int signalNumber)
{
    if (signalNumber != SIGINT) {
        return;
    }

	std::cout << "\nYou typed Control-C\n";
	
	std::cout << s_promptValue << ": ";
	fflush(stdout);

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

int getCommand(std::string& input, array* command, int* argsCount, int* outputType, char** outputFilePath) {		
	array_create(command);

    char* word = (char*)calloc(STRING_MAXSIZE, 1);

    char* wordPtr = word;

    char inString = 0;
    char tv = input.front();
	input.erase(input.begin());

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
                tv = input.front();
				input.erase(input.begin());
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
            tv = input.front();
			input.erase(input.begin());
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
        } else if (strcmp(outputOptial, "<") == 0) {
            *argsCount = commandLength - 2;
            *outputType = 4;
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

bool validVariableChar(char tv)
{
	if (tv >= 'a' && tv <= 'z') {
		return true;
	}

	if (tv >= 'A' && tv <= 'Z') {
		return true;
	}

	if (tv >= '0' && tv <= '9') {
		return true;
	}

	if (tv == '_') {
		return true;
	}

	return false;
}

bool handleFlowCommands(array* command, int argsCount, bool* mainLoopRunning) {
	if (s_ifState == 1) {
		s_ifState = 2;
	}

	if (s_ifState == 2) {
		if (argsCount != 1) {
			printf("must be 'then'\n");
			return true;
		}

		if (strcmp(command->data[0], "then") != 0) {
			printf("must be 'then'\n");
			return true;
		}

		s_ifState = 3;
		return true;
	}

	if (s_ifState == 3 && argsCount == 1 && strcmp(command->data[0], "else") == 0) {
		s_ifState = 4;
		return true;
	}

	if (s_ifState == 3 && argsCount == 1 && strcmp(command->data[0], "fi") == 0) {
		s_ifState = 0;
		return true;
	}

	if (s_ifState == 4 && argsCount == 1 && strcmp(command->data[0], "fi") == 0) {
		s_ifState = 0;
		return true;
	}

	if (s_ifState == 3 && s_ifCondition == false) {
		return true;
	}

	if (s_ifState == 4 && s_ifCondition) {
		return true;
	}
	
	if (argsCount <= 0) {
		return false;
	}

	if (strcmp(command->data[0], "if") == 0) {
		s_ifState = 1;
		s_ifCondition = false;
		return false;
	}

	if (strcmp(command->data[0], EXIT_COMMAND) == 0) {
		*mainLoopRunning = false;
		return true;
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

	if (strcmp(command->data[0], READ_COMMAND) == 0) {
		if (argsCount != 2) {
			printf("Error: Invalid command!\n");
			return true;
		}

		std::string variableName = std::string(command->data[1]);

		for (size_t i = 0; i < variableName.size(); i++)
		{
			if (!validVariableChar(variableName[i])) {
				printf("Error: Invalid variable name!\n");
				return true;
			}
		}

		std::string value = "";

		char tv = getCharFromUser();
		value.push_back(tv);

		while (tv != -1 && tv != '\n' && tv != '\0')
		{
			tv = getCharFromUser();
			value.push_back(tv);
		}

		value = value.substr(0, value.size() - 1);

		s_variables[variableName] = value;
		
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

	bool withBackground = false;

    while (progs[progIndexNext] != 0) {
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
			char** lastProg = progs + progIndex;
			char** beforeLastProg = NULL;
			
			while (*lastProg != 0) {
				beforeLastProg = lastProg;
				lastProg++;
			}

			if (strcmp(*beforeLastProg, "&") == 0) {
				withBackground = true;
				*(*beforeLastProg - 1) = '\0';
				*beforeLastProg = NULL;
			}
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

    for (int i = 0; i < k; i++)
    {
        close(handlers[3 * i + 1]);
        close(handlers[3 * i + 2]);
    }

	int executeState = 0;

	if (withBackground) {
		k -= 1;
	}

    for (int i = 0; i < k; i++)
    {
		int childStatus;
        waitpid(handlers[3 * i], &childStatus, 0);

		if (i + 1 == k) {
			executeState = childStatus % 255;
		}
    }

    return executeState;
}
