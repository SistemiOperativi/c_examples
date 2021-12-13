#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>          /* For truncate */
#include <stdio.h>           /* For printf */

#include "prod_cons.h"



int main(int argc, char *argv[]){
    shared_data_t *shared_data;    
    pid_t pid = getpid();
    pthread_barrierattr_t pthread_barrierattr;
    pthread_mutexattr_t   pthread_mutexattr;

    int fd = shm_open(SHARED_NAME, O_CREAT | O_RDWR, 0666);
    
    ftruncate(fd, SHARED_SIZE); 
    shared_data = mmap(NULL, SHARED_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    pthread_barrierattr_setpshared(&pthread_barrierattr, PTHREAD_PROCESS_SHARED );
    pthread_barrier_init(&shared_data->barrier, &pthread_barrierattr, 2);

    pthread_mutexattr_settype(&pthread_mutexattr, PTHREAD_MUTEX_DEFAULT);
    pthread_mutexattr_setpshared(&pthread_mutexattr,  PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shared_data->mutex, &pthread_mutexattr);

    log("I'll WAIT for a process at %p\n", shared_data);
    shared_data->prod_pid = pid;
    shared_data->prod_base_addr = shared_data;
    shared_data->message[MSG_SIZE] = '\0';
    pthread_mutex_lock(&shared_data->mutex);
    pthread_barrier_wait(&shared_data->barrier);
    log("Process %d arrived\n", shared_data->cons_pid);


    log("I'll produce data...\n");
    sprintf(shared_data->message, "Nice to meet you");
    log("Done\n");

    pthread_mutex_unlock(&shared_data->mutex);


}
