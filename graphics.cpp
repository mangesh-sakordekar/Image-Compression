/*
                ***** graphics.cpp *****

OpenGL/GLUT graphics module for image display. This includes the quadtree 
functions that compress the image and display the boundary lines of the quadtree
compression.

Authors: John M. Weiss, Ph.D.
         Mangesh Sakordekar
         Christy Bergevin
Class:   CSC 315 Data Structures & Algorithms
Date:    Spring 2018
*/

#include <iostream>
#include <string>
#include <cstdio>
#include <GL/glut.h>
#include "image.h"

using namespace std;

typedef unsigned char byte;

// OpenGL callback function prototypes etc.
void display( );
void reshape( int w, int h );
void keyboard( unsigned char key, int x, int y );
void displayColor( int x, int y, int w, int h, byte** image );
void displayMonochrome( int x, int y, int w, int h, byte** image );
void DrawTextString( char *string, int x, int y, byte r, byte g, byte b );

// symbolic constants
const int ESC = 27;

// global vars (unfortunately necessary due to OpenGL callback functions)
static unsigned height, width,leaves =0, nodes=0;
static int range = 10;
static string fname;
static byte** imageRGB = NULL;
static byte** imageMono = NULL;
static byte** imageQuad = NULL;
static byte** imageGray = NULL;
static bool showLines = false;

/************************************************************/

// various commands to initialize OpenGL and GLUT
void initOpenGL( const char *filename, unsigned w, unsigned h, byte** imgRGB, 
        byte** imgGray )
{
    height = h;
    width = w;
    imageRGB = imgRGB;
    imageGray = imgGray;
    fname=filename;

    //Two new arrays to store the lossy image and the image to show quadlines 
    imageMono = grayscale( width, height, imageRGB );
    imageQuad = grayscale( width, height, imageRGB );
   
    glutInitDisplayMode( GLUT_RGB | GLUT_SINGLE );    // 24-bit graphics and single buffering

    glutInitWindowSize( 2 * width, height+25 );     // initial window size
    glutInitWindowPosition( 0, 0 );		 // initial window position
    glutCreateWindow( filename );		 // window title

    glClearColor( 0.0, 0.0, 0.0, 0.0 );          // use black for glClear command

    // callback routines
    glutDisplayFunc( display );	                 // how to redisplay window
    glutReshapeFunc( reshape );		         // how to resize window
    glutKeyboardFunc( keyboard );                // how to handle key presses
}

/************************************************************/

// GLUT display callback
void display( )
{
    glClear( GL_COLOR_BUFFER_BIT );

    //Assigning the images to greyscale image before compressing 
    copyGray(height, width, imageGray, imageMono);
    copyGray(height, width, imageGray, imageQuad);

    //Initialze the leaf node counter and node counter to 0
    leaves=0;
    nodes =0;

    //Calls the quadtree function to obtain the lossy image
    quadtree(0, 0, height, width, imageGray, imageMono);
    //Call the quadlines function to show the splitting of the image in quads
    quadlines(0, 0, height, width, imageGray, imageQuad);

    //Printing the image size ratio and range to the console
    printsize();

    // display color and monochrome images, side by side in window
    //displayColor( 0, 0, width, height, imageRGB );
    displayMonochrome( 0, 0, width, height, imageGray );
    if(showLines)
        displayMonochrome( width, 0, width, height, imageQuad );
    else 
        displayMonochrome( width, 0, width, height, imageMono );
    // write text labels (in white)
    char str[257];
    sprintf( str, "Original Image" );
    DrawTextString( str, 8, 520, 255, 255, 255 );
    sprintf( str, "Quadtree Image,  Range : %d   (Spacebar Toggles Quads)", range );
    DrawTextString( str, width + 8, 520, 255, 255, 255 );

    glFlush(  );
}

// GLUT reshape callback (handles window resizing)
void reshape( int w, int h )
{
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity(  );
    gluOrtho2D( 0.0, w, 0.0, h );
}

