Authors: Aaron Elliot, Justin Montes
pa32

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

/* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */
#define DSIZE       8			/* Doubleword size (bytes) */
#define CHUNKSIZE  (1<<12)  /* Extend heap by this amount (bytes) */
#define VHEAP

// Define baseptr and such
typedef char *addrs_t;
typedef void *any_t;

// Part 1

// Linked list architecture for tracking
struct node{
  // start of memory chunk
  addrs_t start;
  // end of memory chunk
  addrs_t end;
  // next node in linked list
  struct node *next;

};

// (NC) node null constructor
void init_node_types_1(struct node* n){
   n->start = NULL;
   n->end = NULL;
   n->next = NULL;
}

// (NC) node constructor for end of linked list
void init_node_types_2(struct node* n, addrs_t start, addrs_t end){
   n->start = start;
   n->end = end;
   n->next = NULL;
}

// (NC) node constructor for middle of linked list
void init_node_types_3(struct node* n, addrs_t start, addrs_t end, struct node* nextNode){
   n->start = start;
   n->end = end;
   n->next = nextNode;
}

// Init head of list
struct node * Head;
struct node * Tail;
size_t TOTALSIZE;

// Init func
void VInit (size_t size) { // heap alloc
   addrs_t baseptr;
   baseptr = (addrs_t) malloc (size);
   Head = malloc(sizeof(struct node));
   Tail = Head;
   init_node_types_2(Head,baseptr, baseptr);
   TOTALSIZE = size;
}


// VMalloc
addrs_t * VMalloc (size_t size) {
  size = (size+(0x8-size%0x8)%0x8);
  if (TOTALSIZE+Head->end - Tail->end >= size){ // IF we reach the end of the linked list, THEN check if there is space
    struct node * new = malloc(sizeof(struct node));
    init_node_types_2(new,Tail->end,Tail->end+size);
    Tail->next = new;
	Tail = new;
    return &(new->start);
  }
  else{
    printf("NoSpaceLeftError : no space left\n");
    return (NULL);
  }
}
// VFree
void VFree (addrs_t *addr) {
  struct node* current = Head;
  struct node* temp = current->next;
  while (temp!=NULL){
	if (((char*)(&(temp->start))) == ((char*)(addr)) ){
	  current->next = temp->next;
	  if (temp->next==NULL){
		Tail = current;
	  }
	  free(temp);
	  break;
	}
    current = temp;
	temp = current->next;
  }
  temp = current->next;
  while(temp != NULL){
	size_t size = temp->end-temp->start;
	memmove(current->end,temp->start,size);
	temp -> start = current -> end;
	temp -> end   = temp->start + size;
	current = temp;
	temp = current->next;
  }
}
// VPut
addrs_t* VPut (any_t data, size_t size) {
  addrs_t* rtnVal = VMalloc (size);
  if (rtnVal!=NULL){
    memmove(*rtnVal,data, size);
  }
  return rtnVal;
}

// VGet
void VGet (any_t return_data, addrs_t *addr, size_t size) {
  memmove(return_data, *addr, size);
  VFree(addr);
}


