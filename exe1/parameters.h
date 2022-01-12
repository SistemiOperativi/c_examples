#ifndef __PARAMETERS__
#define __PARAMETERS__



#define abort(msg) do{printf(msg);exit(1);}while(0)

#define FILE_SIZE (32*1024*sizeof(short))   // FILE SIZE = 64KB
#define FILENAME "dataset.bin"			
#define CORES 8								// CPU UNITS
#define MEM_PER_CORE (256*sizeof(short))    // MEMORY ALLOCABLE FOR EACH INDEPENDENT THREAD PROCESS = 512B
#define MAX_SHARED_MEM (8*1024)             // MAXIMUM ALLOCABLE SHARED MEMORY SEGMENT = 8KB 


#endif