// GLUT keyboard callback
void keyboard( unsigned char key, int x, int y )
{
    switch ( key )
    {
        case ESC:
            exit( 0 );
            break;
        case ' ':
            //update the boolean to show or not show the lines
            showLines = !showLines;
            glutPostRedisplay();
	    break;
        case '+':
            range++;
	    //Set the range to 255 if it goes above 255
	    if(range>255)
	      {
		range=255;
		cout<<endl<<"Range must be between 0 to 255."<<endl;
		break;
	      }
            glutPostRedisplay();
	    break;
        case '-':
            range--;
	    //Sets the range to 0 if the range goes in negative
	    if(range<0)
	      {
		range=0;
		cout<<endl<<"Range must be between 0 to 255."<<endl;
		break;
	      }
            glutPostRedisplay();
	    break;
    }
}

/******************************************************************************/

// display 24-bit color image
void displayColor( int x, int y, int w, int h, byte** image )
{
    glRasterPos2i( x, y );
    glDrawPixels( w, h, GL_RGB, GL_UNSIGNED_BYTE, *image );
}

// display 8-bit monochrome image
void displayMonochrome( int x, int y, int w, int h, byte** image )
{
    glRasterPos2i( x, y );
    glDrawPixels( w, h, GL_LUMINANCE, GL_UNSIGNED_BYTE, *image );
}

// write a text string
void DrawTextString( char *string, int x, int y, byte r, byte g, byte b )
{
    glColor3ub( r, g, b );
    glRasterPos2i( x, y );
    while ( *string )
    {
        glutBitmapCharacter( GLUT_BITMAP_8_BY_13, *string );
        string++;
    }
}

/*************************************************************************//**
 * Author: Mangesh Sakordekar
 *
 * Description:
 * The function fills in the specified block of image with the average value
 * of pixels in the black.
 *
 ****************************************************************************/
void fillsq( int avg, int xcod, int ycod , int h, int w, byte** imageQ)
{
  
    for(int i=0;i<h;i++)
    {
        for(int j=0;j<w;j++)
            imageQ[xcod+i][ycod+j]=(byte)avg;
    }
}





/*************************************************************************//**
 * Author: Mangesh Sakordekar
 *
 * Description:
 * Recursive function to immitate the working of a quadtree image compression.
 * The function calculates the average value of all the pixels in the block 
 * Calculates the min aand max values and checkes if they are in the range.
 * if they are in the range fillsq() function is called to fill the block 
 * with the average value or else the block is split into four quadrants and 
 * quadtree() function is called on each of it
 *
 ****************************************************************************/
void quadtree(int xcod, int ycod, int row, int col,byte** imggray, byte** imgquad)
{
    //Variables for storing min max average and sum
    int min=255, max=0;
    int avg=0,sum=0;
   
    //Returns the same image if the range is 0 
    if (range==0)
        return;

    // Base case for the recursive function
    if (row ==0 || col == 0 )
      return;

    //Nested for loops to calculate sum and find min and max values
    for(int i=0;i<row;i++)
    {
        for(int j=0;j<col;j++)
        {
            //typecasting to integer before adding to the sum
            sum+=(int)imggray[xcod+i][ycod+j];

            //finding the minimum value 
            if(min>imggray[xcod+i][ycod+j])
	      min=(int)imggray[xcod+i][ycod+j];

            //Finding the max value
            if(max<imggray[xcod+i][ycod+j])
	      max=(int)imggray[xcod+i][ycod+j];
        }
    }
    //Calculating the average
    avg=sum/(col*row);
    //cout<<avg<<endl;
    //Checking if max and min are within the range 
    if((min>=avg-range) && (max<=avg+range))
    {
        //Calls fillsq() to fill the image block
      fillsq(avg, xcod,ycod,row,col, imgquad);
        return;
    }

    //Splitting the current image block into four parts
    quadtree(xcod,ycod,row/2,col/2,imggray,imgquad);             //1st quadrant
    quadtree(xcod,ycod+row/2,row/2,col/2,imggray,imgquad);       //2nd quadrant
    quadtree(xcod+col/2,ycod,row/2,col/2,imggray,imgquad);        //3rd quadrant
    quadtree(xcod+col/2,ycod+row/2,row/2,col/2,imggray,imgquad);  //4th quadrant
    
    return;
}



