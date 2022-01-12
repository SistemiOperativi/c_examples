#ifndef __PARAMETERS__
#define __PARAMETERS__



#define abort(msg) do{printf(msg);exit(1);}while(0)

#define FILE_SIZE (32*1024*sizeof(short))
#define CORES 8
#define MEM_PER_CORE (256*sizeof(short))
#define MAX_SHARED_MEM (8*1024)
#define FILENAME "dataset.bin"


#endif