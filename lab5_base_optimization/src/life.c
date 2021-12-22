/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include "life.h"

/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/
/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[(__j) + LDA*(__i)])

typedef struct thread_args {
    char* inboard;
    char* outboard;
    int id;
    int nrows;
    int ncols;
    int gens_max;
} thread_args;

const int NUM_THREADS = 16;
pthread_barrier_t sync_universe;

/*****************************************************************************
 * Game of life implementation
 ****************************************************************************/
char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max){

    if (NUM_THREADS == 1){
        return sus_life(outboard, inboard, nrows, ncols, gens_max);
    }

    pthread_t threads[NUM_THREADS];
    thread_args args[NUM_THREADS];
    
    pthread_barrier_init(&sync_universe, NULL, NUM_THREADS);
    cpu_set_t cpu;
    
    //first thread
    args[0].id = 0;
    args[0].inboard = inboard;
    args[0].outboard = outboard;
    args[0].nrows = nrows;
    args[0].ncols = ncols;
    args[0].gens_max = gens_max;
    CPU_ZERO(&cpu);
    CPU_SET(0, &cpu);
    pthread_create(&threads[0], NULL, sus_life_first, &args[0]);
    pthread_setaffinity_np(threads[0], sizeof(cpu_set_t), &cpu);

    for (int i = 1; i < NUM_THREADS - 1; i++){
        args[i].id = i;
        args[i].inboard = inboard;
        args[i].outboard = outboard;
        args[i].nrows = nrows;
        args[i].ncols = ncols;
        args[i].gens_max = gens_max;
        CPU_ZERO(&cpu);
        CPU_SET(i, &cpu);
        pthread_create(&threads[i], NULL, sus_life_mid, &args[i]);
        pthread_setaffinity_np(threads[i], sizeof(cpu_set_t), &cpu);
    }

    const int last = NUM_THREADS - 1;
    args[last].id = last;
    args[last].inboard = inboard;
    args[last].outboard = outboard;
    args[last].nrows = nrows;
    args[last].ncols = ncols;
    args[last].gens_max = gens_max;
    CPU_ZERO(&cpu);
    CPU_SET(last, &cpu);
    pthread_create(&threads[last], NULL, sus_life_last, &args[last]);
    pthread_setaffinity_np(threads[last], sizeof(cpu_set_t), &cpu);

    for (int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    return inboard;
}


void * sus_life_first(void * arg){

    thread_args* args = (thread_args *) arg;
    const int rows_to_do = args->nrows / NUM_THREADS;

    const int nrows = args->nrows;
    const int ncols = args->ncols;
    char* inboard = args->inboard;
    char* outboard = args->outboard;
    const int gens_max = args->gens_max;

    // index: XXX CCCC S
	// CCCC is count of neighbors
	// S is status of cell
    const char NEXT_GEN_STATUS[18] = {
        0, 0,
        0, 0,
        0, 1,
        1, 1,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0
    };

    const int LDA = args->nrows;
    int curgen, i, j;

    for (curgen = 0; curgen < gens_max; curgen++)
    {
        int left_col = 0, mid_col = 0, right_col = 0;
        {
            // first corner
            i = 0; j = 0;
            const int inorth1 = nrows - 1;
            const int isouth1 = 1;
            const int jwest1 = ncols - 1;
            const int jeast1 = 1;
            left_col = BOARD (inboard, inorth1, jwest1) + 
                BOARD (inboard, i, jwest1) +
                BOARD (inboard, isouth1, jwest1);

            mid_col = BOARD (inboard, inorth1, j) + 
                BOARD (inboard, isouth1, j);

            right_col = BOARD (inboard, inorth1, jeast1) + 
                BOARD (inboard, i, jeast1) + 
                BOARD (inboard, isouth1, jeast1);

			const int current_cell_status = BOARD(inboard, i, j);
            BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col << 1 | current_cell_status];

            // shift columns to left (cell to the right)
            // make sure to add the current cell's state
            left_col = mid_col + current_cell_status;
            mid_col = right_col;

        }

        // first row without corner
        i = 0;
        for (j = 1; j < ncols - 1; j++){
            const int inorth = nrows - 1;
            const int isouth = 1;
            const int jeast = j + 1;

            right_col = BOARD (inboard, inorth, jeast) + 
                BOARD (inboard, i, jeast) + 
                BOARD (inboard, isouth, jeast);

			const int current_cell_status = BOARD(inboard, i, j);
            BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];

            // shift columns to left (cell to the right)
            left_col = mid_col;
            mid_col = right_col;
        }

        {
            // second corner
            i = 0; j = ncols - 1;
            const int inorth2 = nrows - 1;
            const int isouth2 = 1;
            const int jeast2 = 0;

            right_col = BOARD (inboard, inorth2, jeast2) + 
                BOARD (inboard, i, jeast2) + 
                BOARD (inboard, isouth2, jeast2);
            
            const int current_cell_status = BOARD(inboard, i, j);
            BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];
        }

        // second row to second last row
        for (i = 1; i < rows_to_do; i++){
            const int inorth = i - 1;
            const int isouth = i + 1;
            {
                // first column
                j = 0;
                const int jwestL = ncols - 1;
                const int jeastL = j + 1;
                left_col = BOARD (inboard, inorth, jwestL) + 
                    BOARD (inboard, i, jwestL) +
                    BOARD (inboard, isouth, jwestL);

                mid_col = BOARD (inboard, inorth, j) + 
                    BOARD (inboard, isouth, j);

                right_col = BOARD (inboard, inorth, jeastL) + 
                    BOARD (inboard, i, jeastL) + 
                    BOARD (inboard, isouth, jeastL);
            
                const int current_cell_status = BOARD(inboard, i, j);
				BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col << 1 | current_cell_status];

                // shift columns to left (cell to the right)
                // making sure to add the current cell into sum
                left_col = mid_col + current_cell_status; 
                mid_col = right_col;
            }

            //non-edge columns
            for (j = 1; j < ncols - 1; j++){

                const int jeast = j + 1;
                right_col = BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, i, jeast) + 
                    BOARD (inboard, isouth, jeast);
            
                const int current_cell_status = BOARD(inboard, i, j);
				BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];

                // shift columns to left (cell to the right)
                left_col = mid_col;
                mid_col = right_col;
            }

            {
                //last column
                j = ncols - 1;
                const int jeastR = 0;

                right_col = BOARD (inboard, inorth, jeastR) + 
                    BOARD (inboard, i, jeastR) + 
                    BOARD (inboard, isouth, jeastR);
            
                const int current_cell_status = BOARD(inboard, i, j);
				BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];
            }
        }

        pthread_barrier_wait (&sync_universe);
        SWAP_BOARDS( outboard, inboard );

    }
}


