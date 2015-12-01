/*
 * floor.h
 *
 *  Created on: Oct 18, 2011
 *      Author: michaelb
 */

#ifndef FLOOR_H_
#define FLOOR_H_

#include <time.h>

extern double floor_dims[40][20];
extern int num_boards_wide;
extern int num_boards_long[40];

void floor_init();
void create_floor(char flr_texture);

#endif /* FLOOR_H_ */
