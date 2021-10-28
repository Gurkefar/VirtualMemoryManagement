#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
  // doubly-linked list
  struct memoryList *last;
  struct memoryList *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next;
static struct memoryList *temporary; // used in myfree
static struct memoryList *best; // used in myfree
static struct memoryList *worst; // used in myfree

int freeMem = 0;

/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

	/* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
	struct memoryList *tmp = head;

	while (tmp != NULL){
		head = head -> next;
		free(tmp);
		tmp = head;
	}

	myMemory = malloc(sz);
	
	/* TODO: Initialize memory management structure. */

	head = malloc(sizeof (struct memoryList));
	head->last = NULL; 
	head->next = NULL;
	head->size = sz;
	head->alloc = 0;
	head->ptr = myMemory;

}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */
void *firstFit(int requested);
void *bestFit(int requested);
void *worstFit(int requested);
void *nextFit(int requested);


void *mymalloc(size_t requested)
{
	assert((int)myStrategy > 0);
	
	switch (myStrategy)
	  {
	  case NotSet: 
	            return NULL;
	  case First:
	            return firstFit(requested);
	  case Best:
	            return bestFit(requested);
	  case Worst:
	            return NULL;
	  case Next:
	            return NULL;
	  }
	return NULL;
}

void *firstFit(int requested){
	temporary = head;
	while (temporary != NULL){
		if (temporary -> size >= requested && temporary -> alloc == 0){
			break;
		}
		temporary = temporary -> next;
	}
	if (temporary == NULL){
		return NULL;
	}

	struct memoryList *newNode = malloc(sizeof(struct memoryList));
	if (temporary -> next != NULL){
		if (temporary -> size == requested){
			temporary -> alloc = 1;
			free(newNode);
			return temporary -> ptr;
		}
		temporary -> next -> last = newNode;
		newNode -> next = temporary -> next;
		temporary -> next = newNode;
		newNode -> last = temporary;
		newNode -> size = temporary -> size - requested;
		temporary -> size = requested;
		newNode -> ptr = temporary -> ptr + requested;

		newNode -> alloc = 0;
		temporary -> alloc = 1;
		return temporary -> ptr;
	}

	temporary -> next = newNode;
	newNode -> last = temporary;
	newNode -> size = temporary -> size - requested;
	temporary -> size = requested;
	newNode -> ptr = temporary -> ptr + requested;
	newNode -> alloc = 0;
	temporary -> alloc = 1;

	if (newNode -> size == 0){
		temporary -> next = NULL;
		free(newNode);
	}
	return temporary -> ptr;
}

