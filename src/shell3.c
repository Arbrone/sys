#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFERSIZE 256
#define RED   "\x1B[31m"
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"

int parseCommand(char *command, char *args[]);

int main()
{
    char *cmd = NULL;
    char *args[BUFFERSIZE];

    char hostname[BUFFERSIZE];
    char user[BUFFERSIZE];
    size_t len = 0;

    // Display prompt
    gethostname(hostname, 60);
    cuserid(user);
    printf(YEL"%s@%s $ "RESET,user,hostname);

    while (getline(&cmd, &len, stdin) != -1 && strcmp(cmd, "exit\n") != 0)
    {
        int fd[2];
        // fd[1] -> write hand
        // fd[2] -> read  hand

        strtok(cmd, "\n");
        parseCommand(cmd,args);

        if(pipe(fd) == -1){
            perror("Error opening pipe");
            return EXIT_FAILURE;
        }

        int pid1 = fork();
        if (pid1 == -1)
        {
            perror("Error on fork_1");
            return EXIT_FAILURE;
        }

        if (pid1 == 0)
        {
            dup2(fd[1],STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);

            if (execvp(args[0], args) == -1)
            {
                perror(RED "Erreur de execlp"RESET);
                return EXIT_FAILURE;
            }
        }

        int pid2 = fork();
        if (pid2 == -1)
        {
            perror("Error on fork_2");
            return EXIT_FAILURE;
        }

        if (pid2 == 0)
        {
            dup2(fd[0],STDIN_FILENO);
            close(fd[0]);
            close(fd[1]);
            
            if (execvp(args[0], args) == -1)
            {
                perror(RED "Erreur de execlp"RESET);
                return EXIT_FAILURE;
            }
        }

        close(fd[0]);
        close(fd[1]);

        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
        
        printf(YEL"%s@%s $ "RESET,user,hostname);
    }

    free(cmd);
    exit(EXIT_SUCCESS);
}

int parseCommand(char* command, char* args[]) {
    // http://stackoverflow.com/questions/3889992/how-does-strtok-split-the-string-into-tokens-in-c/3890186#3890186
    int i = 0;
    char* a;
    for (a = strtok(command, " "); a != NULL; a = strtok(NULL, " ")) {
        args[i] = a;
        i++;
    }
    args[i] = '\0';
    return 0;
}