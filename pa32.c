//Authors: Aaron Elliot, Justin Montes
//pa32

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

// Counters for heapcheck
int count_Vfree = 0;
int count_Vmalloc = 0;
int raw_bytes = 0;
int padded_bytes = 0;
int num_failures = 0;

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
    count_Vmalloc+= 1;
    raw_bytes += size;
    struct node * new = malloc(sizeof(struct node));
    padded_bytes +=((uint64_t)(new->start)-(uint64_t)(new->end));
    init_node_types_2(new,Tail->end,Tail->end+size);
    Tail->next = new;
	Tail = new;
    return &(new->start);
  }
  else{
    printf("NoSpaceLeftError : no space left\n");
    num_failures += 1;
    return (NULL);
  }
}
// VFree
void VFree (addrs_t *addr) {
  struct node* current = Head;
  struct node* temp = current->next;
  while (temp!=NULL){
	if (((char*)(&(temp->start))) == ((char*)(addr)) ){
    count_Vfree+=1;
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
  num_failures += 1;
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

//main
int main (int argc, char **argv) {
  int res;
  unsigned long tot_alloc_time, tot_free_time;
  int numIterations = 1000000;

  unsigned mem_size = (1<<20); // Default
  // Parse the arguments
  if (argc > 2){
    fprintf(stderr, "Usage: %s [buffer size in bytes]\n",argv[0]);
    exit(1);
  }else if (argc == 2){
    mem_size = atoi(argv[1]);
  }

  // Initialize the heap
  INIT(mem_size);

  printf("Evaluating a %s of %d KBs...\n",TESTSUITE_STR,mem_size/1024);

  test_stability(numIterations,&tot_alloc_time,&tot_free_time);

  printf("Number of allocated blocks: %d\n",count_Vmalloc);
  printf("Number of free blocks: %d\n",count_Vfree);
  printf("Raw total number of bytes allocated:  %d\n",raw_bytes);
  printf("Padded total number of bytes allocated: %d\n",padded_bytes);
  printf("Raw total number of bytes free: %d\n",(padded_bytes-raw_bytes));
  printf("Aligned total number of bytes free: %d\n",(padded_bytes-raw_bytes)*8);
  printf("Total number of Malloc requests :%d\n", count_Vmalloc);
  printf("Total number of Free requests :%d\n", count_Vfree);
  printf("Total number of request failures: %d\n",num_failures);
  printf("Average clock cycles for a Malloc request: :%lu\n",tot_alloc_time/numIterations);
  printf("Average clock cycles for a Free request: :%lu\n", tot_free_time/numIterations);
  printf("Total clock cycles for all requests: %lu\n",(tot_alloc_time+tot_free_time)/numIterations);
  printf("\n");

  return 0;
}
