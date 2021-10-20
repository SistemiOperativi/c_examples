#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void main() {
   char comando[256];
   pid_t pid; int status;

   while(1) 
   {
     printf("Digitare un comando: ");
     scanf("%s",comando);
     pid = fork();
     if ( pid == -1 ) {
       printf("Errore nella fork.\n");
       exit(1);
     }
     if ( pid == 0 )
        execlp(comando, comando, NULL);
     else wait(&status);
   }
}
