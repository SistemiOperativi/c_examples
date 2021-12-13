#define _GNU_SOURCE 
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sched.h>

#define MAX_THREADS		(32)
#define CORES			(4)
#define SECONDS			2
#define ARRAY_LEN		256
#define ARRAY_LEN_RAW	(ARRAY_LEN+1)

#define TAS 		0
#define TTAS		1
#define PT_TTAS		2
#define TICKET		3
#define PT_MUTEX	4

#define MAX_TH_SIZE	12

volatile int stop = 0;
int lock_type = 0;
long ops = 0;
pthread_barrier_t  ptbarrier;
unsigned char shared[ARRAY_LEN_RAW];
char pad0[64-sizeof(long)-2*sizeof(int)-sizeof(pthread_barrier_t)-ARRAY_LEN_RAW%(64-sizeof(long)-2*sizeof(int)-sizeof(pthread_barrier_t))];

/* TAS, TTAS, TICKET */
volatile int lock = 0;
char pad1[60];

/* SERVING TICKET */
volatile int now = 0;
char pad2[60];

/* PTHREAD SPINLOCK */
pthread_spinlock_t ptspin;
char pad5[64-sizeof(pthread_spinlock_t)];

/* PTHREAD MUTEX */
pthread_mutex_t    ptmutex;
char pad6[64-sizeof(pthread_mutex_t)];


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
	len = sprintf(buf, "%ld", ops);
	printf("|");
	while(len++ <MAX_TH_SIZE) printf(" ");
	printf("%s ", buf);
	fflush(stdout);
}


void print_array(){
	int i;
	for(i = 0; i< ARRAY_LEN; i++)
		printf("%u ", ((unsigned int)shared[i]));
	printf("\n");
		
}

/* LOCK OPERATIONS */
void acquire(){
	if(lock_type == TAS)
		while(__sync_lock_test_and_set(&lock,1));
	if(lock_type == TTAS){
		while(__sync_lock_test_and_set(&lock,1))
			while(lock);
	}
	if(lock_type == TICKET){
		int myticket = __sync_fetch_and_add(&lock, 1);
		while(now != myticket);
	}
	if(lock_type == PT_TTAS) pthread_spin_lock(&ptspin);
	if(lock_type == PT_MUTEX)pthread_mutex_lock(&ptmutex);
}

void release(){
	if(lock_type == TAS || lock_type == TTAS){	asm volatile ("":::"memory");	lock = 0;}
	if(lock_type == TICKET){					asm volatile ("":::"memory");	now = now+1;}
	if(lock_type == PT_TTAS) 					pthread_spin_unlock(&ptspin);
	if(lock_type == PT_MUTEX)					pthread_mutex_unlock(&ptmutex);
}



/* MINI BENCHMARK */
void* stress_test(void *arg){
	long my_ops = 0;
	int i = 0;
	pthread_barrier_wait(&ptbarrier);

	while(!stop){
		acquire();
		for(i=0;i<ARRAY_LEN/2;i++){
			shared[i] ^= shared[ARRAY_LEN-1-i]; 
			shared[ARRAY_LEN-1-i] ^= shared[i];  
			shared[i] ^= shared[ARRAY_LEN-1-i]; 
		}
		release();
		my_ops+=1;
	}
	
	__sync_fetch_and_add(&ops, my_ops);
	return NULL;
}


int main(int argc, char *argv[]){
	pthread_t ptids[MAX_THREADS];
	int i,j;
	if(argc != 2){
		printf("No parameters.\nUse as the following: flip_vector <num_locks>\n");
		exit(1);
	}
	int num_locks = atoi(argv[1]);

	cpu_set_t my_set;      
	CPU_ZERO(&my_set); 
	for(i=0;i<CORES;i++)
		CPU_SET(i, &my_set);

	for(j=0;j<ARRAY_LEN;j++)  shared[j] = j;
                        shared[j] = '\0';

	sched_setaffinity(0, sizeof(cpu_set_t), &my_set);
	pthread_spin_init(&ptspin,  PTHREAD_PROCESS_PRIVATE);
	pthread_mutex_init(&ptmutex, NULL);

	print_header(num_locks);

	for(i=1;i<=MAX_THREADS;i<<=1)
	{
		printf("%d  ", i);

		for(lock_type=0; lock_type<num_locks;lock_type++){
			ops = 0L;
			stop = 0;
			lock = 0;
			now  = 0;
			//for(j=0;j<ARRAY_LEN;j++)  shared[j] = j+1;
			//shared[j] = '\0';

			pthread_barrier_init(&ptbarrier, NULL, i);

			for(j=0;j<i;j++)	pthread_create(ptids +j, NULL, stress_test, NULL);
					
			sleep(SECONDS);
			__sync_bool_compare_and_swap(&stop, 0, 1);
			
			for(j=0;j<i;j++)	pthread_join(ptids[j],  NULL);

			pthread_barrier_destroy(&ptbarrier);

			print_throughput(ops);
			
		}
		printf("\n");
	}
	print_array();
}
