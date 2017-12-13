#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>


#define KBLU  "\x1B[34m"
#define KRED  "\x1B[31m"
#define KRESET "\x1B[0m"

#define ERROR_OUT_OF_MEM    0x1
#define ERROR_DATA_INCON    0x2
#define ERROR_ALIGMENT      0x4
#define ERROR_NOT_FF        0x8

#define ALIGN 8

#define rdtsc(x)      __asm__ __volatile__("rdtsc \n\t" : "=A" (*(x)))

#include "pa31.c" // Inject student's code

// Heap Checker
/*
Number of allocated blocks : XXXX
Number of free blocks  : XXXX (discounting padding bytes)
Raw total number of bytes allocated : XXXX (which is the actual total bytes requested)
Padded total number of bytes allocated : XXXX (which is the total bytes requested plus internally fragmented blocks wasted due to padding/alignment)
Raw total number of bytes free : XXXX
Aligned total number of bytes free : XXXX (which is sizeof(M1) minus the padded total number of bytes allocated. You should account for meta-datastructures inside M1 also)
Total number of Malloc requests : XXXX
Total number of Free requests: XXXX
Total number of request failures: XXXX (which were unable to satisfy the allocation or de-allocation requests)
Average clock cycles for a Malloc request: XXXX
Average clock cycles for a Free request: XXXX
Total clock cycles for all requests: XXXX
*/

// Heap Checker funtions based off testsuite
#ifdef VHEAP
  #define TESTSUITE_STR         "Virtualized Heap"
  #define INIT(msize)           VInit(msize)
  #define MALLOC(msize)         VMalloc(msize)
  #define FREE(addr, size)      VFree(addr)
  #define PUT(data,size)        VPut(data,size)
  #define GET(rt,addr,size)     VGet(rt,addr,size)
  #define ADDRS                 addrs_t*
  #define LOCATION_OF(addr)     ((size_t)(*addr))
  #define DATA_OF(addr)         (*(*(addr)))
#else
  #define TESTSUITE_STR         "Heap"
  #define INIT(msize)           Init(msize)
  #define MALLOC(msize)         Malloc(msize)
  #define FREE(addr,size)       Free(addr)
  #define PUT(data,size)        Put(data,size)
  #define GET(rt,addr,size)     Get(rt,addr,size)
  #define ADDRS                 addrs_t
  #define LOCATION_OF(addr)     ((size_t)addr)
  #define DATA_OF(addr)         (*(addr))
#endif

unsigned long tot_alloc_time, tot_free_time;
int numIterations = 1000000;

void print_testResult(int code){
  if (code){
    printf("Number of allocated blocks: %d\n",count_malloc);
    printf("Number of free blocks: %d\n",count_free);
    printf("Raw total number of bytes allocated:  %d\n",raw_bytes);
    printf("Padded total number of bytes allocated: %d\n",padded_bytes);
    printf("Raw total number of bytes free: %d\n",(padded_bytes-raw_bytes));
    printf("Aligned total number of bytes free: %d\n",(padded_bytes-raw_bytes)*8);
    printf("Total number of Malloc requests :%d\n", count_malloc);
    printf("Total number of Free requests :%d\n", count_free);
    printf("Total number of request failures: %d\n",num_failures);
    printf("Average clock cycles for a Malloc request: :%lu\n",tot_alloc_time/numIterations);
    printf("Average clock cycles for a Free request: :%lu\n", tot_free_time/numIterations);
    printf("Total clock cycles for all requests: %lu\n",(tot_alloc_time+tot_free_time)/numIterations);
    printf("\n");
  }else{
    printf("hey");
    printf("Number of allocated blocks: %d\n",count_malloc);
    printf("Number of free blocks: %d\n",count_free);
    printf("Raw total number of bytes allocated:  %d\n",raw_bytes);
    printf("Padded total number of bytes allocated: %d\n",padded_bytes);
    printf("Raw total number of bytes free: %d\n",(padded_bytes-raw_bytes));
    printf("Aligned total number of bytes free: %d\n",(padded_bytes-raw_bytes)*8);
    printf("Total number of Malloc requests :%d\n", count_malloc);
    printf("Total number of Free requests :%d\n", count_free);
    printf("Total number of request failures: %d\n",num_failures);
    printf("Average clock cycles for a Malloc request: :%lu\n",tot_alloc_time/numIterations);
    printf("Average clock cycles for a Free request: :%lu\n", tot_free_time/numIterations);
    printf("Total clock cycles for all requests: %lu\n",(tot_alloc_time+tot_free_time)/numIterations);;
  }
}

int test_stability(int numIterations, unsigned long* tot_alloc_time, unsigned long* tot_free_time){
  int i, n, res = 0;
  char s[80];
  ADDRS addr1;
  ADDRS addr2;
  char data[80];
  char data2[80];

  unsigned long start, finish;
  *tot_alloc_time = 0;
  *tot_free_time = 0;

  for (i = 0; i < numIterations; i++) {
    rdtsc(&start);
    addr1 = PUT(s, n+1);
    rdtsc(&finish);
    *tot_alloc_time += finish - start;
    addr2 = PUT(s, n+1);
    // Check for heap overflow
    if (!addr1 || !addr2){
      res |= ERROR_OUT_OF_MEM;
      break;
    }
    // Check aligment
    if (((uint64_t)addr1 & (ALIGN-1)) || ((uint64_t)addr2 & (ALIGN-1)))
      res |= ERROR_ALIGMENT;
    // Check for data consistency
    rdtsc(&start);
    GET((any_t)data2, addr2, n+1);
    rdtsc(&finish);
    *tot_free_time += finish - start;
    if (!strcmp(data,data2))
      res |= ERROR_DATA_INCON;
    GET((any_t)data2, addr1, n+1);
    if (!strcmp(data,data2))
      res |= ERROR_DATA_INCON;
  }
  return res;
}

int main (int argc, char **argv) {
  int res;
  unsigned mem_size = (1<<20); // Default
  // Parse the arguments
  if (argc > 2){
    fprintf(stderr, "Usage: %s [buffer size in bytes]\n",argv[0]);
    exit(1);
  }else if (argc == 2){
    mem_size = atoi(argv[1]);
  }

  printf("Evaluating a %s of %d KBs...\n",TESTSUITE_STR,mem_size/1024);

  // Initialize the heap
  INIT(mem_size);
  print_testResult(test_stability(numIterations,&tot_alloc_time,&tot_free_time));

  return 0;
}
