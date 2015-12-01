/*
 * game_area.h
 *
 *  Created on: Nov 21, 2011
 *      Author: michaelb
 */

#ifndef GAME_AREA_H_
#define GAME_AREA_H_

#include "CSCIx229.h"

#include "final_project_mbb.h"
#include "cyl_shapes.h"
#include "sphr_shapes.h"
#include "basic_shapes.h" //(?)
#include "cmpst_shapes.h" //(?)
#include "terrain.h"
#include "renny.h"

typedef struct
{
    int type;
    float pos[3];
    float rot[3];
    float scale[3];
    float radius;
    unsigned char draw;
    int area[2];
    int quad[2];
    int alpha;
    int color;
    int texture;
    int pursuit;  // only used for teapots
    int reward;
    int penalty;
} Object;

typedef struct
{
    int type;
    char name[21];
    double radius;
    int reward;
    int penalty;
} ShapeData;

typedef struct
{
    int populated;
    unsigned char in_fog;
    double center[2];
    double x_range[2];
    double z_range[2];
    int index[2];
    int i_range[2];
    int j_range[2];
    int num_objs;
    int objs_in_quad[12];
} QuadData;

#define NUM_X_QUADS  NUM_X_AREAS/X_QUAD_DIM
#define NUM_Z_QUADS  NUM_Z_AREAS/Z_QUAD_DIM
#define MAX_NUM_OBJS 7*NUM_X_QUADS*NUM_Z_QUADS

extern const ShapeData shapes[8];
extern Object full_objects[MAX_NUM_OBJS];
extern QuadData quads[NUM_X_QUADS][NUM_Z_QUADS];

extern int num_game_objs;
extern int num_prizes;
extern int prizes_collected;

extern float martini_angle;
extern float martini_angle_rate;

void populate_game();
void quad_from_current_pos(int quad_coords[2]);
void quad_from_pos(float coords[2],int quad_coords[2]);
void martini_glass_fly(int i);
void tea_pot_pursuit(int i);
void energy_cube_spin(int i);

#endif /* GAME_AREA_H_ */
