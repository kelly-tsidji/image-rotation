/*
 *                      testarrayb.c
 *
 *         This program illustrates the use of the uarray2b interface.
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <uarray2b.h>

typedef int number;

const int DIM1 = 200;
const int DIM2 = 200;
const int ELEMENT_SIZE = sizeof(number);
const int MARKER = 99;

void
check_and_print(int i, int j, UArray2b_T a, void *p1, void *p2) 
{
        number *entry_p = p1;

        *((bool *)p2) &= UArray2b_at(a, i, j) == entry_p;

        if ( (i == (DIM1 - 1) ) && (j == (DIM2 - 1) ) ) {
                /* we got the corner */
                *((bool *)p2) &= (*entry_p == MARKER);
        }

        printf("ar[%d,%d]\n", i, j);
}


int main(int argc, char *argv[])
{
        (void)argc;
        (void)argv;

        UArray2b_T test_array;
        bool OK = true;
        // (void)OK;

        // test_array = UArray2b_new(DIM1, DIM2, ELEMENT_SIZE, 3);
        test_array = UArray2b_new_64K_block(DIM1, DIM2, ELEMENT_SIZE);

        // printf("TESTTTTTTTT");
        // (void)test_array;

        /* test for given array */
        printf("\nwidth of array: %d \nheight of array: %d\n element size: %d\n", 
        UArray2b_width(test_array),  UArray2b_height(test_array),  UArray2b_size(test_array));

        printf("size of block: %d\n", UArray2b_blocksize(test_array));
        
        /* for the tests above we simply changed outputs to check for different 
        behaviors in the function, either with assertions/RAISES or just returning
        a fine 2-D array
        - we still do not have the means to check if the array was properly
        initialized, and that is why the next big function to focus will be At
        */

      /* Before moving to At, we checked for width, height and size functions */

        OK = (UArray2b_width(test_array) == DIM1) &&
             (UArray2b_height(test_array) == DIM2) &&
             (UArray2b_size(test_array) == ELEMENT_SIZE);

        if (OK) {
          printf("All the parameters were properly assigned\n");
        }

      /* Tests for checking individual assertions on the the functions */
        // UArray2_width(test_array);
        // UArray2_height(test_array);
        // UArray2_size(test_array);

      /* Here we tested for a case where we are using an empty struct of 2D-array 
        - After that, we tested for 2-D arrays that exist but that are empty,
        checking them for other assertions included in the Uarray2_at
        - After checking for exception inputs on this function, we ran the real
        tests by using the given test code. We accessed the element in the heap
        and changed the value there to a pre-established value
            - we ran unto some issues with deferrencing it, but Alec Xu aided us
        - Then we held the address accessed by the function and tried to print
        it, confirming that the value was indeed changed
      */
        *(number *)UArray2b_at(test_array, 0, 0) = MARKER;

        int tempcheck = *(number *)UArray2b_at(test_array, 0, 0);   

        printf("/n value on the 2D array was changed to 99: %d\n", tempcheck);
      
        
        /* Here we started testing for the functions having higher level 
        functions as parameters. We started first with the row-major one, 
        in which we used the print_check function given to us in this code
        as the "apply" parameter. Assertions and correct values were 
        apparently properly printed
         */

        printf("Trying block map \n");
        // UArray2b_map(test_array, check_and_print, &OK);
        
        // printf("Trying row major\n");
        // UArray2_map_row_major(test_array, check_and_print, &OK);
      
        /* Here we did the same but for column */ 
        // printf("Trying column major\n");
        // UArray2_map_col_major(test_array, check_and_print, &OK);

        
        /* Note: we are only setting a value on the corner of the array */
        // *((number *)UArray2_at(test_array, DIM1 - 1, DIM2 - 1)) = MARKER;

        // printf("Trying column major\n");
        // UArray2_map_col_major(test_array, check_and_print, &OK);

        // printf("Trying row major\n");
        // UArray2_map_row_major(test_array, check_and_print, &OK);
        printf("done mapping\n");
        UArray2b_free(&test_array);

        // printf("The array is %sOK!\n", (OK ? "" : "NOT "));

}
