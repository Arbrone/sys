#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

// recupere la commande a exectuer
// on fait un nouveau processus
// on execute la commande dans le processus enfant
// on attend qu'il meurt et on termine le programme
#define BUFFERSIZE 256
#define RED   "\x1B[31m"
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"

int main()
{
    char *cmd = NULL;

    char hostname[BUFFERSIZE];
    char user[BUFFERSIZE];
    size_t len = 0;

    // Display prompt
    gethostname(hostname, 60);
    cuserid(user);
    printf(YEL"%s@%s $ "RESET,user,hostname);

    while (getline(&cmd, &len, stdin) != -1 && strcmp(cmd, "exit\n") != 0)
    {
        strtok(cmd, "\n");

        int pid = fork();
        if (pid == -1)
        {
            perror("Error on fork");
            return EXIT_FAILURE;
        }

        if (pid == 0)
        {
            if (execlp(cmd, cmd, NULL) == -1)
            {
                perror(RED "Erreur de execlp"RESET);
                return EXIT_FAILURE;
            }
        }
        waitpid(pid, NULL, 0);
        
        printf(YEL"%s@%s $ "RESET,user,hostname);
    }

    free(cmd);
    exit(EXIT_SUCCESS);
}