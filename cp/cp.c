/***************************************
 * This might not work correctly. Why? *
 ***************************************/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 250

#define abort(msg) do{printf(msg);exit(1);}while(0)

int main(int argc, char *argv[]) {
    int ifd, ofd, size_r, size_w, end = 0; 
    char buffer[BUFSIZE];

    /* check parameters */
    if (argc != 3) abort("usage: copy <source> <target>\n");

    /* open the input file and check errors */
    ifd=open(argv[1],O_RDONLY);
    if (ifd == -1) abort("input open error\n");
    
    /* opend output file and check errors */
    ofd=open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0660); 
    if (ofd == -1) abort("output creation error\n");
    
    while(!end){
        /* read up to BUFSIZE from input file and check errors */
        size_r=read(ifd,buffer,BUFSIZE);
        if (size_r == -1) abort("read error\n"); 

        /* has EOF been reached? */
        end = size_r == 0;

        /* write BUFSIZE to destination file */ 
        size_w = write(ofd,buffer,size_r);             
        if (size_w == -1) abort("write error\n");
        printf("written: %d\n", size_w);
    } 

    /* close file descriptors */
    close(ifd);
    close(ofd);
}


