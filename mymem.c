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

size_t tailMem;
size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next;
static struct memoryList *temporary; // used in myfree


static struct memoryList *best;
static struct memoryList *worst;
static struct memoryList *nextFit;
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
	printf("hello\n");
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

	nextFit = head; // used for next-fit

}
//hello
/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

/*mymalloc(100) and after mymalloc(50):
null <- 500(free) -> null // Initialy there is only one head node (a block) which size is equal to the whole memory pool

null <- 100(alloc) <-> 400(free) -> null // Efter mymalloc(100) 

null <- 100(alloc) <-> 50(alloc) <-> 350(free) -> null // Efter mymalloc(50) 


null <- 100(alloc) <-> 50(alloc) <-> 100(alloc) <-> 250(free) -> null // Efter mymalloc(100) 



*/


void *newHead(size_t requested, struct memoryList *tmp){ // if new head is to be allocated
	next = (struct memoryList*) malloc(sizeof (struct memoryList));
	next -> next = tmp;
	tmp -> last = next;
	next -> last = NULL;

	tmp -> size = (tmp -> size) - requested;
	next -> size = requested;

	tmp -> alloc = 0;
	next -> alloc = 1;

	next -> ptr = myMemory;
	tmp -> ptr = tmp -> ptr + requested;

	return next;
}

void *newBlock(size_t requested, struct memoryList *tmp){ // allocate new block in memory pool
	next = (struct memoryList*) malloc(sizeof (struct memoryList));
	next -> next = tmp;
	next -> last = tmp -> last;
	tmp -> last = next;

	(tmp -> last -> last) -> next = next;

	next -> alloc = 1;
	next -> size = requested;
	tmp -> size = (tmp -> size) - requested;

	next -> ptr = tmp -> ptr;
	tmp -> ptr = next -> ptr + requested;

	if (tmp -> size == 0){
		tmp -> next -> last = next;
		next -> next = tmp -> next;
		free(tmp);
		return tmp;
	}

	return tmp;
}

void *mymalloc(size_t requested)
{
	struct memoryList *tmp = head;
	assert((int)myStrategy > 0);
	
	switch (myStrategy)
	  {
	  case NotSet: 
	  			return NULL;
	  case First:
	  			// tmp = firstFit(requested);
	  			//break;
	  			//printf("hello\n");
				while(tmp != NULL){

					
					if (tmp -> alloc == 0 && (tmp -> size >= requested) && tmp == head){
						head = newHead(requested, tmp);
						printf("%p\n", head -> next -> ptr);
						return head -> ptr;
					}	
					else if(tmp -> alloc == 0 && (tmp -> size >= requested)){
						newBlock(requested, tmp);
						return next -> ptr;
					}		

					tmp = tmp -> next;
				}
				//return NULL;
	  	//hello		
	  case Best:
	  			// tmp = bestFit(requested);
	  			//break;
	 			if (tmp -> alloc == 0 && (tmp -> size >= requested) && tmp == head){

	 				head = newHead(requested, tmp);
	 				best = head -> next;
					return head; // head is global

				}	

	            while (tmp != NULL){
            		if (best -> size < requested){ // if current best node is too small, reset best node
            			if (tmp -> alloc == 0 && tmp -> size >= requested){
            				best = tmp;
            			}
            		}
	            	if (tmp -> alloc == 0 && (tmp -> size) < (best -> size) && ((tmp -> size) >= requested)){
	            		best = tmp;
	            	}
	            	tmp = tmp -> next;
	            }

	            if (best -> alloc == 1 || best -> size < requested){
	            	printf("Allocation scheme, before illegal allocation of %lu bytes:\n\n", requested);
	            	print_memory();
	            	printf("\nMaximum allocatable block is %d\nwhich is smaler than the requested amount (%ld)\n", mem_largest_free(), requested);
	            	exit(0);
	            }

	            best = newBlock(requested, best);
				return next; // next is global
	            
	  case Worst:
	  			// tmp = worstFit(requested);
	  			//break;
	 			if (tmp -> alloc == 0 && (tmp -> size >= requested) && tmp == head){
	 				
	 				worst = newHead(requested, tmp);
	 				worst = head -> next;
	 				return head; // head is global
				}	



	            while (tmp != NULL){
            		if (worst -> size < requested){ // if current best node is too small, reset best node
            			if (tmp -> alloc == 0 && tmp -> size >= requested){
            				worst = tmp;
            			}
            		}
	            	if (tmp -> alloc == 0 && (tmp -> size) > (worst -> size) && ((tmp -> size) >= requested)){
	            		worst = tmp;
	            	}
	            	tmp = tmp -> next;
	            }

	            if (worst -> alloc == 1 || best -> size < requested){
	            	printf("Allocation scheme, before illegal allocation of %lu bytes:\n\n", requested);
	            	print_memory();
	            	printf("\nMaximum allocatable block is %d\nwhich is smaler than the requested amount (%ld)\n", mem_largest_free(), requested);
	            	exit(0);
	            }

	            worst = newBlock(requested, worst);
				return next;
	            	            
	  case Next:
	  			// tmp = nextFit(requested);
	  			//break;
	            while(nextFit != NULL){
	            	if (nextFit -> alloc == 0 && (nextFit -> size >= requested) && nextFit == head){
						head = newHead(requested, nextFit);
						return head;
					}	
					else if(nextFit -> alloc == 0 && (nextFit -> size >= requested)){
						newBlock(requested, nextFit);
						return next;
					}
					else{
						nextFit = head; // make wrap-around
						continue;
					}		
	            	nextFit = nextFit -> next;
	            }
	  }
	return NULL;
}


