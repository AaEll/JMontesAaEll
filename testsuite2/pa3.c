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
  size = (size+(0x8-size%0x8)%0x8)/8;
  if(Head == NULL){ // if Init has not been used, then allocate just enough space for this item
	printf("UnitializedError : Init has not been ran	");
	addrs_t baseptr; // maybe remove this sub-section
	baseptr = (addrs_t) malloc (size);
    Head = malloc(sizeof(struct node));
	struct node * new;
	new = malloc(sizeof(struct node));
	init_node_types_3(Head,baseptr-1, baseptr,new);
	init_node_types_2(new,baseptr,baseptr+size+1);	
	TOTALSIZE = size;
    return new->start;
  }
  pointer = Head;
  while(pointer!=NULL){
    look_ahead = pointer->next;
    if (look_ahead != NULL){
      if (look_ahead->start-pointer->end>=size){
        // Make a new node and set pointer.next = to it, and its pointer to look_ahead
        struct node * new = malloc(sizeof(struct node));
        init_node_types_3(new,pointer->end,pointer->end+size+1,pointer->next);
        pointer->next = new;
        return new->start;
      }
	  printf("DXC");
    }
    else if (TOTALSIZE+Head->end - pointer->end >=size){ // IF we reach the end of the linked list, THEN check if there is space
      struct node * new = malloc(sizeof(struct node));
      init_node_types_2(new,pointer->end,pointer->end+size+1);
      pointer->next = new;
	  printf("valAssigneda = 0x%08x",new->start);
      return new->start;
    }
    pointer = look_ahead;
  }
  printf("NoSpaceLeftError : no space left");
  return (NULL);
}

// free memory address
void Free (addrs_t addr) {
  struct node* current = Head;
  while (current!=NULL){
    if ( (*(char*)(current->next)) == (*(char*)(addr)) ){
	  struct node* temp = current->next;
	  current->next = current->next->next;
	  free(temp);
      break;
    }
    current = current->next;
  }
}

addrs_t Put (any_t data, size_t size) {
  /*TODO add the "data" to the position rtnval */
  addrs_t rtnVal = Malloc (size); 
  memmove(&data,rtnVal,size);
  printf("valreturned = 0x%08x\n",rtnVal);
  return rtnVal;
}

void Get (any_t return_data, addrs_t addr, size_t size) {
  memmove(return_data, addr, size); //This is giving a segfault
  /*
  int i;
  struct node* pointer = Head;
  for (i=0; i<size; i++){
	  if(pointer.start==addr){
		  *((char*)(return_data)) = *((char*)(addr));
	  }
	  pointer = pointer->next;
  }*/
  Free(addr);
}


