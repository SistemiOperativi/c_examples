/*******************************
 * ESERCIZIO 8.3
 *******************************
 * 
 * 
 * Scrivere un programma C invert che dato un file A ne inverte il contenuto e lo memorizza in nuovo file B. 
 * Il programma deve:
 *  1. riportare il contenuto di A in memoria;
 *  2. invertire la posizione di ciascun byte utilizzando un numero N di thread/processi concorrenti;
 *  3. scrivere il risultato in un nuovo file B.
 * A, B e N sono parametri che il programma deve acquisire da linea di comando.
 * 
 ********************************/


/*******************************
 * SOLUTION SUMMARY
 *******************************
 * 
 * The overall approach is: keep it simple!
 * 
 * 1. Put the whole file in memory <- we have seen the usage of read operation in multiple code example
 * 2. Avoid to invert bytes in place, but use a second buffer! (no synchronization at all) 
 * 3. Write the second buffer into the output file <- we have seen the usage of write operation in multiple code example
 * 
 ********************************/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

// this are global variables that will be set by the main thread before creating child threads!
// no need for volatile!

char *input_buffer, *output_buffer;
off_t file_lenght;
int effective_thread_count;

void* thread_func(void* param){
  // I am the idx-th thread created
  int idx = *(int*)param; 

  // compute the common slice size
  off_t slice_size = file_lenght/effective_thread_count;
  off_t remainder  = file_lenght%effective_thread_count;
  // If I am the last created thread get the remaining bytes 
  off_t my_slice_size =  slice_size + (-(idx == (effective_thread_count-1)) & remainder); // AGAIN!!! but this try to understand it by yourself :) 
  
  printf("I am thread with index %d and slice size %ld\n", *(int*)param, my_slice_size);

  for(int i=0;i<my_slice_size;i++){
    size_t source_byte       = idx*slice_size;            // skip the slice of other threads
    source_byte             += i;                         // this is the byte I want to copy into the output buffer     
    size_t destination_byte  = file_lenght-1-source_byte; // this is the position of the source byte in the destination memory buffer
    output_buffer[destination_byte] = input_buffer[source_byte]; // write in the output buffer
  }
  return NULL;
}


void parallel_invert(int thread_count){
  // recompute the effective thread count
  // each thread should be able to swap at least one item
  // so the maximum number of threads if bounded by the file_lenght 
  int maximum_thread_count = file_lenght; 

  // compute the minimum between thread_count and maximum_thread_count and store it in effective_thread_count (a global variable)
  effective_thread_count = thread_count^( (thread_count^maximum_thread_count) & (-(thread_count>maximum_thread_count)) );

  // if you do not believe this line does the check for you
  if(effective_thread_count != (thread_count < maximum_thread_count ? thread_count : maximum_thread_count) ) 
    printf("The lecturer was wrong the min is %d vs %d\n", (thread_count < maximum_thread_count ? thread_count : maximum_thread_count), effective_thread_count);
  // if you still do not understand the above line this check is more explicit
  else if(thread_count <  maximum_thread_count && effective_thread_count != thread_count)
    printf("The lecturer was wrong the min is %d vs %d\n", thread_count, effective_thread_count);
  else if(thread_count >= maximum_thread_count && effective_thread_count != maximum_thread_count)
    printf("The lecturer was wrong the min is %d vs %d\n", maximum_thread_count, effective_thread_count);
  else
    printf("The lecturer was rigth the min is %d\n", effective_thread_count);
  

  // step 2.1 allocate memory for tids
  pthread_t *tids     = malloc(effective_thread_count*sizeof(pthread_t));
  // step 2.2 allocate memory for thread index
  int *indexes = malloc(effective_thread_count*sizeof(pthread_t));

  for(int i =0; i< effective_thread_count; i++){
    indexes[i] = i;
    pthread_create(tids+i, NULL, thread_func, indexes+i); // create a child thread passing as parameter a unique index
  }

  for(int i =0; i< effective_thread_count; i++)
    pthread_join(tids[i], NULL);
  
  free(tids);
  free(indexes);
}

int main(int argc, char *argv[]) {
  if(argc != 4){
    printf("Command usage: <input file> <output file> <num_threads>");
    exit(1);
  }

  char *in_file_path  = argv[1];       // parameter A
  char *out_file_path = argv[2];       // parameter B
  int num_threads     = atoi(argv[3]); // parameter N
  if(num_threads < 1) num_threads = 1;

  // open input and output files
  int input_fd        = open(in_file_path, O_RDONLY);                               
  int output_fd       = open(out_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0660);
  size_t read_bytes   = 0;
  size_t write_bytes  = 0;

  // check for open errors
  if( input_fd < 0) { printf("error while opening %s\n",  in_file_path);exit(1);}
  if(output_fd < 0) { printf("error while opening %s\n", out_file_path);exit(1);}
  
  //////////////////////////////////////////
  // STEP 1. load file in memory
  //////////////////////////////////////////
  
  // step 1.1 get file size by moving file pointer to the end
  file_lenght = lseek(input_fd, 0, SEEK_END); // note that this is a global variable that can be accessed by child threads

  // step 1.2 restore file pointer to the beginning of the file
  lseek(input_fd, 0, SEEK_SET);
  
  // step 1.3 allocate memory for storing original and reverted file in memory
  // note that this are global variables easy that can be easily accessed by child threads
  input_buffer  = malloc(file_lenght+1);
  output_buffer = malloc(file_lenght+1);
  input_buffer[file_lenght] = 0;
  output_buffer[file_lenght] = 0;

  if(!input_buffer || !output_buffer) {printf("Cannot allocate memory\n");exit(1);}
  
  // step 1.4 read whole file in memory
  while(read_bytes < file_lenght){
    // try with a single read
    // if it does not read all bytes retry, cleary the starting address should be update for each attempt
    size_t cur_read_bytes = read(input_fd, input_buffer+read_bytes, file_lenght);
    // error check
    if(cur_read_bytes < 0) {printf("Error while reading input file\n");exit(1);}
    // update overall read bytes for next round
    read_bytes += cur_read_bytes;
  }
  
  //////////////////////////////////////////
  // STEP 2. invert the file with N threads
  //////////////////////////////////////////

  parallel_invert(num_threads);
  
  //////////////////////////////////////////
  // STEP 3. write the result on file
  //////////////////////////////////////////

  // step 3.1 write the contente of the output memory buffer into the output file
  while(write_bytes < file_lenght){
    // try with a single write
    // if it does not write all bytes retry, cleary the starting address should be update for each attempt
    size_t cur_write_bytes = write(output_fd, output_buffer+write_bytes, file_lenght);
    // error check
    if(cur_write_bytes < 0) {printf("Error while writing output file\n");exit(1);}
    // update overall write bytes for next round
    write_bytes += cur_write_bytes;
  }  

  // close each file descriptor
  close(input_fd); close(output_fd);
  // free allocated memory
  free(input_buffer);
  free(output_buffer);


}

