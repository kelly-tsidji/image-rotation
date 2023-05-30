/*****************************************************************************
 *
 *                               uarray2b.c
 *
 *     Assignment: locality
 *     Authors:  - Joao Quintanilha (jquint04)
 *               - Kelly Tsidji (atsidj01) 
 *     Date:     10/09/2022
 *     
 *     uarray2b.c is the implementation for the UArray2b_T specifying 
 *     functions to make a new blocked 2D array. A UArray2b_T struct is 
 *     represented by a Uarray2 whose elements are each UArrays. The UArray2
 *     essentially holds blocks (the elements of each block are stored in a 
 *     UArray). To find the cell at index (i, j), first find the block at 
 *     index (i / blocksize, j / blocksize). Within that block, use the cell
 *     at index blocksize * (i % blocksize) + (j % blocksize)
 *
 ****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <except.h>
#include <math.h>

/* Hanson's Data Structures */
#include "uarray.h"
#include "uarray2.h"
#include "assert.h"
#include "mem.h"
#include "uarray2b.h"

Except_T Out_Of_Ranges = 
{"Width, row, and/or size provided is/are out of range"};

const int max_ram_size = 64 * 1024;  /* the number of bytes in 64KB of RAM */

struct UArray2b_T {
        int width;      /* number of columns in the 2D array */
        int height;     /* number of rows in the 2D array */
        int size;       /* number of bytes in one element */
        int blocksize;  /* number of cells on one side of a block */
        int hor_num_block; /* number of blocks horizontally */
        int ver_num_block; /* number of blocks vertically */
        UArray2_T big_array;
};



/*     function UArray2b_new
 *     Purpose: create a new blocked 2d array, allocating space for it on the
 *              heap
 *     Parameters:
 *                - integer width of the 2D array 
 *                - integer height of the 2D array
 *                - integer number of bytes in one element (size of the type)
 *                - integer blocksize, the number of elements on one side of
 *                  a block
 *     Returns: a pointer to a UArray2b_T struct representing the 2-D array
 *              and its specifications
 *     Expectations: 
 *                 - width should be greater than or equal to 0
 *                 - height should be greater than or equal to 0
 *                 - number of bytes should be greater than 0
 *                 - returned 2D array created will be empty if either width
 *                   and/or height equals 0, but the given parameters will be
 *                   stored in the struct of the 2D array
 *                 - blocksize should be greater than or equal to 1
 *                 - blocksize = square root of # of cells in block
 */
UArray2b_T UArray2b_new(int width, int height, int size, int blocksize)
{
        assert(blocksize >= 1);

        if ((width < 0) || (height < 0) || (size <= 0)) {
                RAISE(Out_Of_Ranges);
        }

        /* calculate the number of blocks needed to cover the table */
        int horizontal_num_block = ceil((double)width / blocksize);
        int vertical_num_block = ceil((double)height / blocksize);
        int cell_per_block = blocksize * blocksize;
        
        /* create a UArray2b_T struct */
        UArray2b_T array2d = malloc(sizeof(*array2d));
        assert (array2d != NULL);

        /* set the 2d array's width, height, size, and blocksize */
        array2d->width = width;
        array2d->height = height;
        array2d->size = size;
        array2d->blocksize = blocksize;
        array2d->hor_num_block = horizontal_num_block;
        array2d->ver_num_block = vertical_num_block;

        /* create the Uarray2_T's whose elements each hold a Uarrray_T */
        array2d->big_array = UArray2_new(horizontal_num_block, 
        vertical_num_block, sizeof(UArray_T));

        /* populate the 2d array (Uarray2_T) with arrays (Uarray_T) */
        for (int row = 0; row < vertical_num_block; row++) {
                for (int col = 0; col < horizontal_num_block; col++) {
                        UArray_T block = UArray_new(cell_per_block, size);
                        *(UArray_T *)UArray2_at(array2d->big_array, col, row) =
                        block;
                }
        }

        
        return array2d;
}


/*     function UArray2b_new_64K_block
 *     Purpose: allocate space for the 2Darray itself with a block occupying 
 *              at most 64KB
 *     Parameters:
 *                - integer width of the 2D array 
 *                - integer height of the 2D array
 *                - integer number of bytes in one element (size of the type)
 *     Returns: a UArray2b_T struct representing the 2-D array
 *              and its specifications
 *     Note: - once it figures out the blocksize, it delegates the creation 
 *           of the 2b array to Uarray2b_new function
 *           - it uses the global constant max_ram_size
 *     Expectations: 
 *                 - width should be greater than or equal to 0
 *                 - height should be greater than or equal to 0
 *                 - number of bytes should be greater than 0
 *                 - returned 2D array created will be empty if either width
 *                   and/or height equals 0, but the given parameters will be
 *                   stored in the struct of the 2D array
 */
UArray2b_T UArray2b_new_64K_block(int width, int height, int size)
{
        if ((width < 0) || (height < 0) || (size <= 0)) {
          RAISE(Out_Of_Ranges);
        }

        int blocksize = sqrt(max_ram_size/size); 

        /* if a single cell will not fit in 64KB, the block size should be 1 */
        if (size > max_ram_size) {
                blocksize = 1;
        }

        return UArray2b_new(width, height, size, blocksize);
}


