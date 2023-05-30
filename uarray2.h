/**************************************************************
 *
 *                     uarray2.h
 *
 *     Assignment: locality
 *     Authors:  - Joao Quintanilha (jquint04)
 *               - Kelly Tsidji (atsidj01) 
 *     Date:     09/25/2022
 *     
 *     uarray2.h is the interface for the UArray2_T
 *     specifying functions to make a new 2D-Array built 
 *     with an U_Array_T of U_Array_T's
 *
 **************************************************************/

#ifndef UARRAY2_H_INCLUDED
#define URRAY2_H_INCLUDED

#include <stdio.h>
#include "uarray.h"

typedef struct UArray2_T *UArray2_T;

/* struct of a 2D-Array built through Hanson's UArray_T */
struct UArray2_T;


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
extern UArray2_T UArray2_new(int col, int row, int element_size);



/*     function UArray2_width
 *     Purpose: return the width of the 2D-array
 *     Parameters:
 *                - UArray2_T struct instance
 *     Returns: width of the 2D array
 *     Expectations: 
 *                 - UArray2_T instance cannot be NULL
 */
extern int UArray2_width(UArray2_T array);



/*     function UArray2_height
 *     Purpose: return the height of the 2D-array
 *     Parameters:
 *                - UArray2_T struct object
 *     Returns: height of the array
 *     Expectations: 
 *                 - UArray2_T instance cannot be NULL
 */
extern int UArray2_height(UArray2_T array);



/*     function UArray2_size
 *     Purpose: return the size of the type used in the 2D-array elements
 *     Parameters:
 *                - UArray2_T struct object
 *     Returns: size of type used in the 2D-array
 *     Expectations: 
 *                 - UArray2_T instance cannot be NULL
 */
extern int UArray2_size(UArray2_T array);



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
extern void *UArray2_at(UArray2_T array, int col, int row);


 
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
extern void UArray2_map_col_major(UArray2_T array, 
                                  void apply(int col, int row, UArray2_T a, 
                                             void *p1, void *p2), 
                                  void *cl);



/*    function UArray2_map_row_major
 *     Purpose: iterate through the 2D array in row-major order
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
 *                       is empty, do nothing
 *                 - apply function parameter cannot be NULL
 *                 - if the pointer to the closure marker is NULL, then it is
 *                   not used by the function
 */
extern void UArray2_map_row_major(UArray2_T array, 
                                  void apply(int col, int row, UArray2_T a, 
                                             void *p1, void *p2), 
                                  void *cl);



/*    function UArray2_map_free
 *     Purpose: deallocate the 2D-array itself and the UArray_T structure
 *              and clear its arguments
 *     Parameters:
 *                - pointer to a UArray2_T struct object
 *     Returns: nothing 
 *     Expectations: 
 *                 - UArray2_T instance pointer cannot be NULL
 */
extern void UArray2_free(UArray2_T *array);

#endif