/*************************************************************************//**
 * Author: Mangesh Sakordekar
 *
 * Description:
 * Recursive function to show the splitting of an image in a quadtree image 
 * compression. The function calculates the average value of all the pixels in 
 * the block. Calculates the min aand max values and checkes if they are in the 
 * range. if they are in the range function returns or else the block is split 
 * into four quadrants and quadlines() function is called on each of them and
 * lines() function is called to show the lines of split.
 *
 ****************************************************************************/
void quadlines(int xcod, int ycod, int row, int col, byte** imggray, byte** imgline)
{
    //Variables for storing min max average and sum
    int min=255, max=0;
    int avg=0,sum=0;

    //Base case for the recursive function
    if(row == 0 || col==0)
      return;

    nodes++;       //Increment the number of nodes

    //Nested for loops to calculate sum and find min and max values
    for(int i=0;i<row;i++)
    {
        for(int j=0;j<col;j++)
        {
            //typecasting to integer before adding to the sum
            sum+=(int)imggray[xcod+i][ycod+j];
            
            //Finding the min value
            if(min>imggray[xcod+i][ycod+j])
	      min=(int)imggray[xcod+i][ycod+j];
            
            //Finding the max value
            if(max<imggray[xcod+i][ycod+j])
	      max=(int)imggray[xcod+i][ycod+j];
        }
    }

    //Calculating the average
    avg=sum/(col*row);

    //Checking if max and min are within the range
    if((min>=avg-range) && (max<=avg+range))
    {
      leaves++;  // increment leaf node counter
        return;
    }

    //Splitting the current image block into four parts
    quadlines(xcod,ycod,row/2,col/2, imggray, imgline);
    quadlines(xcod,ycod+row/2,row/2,col/2, imggray, imgline);
    quadlines(xcod+col/2,ycod,row/2,col/2, imggray, imgline);
    quadlines(xcod+col/2,ycod+row/2,row/2,col/2, imggray, imgline);

    // DELETED PARAMETERS 
    //Calls lines() to draw the lines showing the split
    lines( xcod, ycod, row, col, imgline);

    return;
}


/*************************************************************************//**
 * Author: Mangesh Sakordekar
 *
 * Description:
 * The function draws a vertical and horizontal line through the middle of the 
 * block splitting it into 4 quadrants
 *
 ****************************************************************************/
void lines( int xcod, int ycod, int h, int w, byte** imageQ)
{
   
    //Drawing the horizontal line
    for(int i=0;i<h;i++)
    {
            imageQ[xcod+i][ycod+w/2]=(byte)255;
    }

    //Drawing the vertical line
    for(int j=0;j<w;j++)
    {
            imageQ[xcod+h/2][ycod+j]=(byte)255;
    }

    return;
} 


/*************************************************************************//**
 * Author: Mangesh Sakordekar
 *
 * Description:
 * The function assigns the value passed in the command line argument to the 
 * global variable range
 *
 ****************************************************************************/
void assign_rng(int rng)
{
  range=rng;
}


/*************************************************************************//**
 * Author: Mangesh Sakordekar
 *
 * Description:
 * The function prints the image information, number of nodes,
 * number of leaf nodes, estimated number of bytes required 
 * to store the quadtree encoding and the compression ratio
 * to the terminal.
 *
 ****************************************************************************/
void printsize()
{
  double avg;
  avg= (200.0*leaves)/(height*width);
  cout<<endl;
  cout<<"File Name : " << fname <<endl;
  cout<<"Dimentions : " << height <<" x " << width <<endl;
  cout<<"Number of Pixels : " << height*width <<endl;
  cout<<"Number of Nodes : " << nodes << endl;
  cout<<"Number of Leaf Nodes : " << leaves <<endl;
  cout<<"Bytes required for Quadtree encoding : " << 2*leaves << endl;
  cout<<"Compression Ratio :  "<<avg<<endl;
}


/*************************************************************************//**
 * Author Mangesh Sakordekar
 *
 * Description:
 * The function copies one grayscale image into another greyscale image. 
 * The images are passed in as arguments.
 *
 ****************************************************************************/
void copyGray(int h, int w, byte** gray, byte** newimg)
{
  for(int i=0; i<h; i++)
      for(int j=0; j<w; j++)
	newimg[i][j]=gray[i][j];
}
