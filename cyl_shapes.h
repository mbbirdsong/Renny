/*
 * shapes.h
 *
 *  Created on: Sep 17, 2011
 *      Author: michael birdsong
 */

#ifndef CYL_SHAPES_H_
#define CYL_SHAPES_H_

void FunkyCullahs(int th, unsigned char option);

void cone(double x,double y,double z,double r, double l, unsigned char flat,
          unsigned char color, char texture);
void chopped_cone(double x,double y,double z,double r, double l, double d2,
                  unsigned char flats, unsigned char color, char texture);
void cylinder(double x,double y,double z,double r, double l, unsigned char flats,
              unsigned char color, char texture);
void cyl_pipe(double x,double y,double z,double* r, double ri, double ro, double l,
          unsigned char color, char texture);
void notched_cylinder(double x,double y,double z,double r, double l, double d2,
        double spt_x, double spt_y, int spt_w, int spt_th, unsigned char flats,
        unsigned char color, char texture);
void partial_cylinder(double x,double y,double z,double r, double l,
                      int* angle, unsigned char flats, unsigned char color, char texture);

#endif /* CYL_SHAPES_H_ */
