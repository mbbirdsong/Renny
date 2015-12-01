/*
 * basic_shapes.h
 *
 *  Created on: Oct 15, 2011
 *      Author: michaelb
 */

#ifndef BASIC_SHAPES_H_
#define BASIC_SHAPES_H_

void fog_cube();

void energy_cube(double x,double y,double z, double dx,double dy,double dz,
        double th, double scale_x, double scale_y,
        int bevel, unsigned char color, char texture, int obj_num);

void new_cube(double x,double y,double z, double dx,double dy,double dz,
        double th, double scale_x, double scale_y,
        int bevel, unsigned char color, char texture);

void TwoDTree(double x,double y,double z);

#endif /* BASIC_SHAPES_H_ */
