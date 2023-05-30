/*****************************************************************************
 *
 *                               uarray2b.c
 *
 *     Assignment: locality
 *     Authors:  - Joao Quintanilha (jquint04)
 *               - Kelly Tsidji (atsidj01) 
 *     Date:     10/13/2022
 *     
 *    ppmtrans.c implements various transformations on a given ppm image.
 *    The implemented transformations are 0-deg, 90-deg, 180-deg, 270-deg 
 *    rotations. To perform the 90-deg, 180-deg, and 270-deg rotations, a new
 *    Pnm_ppm image is created that holds the dimensions of the rotated image;
 *    then the program iterates through the original Pnm_ppm image, and copies
 *    each of its pixels into the appropriate location in the rotated image.
 *    The Pnm_ppm that stores the rotated image is then printed to output
 *    by the program. To perform the 0-deg rotation, the original image,
 *    which is stored in a Pnm_ppm is simply printed to output.
 *    The order in which the image array is iterated through depends on the
 *    the given mapping order: row-major, col-major, and block-major.
 *    row-major and col-major mappings create a regular UArray2 to hold the
 *    pixels of the image. block-major mapping creates a UArray2b (blocked
 *    array) to hold the pixels of the image.
 *    If you have an original image of size w × h, then when the image is 
 *    rotated 90 degrees, pixel (i, j) in the original becomes pixel 
 *    (h − j − 1, i) in the rotated image. When the image is rotated 180 
 *    degrees, pixel (i, j) becomes pixel (w − i − 1, h − j − 1).
 *    When the image is rotated 270 degrees, pixel (i, j) becomes pixel
 *    (j, w - 1 - i).
 *
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cputiming.h"

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include <stdbool.h>


#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)


static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

/* Functions */
Pnm_ppm file_to_image(A2Methods_T methods, char *filename);
Pnm_ppm rotate_img(Pnm_ppm img, int rotate_deg, double *time_used, 
A2Methods_mapfun *map);
void rotate90(int col, int row, A2Methods_UArray2 rotated_image, 
A2Methods_Object *ptr, void *original_image);
void rotate180(int col, int row, A2Methods_UArray2 rotated_image, 
A2Methods_Object *ptr, void *original_image);
void rotate270(int col, int row, A2Methods_UArray2 raster_image, 
A2Methods_Object *ptr, void *rotated_image);
void time_reader(Pnm_ppm image, Pnm_ppm rotated_image, 
                 double time, int rotation, 
                 char *time_filename, char *extra_credit,
                 char *filename, A2Methods_mapfun *map); 


