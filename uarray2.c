/*****************************************************************************
 *
 *                               uarray2.c 
 *
 *     Assignment: locality
 *     Authors:  - Joao Quintanilha (jquint04)
 *               - Kelly Tsidji (atsidj01) 
 *     Date:     09/25/2022
 *     
 *     uarray2.c is the implementation for the UArray2_T specifying 
 *     functions to make a new 2D-Array built with an U_Array_T of 
 *     U_Array_T's. The inner UArrays represent the rows.
 ****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <except.h>

/* Hanson's Data Structures */
#include "uarray.h"
#include "assert.h"
#include "mem.h"
#include "uarray2.h"

Except_T Out_Of_Range = {"One or more of the parameters is out of range"};


/* struct of a 2D-Array built through Hanson's UArray_T */
struct UArray2_T {
        int width;      /* number of columns in the 2D array */
        int height;     /* number of rows in the 2D array */
        int size;       /* number of bytes in one element */
        UArray_T big_array;
};


/*     function UArray2_new
 *     Purpose: allocate space for the 2Darray itself
 *     Parameters:
 *                - integer width of the 2D array 
 *                - integer height of the 2D array
 *                - integer number of bytes in one element (size of the type)
 *     Returns: a pointer to a UArray2_T struct representing the 2-D array
 *              and its specifications
 *     Expectations: 
 *                 - width should be greater than or equal to 0
 *                 - height should be greater than or equal to 0
 *                 - number of bytes should be greater than 0
 *                 - returned 2D array created will be empty if either width
 *                   and/or height equals 0, but the given parameters will be
 *                   stored in the struct of the 2D array
 */
UArray2_T UArray2_new(int col, int row, int element_size)
{

        if ((col < 0) || (row < 0) || (element_size <= 0)) {
                RAISE(Out_Of_Range);
        }
        
        UArray2_T array2d = malloc(sizeof(*array2d));
        assert (array2d != NULL);
        
        array2d->width = col;
        array2d->height = row;
        array2d->size = element_size;

        if ((col == 0) || (row == 0)) {
                array2d->big_array = UArray_new(0, element_size);
                return array2d;
        }

        array2d->big_array = UArray_new(row, sizeof(UArray_T));
        
        for (int i = 0; i < row; i++)  {
                UArray_T small_array = UArray_new(col, element_size);
                *(UArray_T *)UArray_at(array2d->big_array, i) = small_array;

        }

        return array2d;
}



/*     function UArray2_width
 *     Purpose: return the width of the 2D-array
 *     Parameters:
 *                - UArray2_T struct instance
 *     Returns: width of the 2D array
 *     Expectations: 
 *                 - UArray2_T instance cannot be NULL
 */
int UArray2_width(UArray2_T array)
{
        assert(array != NULL);
        return array->width;
}



/*     function UArray2_height
 *     Purpose: return the height of the 2D-array
 *     Parameters:
 *                - UArray2_T struct object
 *     Returns: height of the array
 *     Expectations: 
 *                 - UArray2_T instance cannot be NULL
 */
int UArray2_height(UArray2_T array)
{
        assert(array != NULL);
        return array->height;
}



/*     function UArray2_size
 *     Purpose: return the size of the type used in the 2D-array elements
 *     Parameters:
 *                - UArray2_T struct object
 *     Returns: size of type used in the 2D-array
 *     Expectations: 
 *                 - UArray2_T instance cannot be NULL
 */
int UArray2_size(UArray2_T array)
{
        assert(array != NULL);
        return array->size;
}


/*     function UArray2_at
 *     Purpose: get the element at the specified column and row [col][row]
 *     Parameters:
 *                - UArray2_T struct object
 *                - int col index
 *                - int row index
 *     Returns: a pointer to the element at the specified column and row
 *     Expectations: 
 *                 - UArray2_T instance cannot be NULL
 *                     - UArray (representing the 2d array) cannot be empty
 *                 - col index must be greater than or equal to zero
 *                 - col index must be less than the width
 *                 - row index must be greater than or equal to zero
 *                 - row index must be less than the height
 */
