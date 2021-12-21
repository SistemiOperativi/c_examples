#include <stdio.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#define PASS   "RM"
#define SECRET "NOT SECRET AT ALL"
#define SECRET_FILE "secret2.txt"

char buffer[8]   = "dummy";
char pass[8]     = PASS;
char secret[256] = SECRET;

void disclose_secret(){
   printf("%s\n", "Here is the secret:\n");
   printf("%s\n", secret);
}

void get_string(){
   int res=0;

   printf("Who are you?");
   scanf("%s", buffer);
    
   printf("Check identity of %s:", buffer);
   if(strcmp(buffer, pass) == 0){
     printf("%s\n","OK");
     res = 1;
   }
   else
     printf("%s\n","KO");
   
   if(res == 1) {
     printf("%s\n", "OK\n");
     disclose_secret();
   }
}

void readfile(){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(SECRET_FILE, "r");
    if (fp == NULL) {
        printf("Failed to open %s\n", SECRET_FILE);
        exit(EXIT_FAILURE);
    }

    printf("Getting password...");    
    read = getline(&line, &len, fp);
    line[read-1]='\0';
    strcpy(pass, line);
    if (line) free(line);
    read = getline(&line, &len, fp);
    line[read-1]='\0';
    printf("Done\nGetting secret...");
    strcpy(secret, line);
    if (line) free(line);
    printf("Done\n");
    fclose(fp);
}

int main() {
  readfile();
  get_string();
}
