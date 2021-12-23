#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char buf[256];

void main(int argc, char **argv){
  int len = 9;
  sprintf(buf,"%c%c%s%c",'a','\0',"345678",1);

  int fd = open("tmp", O_CREAT | O_TRUNC | O_RDWR, 0644); 
  write(fd, buf, len);
  close(fd);
  
  
  
  fd = open("tmp", O_RDONLY);
  close(0);
  dup(fd);
  execl(argv[1], argv[1], NULL);
}
