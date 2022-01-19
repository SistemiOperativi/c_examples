#include <stdio.h>

void file_check(char *file_name, int num_threads){
  printf("file_check invoked\n");

}


void main(){
  file_check("test.txt", 8);
}

