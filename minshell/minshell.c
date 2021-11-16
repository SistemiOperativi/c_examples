#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
   char comando[256];
   pid_t pid; int status;

   while(1) 
   {
     printf("Digitare un comando: ");
     int res = scanf("%s",comando);
     if(res == EOF) continue;
     pid = fork();
     if ( pid == -1 ) {
       printf("Errore nella fork.\n");
       exit(1);
     }
     if ( pid == 0 )
        execlp(comando, comando, NULL);
     else wait(&status);
   }
   return 0;
}