void *bestFit(int requested){
	temporary = head;
	while (temporary != NULL){
		if (best == NULL && temporary -> alloc == 0 && temporary -> size >= requested){
			printf("hello\n");
			best = temporary;
		}
		if (temporary -> alloc == 0 && temporary -> size >= requested && temporary -> size < best -> size){
			best = temporary;
		}
		temporary = temporary -> next;
	}
	if (best == NULL){
		return NULL;
	}

	struct memoryList *newNode = malloc(sizeof(struct memoryList));
	temporary = best;

	if (temporary -> next != NULL){
		if (temporary -> size == requested){
			temporary -> alloc = 1;
			free(newNode);
			best = NULL;
			return temporary -> ptr;
		}
		temporary -> next -> last = newNode;
		newNode -> next = temporary -> next;
		temporary -> next = newNode;
		newNode -> last = temporary;
		newNode -> size = temporary -> size - requested;
		temporary -> size = requested;
		newNode -> ptr = temporary -> ptr + requested;

		newNode -> alloc = 0;
		temporary -> alloc = 1;
		best = newNode;
		return temporary -> ptr;
	}

	temporary -> next = newNode;
	newNode -> last = temporary;
	newNode -> size = temporary -> size - requested;
	temporary -> size = requested;
	newNode -> ptr = temporary -> ptr + requested;
	newNode -> alloc = 0;
	temporary -> alloc = 1;

	if (newNode -> size == 0){
		temporary -> next = NULL;
		best = NULL;
		free(newNode);
	}
	best = newNode;
	return temporary -> ptr;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
	temporary = head;

	while(temporary != NULL){
		if (temporary -> ptr == block){
			break;
		}
		temporary = temporary -> next;
	}

	if (temporary == NULL){
		printf("Adress of block (%p) does not exist in the virtual memory pool\n\n", block);
		return NULL;
	}

	struct memoryList *deleteMe;
	temporary -> alloc = 0;


	if (temporary -> last != NULL && temporary -> last -> alloc == 0){
		deleteMe = temporary;
		temporary = temporary -> last;
		temporary -> size += deleteMe -> size;
		temporary -> next = deleteMe -> next;
		deleteMe -> next -> last = temporary;
		free(deleteMe);
	}
	if (temporary -> next != NULL && temporary -> next -> alloc == 0){
		deleteMe = temporary -> next;
		temporary -> size += deleteMe -> size;
		temporary -> next = deleteMe -> next;
		if (deleteMe -> next != NULL){
			deleteMe -> next -> last = temporary;
		}
		free(deleteMe);

	}
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
	freeMem = 0;
	int holes = 0;
	temporary = head;
	while (temporary != NULL){
		if (temporary -> next == NULL && temporary -> last == NULL){
			return 0;
		}
		if (temporary -> alloc == 0){
			freeMem += temporary -> size;
			holes++;
		}
		temporary = temporary -> next;
	}
	return holes;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	int allocated = 0;
	temporary = head;
	while (temporary != NULL){
		if (temporary -> alloc == 1){
			allocated += temporary -> size;
		}
		temporary = temporary -> next;
	}
	return allocated;
}

/* Number of non-allocated bytes */
int mem_free()
{

	return freeMem;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
	int largestFree = 0;
	temporary = head;
	while (temporary != NULL){
		if (largestFree == 0 && temporary -> alloc == 0){
			largestFree = temporary -> size;
		}
		if (temporary -> size > largestFree && temporary -> alloc == 0){
			largestFree = temporary -> size;
		}
		temporary = temporary -> next;
	}
	return largestFree;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
	int counter = 0;
	temporary = head;
	while (temporary != NULL){
        if (temporary -> alloc == 0 && temporary -> size <= size){
       		counter++;
        }
        temporary = temporary -> next;
    }
    return counter;
}       

char mem_is_alloc(void *ptr)
{
    temporary = head;

    while (temporary != NULL){
    	if (temporary -> ptr == ptr){
        	return temporary -> alloc;
        }
        temporary = temporary -> next;
    }
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy)
{
	switch (strategy)
	{
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{
	struct memoryList *tmp = head;

	while(tmp != NULL){
		//printf("size\t: %d\nalloc\t: %d\nlocation\t: %p\n", tmp->size, tmp -> alloc, tmp -> ptr);
		printf("Node is\t\t: ");

		if (tmp -> alloc == 0){
			printf("is free\n");
		}
		else{
			printf("is allocated\n");
		}

		printf("Size\t\t: %d\n", tmp -> size);
		printf("Node's address \t: %p\n", tmp);
		printf("Prev ptr\t: %p\n", tmp -> last);
		printf("Next ptr\t: %p\n", tmp -> next);
		printf("Mem ptr\t\t: %p\n", tmp -> ptr);
		printf("..\n\n");
		tmp = tmp -> next;
	}
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
	//printf("Small holes in memory: %d\n", mem_small_free(9));
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
        strategies strat;
	void *a, *b, *c, *d, *e, *f, *g, *h;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	
	initmem(strat,20);
	
	for (int i = 0; i < 20; i++){
		void *pointer = mymalloc(1);
	}
	for (int i = 1; i < 20; i+= 2)
	{
		myfree(mem_pool() + i);
	}
	for (int i = 1; i < 20; i+= 2)
	{
		void *pointer = mymalloc(1);
		printf("%p\n", pointer);
	}






	print_memory();
	print_memory_status();
	
}


// 100(0) - 50(1) - 50(1) - 100(0) <-> 50(1) <-> 100(0) 