int main(int argc, char *argv[]) 
{
        char *file_name = NULL;
        char *time_file_name = NULL;
        int   rotation       = 0;
        int   i;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                        


                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");


                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");


                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }

                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i]; 

                } else if (strcmp(argv[i], "-flip") == 0) {
                        if (!(i + 1 < argc)) {      /* no flip value */
                                usage(argv[0]);
                        } else if (strcmp(argv[++i], "horizontal") == 0) {
                                fprintf(stderr, 
                                        "-flip horizontal not implemented\n");
                                exit(1);
                        } else if (strcmp(argv[i], "vertical") == 0) {
                                fprintf(stderr, 
                                        "-flip vertical not implemented\n");
                                exit(1);
                        }
                        else {
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-transpose") == 0) {
                        fprintf(stderr, "-transpose not implemented\n");
                        exit(1);
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (file_name == NULL) {
                        file_name = argv[i];

                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }

        Pnm_ppm image = file_to_image(methods, file_name);
        
        Pnm_ppm rotated_image;
        double time_used = 0;
      
        
        
        if (rotation == 0) {
                Pnm_ppmwrite(stdout, image);
        }
        else {
               rotated_image = rotate_img(image, rotation, &time_used, map);

               Pnm_ppmwrite(stdout, rotated_image);
               Pnm_ppmfree(&rotated_image);
        }

        if (time_file_name != NULL) {
          time_reader(image, rotated_image, 
                    time_used, rotation, 
                    time_file_name, NULL,
                    file_name, map);
        }

        
        /* section for freeing memory */
        Pnm_ppmfree(&image);
        return 0;
}


/*     function file_to_image
 *     Purpose: allocate space for the 2Darray itself with a block occupying 
 *              at most 64KB
 *     Parameters:
 *                - a pointer to a A2Methods_T struct called methods, which
 *                  holds the type of array (plain or blocked) that will be
 *                  used to store the given image
 *                 - a character pointer holding the name of the file holding
 *                   the given image
 *     Returns: a pointer to a Pnm_ppm that represents the given image
 *     Expectations: 
 *                 - methods cannot be null
 */
Pnm_ppm file_to_image(A2Methods_T methods, char *filename) 
{
        assert(methods != NULL); /* methods should not be null */

        Pnm_ppm image = NULL;
        FILE *input;

        /* if filename is null, standard input will be used */
        if (filename == NULL) {
                image = Pnm_ppmread(stdin, methods);
        } else {
                input = fopen(filename, "rb");
                if (input == NULL) {
                        fprintf(stderr, "Input file cannot be opened\n");
                        exit(EXIT_FAILURE);
                }
                image = Pnm_ppmread(input, methods);  
                fclose(input);
        }

        return image;        
}


/*     function rotate_img
 *     Purpose: rotate the image based on the given rotation degree
 *              and iterates through the given image using the given mapping
 *              order
 *     Parameters:
 *                - a pointer to a Pnm_ppm that represents the given image
 *                - the rotation degree (representing how much to rotate
 *                  the image)
 *                - a pointer to a double that will hold the time it took
 *                  to complete the given rotation on the image
 *                - a double pointer to a A2Methods_mapfun function called map,
 *                  which holds the mapping function to use when iterating
 *                  through the array
 *     Returns: a pointer to a Pnm_ppm that represents the rotated image
 *     Expectations: 
 *                 - map cannot be null
 *                 - the Pnm_ppm struct cannot be null
 */
Pnm_ppm rotate_img(Pnm_ppm img, int rotate_deg, double *time_used, 
A2Methods_mapfun *map)
{
        
        assert(img != NULL);
        assert(map != NULL);

        int rotated_img_width = img->width;
        int rotated_img_height = img->height;
        int rotated_img_size = img->methods->size(img->pixels);

        /* if the rotate angle is 90 or 270, the width and height measurements
        are switched*/
        if (rotate_deg == 90 || rotate_deg == 270) {
                rotated_img_width = img->height;
                rotated_img_height = img->width;
        }
        
        /* Creates a rotated version of the given image */
        Pnm_ppm rotated_img = malloc(sizeof(*rotated_img));
        assert(rotated_img != NULL);

        rotated_img->width = rotated_img_width;
        rotated_img->height = rotated_img_height;
        rotated_img->denominator = img->denominator;
        rotated_img->methods = img->methods;
        rotated_img->pixels = rotated_img->methods->new(rotated_img_width, 
        rotated_img_height, rotated_img_size);

        /* creating the timer */
        CPUTime_T timer; // declare an instance of the timer
        timer = CPUTime_New(); // initialize the instance

        /* populating the rotated image with the information from the given 
        image */
        if (rotate_deg == 90) {
                CPUTime_Start(timer); /* starting the timer */
                map(img->pixels, rotate90, (void *)rotated_img);
                *time_used = CPUTime_Stop(timer); /* stopping the timer */
        }
        if (rotate_deg == 180) {
                CPUTime_Start(timer); /* starting the timer */
                map(img->pixels, rotate180, (void *)rotated_img);
                *time_used = CPUTime_Stop(timer); /* stopping the timer */
        }
        if (rotate_deg == 270) {
                CPUTime_Start(timer); /* starting the timer */
                map(img->pixels, rotate270, (void *)rotated_img);
                *time_used = CPUTime_Stop(timer); /* stopping the timer */
        }

        CPUTime_Free(&timer);
        return rotated_img;
}


/*     function rotate90
 *     Purpose: rotate the given image 90 degrees
 *     Parameters:
 *                - a integer called column holding the column index
 *                - an integer called row holding the row index
 *                - a pointer to a A2Methods_UArray2 struct which holds the 
 *                  pixels of the image
 *                - a double pointer to a A2Methods_Object struct
 *                - a void pointer that will hold a Pnm_ppm representing the
 *                  rotated image
 *     Returns: nothing
 *     Expectations: 
 *                 - map cannot be null
 *                 - the Pnm_ppm struct cannot be null
 */
void rotate90(int col, int row, A2Methods_UArray2 raster_image, 
A2Methods_Object *ptr, void *rotated_image) 
{
        assert(raster_image != NULL);
        assert(rotated_image != NULL);

        /* right now we will be using a real rgb instance -> no pointer */
        Pnm_rgb pixel = NULL;

        /* cast the closure to the Pnm_ppm rotated image */
        Pnm_ppm rotated = (Pnm_ppm)rotated_image;

        /* we extract methods from the closure element -> Pnm_ppm */
        A2Methods_T methods = (A2Methods_T)rotated->methods;

        /* KEY POINT: when we call at, we have to cast it as PIXEL, 
        NOT A POINTER */
        pixel = (Pnm_rgb)methods->at(raster_image, col, row);

        int height = methods->height(raster_image);
        
        /* store the location that will hold the pixel */
        A2Methods_Object *location = methods->at(rotated->pixels, 
        (height - row - 1), col);
        assert(location != NULL);
        
        /* assign the pixel from the original image to the new location */
        /* NOTE: we are just changing the locations of the pixels, 
        w/o deleting anything or allocating any new memory in the heap */
        *(Pnm_rgb)location = *pixel;
        
        (void)ptr;
}


/*     function rotate180
 *     Purpose: rotate the given image 180 degrees
 *     Parameters:
 *                - a integer called column holding the column index
 *                - an integer called row holding the row index
 *                - a pointer to a A2Methods_UArray2 struct which holds the 
 *                  pixels of the image
 *                - a double pointer to a A2Methods_Object struct
 *                - a void pointer that will hold a Pnm_ppm representing the
 *                  rotated image
 *     Returns: nothing
 *     Expectations: 
 *                 - map cannot be null
 *                 - the Pnm_ppm struct cannot be null
 */
void rotate180(int col, int row, A2Methods_UArray2 raster_image, 
A2Methods_Object *ptr, void *rotated_image) 
{
        assert(raster_image != NULL);
        assert(rotated_image != NULL);

        /*UPDATES*/
        Pnm_rgb pixel = NULL;

        /* cast the closure to the Pnm_ppm rotated image */
        Pnm_ppm rotated = (Pnm_ppm)rotated_image;

        /* we extract methods from the closure element -> Pnm_ppm */
        A2Methods_T methods = (A2Methods_T)rotated->methods;

        /* KEY POINT: when we call at, we have to cast it as PIXEL, 
        NOT A POINTER */
        pixel = (Pnm_rgb)methods->at(raster_image, col, row);

        int height = methods->height(raster_image);
        int width = methods->width(raster_image);
        
        /* store the location that will hold the pixel */
        A2Methods_Object *location = methods->at(rotated->pixels, 
        (width - col - 1), height - row - 1);
        assert(location != NULL);
        
        /* assign the pixel from the original image to the new location */
        *(Pnm_rgb)location = *pixel;
        
        (void)ptr;
}


/*     function rotate270
 *     Purpose: rotate the given image 270 degrees
 *     Parameters:
 *                - a integer called column holding the column index
 *                - an integer called row holding the row index
 *                - a pointer to a A2Methods_UArray2 struct which holds the 
 *                  pixels of the image
 *                - a double pointer to a A2Methods_Object struct
 *                - a void pointer that will hold a Pnm_ppm representing the
 *                  rotated image
 *     Returns: nothing
 *     Expectations: 
 *                 - map cannot be null
 *                 - the Pnm_ppm struct cannot be null
 */
void rotate270(int col, int row, A2Methods_UArray2 raster_image, 
A2Methods_Object *ptr, void *rotated_image) 
{
        assert(raster_image != NULL);
        assert(rotated_image != NULL);

        /* UPDATES */
        /* right now we will be using a real rgb instance -> no pointer */
        Pnm_rgb pixel = NULL;

        /* cast the closure to the Pnm_ppm rotated image */
        Pnm_ppm rotated = (Pnm_ppm)rotated_image;

        /* we extract methods from the closure element -> Pnm_ppm */
        A2Methods_T methods = (A2Methods_T)rotated->methods;

        /* KEY POINT: when we call at, we have to cast it as PIXEL, 
        NOT A POINTER */
        pixel = (Pnm_rgb)methods->at(raster_image, col, row);

        // int height = methods->height(raster_image);
        int width = methods->width(raster_image);
        
        /* store the location that will hold the pixel */
        A2Methods_Object *location = methods->at(rotated->pixels, row, 
        width - 1 - col);
        
        /* assign the pixel from the original image to the new location */
        *(Pnm_rgb)location = *pixel;
        
        (void)ptr;

}


/*     function time_reader
 *     Purpose: prints the time report 
 *              (including the total CPU time for an image rotation)
 *              to the given file
 *     Parameters:
 *                - a pointer to a Pnm_ppm that represents the given image
 *                - a pointer to a Pnm_ppm that represents the rotated image
 *                - a double representing the time it took for the image 
 *                  rotation
 *                - the rotation degree (representing how much to rotate
 *                  the image)
 *                - a char pointer holding the name of the file to write
 *                  the time report to
 *                - a char pointer representing whether the image
 *                  transformation was for extra credit
 *                - a char pointer holding the name of the file containing
 *                  the original ppm image
 *                - a double pointer to a A2Methods_mapfun function called map,
 *                  which holds the mapping function used when iterating
 *                  through the array
 *     Returns: nothing
 *     Expectations: 
 *                 - map cannot be null
 *                 - the Pnm_ppm struct cannot be null
 */
void time_reader(Pnm_ppm image, Pnm_ppm rotated_image, 
                 double time, int rotation, 
                 char *time_filename, char *extra_credit,
                 char *filename, A2Methods_mapfun *map) 
{
        assert(image != NULL);
        assert(map != NULL);

        FILE *time_output;

        if (time_filename == NULL) {
          return;
        }

        time_output = fopen(time_filename, "a");
        assert(time_output != NULL);

        fprintf(time_output, 
        "\n/*********************** TIME REPORT ***********************/\n");

        fprintf(time_output, 
        "-- All tests were performed in jquint04's virtual machine --\n");
        fprintf(time_output, 
        "--------------------- Linux Lab Machine --------------------\n");


        fprintf(time_output, 
        "\n------------------------Image details-----------------------\n");
        fprintf(time_output, "Image's name:        %s\n", filename);
        
        
        fprintf(time_output, "Width and height:    %d x %d\n", image->width, 
        image->height);
        fprintf(time_output, "Number of pixels:    %d\n", 
        (image->width * image->height));

        fprintf(time_output, 
        "\n--------------------Transformation details------------------\n");
        
        if (uarray2_methods_plain == image->methods) {
          fprintf(time_output, "Method used:         UArray2\n");
        } else if (uarray2_methods_blocked == image->methods) {
          fprintf(time_output, "Method used:         UArray2b\n");
        }
        
        if (map == image->methods->map_row_major) {
          fprintf(time_output, "Map used:            row major\n");
        } else if (map == image->methods->map_col_major) {
          fprintf(time_output, "Map used:            col major\n");
        } else if (map == image->methods->map_block_major) {
          fprintf(time_output, "Map used:            block major\n");
        }

        if (extra_credit == NULL) {
          fprintf(time_output, "Transformation used: %d-degree rotation\n", 
          rotation);
        } else {
          fprintf(time_output, "Transformation used: %s\n", extra_credit);
        }

        fprintf(time_output, 
        "\n------------------------Time details------------------------\n");
        fprintf(time_output, "TOTAL CPU TIME:      %f nanoseconds\n", time);
        fprintf(time_output, "Time per pixel:      %f nanoseconds\n", 
        (time / (image->width * image->height)));

        
        

        fprintf(time_output, 
        "\n/***********************************************************/\n");
        fprintf(time_output, 
        "------------------------------------------------------------\n");

        
        (void)rotated_image;

        fclose(time_output);

}