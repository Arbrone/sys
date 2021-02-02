#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

// recupere la commande a exectuer
// on fait un nouveau processus 
// on execute la commande dans le processus enfant
// on attend qu'il meurt et on termine le programme
#define BUFFERSIZE 10

int main(){

    char buffer[BUFFERSIZE];
    fgets (buffer, BUFFERSIZE, stdin);

    strtok(buffer, "\n");

    int pid = fork();

    if(pid == -1) return EXIT_FAILURE;

    //action dans le processus fils
    if(pid == 0){
       if(execlp(buffer, buffer, NULL) == -1){
           perror("Erreur de execlp");
           return EXIT_FAILURE;
       }
    }

    //TODO wait() pour s'assurer de ne pas avoir un processus zombie
    waitpid(pid,NULL,0);
    
    return 0;
}