//hello
/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void *block)
{
	printf("in free\n");
	temporary = (struct memoryList *)block;
	temporary -> alloc = 0;

	//printf("Current node:%d %d\n", temporary -> size, temporary -> alloc);
	//printf("Next node:%d %d\n", temporary -> next -> size, temporary -> next -> alloc);

	if (temporary -> next != NULL && temporary != NULL){
		if ((temporary -> next) -> alloc == 0){

			if (temporary -> last == NULL || (temporary -> last -> last == NULL && temporary -> last -> alloc == 0)){ // if only two unallocated nodes are left
				temporary = temporary -> next;
				temporary -> size = (temporary -> size) + (temporary -> last -> size);
				head = temporary;
				head -> ptr = myMemory;
				return;
			}
			temporary = temporary -> next;
			temporary -> ptr = temporary -> last -> ptr;
			temporary -> size = (temporary -> size) + (temporary -> last -> size);
			temporary -> last -> last -> next = temporary;
			temporary -> last = temporary -> last -> last;
			//free(temporary -> last);
		}
	}
	if (temporary -> last != NULL && temporary != NULL){
		if ((temporary -> last) -> alloc == 0){	
			myfree(temporary -> last);
		}
	}

	// temporary = head
	/* while(temporary != NULL){
		if (temporary -> ptr == block){
			break
		}
		temporary = temporary -> next
	}
	if (temporary == NULL){
		return NULL
	}
	struct mymemorylist *toBeDeleted;
	temporary -> alloc = 0;
	if (temporary -> last != NULL && temporary -> last -> alloc == 0){
		toBeDeleted = temporary;
		temporary = temporary -> last;
		...
		free(toBeDeleted)
	}
	if (temporary -> next != NULL && temporary -> next -> alloc == 0){
		toBeDeleted = temporary -> next;
		...
		free(toBeDeleted)
	}



	*/
}

// 

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
	struct memoryList *tmp = head;
	int counter = 0;

	while(tmp != NULL){
		if (tmp -> alloc == 0){
			counter++;
		}
		tmp = tmp -> next;
	}

	return counter;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	struct memoryList *tmp = head;
	int total = 0;

	while (tmp != NULL){
		if (tmp -> alloc == 1){
			total += tmp -> size;
		}
		tmp = tmp -> next;
	}
	return total;
}

/* Number of non-allocated bytes */
int mem_free()
{
	struct memoryList *tmp = head;
	int freeMem = 0;

	while(tmp != NULL){
		if (tmp -> alloc == 0){
			freeMem = freeMem + tmp -> size;
		}
		tmp = tmp -> next;
	}

	return freeMem;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
	struct memoryList *tmp = head;
	int maxFreeMem = 0;

	while(tmp != NULL){
		if (tmp -> alloc == 0 && maxFreeMem == 0){
			maxFreeMem = tmp -> size;
		}
		else if(tmp -> alloc == 0 && tmp -> size > maxFreeMem){
			maxFreeMem = tmp -> size;
		}
		tmp = tmp -> next;
	}

	return maxFreeMem;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
	return 0;
}       

char mem_is_alloc(void *ptr)
{
        return 0;
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
//asd

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
			printf("is allocated\n");
		}
		else{
			printf("is free\n");
		}
		printf("Size\t\t: %d\n", tmp -> size);
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
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
        strategies strat;
	void *a, *b, *c, *d, *e, *f, *g, *h, *i, *j;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	
	initmem(strat,500);
	a = mymalloc(75);
	b = mymalloc(26);
	c = mymalloc(66);


	

	
	print_memory();
	print_memory_status();
	
}

