// stshell header

#include "array.h"

#define STRING_MAXSIZE 256
#define MAX_COMMANDS 256

#define EXIT_COMMAND "quit"
#define PROMPT_COMMAND "prompt"
#define CD_COMMAND "cd"

/**
 * @brief The tool options
 * 
 */
struct Options {
    unsigned char verbose;
};

/**
 * @brief handle signals in the main fork
 * 
 * @param signalNumber the signal
 */
void signalMainHandler(int signalNumber);

/**
 * @brief handle signals in the child fork
 * 
 * @param signalNumber the signal
 */
void signalChildHandler(int signalNumber);

char getCharFromUser();

/**
 * @brief Get command from the user
 * 
 * @param command container for the command
 * @param argsCount count of args in the full command (before >, >>, 2>)
 * @param outputType 0 - defualt output, 1 - write into file, 2 - appand into file, 3 - write errors into file
 * @param outputFilePath file output path, NULL - if defualt output
 * @return int 0 - good, otherwize - error
 */
int getCommand(std::string& input, array* command, int* argsCount, int* outputType, char** outputFilePath);

/**
 * @brief Valid command from the user
 * 
 * @param command container for the command
 * @param argsCount count of args in the full command (before >, >>)
 * @param outputType 0 - defualt output, 1 - write into file, 2 - appand into file
 * @param outputFilePath file output path, NULL - if defualt output
 * @return int 0 - good, otherwize - error
 */
int validCommand(array* command, int* argsCount, int* outputType, char** outputFilePath);

/**
 * @brief Handle myshell commands (like prompt) and return true if the command is myshell command
 * commands: prompt, cd
 * 
 * @param command the command args
 * @param argsCount count of command args
 * @return is myshell command
 */
bool handleFlowCommands(array* command, int argsCount, bool* mainLoopRunning);

/**
 * @brief execute a command, support multi pipes
 * 
 * @param progs arguments (argv), 0 - for |. endend by two zeros
 * @param fd_in fd as input to the command
 * @param fd_out df as output the command
 * @param fd_error df as error output the command
 * @return int 0 - good, otherwize - error 
 */
int executeFullCommand(char** progs, int fd_in, int fd_out, bool redirect_error);
