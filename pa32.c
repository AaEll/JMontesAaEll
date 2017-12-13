#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
int main(){
  Init(80);
  char data[80];
  int num_trials = 1000000;
  int counter;
  for(counter = 0; counter < num_trials; counter++ ){
    VPut(TOTALSIZE);
    VGet(VMalloc(TOTALSIZE));
  }
  printf("Num of VMalloc calls for VPut run with trial size n = 1,000,000 = %d\n",count_Vmalloc);
  printf("Num of VFree calls for VGet run with trial size n = 1,000,000 = %d\n",count_Vfree);
}