void * sus_life_mid(void * arg){

    thread_args* args = (thread_args *) arg;
    const int rows_to_do = args->nrows / NUM_THREADS;

    const int nrows = args->nrows;
    const int ncols = args->ncols;
    char* inboard = args->inboard;
    char* outboard = args->outboard;
    const int gens_max = args->gens_max;

    // index: XXX CCCC S
	// CCCC is count of neighbors
	// S is status of cell
    const char NEXT_GEN_STATUS[18] = {
        0, 0,
        0, 0,
        0, 1,
        1, 1,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0
    };

    const int LDA = args->nrows;
    int curgen, i, j;

    for (curgen = 0; curgen < gens_max; curgen++)
    {

        int left_col = 0, mid_col = 0, right_col = 0;

        // second row to second last row
        for (i = args->id * rows_to_do; i < args->id * rows_to_do + rows_to_do; i++){
            const int inorth = i - 1;
            const int isouth = i + 1;
            {
                // first column
                j = 0;
                const int jwestL = ncols - 1;
                const int jeastL = j + 1;
                left_col = BOARD (inboard, inorth, jwestL) + 
                    BOARD (inboard, i, jwestL) +
                    BOARD (inboard, isouth, jwestL);

                mid_col = BOARD (inboard, inorth, j) + 
                    BOARD (inboard, isouth, j);

                right_col = BOARD (inboard, inorth, jeastL) + 
                    BOARD (inboard, i, jeastL) + 
                    BOARD (inboard, isouth, jeastL);
            
                const int current_cell_status = BOARD(inboard, i, j);
				BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col << 1 | current_cell_status];

                // shift columns to left (cell to the right)
                // making sure to add the current cell into sum
                left_col = mid_col + current_cell_status; 
                mid_col = right_col;
            }

            //non-edge columns
            for (j = 1; j < ncols - 1; j++){

                const int jeast = j + 1;
                right_col = BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, i, jeast) + 
                    BOARD (inboard, isouth, jeast);
            
                const int current_cell_status = BOARD(inboard, i, j);
				BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];

                // shift columns to left (cell to the right)
                left_col = mid_col;
                mid_col = right_col;
            }

            {
                //last column
                j = ncols - 1;
                const int jeastR = 0;

                right_col = BOARD (inboard, inorth, jeastR) + 
                    BOARD (inboard, i, jeastR) + 
                    BOARD (inboard, isouth, jeastR);
            
                const int current_cell_status = BOARD(inboard, i, j);
				BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];
            }
        }

        pthread_barrier_wait (&sync_universe);
        SWAP_BOARDS( outboard, inboard );

    }
}


