/*
 * sphr_shapes.h
 *
 *  Created on: Oct 15, 2011
 *      Author: michaelb
 */

#ifndef SPHR_SHAPES_H_
#define SPHR_SHAPES_H_

void Vertex(double th,double ph, unsigned char color, unsigned char set_norm, char texture);
void Vertex2(double th,double ph, double rr, double xcr, unsigned char color,
             unsigned char set_norm, char texture);
void ball(double x,double y,double z,double* r, unsigned char color, char texture);
void partial_sphere(double x,double y,double z,double* r, int* theta,
                    int* phi, unsigned char flats, unsigned char color, char texture);
void torus(double x,double y,double z,double* r,double rr,double xcr,
           unsigned char color, char texture);
void partial_torus(double x,double y,double z,double* r,double rr,double xcr,
                   int* theta_min, int* phi_min, int* theta_max, int* phi_max,
                   unsigned char flats, unsigned char color, char texture);
void taper_partial_torus(double x,double y,double z,double* r,double rr,double xcr,
                   double xcr2, int* theta_min, int* phi_min, int* theta_max, int* phi_max,
                   unsigned char flats, unsigned char color, char texture);


#endif /* SPHR_SHAPES_H_ */
