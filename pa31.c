// Authors: Justin Montes Aaron Elliot
//pa3

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

// Define baseptr and such
typedef char *addrs_t;
typedef void *any_t;

// Counters for heapcheck
int count_free = 0;
int count_malloc = 0;
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
  // previous item in linked list
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
size_t TOTALSIZE;

// Init func
void Init (size_t size) {
   addrs_t baseptr;
   baseptr = (addrs_t) malloc (size);
   Head = malloc(sizeof(struct node));
   init_node_types_2(Head,baseptr, baseptr);
   TOTALSIZE = size;
}

// Malloc
addrs_t Malloc (size_t size) {
  struct node * pointer;
  struct node * look_ahead;
  size = (size+(0x8-size%0x8)%0x8);
  pointer = Head;
  while(pointer!=NULL){
    look_ahead = pointer->next;
    if (look_ahead != NULL){
  		if ((uint64_t)(look_ahead->start)-(uint64_t)(pointer->end)>=size){
        // heapcheck values
        raw_bytes += (uint64_t)(look_ahead->start)-(uint64_t)(pointer->end);
        padded_bytes += size;
        count_malloc++;
  			// Make a new node and set pointer.next = to it, and its pointer to look_ahead
  			struct node * new = malloc(sizeof(struct node));
  			init_node_types_3(new,pointer->end,pointer->end+size,pointer->next);
  			pointer->next = new;
  			return new->start;
    		}
//    }
    else if (TOTALSIZE+Head->end - pointer->end >=size){ // IF we reach the end of the linked list, THEN check if there is space
      count_malloc++;
      raw_bytes += (uint64_t)(look_ahead->start)-(uint64_t)(pointer->end);
      padded_bytes += size;
      struct node * new = malloc(sizeof(struct node));
      init_node_types_2(new,pointer->end,pointer->end+size);
      pointer->next = new;
      return new->start;
    }
    pointer = look_ahead;
  }
}
  printf("NoSpaceLeftError : no space left\n");
  num_failures += 1;
  return (NULL);
}

// Free memory address
void Free (addrs_t addr) {
  struct node* current = Head;
  struct node* temp = current->next;
  while (temp!=NULL){
	if (((char*)(temp->start)) == ((char*)(addr)) ){
    count_free++;
	  current->next = temp->next;
	  free(temp);
	  break;
	}
  num_failures += 1;
  current = temp;
	temp = current->next;
  }
}

// Put
addrs_t Put (any_t data, size_t size) {
  addrs_t rtnVal = Malloc (size);
  if (rtnVal!=NULL){
    memmove(rtnVal,data, size);
  }
  return rtnVal;
}

// Get
void Get (any_t return_data, addrs_t addr, size_t size) {
  memmove(return_data, addr, size);
  Free(addr);
}