void * sus_life_last(void * arg){

    thread_args* args = (thread_args *) arg;
    const int rows_to_do = args->nrows / NUM_THREADS;

    const int nrows = args->nrows;
    const int ncols = args->ncols;
    char* inboard = args->inboard;
    char* outboard = args->outboard;
    const int gens_max = args->gens_max;

    // index: XXX CCCC S
	// CCCC is count of neighbors
	// S is status of cell
    const char NEXT_GEN_STATUS[18] = {
        0, 0,
        0, 0,
        0, 1,
        1, 1,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0
    };

    const int LDA = args->nrows;
    int curgen, i, j;

    for (curgen = 0; curgen < gens_max; curgen++)
    {

        int left_col = 0, mid_col = 0, right_col = 0;
        // second row to second last row
        for (i = args->id * rows_to_do; i < nrows - 1; i++){
            const int inorth = i - 1;
            const int isouth = i + 1;
            {
                // first column
                j = 0;
                const int jwestL = ncols - 1;
                const int jeastL = j + 1;
                left_col = BOARD (inboard, inorth, jwestL) + 
                    BOARD (inboard, i, jwestL) +
                    BOARD (inboard, isouth, jwestL);

                mid_col = BOARD (inboard, inorth, j) + 
                    BOARD (inboard, isouth, j);

                right_col = BOARD (inboard, inorth, jeastL) + 
                    BOARD (inboard, i, jeastL) + 
                    BOARD (inboard, isouth, jeastL);
            
                const int current_cell_status = BOARD(inboard, i, j);
				BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col << 1 | current_cell_status];

                // shift columns to left (cell to the right)
                // making sure to add the current cell into sum
                left_col = mid_col + current_cell_status; 
                mid_col = right_col;
            }

            //non-edge columns
            for (j = 1; j < ncols - 1; j++){

                const int jeast = j + 1;
                right_col = BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, i, jeast) + 
                    BOARD (inboard, isouth, jeast);
            
                const int current_cell_status = BOARD(inboard, i, j);
				BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];

                // shift columns to left (cell to the right)
                left_col = mid_col;
                mid_col = right_col;
            }

            {
                //last column
                j = ncols - 1;
                const int jeastR = 0;

                right_col = BOARD (inboard, inorth, jeastR) + 
                    BOARD (inboard, i, jeastR) + 
                    BOARD (inboard, isouth, jeastR);
            
                const int current_cell_status = BOARD(inboard, i, j);
				BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];
            }
        }

        {
            // third corner
            i = nrows - 1; j = 0;
            const int inorth3 = i - 1;
            const int isouth3 = 0;
            const int jwest3 = ncols - 1;
            const int jeast3 = 1;

            left_col = BOARD (inboard, inorth3, jwest3) + 
                BOARD (inboard, i, jwest3) +
                BOARD (inboard, isouth3, jwest3);

            mid_col = BOARD (inboard, inorth3, j) + 
                BOARD (inboard, isouth3, j);

            right_col = BOARD (inboard, inorth3, jeast3) + 
                BOARD (inboard, i, jeast3) + 
                BOARD (inboard, isouth3, jeast3);
            
            const int current_cell_status = BOARD(inboard, i, j);
            BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col << 1 | current_cell_status];

            // shift columns to left (cell to the right)
            // make sure to add the current cell's state
            left_col = mid_col + current_cell_status; 
            mid_col = right_col;
        }

        // last row
        i = nrows - 1;
        for (j = 1; j < ncols - 1; j++){
            const int inorth = i - 1; 
            const int isouth = 0;
            int jeast = j + 1;

             right_col = BOARD (inboard, inorth, jeast) + 
                BOARD (inboard, i, jeast) + 
                BOARD (inboard, isouth, jeast);
            
            const int current_cell_status = BOARD(inboard, i, j);
            BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];

            // shift columns to left (cell to the right)
            left_col = mid_col;
            mid_col = right_col;
        }

        {
            // fourth corner
            i = nrows - 1; j = ncols - 1;
            const int inorth4 = i - 1;
            const int isouth4 = 0;
            const int jeast4 = 0;
            right_col = BOARD (inboard, inorth4, jeast4) + 
                BOARD (inboard, i, jeast4) + 
                BOARD (inboard, isouth4, jeast4);
            
            const int current_cell_status = BOARD(inboard, i, j);
            BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];
        }
        pthread_barrier_wait (&sync_universe);
        SWAP_BOARDS( outboard, inboard );

    }
}


