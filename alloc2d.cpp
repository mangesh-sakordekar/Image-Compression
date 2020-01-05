/*
                ***** alloc2d.cpp *****

Routines for dynamic 2-D array (de)allocation.

Author: John M. Weiss, Ph.D.
Class:  CSC 315 Data Structures & Algorithms
Date:   Spring 2018
*/

#include <cstring>

typedef unsigned char byte;

// allocate 2-D array
byte** alloc2D( int nrows, int ncols )
{
    // allocate a row of pointers
    byte** image = new byte* [nrows];
    if ( image == NULL ) return NULL;

    // allocate array elements
    byte* pixels = new byte [ nrows * ncols ];
    if ( pixels == NULL ) { delete [] image; return NULL; }

    // zero out array elements
    memset( pixels, 0, nrows * ncols * sizeof (byte) );

    // point the row pointers at each row of the 2-D array
    for ( int row = 0; row < nrows; ++row, pixels += ncols )
        image[ row ] = pixels;

    // and return...
    return image;
}

// deallocate 2-D array
void free2D( byte** image )
{
    if ( image )
    {
        delete [] *image;
        delete [] image;
    }
}