/*    function UArray2b_free
 *     Purpose: deallocate the 2D-array itself, the UArray2_T structure
 *              and the UArray_T (block) structure
 *     Parameters:
 *                - pointer to a UArray2b_T struct object
 *     Returns: nothing 
 *     Expectations: 
 *                 - UArray2b_T instance pointer cannot be NULL
 */
void UArray2b_free (UArray2b_T *array2b)
{
        assert(array2b && *array2b);

        for (int row_block = 0;
             row_block < UArray2_height((*array2b)->big_array); row_block++) {
                for (int col_block = 0;
                     col_block < UArray2_width((*array2b)->big_array); 
                     col_block++) {

                        UArray_T *block = UArray2_at((*array2b)->big_array, 
                        col_block, row_block);

                        UArray_free(block);
                }
        }

        UArray2_free(&((*array2b)->big_array));
        FREE(*array2b);
}


/*     function UArray2b_width
 *     Purpose: get the width of the 2D-array
 *     Parameters:
 *                - UArray2b_T struct instance
 *     Returns: width of the 2D array
 *     Expectations: 
 *                 - UArray2b_T instance cannot be NULL
 */
int UArray2b_width (UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->width;
}


/*     function UArray2b_height
 *     Purpose: get the height of the 2D-array
 *     Parameters:
 *                - UArray2b_T struct instance
 *     Returns: height of the 2D array
 *     Expectations: 
 *                 - UArray2b_T instance cannot be NULL
 */
int UArray2b_height (UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->height;

}


/*     function UArray2_size
 *     Purpose: get the number of bytes in one element of the 
 *              2d array (size of the type)
 *     Parameters:
 *                - UArray2b_T struct object
 *     Returns: the number of bytes in one element of the 
 *              2d array (size of the type)
 *     Expectations: 
 *                 - UArray2b_T instance cannot be NULL
 */
int UArray2b_size (UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->size;
}


/*     function UArray2b_blocksize
 *     Purpose: get the blocksize of the 2D-array
 *     Parameters:
 *                - UArray2b_T struct instance
 *     Returns: blocksize of the 2D array
 *     Expectations: 
 *                 - UArray2b_T instance cannot be NULL
 */
int UArray2b_blocksize(UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->blocksize;
}


/*     function UArray2b_at
 *     Purpose: get the element of the 2D table at the specified 
 *              column and row [col][row]
 *     Parameters:
 *                - UArray2b_T struct object
 *                - int column index
 *                - int row index
 *     Returns: a pointer to the cell at the specified column and row
 *     Expectations: 
 *                 - UArray2b_T instance cannot be NULL
 *                 - column index must be greater than or equal to zero
 *                 - column index must be less than the width
 *                 - row index must be greater than or equal to zero
 *                 - row index must be less than the height
 */
void *UArray2b_at(UArray2b_T array2b, int column, int row)
{
        assert(array2b != NULL);
        assert(column >= 0 && column < array2b->width);
        assert(row >= 0 && row < array2b->height);

        UArray_T *block = UArray2_at(array2b->big_array, 
        column / array2b->blocksize, row / array2b->blocksize);

        return UArray_at(*block, array2b->blocksize * 
        (column %  array2b->blocksize) + (row % array2b->blocksize));
}


/*     function UArray2b_map
 *     Purpose: visits every cell in the 2D table, block-by-block
 *     Parameters:
 *                - UArray2b_T struct object
 *                - void apply function
 *                    - int col index
 *                    - int row index
 *                    - UArray2b_T struct object
 *                    - pointer to the current element at
 *                      the current col and row indices, elem
 *                    - pointer to a closure marker, cl
 *                - pointer to a closure marker, cl
 *     Returns: nothing
 *     Expectations: 
 *                 - UArray2b_T instance cannot be NULL
 *                 - apply function parameter cannot be NULL
 *                 - if the pointer to the closure marker is NULL, then it is
 *                   not used by the function
 *                 - apply function should not be applied to unused cells that
 *                   may be located at the bottom and right edges of a block
 */
void UArray2b_map(UArray2b_T array2b,
void apply(int col, int row, UArray2b_T array2b,
void *elem, void *cl),
void *cl)
{
        assert(array2b != NULL);
        assert(apply != NULL);

        /* number of blocks vertically */
        int block_height = array2b->ver_num_block;

        /*number of blocks horizontally*/
        int block_width = array2b->hor_num_block;
        
        int block_size = array2b->blocksize; /*number of cells in one block*/
        int b_length = block_size * block_size; /* length of block */
        void *curr_block = NULL; /* pointer to hold current block */


        for (int block_row = 0; block_row < block_height; block_row++) {
                for (int block_col = 0; block_col < block_width;
                     block_col++) {
                        
                        /* keep track of the current block element */
                        curr_block = UArray2_at(array2b->big_array, 
                                                block_col, block_row);

                        /* iterate through UArray in the stored block */
                        for (int index = 0; index < b_length; index++) {
                                /* calculate cell columns and rows */
                                int cell_col = (index / block_size) + 
                                              (block_col * block_size);

                                int cell_row = (index % block_size) +
                                               (block_row * block_size);

                                /* only elements on range get apply function */
                                if (cell_col < array2b->width && cell_col >= 0
                                    && cell_row < array2b->height 
                                    && cell_row >= 0) {
                                        apply(cell_col, cell_row, array2b,
                                        UArray_at(*((UArray_T *)curr_block), 
                                                  index),cl);
                                }
                        }
                }

        }
}