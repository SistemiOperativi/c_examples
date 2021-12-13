#ifndef __PROD_CONS__
#define __PROD_CONS__

#define SHARED_SIZE 4096
#define SHARED_NAME "prod_cons_shm"
#define LONG_SHARED_NAME "/dev/shm/"SHARED_NAME
#define MSG_SIZE 255
#define MSG_SIZE_RAW 256
#define log(...) do{printf("[%d]:", getpid()); printf(__VA_ARGS__);}while(0)

#include <pthread.h>

typedef struct __shared_data{
	pthread_barrier_t barrier;
	pthread_mutex_t mutex;
	void *prod_base_addr;
	pid_t prod_pid;
	pid_t cons_pid;
	char message[256];
}shared_data_t;

#endif