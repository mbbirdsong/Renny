#ifndef CSCIx229
#define CSCIx229
//#define WINDOWS

//  Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

//  Include GLEW if you need access to advanced features
#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  Include GLUT and GL
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

extern char app_launch[256];

//  Handy macros for cos and sin in degrees
#define DEGS_TO_RADS 3.1415926/180.0
#define RADS_TO_DEGS 180.0/3.1415926
#define Cos(th) cos(DEGS_TO_RADS*(th))
#define Sin(th) sin(DEGS_TO_RADS*(th))

//  Functions provided by the library
void Print(const char* format , ...);
void Fatal(const char* format , ...);
unsigned int LoadTexBMP(char* file);
void Project(double fov,double asp,double dim);
void ErrCheck(char* where);
int  LoadOBJ(char* file);

#endif

