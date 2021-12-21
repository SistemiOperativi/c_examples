#include <stdio.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#define PASS   "RM"
#define SECRET "NOT SECRET AT ALL"

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


int main() {
  get_string();
}
