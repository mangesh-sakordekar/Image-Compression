/*
                ***** image.h *****

Header file for various image processing applications.

Authors: John M. Weiss, Ph.D.
	 Mangesh Sakordekar
	 Christy Bergevin	
Class:   CSC 315 Data Structures & Algorithms
Date:    Spring 2018
*/

#ifndef IMAGE_H
#define IMAGE_H

// convenience data type
typedef unsigned char byte;

// function prototypes
byte** alloc2D( int nrows, int ncols );
void free2D( byte** image );
byte** readPNG( char* filename, unsigned& width, unsigned& height );
byte** grayscale( unsigned width, unsigned height, byte** imgRGB );
void initOpenGL( const char *filename, unsigned w, unsigned h, byte** imgRGB, byte** imgGray );

void fillsq( int avg, int xcod, int ycod,int height, int width, byte** imageQ );
void quadtree(int xcod, int ycod, int row, int col, byte** imggray,byte** imgquad);
void quadlines(int xcod, int ycod, int row, int col, byte** imggray,byte** imgquad);
void lines( int xcod, int ycod,int height, int width, byte** imageQ );
void assign_rng(int rng);
void printsize();
void copyGray(int h, int w, byte** gray, byte** newimg);
#endif
