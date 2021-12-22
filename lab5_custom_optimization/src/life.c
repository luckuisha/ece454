/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include "life.h"

/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/

void birth_cell(uint_fast8_t* cell, const int row, const int col, const int rows, const int cols){
	uint_fast8_t* cell_ptr = cell;
	int xoleft, xoright, yoabove, yobelow;
   unsigned char *cell_ptr = cells + (y * w) + x;

   // Calculate the offsets to the eight neighboring cells,
   // accounting for wrapping around at the edges of the cell map
   if (x == 0)
      xoleft = w - 1;
   else
      xoleft = -1;
   if (y == 0)
      yoabove = length_in_bytes - w;
   else
      yoabove = -w;
   if (x == (w - 1))
      xoright = -(w - 1);
   else
      xoright = 1;
   if (y == (h - 1))
      yobelow = -(length_in_bytes - w);
   else
      yobelow = w;

   *(cell_ptr) |= 0x01;
   *(cell_ptr + yoabove + xoleft) += 2;
   *(cell_ptr + yoabove) += 2;
   *(cell_ptr + yoabove + xoright) += 2;
   *(cell_ptr + xoleft) += 2;
   *(cell_ptr + xoright) += 2;
   *(cell_ptr + yobelow + xoleft) += 2;
   *(cell_ptr + yobelow) += 2;
   *(cell_ptr + yobelow + xoright) += 2;

	int leftSum, rightSum, topSum, bottomSum = 1;
	// set current cell as alive
	*(cell_ptr) |= 0x01;

	// check if there exists padding and change the amount to add
	// must add 2 if neighbor cell is not padding
	// left
	if (*(cell_ptr - 1) == seventh){
		leftSum = 0;
	}
	// right
	if (*(cell_ptr + 1) == seventh){
		rightSum = 0;
	}
	// top
	if (*(cell_ptr + 1) == seventh){
		topSum = 0;
	}
	// down
	if (*(cell_ptr + 1) == seventh){
		bottomSum = 0;
	}
	// top left
	*(cell_ptr - cols - 1) += (leftSum + topSum) * leftSum * topSum;
	// top
   	*(cell_ptr - cols) += (topSum + topSum) * topSum;
	// top right
  	*(cell_ptr - cols + 1) += (rightSum + topSum) * rightSum * topSum;
   	*(cell_ptr - 1) += (leftSum + leftSum) * leftSum;
   	*(cell_ptr + 1) += (rightSum + rightSum) * rightSum;
   	*(cell_ptr + cols - 1) += (leftSum + bottomSum) * leftSum * bottomSum;
   	*(cell_ptr + cols) += (bottomSum + bottomSum) * bottomSum;
   	*(cell_ptr + cols + 1) += (rightSum + bottomSum) * rightSum * bottomSum;
}

void kill_cell(uint_fast8_t* cell, const int row, const int col, const int rows, const int cols){
	uint_fast8_t* cell_ptr = cell;
	uint_fast8_t seventh = (1 << 7);
	int leftSum, rightSum, topSum, bottomSum = 1;
	// set current cell as alive
	*(cell_ptr) &= ~0x01;

	// check if there exists padding and change the amount to add
	// must add 2 if neighbor cell is not padding
	// left
	if (*(cell_ptr - 1) == seventh){
		leftSum = 0;
	}
	// right
	if (*(cell_ptr + 1) == seventh){
		rightSum = 0;
	}
	// top
	if (*(cell_ptr + 1) == seventh){
		topSum = 0;
	}
	// down
	if (*(cell_ptr + 1) == seventh){
		bottomSum = 0;
	}
	// top left
	*(cell_ptr - cols - 1) += (leftSum + topSum) * leftSum * topSum;
	// top
   	*(cell_ptr - cols) += (topSum + topSum) * topSum;
	// top right
  	*(cell_ptr - cols + 1) -= (rightSum + topSum) * rightSum * topSum;
   	*(cell_ptr - 1) -= (leftSum + leftSum) * leftSum;
   	*(cell_ptr + 1) -= (rightSum + rightSum) * rightSum;
   	*(cell_ptr + cols - 1) -= (leftSum + bottomSum) * leftSum * bottomSum;
   	*(cell_ptr + cols) -= (bottomSum + bottomSum) * bottomSum;
   	*(cell_ptr + cols + 1) -= (rightSum + bottomSum) * rightSum * bottomSum;
}

static uint_fast8_t*
create_universe (FILE* input, const int rows, const int cols)
{
	uint_fast8_t* board = calloc (rows * cols, sizeof (uint_fast8_t));
  	assert (board != NULL);
  	int ngotten = 0, index, row, col;

  	/* Fill in the board with values from the input file */
  	for (row = 0; row < rows; row++){
		for (col = 0; col < cols; col++){
			index = row + col;
			ngotten = fscanf (input, "%hhu\n", &board[index]);
			if (board[index] == 1){
				birth_cell(&board[index], row, col, rows, cols);
			}
			if (ngotten < 1){
				fprintf (stderr, "* Ran out of input at item %d ***\n", index);
				fclose (input);
				exit (EXIT_FAILURE);
			}
		}
    }
  return board;
}

