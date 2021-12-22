#ifndef _life_h
#define _life_h
#include <stdint.h>
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_PTRS( b1, b2 )  do { \
  uint_fast8_t* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)


/**
 * set cell will set the cell as alive
 * and increment its neighboring neighbour count
 *  
 */
void birth_cell(uint_fast8_t* cell, const int row, const int col, const int rows, const int cols);

/**
 * kill cell (change cell state to dead)
 * decrement its neighboring neighbor count
 * 
 */
void kill_cell(uint_fast8_t* cell, const int row, const int col, const int rows, const int cols);

uint_fast8_t*
initialize_universe(FILE* input, int* rows, int* cols);

/**
 * Given the initial board state in inboard and the board dimensions
 * nrows by ncols, evolve the board state gens_max times by alternating
 * ("ping-ponging") between outboard and inboard.  Return a pointer to 
 * the final board; that pointer will be equal either to outboard or to
 * inboard (but you don't know which).
 */
char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max);

/**
 * Same output as game_of_life() above, except this is not
 * parallelized.  Useful for checking output.
 */
char*
sequential_game_of_life (char* outboard, 
			 char* inboard,
			 const int nrows,
			 const int ncols,
			 const int gens_max);

uint_fast8_t*
sus_life(uint_fast8_t* outboard, 
		uint_fast8_t* inboard, 
		const int nrows, 
		const int ncols, 
		const int gens_max);
#endif /* _life_h */
