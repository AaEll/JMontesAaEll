// Authors: Justin Montes Aaron Elliot
//pa3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

//(NC) node constructor for end of linked list
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
	printf("value of a = 0x%08x",baseptr);
}

addrs_t Malloc (size_t size) {
  struct node * pointer;
  struct node * look_ahead;
  size = (size+(0x8-size%0x8)%0x8);
  pointer = Head;
  while(pointer!=NULL){
    look_ahead = pointer->next;
    if (look_ahead != NULL){
		if ((uint64_t)(look_ahead->start)-(uint64_t)(pointer->end)>=size){
			// Make a new node and set pointer.next = to it, and its pointer to look_ahead
			struct node * new = malloc(sizeof(struct node));
			init_node_types_3(new,pointer->end,pointer->end+size,pointer->next);
			pointer->next = new;
<<<<<<< HEAD
			return new->start;
=======
			return new->start;		
>>>>>>> 3290483d880791fb9b16bfef0a9c849dfd1dcc28
		}
    }
    else if (TOTALSIZE+Head->end - pointer->end >=size){ // IF we reach the end of the linked list, THEN check if there is space
      struct node * new = malloc(sizeof(struct node));
      init_node_types_2(new,pointer->end,pointer->end+size);
      pointer->next = new;
      return new->start;
    }
    pointer = look_ahead;
  }
  //printf("NoSpaceLeftError : no space left");
  return (NULL);
}

// free memory address
void Free (addrs_t addr) {
  struct node* current = Head;
  while (current!=NULL){
    struct node* temp = current->next;
		if (((char*)(temp->start)) == ((char*)(addr)) ){
		  current->next = current->next->next;
		  free(temp);
		  break;
		}
    current = temp;
  }
}

addrs_t Put (any_t data, size_t size) {
<<<<<<< HEAD
  addrs_t rtnVal = Malloc (size);
  memmove(rtnVal,data,size);
=======
  /*TODO add the "data" to the position rtnval */
  addrs_t rtnVal = Malloc (size); 
  memmove(&data,rtnVal,size);
>>>>>>> 3290483d880791fb9b16bfef0a9c849dfd1dcc28
  return rtnVal;
}

void Get (any_t return_data, addrs_t addr, size_t size) {
<<<<<<< HEAD
  memmove(return_data, addr, size); //This is giving a segfault
<<<<<<< HEAD
  Free(addr);
}

// Part 2

//init array dynamically in heap

void heap_init(size_t size){
  addrs_t baseptr;
  baseptr = (addrs_t) malloc(size);
  Head = malloc(sizeof(struct node));
  init_node_types_2(Head,baseptr, baseptr);
  TOTALSIZE = size;
  printf("value of a = 0x%08x",baseptr);
}

addrs_t *VMalloc (size_t size){
  struct node * pointer;
  struct node * look_ahead;
  size = (size+(0x8-size%0x8)%0x8);
  pointer = Head;
  while(pointer!=NULL){
    look_ahead = pointer->next;
    if (look_ahead != NULL){
		if ((uint64_t)(look_ahead->start)-(uint64_t)(pointer->end)>=size){
			// Make a new node and set pointer.next = to it, and its pointer to look_ahead
			struct node * new = malloc(sizeof(struct node));
			init_node_types_3(new,pointer->end,pointer->end+size,pointer->next);
			pointer->next = new;
			return new->start;
		}
    }
    else if (TOTALSIZE+Head->end - pointer->end >=size){ // IF we reach the end of the linked list, THEN check if there is space
      struct node * new = malloc(sizeof(struct node));
      init_node_types_2(new,pointer->end,pointer->end+size);
      pointer->next = new;
      return new->start;
    }
    pointer = look_ahead;
  }
  //printf("NoSpaceLeftError : no space left");
  return (NULL);
}

void VFree (addrs_t *addr){
  // frees corresponding node
  struct node* current = Head;
  while (current!=NULL){
    struct node* temp = current->next;
    if ( ((char*)(temp->start)) == ((char*)(addr)) ){
	  current->next = current->next->next;
	  free(temp);
      break;
    }
    current = temp;
  }
    while(current != NULL){
      struct node* temp = current->next;
      if(temp != NULL){
        memmove(current->end,temp->start,(*uint64_t)temp->end-(*uint64_t)temp->start);
        temp->end = current->end+(*uint64_t)temp->end-(*uint64_t)temp->start);
        temp->start = current->end;
      }
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
=======
  /*
  int i;
  struct node* pointer = Head;
  for (i=0; i<size; i++){
	  if(pointer.start==addr){
		  *((char*)(return_data)) = *((char*)(addr));
	  }
	  pointer = pointer->next;
  }*/
=======
  memmove(return_data, addr, size);
>>>>>>> 1c96bbded24b16b40d5c1247a6d1074fa16be35d
  Free(addr);
}


>>>>>>> 3290483d880791fb9b16bfef0a9c849dfd1dcc28