// Heap Checker
/*
Number of allocated blocks : XXXX
Number of free blocks  : XXXX
Raw total number of bytes allocated : XXXX
Padded total number of bytes allocated : XXXX
Raw total number of bytes free : XXXX
Aligned total number of bytes free : XXXX (which is sizeof(M2) minus the padded total number of bytes allocated. You should account for meta-datastructures inside M2 also)
Total number of VMalloc requests : XXXX
Total number of VFree requests: XXXX
Total number of request failures: XXXX
Average clock cycles for a VMalloc request: XXXX
Average clock cycles for a VFree request: XXXX
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

void print_testResult(int code){
  if (code){
    printf("[%sFailed%s] due to: ",KRED,KRESET);
    if (code & ERROR_OUT_OF_MEM)
      printf("<OUT_OF_MEM>");
    if (code & ERROR_DATA_INCON)
      printf("<DATA_INCONSISTENCY>");
    if (code & ERROR_ALIGMENT)
      printf("<ALIGMENT>");
    printf("\n");
  }else{
    printf("[%sPassed%s]\n",KBLU, KRESET);
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

int test_ff(){
  int err = 0;
  // Round 1 - 2 consequtive allocations should be allocated after one another
  ADDRS v1;
  ADDRS v2;
  ADDRS v3;
  ADDRS v4;
  v1 = MALLOC(8);
  v2 = MALLOC(4);
  if (LOCATION_OF(v1) >= LOCATION_OF(v2))
    err |= ERROR_NOT_FF;
  if ((LOCATION_OF(v1) & (ALIGN-1)) || (LOCATION_OF(v2) & (ALIGN-1)))
    err |= ERROR_ALIGMENT;
  // Round 2 - New allocation should be placed in a free block at top if fits
  FREE(v1,8);
  v3 = MALLOC(64);
  v4 = MALLOC(5);
  if (LOCATION_OF(v4) != LOCATION_OF(v1) || LOCATION_OF(v3) < LOCATION_OF(v2))
    err |= ERROR_NOT_FF;
  if ((LOCATION_OF(v3) & (ALIGN-1)) || (LOCATION_OF(v4) & (ALIGN-1)))
    err |= ERROR_ALIGMENT;
  // Round 3 - Correct merge
  FREE(v4,5);
  FREE(v2,4);
  v4 = MALLOC(10);
  if (LOCATION_OF(v4) != LOCATION_OF(v1))
    err |= ERROR_NOT_FF;
  // Round 4 - Correct Merge 2
  FREE(v4,10);
  FREE(v3,64);
  v4 = MALLOC(256);
  if (LOCATION_OF(v4) != LOCATION_OF(v1))
    err |= ERROR_NOT_FF;
  // Clean-up
  FREE(v4,256);
  return err;
}

int test_maxNumOfAlloc(){
  int count = 0;
  char *d = "x";
  printf("FF\n");
  const int testCap = 1000000-1;
  ADDRS allocs[testCap];
  printf("FF\n");
  printf("FF\n");


  while ((allocs[count]=PUT(d,1)) && count < testCap){
    if (DATA_OF(allocs[count])!='x') break;
    count++;
  }
  // Clean-up
  int i;
  for (i = 0 ; i < count ; i++)
    FREE(allocs[i],1);
  return count;
}

int test_maxSizeOfAlloc(int size){
  char* d = "x";
  if (!size) return 0;
  ADDRS v1 = MALLOC(size);
  if (v1){
    return size + test_maxSizeOfAlloc(size>>1);
  }else{
    return test_maxSizeOfAlloc(size>>1);
  }
}

//main
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

  unsigned long tot_alloc_time, tot_free_time;
  int numIterations = 1000000;
  // Initialize the heap
  INIT(mem_size);
  // Test 1
  printf("Number of allocated blocks: \t");
  print_testResult(test_stability(numIterations,&tot_alloc_time,&tot_free_time));
  printf("\tNumber of free blocks: %lu\n",tot_alloc_time/numIterations);
  printf("\tRaw total number of bytes allocated:  %lu\n",tot_free_time/numIterations);
  printf("\tPadded total number of bytes allocated: %lu\n",numIterations/tot_alloc_time+tot_free_time);
  // Test 2
  #ifndef VHEAP
  printf("Raw total number of bytes free: \t\t");
  print_testResult(test_ff());
  #endif
  printf("Raw total number of bytes free: \t\t");
  // Test 3:
  printf("Aligned total number of bytes free:\t");
  printf("[%s%i%s]\n",KBLU,test_maxNumOfAlloc(), KRESET);
  // Test 4:
  printf("Total number of Malloc requests :\t\t");
  printf("[%s%i KB%s]\n", KBLU, test_maxSizeOfAlloc(4*1024*1024)>>10, KRESET);
  printf("Total number of Free requests :\t\t");
  printf("Total number of request failures: \t\t");
  printf("Average clock cycles for a Malloc request: \t\t");
  printf("Average clock cycles for a Free request: \t\t");
  printf("Total clock cycles for all requests: \t\t");

  return 0;
}