char*
sus_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
	// index: XXX CCCC S
	// CCCC is count of neighbors
	// S is status of cell
    const char NEXT_GEN_STATUS[18] = {
        0, 0,
        0, 0,
        0, 1,
        1, 1,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0
    };

    const int LDA = nrows;
    int curgen, i, j;

    for (curgen = 0; curgen < gens_max; curgen++)
    {

        // sliding window of counting neighbours from previous cell
        int left_col = 0, mid_col = 0, right_col = 0;

        {
            // first corner
            i = 0; j = 0;
            const int inorth1 = nrows - 1;
            const int isouth1 = 1;
            const int jwest1 = ncols - 1;
            const int jeast1 = 1;
            left_col = BOARD (inboard, inorth1, jwest1) + 
                BOARD (inboard, i, jwest1) +
                BOARD (inboard, isouth1, jwest1);

            mid_col = BOARD (inboard, inorth1, j) + 
                BOARD (inboard, isouth1, j);

            right_col = BOARD (inboard, inorth1, jeast1) + 
                BOARD (inboard, i, jeast1) + 
                BOARD (inboard, isouth1, jeast1);

			const int current_cell_status = BOARD(inboard, i, j);
            BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col << 1 | current_cell_status];

            // shift columns to left (cell to the right)
            // make sure to add the current cell's state
            left_col = mid_col + current_cell_status;
            mid_col = right_col;

        }

        // first row without corner
        i = 0;
        for (j = 1; j < ncols - 1; j++){
            const int inorth = nrows - 1;
            const int isouth = 1;
            const int jeast = j + 1;

            right_col = BOARD (inboard, inorth, jeast) + 
                BOARD (inboard, i, jeast) + 
                BOARD (inboard, isouth, jeast);

			const int current_cell_status = BOARD(inboard, i, j);
            BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];

            // shift columns to left (cell to the right)
            left_col = mid_col;
            mid_col = right_col;
        }

        {
            // second corner
            i = 0; j = ncols - 1;
            const int inorth2 = nrows - 1;
            const int isouth2 = 1;
            const int jeast2 = 0;

            right_col = BOARD (inboard, inorth2, jeast2) + 
                BOARD (inboard, i, jeast2) + 
                BOARD (inboard, isouth2, jeast2);
            
            const int current_cell_status = BOARD(inboard, i, j);
            BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];
        }

        // second row to second last row
        for (i = 1; i < nrows - 1; i++){
            const int inorth = i - 1;
            const int isouth = i + 1;
            {
                // first column
                j = 0;
                const int jwestL = ncols - 1;
                const int jeastL = j + 1;
                left_col = BOARD (inboard, inorth, jwestL) + 
                    BOARD (inboard, i, jwestL) +
                    BOARD (inboard, isouth, jwestL);

                mid_col = BOARD (inboard, inorth, j) + 
                    BOARD (inboard, isouth, j);

                right_col = BOARD (inboard, inorth, jeastL) + 
                    BOARD (inboard, i, jeastL) + 
                    BOARD (inboard, isouth, jeastL);
            
                const int current_cell_status = BOARD(inboard, i, j);
				BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col << 1 | current_cell_status];

                // shift columns to left (cell to the right)
                // making sure to add the current cell into sum
                left_col = mid_col + current_cell_status; 
                mid_col = right_col;
            }

            //non-edge columns
            for (j = 1; j < ncols - 1; j++){

                const int jeast = j + 1;
                right_col = BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, i, jeast) + 
                    BOARD (inboard, isouth, jeast);
            
                const int current_cell_status = BOARD(inboard, i, j);
				BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];

                // shift columns to left (cell to the right)
                left_col = mid_col;
                mid_col = right_col;
            }

            {
                //last column
                j = ncols - 1;
                const int jeastR = 0;

                right_col = BOARD (inboard, inorth, jeastR) + 
                    BOARD (inboard, i, jeastR) + 
                    BOARD (inboard, isouth, jeastR);
            
                const int current_cell_status = BOARD(inboard, i, j);
				BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];
            }
        }

        {
            // third corner
            i = nrows - 1; j = 0;
            const int inorth3 = i - 1;
            const int isouth3 = 0;
            const int jwest3 = ncols - 1;
            const int jeast3 = 1;

            left_col = BOARD (inboard, inorth3, jwest3) + 
                BOARD (inboard, i, jwest3) +
                BOARD (inboard, isouth3, jwest3);

            mid_col = BOARD (inboard, inorth3, j) + 
                BOARD (inboard, isouth3, j);

            right_col = BOARD (inboard, inorth3, jeast3) + 
                BOARD (inboard, i, jeast3) + 
                BOARD (inboard, isouth3, jeast3);
            
            const int current_cell_status = BOARD(inboard, i, j);
            BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col << 1 | current_cell_status];

            // shift columns to left (cell to the right)
            // make sure to add the current cell's state
            left_col = mid_col + current_cell_status; 
            mid_col = right_col;
        }

        // last row
        i = nrows - 1;
        for (j = 1; j < ncols - 1; j++){
            const int inorth = i - 1; 
            const int isouth = 0;
            int jeast = j + 1;

             right_col = BOARD (inboard, inorth, jeast) + 
                BOARD (inboard, i, jeast) + 
                BOARD (inboard, isouth, jeast);
            
            const int current_cell_status = BOARD(inboard, i, j);
            BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];

            // shift columns to left (cell to the right)
            left_col = mid_col;
            mid_col = right_col;
        }

        {
            // fourth corner
            i = nrows - 1; j = ncols - 1;
            const int inorth4 = i - 1;
            const int isouth4 = 0;
            const int jeast4 = 0;
            right_col = BOARD (inboard, inorth4, jeast4) + 
                BOARD (inboard, i, jeast4) + 
                BOARD (inboard, isouth4, jeast4);
            
            const int current_cell_status = BOARD(inboard, i, j);
            BOARD(outboard, i, j) = NEXT_GEN_STATUS[left_col + mid_col + right_col - current_cell_status << 1 | current_cell_status];
        }

        SWAP_BOARDS( outboard, inboard );

    }
    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
    return inboard;
}