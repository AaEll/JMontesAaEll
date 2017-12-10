// Authors: Justin Montes Aaron Elliot
//pa3

#include <stdio.h>
#include <stdlib.h>

/* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */
#define DSIZE       8			/* Doubleword size (bytes) */
#define CHUNKSIZE  (1<<12)  /* Extend heap by this amount (bytes) */

//define baseptr and such
typedef char *addrs_t;
typedef void *any_t;

//Part 1

// Linked list architecture for tracking

struct node{
  // start of memory chunk
  addrs_t start;
  // end of memory chunk
  addrs_t end;
  // any_t data value
  struct node *next;
  // previous item in linked list
};

// (NC) node constructor end of linked list
void init_node_types_1(struct node* n){
   n->start = NULL;
   n->end = NULL;
   n->next = NULL;
}

//(NC)
void init_node_types_2(struct node* n, addrs_t start, addrs_t end){
   n->start = start;
   n->end = end;
   n->next = NULL;
}

// (NC)
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
   init_node_types_2(Head,NULL, baseptr);
   TOTALSIZE = size;
}

addrs_t Malloc (size_t size) { // This fails in the edge case of first node deleted.
  struct node * pointer;
  struct node * look_ahead;
  pointer = Head->next;
  size = size+(0x8-size%0x8)%0x8;
  while(pointer!=NULL){
    look_ahead = pointer->next;
    if (look_ahead != NULL){
      if (look_ahead->start-pointer->end>=size){
        // Make a new node and set pointer.next = to it, and its pointer to look_ahead
        struct node * new = malloc(sizeof(struct node));
        init_node_types_3(new,pointer->end,pointer->end+size,pointer->next);
        pointer->next = new;
        return new->start;
      }
    }
    else if (TOTALSIZE - pointer->end >=size){ // IF we reach the end of the array
      struct node * new = malloc(sizeof(struct node));
      init_node_types_2(new,pointer->end,pointer->end+size);
      pointer->next = new;
      return new->start;
    }
    pointer = look_ahead;
  }
  printf("Error");
  return (NULL);
}

// free memory address
void Free (addrs_t addr) {
  struct node* current = Head;
  while (current!=NULL){
    if ( (*(char*)(current->next)) == (*(char*)(addr)) ){
      current->next = current->next->next;
      free(addr);
      break;
    }
    current = current->next;
  }
}

addrs_t Put (any_t data, size_t size) {
  /*TODO add the "data" to the position rtnval */
  addrs_t rtnVal = Malloc (size);
  *((char*)(rtnVal))= *((char*)(data));
  rtnVal = *((addrs_t*)(rtnVal));
  return rtnVal;
}

void Get (any_t return_data, addrs_t addr, size_t size) {
  int i;
  for(i = 0; i < size; i++){
    *((char*)(return_data)) = *((char*)(addr));
  }
  Free(addr);
}

// Part 2

// VMalloc, VFree, VPut, and VGet prototypes initialization
// Note: These functions behave almost identically to Malloc() and Free(), except they work with POINTERS to addrs_t

addrs_t *VMalloc (size_t size){
  struct node * pointer;
  struct node * look_ahead;
  if(Head == NULL){
    struct node * new = malloc(sizeof(struct node));
  }
  if(Head != NULL){
    pointer = Head->next;
    size = size+(0x8-size%0x8)%0x8;
    while(pointer!=NULL){
      look_ahead = pointer->next;
      if (look_ahead != NULL){
        if (look_ahead->start-pointer->end>=size){
          // Make a new node and set pointer.next = to it, and its pointer to look_ahead
          struct node * new = malloc(sizeof(struct node));
          init_node_types_3(new,pointer->end,pointer->end+size,pointer->next);
          pointer->next = new;
          return new->start;
        }
  }
}

void VFree (addrs_t *addr){
  struct node* current = Head;
  while (current!=NULL){
    if ( (*(char*)(current->next)) == (*(char*)(addr)) ){
      current->next = current->next->next;
      free(addr);
      break;
    }
    current = current->next;
  }
}

addrs_t *VPut (any_t data, size_t size) {

   Allocate size bytes from M2 using VMalloc().
   Copy size bytes of data into Malloc'd memory.
   You can assume data is a storage area outside M2.
   Return pointer to redirection table for Malloc'd memory.
}

void VGet (any_t return_data, addrs_t *addr, size_t size) {
    Copy size bytes from the memory area, M2, to data address. The
    addr argument specifies a pointer to a redirection table entry.
    As with VPut(), you can assume data is a storage area outside M2.
    Finally, de-allocate size bytes of memory using VFree() with addr
    pointing to a redirection table entry.
}


int main(){
  return 0;
}
