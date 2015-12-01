/*
 * final_project_mbb.h
 *
 *  Created on: Oct 23, 2011
 *      Author: michaelb
 */

#ifndef FINAL_PROJECT_MBB_H_
#define FINAL_PROJECT_MBB_H_
//#define DEBUG_PRINT
#define ENERGY_BOOST 108
#define FLY_BONUS 42


extern const int draw_angle;

extern int th;         //  Azimuth of view angle
extern int ph;         //  Elevation of view angle
extern int axes;       //  Display axes
extern int mode;       //  What to display
extern int ntex;
extern char grass_texture;

extern unsigned char show_warn; /* warning set, 1==TRUE, 0==FALSE */
extern char *warning;

// Light values
extern int one;
extern float shinyvec[1]; // Shininess (value)
extern int emission;      // Emission intensity (%)
extern int inc;           // Ball increment
extern float glb_light_level;

// Texture values
extern unsigned int textures[15]; // Object Texture names
extern unsigned int floor_textures[2][4]; // Floor Texture names


extern double zh;         //  Rotation of light
extern double zh1;        //  Rotation of flying glasses
extern double t_now;
extern double t_last;
extern double t_coll;

extern double rny;  // Renny motion angle
extern double rny_last;  // Renny motion angle last step
extern double rny_speed;

extern double current_normal[3];

extern double glb_scale[3];

// object array 0=x_pos, 1=y_pos, 2=z_pos, 3=radius
extern double objects[100][4];
extern int num_objects;
extern int num_cubes;

// vector r,g,b values
extern double rd;
extern double gr;
extern double bl;

// game global variables
extern double energy;  // initial energy level
extern int score;  // initial player score
extern double fly_time;

extern double view_vector[3];
extern double view_pos[3];

extern unsigned char drawing_renny;

extern int fog_off;


#endif /* FINAL_PROJECT_MBB_H_ */
