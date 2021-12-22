/*
 * ECE454 Lab 3 - Malloc
 */
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "mm.h"
#include "memlib.h"

/**
 * 
 * free block: [header (1 word) | prev | next | empty | footer (1 word)]
 * allocated block: [header (1 word) | data | padding | footer (1 word)]
 * 
 * initial init: prologue [0 | header | footer] epilogue [header]
 * 
 * must be 16B aligned, therefore double word, minimum allocator must be 16B
 * 
 * use first fit
 *  
 * round up to (exclusive) 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192. beyond
 * values below 512 should be rounded up to the nearest power of 2
 * the rest is split and re inserted
 * aka 10 linked lists
 * 
 * once fit found, take remainder after padding, and try to insert into list again
 * 
 **/


/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "default-name",
    /* First member's first name and last name */
    "kyu bum:kim",
    /* First member's student number */
    "1003969100",
    /* Second member's first name and last name (do not modify if working alone) */
    "",
    /* Second member's student number (do not modify if working alone) */
    ""
};

#define DEBUG 0

/*************************************************************************
 * Basic Constants and Macros
 * You are not required to use these macros but may find them helpful.
*************************************************************************/
/* number of segregated linked lists */
#define NUM_SEG_LISTS 10

#define MIN_POWER_SIZE 5
#define MIN_BLOCK_SIZE (1 << MIN_POWER_SIZE)

#define MIN_POWER_ROUND 7
#define MIN_ROUND_SIZE (1 << MIN_POWER_ROUND)

#define WSIZE       sizeof(void *)            /* word size (bytes) */
#define DSIZE       (2 * WSIZE)            /* doubleword size (bytes) */
#define CHUNKSIZE   (1<<7)      /* initial heap size (bytes) */

#define MAX(x,y) ((x) > (y)?(x) :(y))
#define MIN(x,y) ((x) > (y)?(y) :(x))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)          (*(uintptr_t *)(p))
#define PUT(p,val)      (*(uintptr_t *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)     (GET(p) & ~(DSIZE - 1))
#define GET_ALLOC(p)    (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)        ((char *)(bp) - WSIZE)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

void* heap_listp = NULL;

/**
 * doubly linked list of the free blocks
 */
typedef struct FreeBlocksList {
    struct FreeBlocksList * next;
    struct FreeBlocksList * prev;
} freeBlocksList;

/**
 * the list of the segregated linked lists based on size
 **/
freeBlocksList * segregatedListArray[NUM_SEG_LISTS];

/**********************************************************
 * ******************** Function Declarations *************
 * ********************************************************/
void segregatedListArray_init();
unsigned return_next_pow_2(unsigned val);
unsigned return_next_pow_2_exp(unsigned val);
int get_list_index(size_t size);
void insert_into_list(freeBlocksList * bp);
void remove_from_list(freeBlocksList * bp);
void print_segregated_list();
void debugPrint(const char* str, ...);
int mm_check();

/**********************************************************
 * Initialize the segregated list array with nulls
 **********************************************************/
void segregatedListArray_init(){
    for (int i = 0; i < NUM_SEG_LISTS; i++){
        segregatedListArray[i] = NULL;
    }
}

unsigned return_next_pow_2(unsigned val){
    unsigned power = 1;
    while (val >>= 1) power++;
    return 1 << power;
}

unsigned return_next_pow_2_exp(unsigned val){
    unsigned power = 1;
    while (val >>= 1) power++;
    return power;
}

int get_list_index(size_t size){
    unsigned power = return_next_pow_2_exp(size);
    //if index smaller than the minimum block, return 0 as index
    if (power < MIN_POWER_SIZE) return 0;
    return MIN(power - MIN_POWER_SIZE - 1, NUM_SEG_LISTS - 1);
}

void insert_into_list(freeBlocksList * bp){
    int index = get_list_index(GET_SIZE(HDRP(bp)));
    debugPrint("inserting block %p into index %d of list\n", bp, index);
    bp->next = segregatedListArray[index];
    bp->prev = NULL;
    if (segregatedListArray[index] != NULL){
        segregatedListArray[index]->prev = bp;
    }
    segregatedListArray[index] = bp;
#if DEBUG
    print_segregated_list();
#endif
}

void remove_from_list(freeBlocksList * bp){
    int index = get_list_index(GET_SIZE(HDRP(bp)));
    debugPrint("removing block %p from index %d of list\n", bp, index);

    //delete at head
    if (segregatedListArray[index] == bp){
        segregatedListArray[index] = bp->next;
    }

    //change next only if bp is NOT last
    if(bp->next != NULL){
        bp->next->prev = bp->prev;
    }

    //change prev if bp is NOT first 
    if (bp->prev != NULL){
        bp->prev->next = bp->next;
    }
#if DEBUG
    print_segregated_list();
#endif
}

