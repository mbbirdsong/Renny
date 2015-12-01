/*
 * terrain.h
 *
 *  Created on: Nov 15, 2011
 *      Author: michaelb
 */

#ifndef TERRAIN_H_
#define TERRAIN_H_

#include <time.h>

//#define MIN_DEBUG_PRINT_TERRAIN
//#define DEBUG_PRINT_TERRAIN
//#define DEBUG_TERRAIN
//#define SHADOW_DEBUG
#define TERRAIN_FINE_
#ifdef TERRAIN_FINE_
#define NUM_X_AREAS 55
#define NUM_Z_AREAS 55
#define X_DIM 50
#define Z_DIM 50
#define X_0 18
#define Z_0 26
#define X_1 38
#define Z_1 38
#define X_QUAD_DIM 5
#define Z_QUAD_DIM 5
#define FOG_QUAD_SPAN 1
#define X_DARK_RANGE 6
#define Z_DARK_RANGE 6
#define X_TOTAL_DARK 4
#define Z_TOTAL_DARK 4
#define X_EDGE 10
#define Z_EDGE 10
#else
#define NUM_X_AREAS 21
#define NUM_Z_AREAS 21
#define X_DIM 125
#define Z_DIM 125
#define X_0 5
#define Z_0 11
#define X_1 17
#define Z_1 17
#define X_QUAD_DIM 3
#define Z_QUAD_DIM 3
#define FOG_QUAD_SPAN 1
#define X_DARK_RANGE 2
#define Z_DARK_RANGE 2
#define X_TOTAL_DARK 1
#define Z_TOTAL_DARK 1
#define X_EDGE 4
#define Z_EDGE 4
#endif /* TERRAIN_FINE_ */

#ifdef DEBUG_TERRAIN
#define NUM_HILLS 2
#else
#ifdef TERRAIN_FINE_
#define NUM_HILLS 30
#else
#define NUM_HILLS 12
#endif
#endif

extern int i_cntr;
extern int j_cntr;

extern float dims[2];
extern float x_offset;
extern float z_offset;
extern float max_x_length;
extern float max_z_length;
extern float dist_energy;
extern float current_elev;
extern float last_elev;

/* fog variables */
extern float fog_x_range[2];
extern int fog_center_min_el[2];
extern float fog_z_range[2];
extern float fog_center_loc[2];

extern int max_el_x_z[2];
extern float dark_center_loc[2];
extern float dark_center_range[2];
extern int dark_center[2];
extern float dark_center_range[2];
extern float dark_center_inner[2];
extern float dark_range_sq;
extern float dark_range;
extern float dark_inner_sq;
extern float dark_inner;


extern float min_el;
extern float max_el;

extern float fog_mid_el;
extern float fog_max_el;

void terrain_init();
void create_terrain(char terain_texture);
void renny_shadow_area();

void get_current_normal(float normal[]);
void get_normal(int i, int j, float normal[]);
void get_current_elevation();
float get_elevation(float x_obj, float z_obj);
void get_area_data(int coords[2], float center[2]);
void shadow_normal_and_plane(float x_obj, float z_obj, float N[], float E[]);
#endif /* TERRAIN_H_ */
