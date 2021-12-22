#ifndef _life_h
#define _life_h
#define _GNU_SOURCE

#include <pthread.h>
#include <sched.h>
#include <stdio.h>

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

/**
 * MULTITHREADED IMPLEMENTATION
 */

/**
 * if you see sus_life, 
 * the function has different conditions for each cell
 * depending on the row and col. 
 * this is only for the first thread
 */
void * sus_life_first(void * arg);

/**
 * if you see sus_life, 
 * the function has different conditions for each cell
 * depending on the row and col. 
 * this is for threads that are not the first or last thread
 */
void * sus_life_mid(void * arg);

/**
 * if you see sus_life, 
 * the function has different conditions for each cell
 * depending on the row and col. 
 * this is only for the last thread
 */
void * sus_life_last(void * arg);

/**
 * optimizes the sequential implementation
 * by using a sliding window for counting neighbors, 
 * only checking boundary conditions when required, 
 * and a lookup table
 */
char*
sus_life (char* outboard, 
			 char* inboard,
			 const int nrows,
			 const int ncols,
			 const int gens_max);

#endif /* _life_h */
