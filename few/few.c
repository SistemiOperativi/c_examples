#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
	int res, status;
	printf("I'm a process and I'm going to create a child\n");
	res = fork();
	if(res < 0) printf("I cannot create a child");
	else if(res == 0){
		sleep(5);
		printf("I'm the child!\n");
		exit(1);
	}
	else{
		printf("I'm now a parent and I'll wait for my child to die...\n");
		wait(&status);
		printf("My child has invoked exit(%d)\n", WEXITSTATUS(status));
	}
	printf("My child is dead, so it's my time to die\n");
	exit(0);
}
