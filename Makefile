# Makefile for PNG display application

# Author: John M. Weiss, Ph.D.
# Class:  CSC 315 Data Structures & Algorithms
# Date:   Spring 2018

# C++ compiler flags
CXXFLAGS = -Wall -O2 -fmax-errors=5 -std=c++11

# source and object files
SRC = displayPNG.cpp alloc2d.cpp graphics.cpp imageio.cpp lodepng.cpp
OBJ = $(SRC:.cpp=.o)

# OpenGL/GLUT libraries for Linux
GL_LIBS = -lglut -lGLU -lGL
# add in POSIX threads library, to prevent driver issues in Linux Lab
GL_LIBS += -lpthread

# Targets
quadtree:	$(OBJ)
	g++ -o $@ $^ $(GL_LIBS)

clean:
	rm -f *.o
