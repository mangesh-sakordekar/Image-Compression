/*
                ***** imageio.cpp *****

readPNG:    read in a PNG image with LodePNG, return 24-bit RGB color image.
grayscale:  convert 24-bit RGB color image to 8-bit grayscale.

Author: John M. Weiss, Ph.D.
Class:  CSC 315 Data Structures & Algorithms
Date:   Spring 2018
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "lodepng.h"
#include "image.h"

// read PNG image using LodePNG, and return as 2-D array of RGB pixels
byte** readPNG( char* filename, unsigned& width, unsigned& height )
{
    // read input PNG file
    byte* pixels;
    unsigned error = lodepng_decode_file( &pixels, &width, &height, filename, LCT_RGB, 8 );
    if ( error )
    {
        printf( "decoder error while reading file %s\n", filename );
        printf( "error code %u: %s\n", error, lodepng_error_text( error ) );
        return NULL;
    }
    // printf( "%s: %d x %d\n", filename, width, height );
    unsigned nbytes = width * 3;

    // make width a multiple of 4, else grayscale image does not display properly in OpenGL
    unsigned pad = 0;
    if ( width % 4 != 0 ) pad = 4 - width % 4;
    width += pad;

    // flip image as we copy it into 2-D array
    byte** image = alloc2D( height, width * 3 );
    byte* img = pixels;
    for ( int row = height - 1; row >= 0; --row, pixels += nbytes )
        memcpy( image[row], pixels, nbytes );

    // finish up
    free( img );
    return image;
}

// generate 8-bit grayscale intensity image from 24-bit RGB color image
byte** grayscale( unsigned width, unsigned height, byte** imgRGB )
{
    byte** image = alloc2D( height, width );
    byte* img = *imgRGB;
    for ( unsigned row = 0; row < height; ++row )
    {
        for ( unsigned col = 0; col < width; ++col )
        {
            int r = *img++;
            int g = *img++;
            int b = *img++;
            image[row][col] = 0.3 * r + 0.6 * g + 0.1 * b + 0.5;
        }
    }
    return image;
}