void *UArray2_at(UArray2_T array, int col, int row)
{

        assert(array != NULL);
        assert(UArray_length(array->big_array) != 0);
        assert(col >= 0 && col < array->width);
        assert(row >= 0 && row < array->height);

        UArray_T *temp = UArray_at(array->big_array, row);

        return UArray_at(*temp, col);
}


/*     function UArray2_map_col_major
 *     Purpose: iterate through the 2D array in column-major order
 *              performing the specified function
 *     Parameters:
 *                - UArray2_T struct object
 *                - void apply function
 *                    - int col index
 *                    - int row index
 *                    - UArray2_T struct object
 *                    - pointer to the current element at
 *                      the current col and row indices, p1
 *                    - pointer to a closure marker, p2
 *                - pointer to a closure marker
 *     Returns: nothing
 *     Expectations: 
 *                 - UArray2_T instance cannot be NULL
 *                     - if the UArray (representing the 2d array) 
 *                      is empty, do nothing
 *                 - apply function parameter cannot be NULL
 *                 - if the pointer to the closure marker is NULL, then it is
 *                   not used by the function
 */
void UArray2_map_col_major(UArray2_T array, 
                           void apply(int col, int row, UArray2_T a, 
                                      void *p1, void *p2), 
                           void *cl)
{
        assert(array != NULL);
        assert(apply != NULL);
        
        if (array->width != 0 && array->height != 0) {

                for (int i = 0; i < array->width; i++) { 
                        /* i represents cols */
                        for (int j = 0; j < array->height; j++) { 
                                /* j represents rows */
                                apply(i, j, array, 
                                UArray2_at(array, i, j), cl);
                        }
                }
        }

}


/*    function UArray2_map_row_major
 *     Purpose: iterate through the 2D array in row-major order
 *              performing the specified function
 *     Parameters:
 *                - UArray2_T struct object
 *                - void apply function
 *                    - int col index
 *                    - freeint row index
 *                    - UArray2_T struct object
 *                    - pointer to the current element at
 *                      the current col and row indices, p1
 *                    - pointer to a closure marker, p2
 *                - pointer to a closure marker
 *     Returns: nothing
 *     Expectations: 
 *                 - UArray2_T instance cannot be NULL
 *                     - if the UArray (representing the 2d array) 
 *                       is empty, do nothing
 *                 - apply function parameter cannot be NULL
 *                 - if the pointer to the closure marker is NULL, then it is
 *                   not used by the function
 */
void UArray2_map_row_major(UArray2_T array, 
                           void apply(int col, int row, UArray2_T a, 
                                      void *p1, void *p2), 
                           void *cl)
{
        assert(array != NULL);
        assert(apply != NULL);
        
        if (array->width != 0 && array->height != 0) {

                for (int i = 0; i < array->height; i++) { 
                        /* i represents rows */
                        for (int j = 0; j < array->width; j++) { 
                                /* j represents cols */
                                apply(j, i, array, 
                                UArray2_at(array, j, i), cl);
                        }
                }

        }
}


/*    function UArray2_free
 *     Purpose: deallocate the 2D-array itself and the UArray_T structure
 *              and clear its arguments
 *     Parameters:
 *                - pointer to a UArray2_T struct object
 *     Returns: nothing 
 *     Expectations: 
 *                 - UArray2_T instance pointer cannot be NULL
 */
void UArray2_free(UArray2_T *array)
{
        assert(array && *array);

        /* freeing all the arrays inside the the big array and then later */
        for (int i = 0; i < (*array)->height; i++) { /* i represents rows */
                UArray_T *small_array = UArray_at((*array)->big_array, i);
                UArray_free(small_array);
        }
        UArray_free(&((*array)->big_array));
        FREE(*array);
}
