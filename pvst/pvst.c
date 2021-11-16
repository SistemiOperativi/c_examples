#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <pthread.h>

#define INIT_VALUE 0
#define PARENT_VAL 1
#define TCHILD_VAL 2
#define PCHILD_VAL 3

int global_var = INIT_VALUE;

void* child_func(void *par){
  *((int*)par) = 1;
  sleep(5);
  global_var = TCHILD_VAL;
  printf("I'm the child and I wrote the global var!\n");
  pthread_exit(par);
}

int main(){
	pthread_t ctid;
	int res, *status;
	printf("I'm a thread. "
               "I'm going to create another thread\n");
	res = pthread_create(&ctid, NULL, child_func, status);
	global_var = PARENT_VAL;
	if(res != 0) printf("I cannot create a child");
	else{
		printf("I'm now a parent thread. "
                       "I'll wait for my child thread %lu to die...\n", ctid);
		pthread_join(ctid, (void*)&status);
		printf("My child has invoked %d\n",*status);
	}
	printf("My child is dead, so it's my time to die. Global var: %d\n", global_var);
        global_var = PARENT_VAL;
        res = fork();
        if(res == -1) exit(1);
        if(res == 0){
            global_var = PCHILD_VAL;
            printf("Child Global var: %d\n", global_var);
            exit(0);
        }
        wait(&res);
        printf("Parent Global var: %d\n", global_var);
    return 0;
}
