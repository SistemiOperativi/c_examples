#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>          /* For truncate */
#include <stdio.h>           /* For printf */
#include <stdlib.h>          /* For exit */

#include "prod_cons.h"



int main(int argc, char *argv[]){
    shared_data_t *shared_data;
    pid_t pid = getpid();


    int fd = shm_open(SHARED_NAME, O_RDWR, 0666);
    if(fd == -1) {log("shared mem not initialized\n");exit(1);}
    shared_data = mmap(NULL, SHARED_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(shared_data == MAP_FAILED) {
        log("I cannot map memory...exit\n");
        close(fd);
        exit(1);
    }

    log("shared_data cons address %p\n", shared_data);
    log("shared_data prod address %p\n", shared_data->prod_base_addr);
    close(fd);  

    log("I'll JOIN for a process at %p\n", shared_data);
    shared_data->cons_pid = pid;
    pthread_barrier_wait(&shared_data->barrier);
    log("Joined. prod pid %d:\n", shared_data->prod_pid);

    pthread_mutex_lock(&shared_data->mutex);    
    log("I'll consume data...\n");
    log("Message: '%s'\n", shared_data->message);
    log("Done\n");

    pthread_mutex_unlock(&shared_data->mutex);

    shm_unlink(SHARED_NAME);
}
