#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <pthread.h>

void* child_func(void *par){
  *((int*)par) = 1;
  pthread_t ctid;
  sleep(5);
  printf("I'm the child!\n");
  pthread_exit(par);
}

void main(){
	pthread_t ctid;
	int res, *status;
	printf("I'm a thread. "
               "I'm going to create another thread\n");
	res = pthread_create(&ctid, NULL, child_func, status);
	if(res != 0) printf("I cannot create a child");
	else{
		printf("I'm now a parent thread. "
                       "I'll wait for my child thread to die...\n");
		pthread_join(ctid, (void*)&status);
		printf("My child has invoked %d\n",*status);
	}
	printf("My child is dead, so it's my time to die\n");
}