/**********************************************************
 * mm_init
 * Initialize the heap, including "allocation" of the
 * prologue and epilogue
 **********************************************************/
 int mm_init(void)
 {
   if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
         return -1;
     PUT(heap_listp, 0);                         // alignment padding
     PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));   // prologue header
     PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));   // prologue footer
     PUT(heap_listp + (3 * WSIZE), PACK(0, 1));    // epilogue header
     heap_listp += DSIZE;

    segregatedListArray_init();
    
     return 0;
 }

/**********************************************************
 * coalesce
 * Covers the 4 cases discussed in the text:
 * - both neighbours are allocated
 * - the next block is available for coalescing
 * - the previous block is available for coalescing
 * - both neighbours are available for coalescing
 **********************************************************/
void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {       /* Case 1 */
        debugPrint ("coalesce: case 1 \n");

        return bp;
    }

    else if (prev_alloc && !next_alloc) { /* Case 2 */
        debugPrint ("coalesce: case 2 \n");

        //remove both current and next block
        remove_from_list((freeBlocksList *) NEXT_BLKP(bp));

        //update size
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        //////////////////////////////DONT NEED THIS?///////////
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));

        return (bp);
    }

    else if (!prev_alloc && next_alloc) { /* Case 3 */
        debugPrint ("coalesce: case 3 \n");

        //remove both current and prev block
        remove_from_list((freeBlocksList *) PREV_BLKP(bp));

        //update size
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        return (PREV_BLKP(bp));
    }

    else {            /* Case 4 */
        debugPrint ("coalesce: case 4 \n");

        //remove all from list
        remove_from_list((freeBlocksList *) PREV_BLKP(bp));
        remove_from_list((freeBlocksList *) NEXT_BLKP(bp));

        //update size
        size += GET_SIZE(HDRP(PREV_BLKP(bp)))  +
            GET_SIZE(FTRP(NEXT_BLKP(bp)))  ;
        PUT(HDRP(PREV_BLKP(bp)), PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size,0));

        return (PREV_BLKP(bp));
    }
}

/**********************************************************
 * extend_heap
 * Extend the heap by "words" words, maintaining alignment
 * requirements of course. Free the former epilogue block
 * and reallocate its new header
 * 
 * only extend heap by the correct number of words
 * and always store data into newly extended heap
 **********************************************************/
void *extend_heap(size_t words)
{
    debugPrint("extending the heap \n");
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignments */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ( (bp = mem_sbrk(size)) == (void *)-1 )
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));                // free block header
    PUT(FTRP(bp), PACK(size, 0));                // free block footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));        // new epilogue header

    return bp;
}

/**********************************************************
 * find_fit
 * Traverse the heap searching for a block to fit asize
 * Return NULL if no free blocks can handle that size
 * Assumed that asize is aligned
 **********************************************************/
void * find_fit(size_t asize)
{
    //go through the linked lists
    for (int index = get_list_index(asize); index < NUM_SEG_LISTS; index++){
        freeBlocksList * bp = segregatedListArray[index];
        //if array empty, go to next line
        if (bp == NULL) continue;
        while (bp != NULL) {
            size_t blockSize = GET_SIZE(HDRP(bp));
            //matching or greater size of wanted size
            if (blockSize >= asize){
                return (void *) bp;
            }
            bp = bp->next;
        }
    }
    return NULL;
}

/**********************************************************
 * place
 * Mark the block as allocated
 * 
 * split the block if possible
 * 
 * remove from the list
 * 
 **********************************************************/
void place(void* bp, size_t asize, bool remove)
{

    /* Get the current block size */
    size_t bsize = GET_SIZE(HDRP(bp));
  
    //get unused space size
    size_t unused_space_size = bsize - asize;

    debugPrint("placing (allocating) %p with unused space size: %lu, block size: %lu, asize: %lu\n", bp, unused_space_size, bsize, asize);
    
    if (remove)
        //remove allocated from seg list
        remove_from_list ((freeBlocksList *) bp);

    if (unused_space_size >= MIN_BLOCK_SIZE){
        //address of the unused space
        void * unused_space_block = bp + asize;

        //place header into block
        PUT(HDRP(unused_space_block), PACK(unused_space_size, 0));
        PUT(FTRP(unused_space_block), PACK(unused_space_size, 0));
        //insert free block back into seg list
        insert_into_list(unused_space_block);

        //update allocated block to the new size, set write bit to 1
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
    }
    else {
        //update block to allocated bit set
        PUT(HDRP(bp), PACK(bsize, 1));
        PUT(FTRP(bp), PACK(bsize, 1));
    }
}

