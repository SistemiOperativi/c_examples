#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <pthread.h>

void* child_func(void *par){
  *((int*)par) = 1;
  sleep(5);
  printf("I'm the child!\n");
  pthread_exit(par);
}

int main(){
	pthread_t ctid;
	int res, *status_ptr, status_val;
        status_ptr = &status_val;
	printf("I'm a thread. "
               "I'm going to create another thread\n");
	res = pthread_create(&ctid, NULL, child_func, &status_val);
	if(res != 0) printf("I cannot create a child");
	else{
		printf("I'm now a parent thread. "
                       "I'll wait for my child %lu thread to die...\n", ctid);
		pthread_join(ctid, (void**)&status_ptr);
		printf("My child has invoked %d\n", *status_ptr);
	}
	printf("My child is dead, so it's my time to die\n");
    return 0;
}
