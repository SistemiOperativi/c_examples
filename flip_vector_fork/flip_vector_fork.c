#define _GNU_SOURCE 
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sched.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define MAX_THREADS		(8)
#define CORES			(4)
#define SECONDS			2
#define ARRAY_LEN		10
#define ARRAY_LEN_RAW	(ARRAY_LEN+1)

#define TAS 		0
#define TTAS		1
#define PT_TTAS		2
#define TICKET		3
#define PT_MUTEX	4

#define NUM_LOCKS   5

#define MAX_TH_SIZE	12

volatile int *stop;
long *ops;
pthread_barrier_t  *ptbarrier;
volatile int *lock;             /* TAS, TTAS, TICKET */
volatile int *now;              /* SERVING TICKET */
pthread_spinlock_t ptspin_v;     /* PTHREAD SPINLOCK */
pthread_mutex_t    ptmutex_v;    /* PTHREAD MUTEX */
char *shared; //[ARRAY_LEN_RAW];


pthread_spinlock_t *ptspin = &ptspin_v;     /* PTHREAD SPINLOCK */
pthread_mutex_t    *ptmutex = &ptmutex_v;    /* PTHREAD MUTEX */

int lock_type = 0;

/* PRINT OUTPUT */
void print_column_header(){
	if(lock_type == TAS)	  printf("|     TAS     "); 
	if(lock_type == TTAS)	  printf("|    TTAS     ");
	if(lock_type == TICKET)   printf("|   TICKET    ");
	if(lock_type == PT_TTAS)  printf("|   PT_TTAS   ");
	if(lock_type == PT_MUTEX) printf("|   PT_MUTEX  ");
}

void print_header(int num_locks){
	printf("#T ");
	for(lock_type=0; lock_type<num_locks;lock_type++)
		print_column_header();
	printf("\n");
}

void print_throughput(long nops){
	int len = 0;
	char buf[MAX_TH_SIZE+1];
	while(len <MAX_TH_SIZE) buf[len++] = ' ';
	buf[len] = '\0';
	len = sprintf(buf, "%ld", *ops);
	printf("|");
	while(len++ <MAX_TH_SIZE) printf(" ");
	printf("%s ", buf);
	fflush(stdout);
}

void print_array(){
	int i;
	for(i = 0; i< ARRAY_LEN; i++)
		printf("%d ", shared[i]);
	printf("\n");
		
}

/* LOCK OPERATIONS */
void acquire(){
	if(lock_type == TAS)
		while(__sync_lock_test_and_set(lock,1));
	if(lock_type == TTAS)	{
		while(__sync_lock_test_and_set(lock,1))
			while(*lock);
	}
	if(lock_type == TICKET){
		int myticket = __sync_fetch_and_add(lock, 1);
		while(*now != myticket);
	}
	if(lock_type == PT_TTAS) pthread_spin_lock(ptspin);
	if(lock_type == PT_MUTEX)pthread_mutex_lock(ptmutex);
}

void release(){
    if(lock_type == TAS || lock_type == TTAS){	asm volatile ("":::"memory");	*lock = 0;}
	if(lock_type == TICKET){					asm volatile ("":::"memory");	*now = *now+1;}
	if(lock_type == PT_TTAS) 					pthread_spin_unlock(ptspin);
	if(lock_type == PT_MUTEX)					pthread_mutex_unlock(ptmutex);
}


/* MINI BENCHMARK */
void* stress_test(void *arg){
	long my_ops = 0;
	int i = 0;
	pthread_barrier_wait(ptbarrier);

	while(!*stop){
		acquire();
		for(i=0;i<ARRAY_LEN/2;i++){
			shared[i] ^= shared[ARRAY_LEN-1-i]; 
			shared[ARRAY_LEN-1-i] ^= shared[i];  
			shared[i] ^= shared[ARRAY_LEN-1-i]; 
		}
		release();
		my_ops+=1;
	}
	
	__sync_fetch_and_add(ops, my_ops);
	return NULL;
}


int main(int argc, char *argv[]){
	pid_t 	   pids[MAX_THREADS];
	if(argc != 2){
		printf("No parameters.\nUse as the following: flip_vector <num_locks>\n");
		exit(1);
	}
	int i,j;
	int num_locks = atoi(argv[1]);
    
	pthread_barrierattr_t pthread_barrierattr;
    pthread_mutexattr_t   pthread_mutexattr;
	int size = sizeof(int)+sizeof(long)+sizeof(pthread_barrier_t)+sizeof(int)+ARRAY_LEN_RAW;
	shared = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	
	stop 		= (int*)shared; 					shared += sizeof(int);
	ptbarrier 	= (pthread_barrier_t*)shared; 		shared += sizeof(pthread_barrier_t);
	ops 		= (long*)shared;					shared += sizeof(long);
	lock		= (int*)shared;						shared += sizeof(int);
	now 		= (int*)shared;						shared += sizeof(int);
	ptspin 		= (pthread_spinlock_t*)shared;		shared += sizeof(pthread_spinlock_t);
	ptmutex		= (pthread_mutex_t*)shared;		    shared += sizeof(pthread_mutex_t);
	
	cpu_set_t my_set;        
	CPU_ZERO(&my_set); 
	for(i=0;i<CORES;i++)
		CPU_SET(i, &my_set);

	sched_setaffinity(0, sizeof(cpu_set_t), &my_set);
	pthread_spin_init(ptspin,  PTHREAD_PROCESS_SHARED);
    pthread_mutexattr_settype(&pthread_mutexattr, PTHREAD_MUTEX_DEFAULT);
    pthread_mutexattr_setpshared(&pthread_mutexattr,  PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(ptmutex, &pthread_mutexattr);

	print_header(num_locks);

	for(i=1;i<=MAX_THREADS;i<<=1)
	{
		printf("%d  ", i);
        fflush(stdout);
		for(lock_type=0; lock_type<num_locks;lock_type++){
			*ops = 0L;
			*stop = 0;
			*lock = 0;
			*now  = 0;
			for(j=0;j<ARRAY_LEN;j++)  shared[j] = j+1;
			shared[j] = '\0';

			pthread_barrierattr_setpshared(&pthread_barrierattr, PTHREAD_PROCESS_SHARED );
			pthread_barrier_init(ptbarrier, &pthread_barrierattr, i);

			for(j=0;j<i;j++) {
				//pthread_create(ptids +j, NULL, stress_test, NULL);
				pids[j] = fork();
				if(pids[j] == 0){ 
					stress_test(NULL);
					exit(0);
				}
			}	
			sleep(SECONDS);
			__sync_bool_compare_and_swap(stop, 0, 1);
			int res;
			for(j=0;j<i;j++)	waitpid(pids[j], &res,  0);	
			pthread_barrier_destroy(ptbarrier);
			print_throughput(*ops);
			
		}
		printf("\n");
	}
	print_array();

}
