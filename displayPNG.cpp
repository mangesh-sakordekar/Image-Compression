/*
        ***** displayPNG.cpp *****

Read in a PNG image with LodePNG, convert to grayscale,
and display gray/quadtree images side by side using OpenGL.

Demo program for CSC315 PA#2.
Usage:  displayPNG image.png

Authors: John M. Weiss, Ph.D.
         Mangesh Sakordekar
         Christy Bergevin
Class:   CSC 315 Data Structures & Algorithms
Date:    Spring 2018
*/

#include <cstdio>
#include <ctime>
#include <GL/freeglut.h>
#include "image.h"

// main function
int main( int argc, char *argv[] )
{
    // process command-line arguments
    char* filename;         // input filename
    int thresh = 128;
    switch ( argc )
    {
        case 3:
            thresh = atoi( argv[2] );
            if ( thresh < 1 || thresh > 255 ) thresh = 128;
        case 2:
            filename = argv[1];
            break;
        default:
            printf( "Usage: %s image.png\n", argv[0] );
            return -1;
    }
    
    // read input PNG file into 1-D array of 24-bit RGB color pixels
    printf( "reading %s: ", filename );
    unsigned width = 0, height = 0;
    byte** imageRGB = readPNG( filename, width, height );
    printf( "%d x %d\n", width, height );

    // generate 8-bit grayscale intensity image from 24-bit color image
    printf( "generating 8-bit grayscale image\n" );
    byte** imageGray = grayscale( width, height, imageRGB );

    //assign range
    assign_rng(thresh);

    // generate quadtree image
    printf( "generating quadtree structure\n");

    // perform various OpenGL initializations
    glutInit( &argc, argv );
    initOpenGL( filename, width, height, imageRGB, imageGray );

    // go into OpenGL/GLUT main loop, never to return
    glutMainLoop();

    // yeah I know, but it keeps compilers from bitching
    return 0;
}
