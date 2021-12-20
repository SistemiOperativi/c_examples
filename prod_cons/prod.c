#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>          /* For truncate */
#include <stdio.h>           /* For printf */
#include <stdlib.h>          /* For exit */

#include "prod_cons.h"



int main(int argc, char *argv[]){
    shared_data_t *shared_data;    
    pthread_barrierattr_t pthread_barrierattr;
    pthread_mutexattr_t   pthread_mutexattr;
    int res;
    pid_t pid = getpid();

    int fd = shm_open(SHARED_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
    if(fd == -1) {
        log("The shared memory already exists...Probably there is another producer...exit\n");
        goto fail_exit;
    }
    
    res = ftruncate(fd, SHARED_SIZE); 
    if(res == -1) {
        log("I cannot allocate enough memory...exit\n");
        goto abort;
    }
    
    shared_data = mmap(NULL, SHARED_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shared_data == MAP_FAILED) {
        log("I cannot map memory...exit\n");
        goto abort;
    }

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

    exit(0);

abort:
    shm_unlink(SHARED_NAME);
    close(fd);
fail_exit:
    exit(1);


}