static void
load_dimensions (FILE* input, int* rows, int* cols)
{
  int ngotten = 0;
  
  ngotten = fscanf (input, "P1\n%d %d\n", rows, cols);
  if (ngotten < 1)
    {
      fprintf (stderr, "* Failed to read 'P1' and board dimensions ***\n");
      fclose (input);
      exit (EXIT_FAILURE);
    }
  if (*rows < 1)
    {
      fprintf (stderr, "* Number of rows %d must be positive! ***\n", *rows);
      fclose (input);
      exit (EXIT_FAILURE);
    }
  if (*cols < 1)
    {
      fprintf (stderr, "* Number of cols %d must be positive! ***\n", *cols);
      fclose (input);
      exit (EXIT_FAILURE);
    }
}

uint_fast8_t*
initialize_universe (FILE* input, int* rows, int* cols)
{
  load_dimensions (input, rows, cols);
  return create_universe (input, *rows, *cols);
}

/*****************************************************************************
 * Game of life implementation
 ****************************************************************************/
char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max)
{
  return sequential_game_of_life (outboard, inboard, nrows, ncols, gens_max);
}



uint_fast8_t*
sus_life(uint_fast8_t* outboard, 
		uint_fast8_t* inboard, 
		const int rows, 
		const int cols, 
		const int gens_max) {

	uint_fast8_t* cell;
	unsigned int generation = 0, row = 0, col = 0, count;
	cell = inboard;
	for (; generation < gens_max; generation++) {
		
		//edge case top
		for (col = 0; col < cols; col++){
			// repeat for each row of cells
			// Process all cells in the current row of the cell map
			do {
				// repeat for each cell in row
				// Zip quickly through as many off-cells with no
				// neighbors as possible
				while (*cell == 0) {
					cell++; // advance to the next cell
					if (++col >= cols) goto RowDone1;
				}
				// Found a cell that's either on or has on-neighbors,
				// so see if its state needs to be changed
				count = (*cell >> 1) & 0x0F; // # of neighboring on-cells
				if (*cell & 0x01) {
					// Cell is on; turn it off if it doesn't have
					// 2 or 3 neighbors
					if ((count != 2) && (count != 3)) {
						kill_cell(cell, row, col, rows, cols);
					}
				} else {
					// Cell is off; turn it on if it has exactly 3 neighbors
					if (count == 3) {
						birth_cell(cell, row, col, rows, cols);
					}
				}
				// Advance to the next cell
				cell++; // advance to the next cell byte
			} while (++col < cols);
			RowDone1:;
		}
		


		for (row++; row < rows; row++) {
				// repeat for each row of cells
				// Process all cells in the current row of the cell map
				col = 0;
				do {
					// repeat for each cell in row
					// Zip quickly through as many off-cells with no
					// neighbors as possible
					while (*cell == 0) {
						cell++; // advance to the next cell
						if (++col >= cols) goto RowDone2;
					}
					// Found a cell that's either on or has on-neighbors,
					// so see if its state needs to be changed
					count = (*cell >> 1) & 0x0F; // # of neighboring on-cells
					if (*cell & 0x01) {
						// Cell is on; turn it off if it doesn't have
						// 2 or 3 neighbors
						if ((count != 2) && (count != 3)) {
							kill_cell(cell, row, col, rows, cols);
						}
					} else {
						// Cell is off; turn it on if it has exactly 3 neighbors
						if (count == 3) {
							birth_cell(cell, row, col, rows, cols);
						}
					}
					// Advance to the next cell
					cell++; // advance to the next cell byte
				} while (++col < cols);
				RowDone2:;
		}


		for (row++; row < rows; row++) {
				// repeat for each row of cells
				// Process all cells in the current row of the cell map
				col = 0;
				do {
					// repeat for each cell in row
					// Zip quickly through as many off-cells with no
					// neighbors as possible
					while (*cell == 0) {
						cell++; // advance to the next cell
						if (++col >= cols) goto RowDone3;
					}
					// Found a cell that's either on or has on-neighbors,
					// so see if its state needs to be changed
					count = (*cell >> 1) & 0x0F; // # of neighboring on-cells
					if (*cell & 0x01) {
						// Cell is on; turn it off if it doesn't have
						// 2 or 3 neighbors
						if ((count != 2) && (count != 3)) {
							kill_cell(cell, row, col, rows, cols);
						}
					} else {
						// Cell is off; turn it on if it has exactly 3 neighbors
						if (count == 3) {
							birth_cell(cell, row, col, rows, cols);
						}
					}
					// Advance to the next cell
					cell++; // advance to the next cell byte
				} while (++col < cols);
				RowDone3:;
		}


		SWAP_PTRS(outboard, inboard);
	}	
	return inboard;
}