/**********************************************************
 * mm_free
 * Free the block and coalesce with neighbouring blocks
 **********************************************************/
void mm_free(void *bp)
{
    if(bp == NULL){
      return;
    }

    debugPrint("freeing: %p\n", bp);

    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size,0));
    PUT(FTRP(bp), PACK(size,0));
    //coalesce everytime when freed, insert into seg list
    insert_into_list((freeBlocksList *) coalesce(bp));
}

/**********************************************************
 * mm_malloc
 * Allocate a block of size bytes.
 * The type of search is determined by find_fit
 * The decision of splitting the block, or not is determined
 *   in place(..)
 * If no block satisfies the request, the heap is extended
 **********************************************************/
void *mm_malloc(size_t size)
{
    size_t asize; /* adjusted block size */
    char * bp;

    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    if (size < MIN_ROUND_SIZE){
        size = return_next_pow_2(size);
    }

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1))/ DSIZE);

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {
        debugPrint("malloc: found fit\n");

        place(bp, asize, true);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    if ((bp = extend_heap(asize/WSIZE)) == NULL)
        return NULL;
    debugPrint("malloc: extending heap\n");
    place(bp, asize, false);
    return bp;

}

/**********************************************************
 * mm_realloc
 * Implemented simply in terms of mm_malloc and mm_free
 *********************************************************/
void *mm_realloc(void *ptr, size_t size)
{
    debugPrint("reallocing with ptr %p and size %lu\n", ptr, size);

    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0){
      mm_free(ptr);
      return NULL;
    }
    /* If oldptr is NULL, then this is just malloc. */
    if (ptr == NULL)
      return (mm_malloc(size));

    //get adjusted size
    size_t asize;
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1))/ DSIZE);

    //get current size
    size_t currentSize = GET_SIZE(HDRP(ptr));

    //1. check if the new size is the same as the current size
    if (currentSize == asize){
        //return the same pointer as nothing has changed
        return ptr;
    }
    //2. check if the current size is larger than the new size
    else if (currentSize > asize){
        //if yes, shrink the current block if possible
        //get unused space size
        place (ptr, asize, false);
        return ptr;
    }
    //3. check if the current size is smaller than the new size
    else {
        //coalesce first: coalesce could give you enough space for the realloc size
        //free current block
        PUT(HDRP(ptr), PACK(currentSize,0));
        PUT(FTRP(ptr), PACK(currentSize,0));

        void * coalescedBlock = coalesce(ptr);

        //coalesced size
        size_t coalescedSize = GET_SIZE(HDRP(coalescedBlock));

        debugPrint("coal size: %lu, asize: %lu, this size block: %lu\n", coalescedSize, asize, currentSize);
        //does fit in coalesced block, therefore copy and place
        if (coalescedSize >= asize){
            //already alloced, dont remove again
            memmove(coalescedBlock, ptr, currentSize);
            place(coalescedBlock, asize, false);
            
            return coalescedBlock;
        }

        //if not fit, extend heap and allocate
        void * nextFitBlock = find_fit(asize);
        if (nextFitBlock != NULL){
            place(nextFitBlock, asize, true);
            memmove(nextFitBlock, ptr, currentSize);
            insert_into_list((freeBlocksList *) coalescedBlock);
            return nextFitBlock;
        }
        else {
            if ((nextFitBlock = extend_heap(asize/WSIZE)) == NULL)
                return NULL;
            place(nextFitBlock, asize, false);
            debugPrint("dest block: %p, src block %p, copy size %lu\n", nextFitBlock, ptr + WSIZE, currentSize - 2*DSIZE);
            memmove(nextFitBlock, ptr , currentSize);
            debugPrint("meme comp val: %d", memcmp(nextFitBlock, ptr, currentSize));
            insert_into_list((freeBlocksList *)coalescedBlock);
            return nextFitBlock;
        }
    }
}

/**********************************************************
 * mm_check
 * Check the consistency of the memory heap
 * Return nonzero if the heap is consistant.
 *********************************************************/
int mm_check(void){
  return 1;
}

void print_segregated_list(){
    for (int i = 0; i < NUM_SEG_LISTS; i++){
        freeBlocksList * bp = segregatedListArray[i];
        while (bp != NULL){
            printf("    Index %d of list, bp: %p, bp-next: %p, bp size: %ld\n", i, (void*) bp, (void*) bp->next, GET_SIZE(HDRP(bp)));
            bp = bp->next;
        }
    }
}

void debugPrint(const char* str, ...) {
#if (DEBUG == 1)
    va_list args;
    va_start(args, str);

    char final_str[1024];

    strcpy(final_str, "[DEBUG] ");
    strcat(final_str, str);

    vfprintf(stdout, final_str, args);
    fflush(stdout);
    va_end(args);
#endif
}