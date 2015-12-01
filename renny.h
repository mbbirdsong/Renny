/*
 * renny.h
 *
 *  Created on: Oct 27, 2011
 *      Author: michaelb
 */

#ifndef RENNY_H_
#define RENNY_H_
extern unsigned char stopping;
extern unsigned char starting;
extern unsigned char renny_moving;
extern unsigned char renny_rocking;
extern unsigned char renny_crashing;
extern unsigned char renny_waving;
extern unsigned char renny_flipping;
extern unsigned char renny_prizing;
extern unsigned char renny_flashing;
extern unsigned char renny_lighting;
extern unsigned char renny_wave_bonus;

extern unsigned char renny_taking_off;
extern unsigned char renny_touching_down;
extern unsigned char renny_flying;

extern int current_prize;
extern int current_flash;

extern float balloon_fade;
extern unsigned char shadow_save;

extern unsigned char show_hand;
extern unsigned char show_balloon;

extern double t_renny_idle;
extern double t_renny_prize;
extern double t_renny_flash;

extern float renny_alt;
extern double renny_pos[3];
extern double renny_x_z_pos[3];
extern double renny_x_last;
extern double renny_z_last;
extern double renny_rot[3];
extern unsigned char sleep_now;

extern int renny_quad[3][2];


void start_renny(unsigned char color, char texture, double walk_scale[2]);
void stop_renny(unsigned char color, char texture, double walk_scale[2]);
void crash_renny(unsigned char color, char texture, double walk_scale[2]);
void wave_renny(unsigned char color, char texture, double walk_scale[2]);
void flip_renny(unsigned char color, char texture, double walk_scale[2]);
void walk_renny(unsigned char color, char texture, double walk_scale[2]);
void takeoff_renny(unsigned char color, char texture, double walk_scale[2]);
void land_renny(unsigned char color, char texture, double walk_scale[2]);
void fly_renny(unsigned char color, char texture, double walk_scale[2]);

void hand(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color, char texture);
void hand2(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color, char texture);

void renny_shadow();

#endif /* RENNY_H_ */
