/*
 * renny.c
 *
 *  Created on: Oct 27, 2011
 *      Author: michaelb
 */

#include "CSCIx229.h"
#include "final_project_mbb.h"
#include "cyl_shapes.h"
#include "sphr_shapes.h"
#include "basic_shapes.h"
#include "cmpst_shapes.h"
#include "terrain.h"
#include "game_area.h"
#include "renny.h"

//#define FLYING_DEBUG

const double renny_walk[3]= {0.9, 0.3, 0};

double renny_x_last = 0.0;
double renny_z_last = 0.0;
double renny_x_delta = 0.0;
double renny_z_delta = 0.0;

double renny_pos[3] = {0.0, 0.3, 0.0};
double renny_x_z_pos[3] = {0.0, 0.0, 0.0};
double renny_landing[3] = {0,0,0};
double renny_crash[3] = {0,0,0};

double renny_rot[3] = {45, 0, 0};
float renny_alt = 0.0;

double renny_wave_rot[3] = {0, 0, 0};
double wave_rot = 0.0;
double wave_pos = 0.0;

double flip_rot = 0.0;
double flip_pos = 0.0;

float balloon_fade = 0.0;

unsigned char show_hand = 0;
unsigned char show_balloon = 0;

unsigned char stopping = 0;
unsigned char starting = 0;
unsigned char renny_moving = 1;
unsigned char renny_rocking = 0;

unsigned char renny_waving = 0;
unsigned char renny_crashing = 0;
unsigned char renny_flipping = 0;
unsigned char renny_in_the_air = 0;
unsigned char renny_out_of_gas = 0;
unsigned char renny_prizing = 0;
unsigned char renny_flashing = 0;
unsigned char renny_lighting = 0;
unsigned char renny_wave_bonus = 1;

unsigned char renny_taking_off = 0;
unsigned char renny_touching_down = 0;
unsigned char renny_flying = 0;


int current_prize = -1;
int current_flash = -1;

unsigned char show_init_renny_angles = 1;

unsigned char sleep_now = 0;

double key_angle = 0.0;
double take_off_key_angle = 0.0;
const double renny_fly_key_angle = -30;

double t_renny_idle = 0.0;
double t_renny_crash = 0.0;
double t_renny_wave = 0.0;
double t_renny_flip = 0.0;
double t_renny_prize = 0.0;
double t_renny_flash = 0.0;
double t_renny_takeoff = 0.0;
double t_renny_touchdown = 0.0;

double ang_renny_idle = 0.0;

double takeoff_normal[3] = {0,1.0,0};
double touchdown_normal[3] = {0,1.0,0};

float scaleballoon[3] =  {1.8, 1.8, 1.8};
float balloonoffset[3] = {-1.25, 0.0, 4.5};

int renny_quad[3][2] = {{NUM_X_QUADS/2-1,NUM_Z_QUADS/2-1},
                        {NUM_X_QUADS/2,NUM_Z_QUADS/2},
                        {NUM_X_QUADS/2+1,NUM_Z_QUADS/2+1}};

int renny_last_quad[2] = {NUM_X_QUADS/2,NUM_Z_QUADS/2};

// values for shadows
unsigned char shadow_save=1;
double shadow_pos[3];
double shadow_transl[3];
double shadow_rot[3];
double shadow_scale[3];
unsigned char shadow_color;
char shadow_texture;
double shadow_walk_scale[2];

void renny(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color, char texture, double walk_scale[2]);
void check_collision();
void right_foot(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color, char texture);
void left_foot(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color, char texture);
void foot(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color, char texture);
void body(double x, double y, double z, double r, unsigned char color, char texture, double walk_scale);
void place_renny(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color,
        char texture, double walk_scale[2]);
int get_scoring(int i, int good);

void takeoff_renny(unsigned char color, char texture, double walk_scale[2])
{
    /* like renny wave at start */
    double pos[3] = {1.05,0,0};
    double transl[3] = {0,0,0};
    double rot[3] = {0,0,0};
    double scaler[3] = {1,1,1};

    if (renny_out_of_gas == 1)
    {
        walk_renny(color, texture, walk_scale);
        show_warn = 1;
        warning = "OUT OF GAS!  GAME OVER!";
        return;
    }

    if (renny_waving == 0  && renny_flipping == 0  && (starting == 0  &&  stopping == 0  && renny_crashing == 0) &&
            renny_taking_off == 0  && renny_touching_down == 0  &&  renny_flying == 0  &&  fly_time > 2.0)
    {
        // set takeoff initial values  and start process
#ifdef FLYING_DEBUG
        printf("begin take off process\n");
#endif
        renny_taking_off = 1;
    }

    if (renny_taking_off == 1)
    {
        // the process
        if (renny_moving == 1)
        {
            /* stop if walking */
            // stop renny if he is walking
#ifdef FLYING_DEBUG
            printf("stop moving to take off\n");
#endif
            stop_renny(color, texture, walk_scale);
        }
        else if (renny_rocking == 1)
        {
#ifdef FLYING_DEBUG
            printf("stopped moving for take off\n");
            /* stop rocking if rocking */
#endif
            // stop renny's rocking
            ang_renny_idle = 90.0*(t_now-t_renny_idle);
            rot[0] = 30*Sin(ang_renny_idle);
            int rock_ang = fabs(rot[0]);
            if (renny_rocking ==1 &&  rock_ang <= 3)
            {
#ifdef FLYING_DEBUG
               printf("stopped rocking to take off\n");
#endif
               renny_rocking = 0;
               t_renny_takeoff = t_now;
               take_off_key_angle = fabs(fmod(key_angle, 180) - renny_fly_key_angle);
               takeoff_normal[0] = current_normal[0];
               takeoff_normal[1] = current_normal[1];
               takeoff_normal[2] = current_normal[2];
            }
            place_renny(pos, transl, rot, scaler, color, texture, walk_scale);
        }
        else if (renny_rocking == 0)
        {
            // start the takeoff
            double t_move = t_now - t_renny_takeoff;
            if (t_move < 2.5)
            {
                if (t_move < 0.5)
                {
                    /* extend open hand */
                    show_hand = 1;
                    wave_pos = 9.5*t_move;
                    /* move key angle to flight position */
                    key_angle = renny_fly_key_angle + (0.5 - t_move)*take_off_key_angle;
                }
                else if (t_move < 1.5)
                {
                    t_move -= 0.5;
                    /* new close hand */
                    show_hand = 2;
                    /* fade in balloon  */
                    show_balloon = 1;
                    balloon_fade = (t_move);
                    /* tilt up on toes */
                    rot[0] = 30*Sin(90.0*(t_move));
                }
                else
                {
                    float norm = 1;
                    t_move -= 1.5;
                    /* fly renny to altitude of 5 */
                    fly_time -= (t_now-t_last);
                    rot[0] = 30;
                    key_angle = renny_fly_key_angle;
                    renny_alt = 5.0*(6.0*(t_move*t_move*(0.5-t_move/3.0)));//t_move;
                    /* move to {0,1,0} normal at 5 units above terrain (min flight alt)*/
                    current_normal[0] = (1-t_move)*takeoff_normal[0];
                    current_normal[1] = (1-t_move)*takeoff_normal[1] + t_move;
                    current_normal[2] = (1-t_move)*takeoff_normal[2];
                    norm = sqrt(current_normal[0]*current_normal[0] + current_normal[1]*current_normal[1] +
                            current_normal[2]*current_normal[2]);
                    current_normal[0] /= norm;
                    current_normal[1] /= norm;
                    current_normal[2] /= norm;
                }
                place_renny(pos, transl, rot, scaler, color, texture, walk_scale);
            }
            else
            {
                mode = 7;
                fly_time -= (t_now-t_last);
                rot[0] = 30;
                key_angle = renny_fly_key_angle;
                renny_alt = 5.0;
                place_renny(pos, transl, rot, scaler, color, texture, walk_scale);
            }
        }
    }


}


/* pretty simple as no foot motion, just altitude, speed and direction */
void fly_renny(unsigned char color, char texture, double walk_scale[2])
{
    double pos[3] = {1.05,0,0};
    double transl[3] = {0,0,0};
    double rot[3] = {0,0,0};
    double scaler[3] = {1,1,1};
    double t_move = t_now - (t_renny_takeoff+2.5);
    double temp = (t_now-t_last);
    double flying_speed = rny_speed;

    float delta_elev = current_elev-last_elev;


    if (renny_taking_off == 1) {
        renny_taking_off = 0;
        renny_flying = 1;
    }

    fly_time -= (t_now-t_last);
    //ramping up Renny's airspeed
    if (t_move < 1.0)
    	flying_speed = rny_speed*t_move;

    temp *= flying_speed;

    // Renny can fly across the world in the X or Z direction and using 80% of his initial energy
    // (but flying faster increases air drag, linearly for now)
    energy -= (1.0 + 0.05*flying_speed)*temp/dist_energy;

    /* energy used to climb or generated going downhill (psuedo areodynamics)
     * Renny will use 20% of his initial energy to climb 500 units,
     * he gets 50% energy return used climbing when going downhill
     */
    if (delta_elev > 0)
        energy -= delta_elev/25.0;
    else
        energy -= delta_elev/50.0;

    /* as time runs out, force altitude down to surface */
    /* have to land Renny as time runs out */
    if (fly_time < 10.0)
    {
        show_warn = 1;
        warning = "Running out of fly time, descending.";
        renny_alt = (renny_alt-5)*fly_time/10.0 + 5.0;
    }
    
    rot[0] = 30.0;
    key_angle = renny_fly_key_angle;
    renny_pos[0]+=Cos(renny_rot[0])*temp;
    renny_pos[2]+=Sin(renny_rot[0])*temp;
    if (fly_time < 0)
    {
        fly_time = 0.0;
        mode = 8;
        show_warn = 1;
        warning = "Out of fly time, grounded for now.";
    }
    place_renny(pos, transl, rot, scaler, color, texture, walk_scale);
    
}


void land_renny(unsigned char color, char texture, double walk_scale[2])
{
    double pos[3] = {1.05,0,0};
    double transl[3] = {0,0,0};
    double rot[3] = {0,0,0};
    double scaler[3] = {1,1,1};
    /* land below 5 units of height */

    if (renny_flying == 1)
    {
        float normal[3]={0,1,0};

        /* forward motion reduces to zero */
        renny_flying = 0;
        renny_touching_down = 1;
        t_renny_touchdown = t_now;
        get_current_normal(normal);
        touchdown_normal[0] = normal[0];
        touchdown_normal[1] = normal[1];
        touchdown_normal[2] = normal[2];
    }
    
    if (renny_touching_down == 1)
    {
        float t_move = t_now - t_renny_touchdown;
        if (t_move < 1.0)
        {
            float norm = 1;
            /* fly renny from altitude of 5 to ground */
            rot[0] = 30;
            key_angle = renny_fly_key_angle;
            renny_alt = 5.0*(1.0-(6.0*(t_move*t_move*(0.5-t_move/3.0))));//t_move);

            /* move from {0,1,0} normal to interpolated surface normal */
            current_normal[0] = t_move*touchdown_normal[0];
            current_normal[1] = t_move*touchdown_normal[1] + (1-t_move);
            current_normal[2] = t_move*touchdown_normal[2];
            norm = sqrt(current_normal[0]*current_normal[0] + current_normal[1]*current_normal[1] +
                        current_normal[2]*current_normal[2]);
            current_normal[0] /= norm;
            current_normal[1] /= norm;
            current_normal[2] /= norm;

        }
        else if (t_move < 2.0)
        {
            /* land on toes, go to flat feet as open hand and release balloon */
            show_hand = 1;
            wave_pos = 4.75*(2.0-t_move);
            rot[0] = 30*Sin(45.0*(2.0-t_move));
            show_balloon = 2;
            /* move key angle to flight position */
            key_angle = renny_fly_key_angle - (2.0 - t_move)*take_off_key_angle;
        }
        else
        {
            /* rock on feet */
            ang_renny_idle = 0;
            t_renny_idle = t_now-2.0;
            show_balloon = 0;
            show_hand=0;
            renny_alt = 0;
            mode = 0;
            renny_rocking = 1;
            renny_touching_down = 0;
        }
        place_renny(pos, transl, rot, scaler, color, texture, walk_scale);        
    }
}


void crash_renny(unsigned char color, char texture, double walk_scale[2])
{
    double pos[3] = {1.05,0,0};
    double transl[3] = {0,0,0};
    double rot[3] = {0,0,0};
    double scaler[3] = {1,1,1};

    // two second crash, big bounce in first second, two bounces in next second
    if (renny_crashing == 0  &&  renny_waving == 0  &&  renny_flipping == 0 &&
            renny_taking_off == 0  && renny_touching_down == 0  && renny_flying == 0)
    {
        int i = 0;
        renny_crashing = 1;
        renny_moving = 0;
        t_renny_crash = t_now;
        for (i=0; i<3; i++)
            renny_crash[i]=renny_pos[i];
        renny_landing[0] = -3*Cos(renny_rot[0])+renny_pos[0];
        renny_landing[2] = -3*Sin(renny_rot[0])+renny_pos[2];
    }

    if (renny_crashing == 1)
    {
        double t_crash = t_now-t_renny_crash;
        renny_pos[0] = renny_crash[0] + 0.5*(t_crash)*(renny_landing[0]-renny_crash[0]);
        renny_pos[2] = renny_crash[2] + 0.5*(t_crash)*(renny_landing[2]-renny_crash[2]);
        if (t_crash < 1.0)
        {
            rot[0]=(int)(90*t_crash);
            rot[1]=(int)(-30*t_crash);
        }
        else
        {
            rot[0]=90;
            rot[1]=-30;
        }
        if (t_crash < 1.0)
        {
            renny_pos[1] = 4.0*(t_crash)*(1.0-t_crash);
            walk_scale[0] = walk_scale[1] = 1.0 - t_crash;
        }
        else if (t_crash < 1.5)
            renny_pos[1] = 3.0*(t_crash-1.0)*(1.5-t_crash);
        else if (t_crash < 2.0)
            renny_pos[1] = 2.0*(t_crash-1.5)*(2.0-t_crash);
        else if (t_crash < 3.0)
            renny_pos[1] = 0.0;
        else if (t_crash < 4.0)
        {
            rot[0]=(int)(90*(4.0-t_crash));
            rot[1]=(int)(-30*(4.0-t_crash));
        }
        else
        {
            rot[0]=0;
            renny_crashing = 0;
            t_renny_idle = glutGet(GLUT_ELAPSED_TIME)/1000.0;
            renny_rocking = 1;
            mode = 0;
            rny=90.0;
            energy -= 2.0;
        }
    }
    place_renny(pos, transl, rot, scaler, color, texture, walk_scale);
}

void flip_renny(unsigned char color, char texture, double walk_scale[2])
{
    double pos[3] = {1.05,0,0};
    double transl[3] = {0,0,0};
    double rot[3] = {0,0,0};
    double scaler[3] = {1,1,1};

    if (renny_out_of_gas == 1)
    {
        walk_renny(color, texture, walk_scale);
        show_warn = 1;
        warning = "OUT OF GAS!  GAME OVER!";
        return;
    }

    if (renny_flipping == 0  && (starting == 0  &&  stopping == 0  && renny_crashing == 0 && renny_waving == 0) &&
            renny_taking_off == 0  && renny_touching_down == 0  && renny_flying == 0)
    {
        // start flip process and save initial values
        renny_flipping = 1;
    }

    if (renny_flipping == 1)
    {
        if (renny_moving == 1)
        {
            // stop renny if he is walking
            stop_renny(color, texture, walk_scale);
        }
        else if (renny_rocking == 1)
        {
            // stop renny's rocking
            ang_renny_idle = 90.0*(t_now-t_renny_idle);
            rot[0] = 30*Sin(ang_renny_idle);
            int rock_ang = fabs(rot[0]);
            if (renny_rocking ==1 &&  rock_ang <= 3)
            {
               renny_rocking = 0;
               t_renny_flip = t_now;
            }
            place_renny(pos, transl, rot, scaler, color, texture, walk_scale);
        }
        else if (renny_rocking == 0)
        {
            double t_flip = t_now - t_renny_flip;
            if (t_now-t_renny_flip < 3.0)
            {
                if (t_flip < 1.0)
                {
                    // crouch to jump
                    rot[1]=30*t_flip;//(6.0*(t_flip*t_flip*(0.5-t_flip/3.0)));
                    flip_pos = 0;
                    flip_rot=0;
                }
                else if (t_flip < 3.0)
                {
                    renny_in_the_air = 1;
                    // the flip
                    t_flip-=1.0;
                    if (t_flip < 0.5)
                    {
                        // take off
                        flip_pos = 0;
                        flip_rot=0;
                        rot[1]=30*(1-t_flip);//6.0*(t_flip*t_flip*(0.5-t_flip/3.0)));
                    }
                    else if (t_flip < 1.5)
                    {
                        // in the air
                        flip_pos = 90*(t_flip-0.5)*(1.5-t_flip);
                        flip_rot=360*(t_flip-0.5);
                        if (t_flip < 0.25)
                            rot[0] = -60*t_flip;
                        else if (t_flip < 1.0)
                        {
                            //t_flip-=0.5;
                            rot[0] = -15;
                            rot[1]=30*(1-t_flip);//6.0*(t_flip*t_flip*(0.5-t_flip/3.0)));
                        }
                        else
                            rot[0]=-60*(1.25 - t_flip);
                    }
                    else
                    {
                        // landing
                        flip_pos = 0;
                        flip_rot=0;
                        t_flip -=1.5;
                        if (t_flip < 0.25)
                        {
                            rot[1]=45*t_flip;//(6.0*(t_flip*t_flip*(0.5-t_flip/3.0)));
                            rot[0]=-15*(4*t_flip-1);
                        }
                        else
                            rot[1]=45*(0.5-t_flip);//6.0*(t_flip*t_flip*(0.5-t_flip/3.0)));
                    }

                }
                place_renny(pos, transl, rot, scaler, color, texture, walk_scale);
                //if (renny_in_the_air == 0)
                    check_collision();
            }
            else if (t_now-t_renny_flip > 3.0)
            {
                renny_in_the_air = 0;
                renny_flipping = 0;
                start_renny(color, texture, walk_scale);
                mode = 2;
                energy -= 1.0;
            }
            else
                walk_renny(color, texture, walk_scale);
        }
    }
}


void wave_renny(unsigned char color, char texture, double walk_scale[2])
{
    double pos[3] = {1.05,0,0};
    double transl[3] = {0,0,0};
    double rot[3] = {0,0,0};
    double scaler[3] = {1,1,1};

    if (renny_out_of_gas == 1)
    {
        walk_renny(color, texture, walk_scale);
        show_warn = 1;
        warning = "OUT OF GAS!  GAME OVER!";
        return;
    }

    if (renny_waving == 0  && (starting == 0  &&  stopping == 0  && renny_crashing == 0) &&
            renny_taking_off == 0  && renny_touching_down == 0  && renny_flying == 0)
    {
        // set wave initial values  and start process
        renny_wave_bonus = 2;
        renny_waving = 1;
        if (th < -180)  th+=360;
        if (th > 180)  th-=360;
        renny_wave_rot[0] = th + 90;
        if (renny_wave_rot[0] > 180)
            renny_wave_rot[0] -=360;
        renny_wave_rot[1] = -ph;
        if (renny_wave_rot[1] < -45)
            renny_wave_rot[1] = -45;
        else if (renny_wave_rot[1] > 45)
            renny_wave_rot[1] = 45;

        wave_rot = 0.0;
    }

    if (renny_waving == 1)
    {
        // the process
        if (renny_moving == 1)
        {
            // stop renny if he is walking
            stop_renny(color, texture, walk_scale);
        }
        else if (renny_rocking == 1)
        {
            // stop renny's rocking
            ang_renny_idle = 90.0*(t_now-t_renny_idle);
            rot[0] = 30*Sin(ang_renny_idle);
            int rock_ang = fabs(rot[0]);
            if (renny_rocking ==1 &&  rock_ang <= 3)
            {
               renny_rocking = 0;
               t_renny_wave = t_now;
            }
            place_renny(pos, transl, rot, scaler, color, texture, walk_scale);
        }
        else if (renny_rocking == 0)
        {
            // do the waving
            double t_move = t_now - t_renny_wave;
            if (t_now-t_renny_wave < 4.0)
            {
                if (t_move < 1.0)
                {
                    // turn to wave
                	/* cubic smooth rotation */
                    rot[0]=renny_wave_rot[1]*(6.0*(t_move*t_move*(0.5-t_move/3.0)));
                    rot[1]=renny_wave_rot[0]*(6.0*(t_move*t_move*(0.5-t_move/3.0)));
                }
                else if (t_move < 3.0)
                {
                    // wave
                    show_hand = 1;
                    rot[0]=renny_wave_rot[1];
                    rot[1]=renny_wave_rot[0];
                    t_move-=1.0;
                    if (t_move < 0.5)
                    {
                        // move hand out
                        wave_pos = 9.0*t_move;
                        wave_rot=0;
                    }
                    else if (t_move < 1.5)
                    {
                        // wave the hand/arm
                        wave_pos = 4.5;
                        wave_rot=20*Sin(720*t_move);
                    }
                    else
                    {
                        // move hand back in
                        wave_pos = 4.5 -9.0*(t_move-1.5);
                        wave_rot=0;
                    }

                }
                else if (t_move < 4.0)
                {
                    // turn back to walk again
                    show_hand = 0;
                    t_move-=3.0;
                    rot[0]=renny_wave_rot[1]*(1-6.0*(t_move*t_move*(0.5-t_move/3.0)));
                    rot[1]=renny_wave_rot[0]*(1-6.0*(t_move*t_move*(0.5-t_move/3.0)));
                }
                place_renny(pos, transl, rot, scaler, color, texture, walk_scale);
            }
            else if (t_now-t_renny_wave > 4.0)
            {
                // start to walk again
                renny_waving = 0;
                start_renny(color, texture, walk_scale);
                mode = 2;
            }
            else
                walk_renny(color, texture, walk_scale);
        }
    }

}

void start_renny(unsigned char color, char texture, double walk_scale[2])
{
    if (renny_out_of_gas == 0)
    {
        if (renny_moving == 0  &&  renny_crashing == 0  &&  renny_waving == 0  && renny_flipping == 0 &&
                renny_taking_off == 0  && renny_touching_down == 0  && renny_flying == 0)
        {
            rny=rny_last=rny+90;
            renny_moving = 1;
            starting = 1;
        }
        if (starting == 1  &&  renny_rocking == 0)
        {
            double start_angle = fmod(rny,180);
            if (start_angle > 180) start_angle-=180;
            walk_scale[1] = walk_scale[0];
            walk_scale[0] = (start_angle)/90.0;
            if (walk_scale[0] >= 1.0)
            {
                starting = 0;
                walk_scale[0] = 1.0;
                walk_scale[1] = 1.0;
                mode = 0;
            }
        }
    }
    else
    {
        show_warn = 1;
        warning = "OUT OF GAS!  GAME OVER!";
    }

    walk_renny(color, texture, walk_scale);
}

void stop_renny(unsigned char color, char texture, double walk_scale[2])
{
     if (((rny >= 0  && rny <= 30) ||  (rny >= 180 && rny <=210))  &&  (renny_moving == 1) && (stopping == 0))
     {
          stopping = 1;
       }
     if (stopping == 1)
     {
         double stop_angle = fmod(rny,180);
         if (stop_angle > 180) stop_angle-=180;
         walk_scale[1] = walk_scale[0];
         walk_scale[0] = 1.0 - (stop_angle)/90.0;
         if (walk_scale[0] <= 0.0)
         {
             stopping = 0;
             walk_scale[0] = 0.0;
             walk_scale[1] = 0.0;
             renny_moving = 0;
             t_renny_idle = glutGet(GLUT_ELAPSED_TIME)/1000.0;
             renny_rocking = 1;
             if (renny_waving == 1)
                 mode = 4;
             else if (renny_flipping == 1)
                 mode = 5;
             else if (renny_taking_off == 1)
                 mode = 6;
             else
                 mode = 0;
         }
     }
     walk_renny(color, texture, walk_scale);
}

void walk_renny(unsigned char color, char texture, double walk_scale[2])
{
    double pos[3] = {1.05,0,0};
    double transl[3] = {0,0,0};
    double rot[3] = {0,0,0};
    double scaler[3] = {1,1,1};

    /* the spot light uses energy */
    if (renny_lighting == 1)
        energy -= 0.25*(t_now-t_last);

    if (renny_moving == 1  &&  renny_rocking == 0)
    {
        double temp = 0;
        float delta_elev = current_elev-last_elev;
        // need a last value of walk_scale when stopping or starting
        temp = fabs(walk_scale[0]*renny_walk[0]*Cos(rny) - walk_scale[1]*renny_walk[0]*Cos(rny_last));
        renny_pos[1]=walk_scale[0]*renny_walk[1]*Sin(rny);
        if (renny_pos[1]<0) renny_pos[1]*=-1.0;
        rot[0]=-walk_scale[0]*10*Cos(rny);
        double foot_rock = rot[0];
        if (foot_rock<0) foot_rock*=-1.0;
        renny_pos[1]+=Sin(foot_rock);

        renny_pos[0]+=Cos(renny_rot[0])*temp;
        renny_pos[2]+=Sin(renny_rot[0])*temp;

        // Renny can walk across the world in the X or Z direction and using 80% of his initial energy
        energy -= temp/dist_energy;

        /* energy used to climb or generated going downhill (very Toyota Prius)
         * Renny will use 20% of his initial energy to climb 500 units,
         * he gets 50% energy return used climbing when going downhill
         */
        if (delta_elev > 0)
            energy -= delta_elev/25.0;
        else
            energy -= delta_elev/50.0;

        if (energy < 0)
        {
            renny_out_of_gas = 1;
            mode = 1;
            show_warn = 1;
            warning = "OUT OF GAS!  GAME OVER!";
        }

        // keep Renny in confines of the world, (wrap around or crash?)
        if (renny_pos[0]<-x_offset) mode = 3;
        else if (renny_pos[0]>x_offset) mode = 3;
        else if (renny_pos[2]<-z_offset) mode = 3;
        else if (renny_pos[2]>z_offset) mode = 3;

        if (mode == 3) score-=5;
    }
    else if (renny_waving == 0)
    {
        // have renny rock up on toes, then back on heels, bored.  two sec. total move, one sec. each part
        // unless he is waving at the user
        ang_renny_idle = 90.0*(t_now-t_renny_idle);
        rot[0] = 30*Sin(ang_renny_idle);
        int rock_ang = abs((int)rot[0]);
        if (starting == 1 &&  renny_rocking ==1 &&  rock_ang <= 3)
        {
           renny_rocking = 0;
        }
    }
    place_renny(pos, transl, rot, scaler, color, texture, walk_scale);

    if (renny_moving == 1  ||  renny_rocking == 1)
        check_collision();
}

void check_collision()
{
    int i=0;
    double radius = 0;
    if (t_now > t_coll)
    {
        int num_objs_in_this_quad = 0;
        int * objs_in_this_quad;
        Object obj_in_this_quad;

        /* set time for next collision check */
        t_coll = t_now + 0.10;

        /* save to check if renny wave_bonus expired by moving to new quad */
        renny_last_quad[0] = renny_quad[1][0];
        renny_last_quad[1] = renny_quad[1][1];
        /* get current quad, the data generated here also used in DrawScene() in "final_project_mbb.c" */
        quad_from_current_pos(renny_quad[1]);
        /* and list of objects in it */
        objs_in_this_quad=quads[renny_quad[1][0]][renny_quad[1][1]].objs_in_quad;
        num_objs_in_this_quad = quads[renny_quad[1][0]][renny_quad[1][1]].num_objs;
        //printf("num_objs_in_this_quad = %d\n", num_objs_in_this_quad);

        /* check if renny wave_bonus expired by moving to new quad */
        if (renny_last_quad[0] != renny_quad[1][0]  ||  renny_last_quad[1] != renny_quad[1][1])
            renny_wave_bonus = 1;

        /* run through objects in quad to check for collisions */
        for (i=0; i<num_objs_in_this_quad; i++)
        {
            /* skip of object invisible (e.g. collected prize) */
            if (full_objects[objs_in_this_quad[i]].draw == 0 )  continue;

            /* make sure we are even in the general dimensions area, or forget it */
            obj_in_this_quad = full_objects[objs_in_this_quad[i]];
            //printf("i=%d: #%d, %s\n", i, objs_in_this_quad[i], shapes[obj_in_this_quad.type].name);
            radius = 1.8*+obj_in_this_quad.radius;
            if (fabs(renny_pos[0]-obj_in_this_quad.pos[0]) < radius)
            {
                if (fabs(renny_pos[2]-obj_in_this_quad.pos[2]) < radius)
                {
                    //printf("near collision\n");
                    /* okay, now get Euclidean */
                    /* currently only x/z, deal with y / [1] for martini glasses and flipping renny? */
                    double real_r_sqr = (renny_pos[0]-obj_in_this_quad.pos[0])*(renny_pos[0]-obj_in_this_quad.pos[0])
                            + (renny_pos[2]-obj_in_this_quad.pos[2])*(renny_pos[2]-obj_in_this_quad.pos[2]);
                    if (real_r_sqr < radius*radius)
                    {
                        //printf("collide!!\n");
                        /* okay, determine change in score, energy level, or flying time [flying TBD] */
                        if (obj_in_this_quad.alpha == 1)
                        {
                            /* good things are transluscent, alpha ==1 */
                            /* it's now 'gone from the game' */
                            full_objects[objs_in_this_quad[i]].draw = 0;
                            /* energy boost */
                            if (obj_in_this_quad.type == 6)
                            {
                                if (current_prize == -1)
                                    energy += ENERGY_BOOST;
                                full_objects[objs_in_this_quad[i]].draw = 1;
                                renny_prizing = 1;
                                current_prize= objs_in_this_quad[i];
                                t_renny_prize = t_now + 0.5;
                            }
                            /* flying time added if renny is in the air in a flip, or make sure it will still be in game */
                            else if (obj_in_this_quad.type == 5)
                            {
                                //printf("near martini glass\n");
                                if (renny_in_the_air==1)
                                {
                                    fly_time += FLY_BONUS;
                                    score+= get_scoring(obj_in_this_quad.type, 1);
                                }
                                else
                                    full_objects[objs_in_this_quad[i]].draw = 1;
                            }
                            else
                            {
                                score+= get_scoring(obj_in_this_quad.type, 1);
                                renny_flashing = 1;
                                prizes_collected += 1;
                                current_flash= objs_in_this_quad[i];
                                t_renny_flash = t_now + 0.5;
                            }
                        }
                        else
                        {
                            if (renny_in_the_air==0)
                            {
                                mode = 3;
                                renny_flipping=0;
                                score+= get_scoring(obj_in_this_quad.type, 0);
                                if (obj_in_this_quad.type==7)
                                	warning = "Dude.  That was a tree...";
                                else
                                    warning = "ouch...";
                                show_warn = 1;
                                //printf("Renny crashed into object #%d\n", objs_in_this_quad[i]);
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}

int get_scoring(int i, int good)
{
    int scoring = shapes[i].penalty/(int)renny_wave_bonus;
    if (good == 1)
        scoring = (int)renny_wave_bonus*shapes[i].reward;
    return scoring;
}



void place_renny(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color,
        char texture, double walk_scale[2])
{
    // the 'flying' normal
    float normal[3] ={0,1,0};

    if (renny_touching_down == 1 || renny_taking_off == 1)
    {
        /* special values stored in current_normal during takeoff and landing */
        normal[0]=current_normal[0];
        normal[1]=current_normal[1];
        normal[2]=current_normal[2];
    }
    else if (renny_flying != 1)
        get_current_normal(normal);

    renny_rot[1] = RADS_TO_DEGS*atan(normal[2]/normal[1]);
    renny_rot[2] = -RADS_TO_DEGS*atan(normal[0]/normal[1]);

    glPushMatrix();
      glScaled(glb_scale[0],glb_scale[1],glb_scale[2]);
      // these translations and rotations in proper order?
      glTranslated(renny_pos[0],renny_pos[1]+current_elev+renny_alt,renny_pos[2]);
      glRotated(renny_rot[1], 1,0,0);
      glRotated(renny_rot[2], 0,0,1);
      glRotated(-renny_rot[0], 0,1,0);
      glPushMatrix();
        if (flip_pos > 0)
            glTranslated(0,flip_pos,0);
        if (flip_rot > 0)
            glRotated(flip_rot, 0,0,1);
        renny(pos, transl, rot, scale, color, texture, walk_scale);
      glPopMatrix();
    glPopMatrix();
}

void renny_shadow()
{
    shadow_save = 0;
    place_renny(shadow_pos, shadow_transl, shadow_rot, shadow_scale, 255,
            shadow_texture, shadow_walk_scale);
    shadow_save = 1;
}

void renny(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color,
           char texture, double walk_scale[2])
{
    if (shadow_save == 1)
    {
        shadow_pos[0]=pos[0]; shadow_pos[1]=pos[1]; shadow_pos[2]=pos[2];
        shadow_transl[0] = transl[0]; shadow_transl[1] = transl[1]; shadow_transl[2] = transl[2];
        shadow_rot[0] = rot[0]; shadow_rot[1] = rot[1]; shadow_rot[2] = rot[2];
        shadow_scale[0] = scale[0]; shadow_scale[1] = scale[1]; shadow_scale[2] = scale[2];
        shadow_color = color;
        shadow_texture =  texture;
        shadow_walk_scale[0] = walk_scale[0]; shadow_walk_scale[1] = walk_scale[1];
    }

    drawing_renny = 1;
    // body
    glPushMatrix();
      double rise =  Sin(rot[0]);
      if (renny_crashing == 0)
      {
          if (rise<0) rise*=-1;
          glTranslated(0,3.5+rise,0);
      }
      else
      {
          double fall_back = rise;
          rise = Cos(rot[0]);
          glTranslated(-3.5*fall_back,2.0*rise+1.5,0);
      }
      if (renny_waving == 1)
      {
          glRotated(180-rot[1],0,1,0);
          if (renny_moving == 0  &&  renny_rocking == 0)
              glRotated(rot[0],0,0,1);
      }
      else if (renny_flipping == 1)
      {
          glRotated(180,0,1,0);
          if (renny_moving == 0 && renny_rocking == 0)
              glRotated(rot[1],0,0,1);
      }
      else
          glRotated(180,0,1,0);
      body(0,0,0, 2.5, color, texture, walk_scale[0]);
      if (show_hand == 1)
          hand(pos, transl, rot, scale, color, texture);
      else if (show_hand == 2)
          hand2(pos, transl, rot, scale, color, texture);

      if (show_balloon == 1)
          BirdsongBalloon(pos[0]+balloonoffset[0], pos[1]+balloonoffset[1], pos[2]+balloonoffset[2], scaleballoon, 1, -1);
      else if (show_balloon == 2)
      {
          float balloon_rise = (t_now -(t_renny_touchdown+1.0))*(t_now -(t_renny_touchdown+1.0));
          BirdsongBalloon(pos[0]+balloonoffset[0], pos[1]+balloonoffset[1]+23*balloon_rise,
                  pos[2]+balloonoffset[2], scaleballoon, 1, -1);
      }

    glPopMatrix();

    // feet and moving them
    glPushMatrix();
      glRotated(-90,0,1,0);
      if (renny_moving == 1  || renny_crashing == 1)
      {
          // walking
          pos[2]=walk_scale[0]*renny_walk[0]*Cos(rny); pos[1]=walk_scale[0]*renny_walk[1]*Sin(rny);
      }
      right_foot(pos, transl, rot, scale, color, texture);
      pos[2]*=-1; pos[1]*=-1;
      left_foot(pos, transl, rot, scale, color, texture);
    glPopMatrix();
    drawing_renny = 0;
}

void left_foot(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color, char texture)
{
    // odd, but required to 'mirror image and position' the left foot
    double temp = -scale[0];
    scale[0] = temp;
    temp = -pos[0];
    pos[0] = temp;
    if (renny_moving == 1  && renny_rocking==0)
    {
        temp = -rot[0];
        rot[0] = temp;
    }
    foot(pos, transl, rot, scale, color, texture);
}

void right_foot(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color, char texture)
{
    foot(pos, transl, rot, scale, color, texture);
}

void foot(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color, char texture)
{
    //  Set specular color to white
    float white[] = {1,1,1,1};
    if (color<2 && drawing_renny==0) emission = 100;
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    emission = 0;
    int heel_angle = 180;
    int toe_angle = 180;
    int part_theta = 360;
    int part_phi = 0;
    double scale_ankle[3] =  {0.5, 0.5, 0.5};


    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

    //  Save transformation
    glPushMatrix();
      //  Offset, scale and rotate
      glTranslated(pos[0],pos[1],pos[2]);
      glRotated(rot[2],0,0,1);
      if (renny_crashing == 1)
          glRotated(rot[0],1,0,0);
      glScaled(scale[0],scale[1],scale[2]);

      glPushMatrix();
        if (renny_rocking == 1 || renny_taking_off == 1 || renny_flying ==1 || renny_touching_down == 1)
        {
            double rise = Sin(rot[0]);
            if (rise<0) rise*=-1;
            glTranslated(0,rise,0);
        }
        else if (renny_crashing == 1)
        {
            double out = Sin(rot[1]);
            glTranslated(-out,0,0);
        }
        glPushMatrix();
          if (renny_rocking == 1   || renny_moving == 1  || renny_flipping == 1 ||
                  renny_taking_off == 1 || renny_flying == 1 || renny_touching_down == 1)
              glRotated(rot[0],1,0,0);
          else if (renny_crashing == 1)
              glRotated(rot[1],0,1,0);
          glPushMatrix();
            glScaled(1,.2,1);
            // main part of foot
            new_cube(0, 1, 0, 1,1,1, 0, 0, 1, 3, color, texture);
            // heel
            glPushMatrix();
              glTranslated(-.5,1,1);
              glRotated(90,1,0,0);
              glScaled(.5,.5,1);
              partial_cylinder(0,0,0, 1, 1, &heel_angle, 1, color, texture);
            glPopMatrix();
            // big toe
            glPushMatrix();
              glTranslated(-.6,1,-1);
              glRotated(-90,1,0,0);
              glScaled(.4,.5,1);
              partial_cylinder(0,0,0, 1, 1, &toe_angle, 1, color, texture);
            glPopMatrix();
            // three little toes
            // one
            glPushMatrix();
              glTranslated(0,1,-1);
              glRotated(-90,1,0,0);
              glScaled(.2,.4,1);
              partial_cylinder(0,0,0, 1, 1, &toe_angle, 1, color, texture);
            glPopMatrix();
            // two
            glPushMatrix();
              glTranslated(.4,1,-1);
              glRotated(-90,1,0,0);
              glScaled(.2,.4,1);
              partial_cylinder(0,0,0, 1, 1, &toe_angle, 1, color, texture);
            glPopMatrix();
            // three
            glPushMatrix();
              glTranslated(.8,1,-1);
              glRotated(-90,1,0,0);
              glScaled(.2,.4,1);
              partial_cylinder(0,0,0, 1, 1, &toe_angle, 1, color, texture);
            glPopMatrix();
          glPopMatrix();

          // ankle
          glPushMatrix();
            glTranslated(-.3,.4,0);
            glRotated(180,1,0,0);
            partial_sphere(0,0,0,scale_ankle, &part_theta, &part_phi, 2, color, texture);
          glPopMatrix();
        glPopMatrix();

        // leg
        glPushMatrix();
          double run = 0.5*Sin(rot[0]);
          if (renny_rocking == 1)
              glTranslated(0,0,run);
          new_cube(-0.1,1.85,0, .05,1.5,.3, 0, 1, 1, 0, color, texture);
          new_cube(-0.3,1.85,0, .2,1.5,.05, 0, 1, 1, 0, color, texture);
          new_cube(-0.5,1.85,0, .05,1.5,.3, 0, 1, 1, 0, color, texture);
        glPopMatrix();
      glPopMatrix();

    //  Undo transformations
    glPopMatrix();
}

/* open. waving hand */
void hand(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color, char texture)
{
    //  Set specular color to white
    float white[] = {1,1,1,1};
    if (color<2 && drawing_renny==0) emission = 100;
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    emission = 0;
    int heel_angle = 180;
    int finger_angle = 180;


    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

    //  Save transformation
    glPushMatrix();
      glRotated(wave_rot,1,0,0);
      glTranslated(pos[0]-1,pos[1]-.5,pos[2]+wave_pos);
      glRotated(180,1,0,0);
      glRotated(90,0,0,1);
      glScaled(.7,.14,.7);
      // wrist/arm
      new_cube(0,0,1, .25,.5,2.5, 0, 1, 1, 0, color, texture);
      glPushMatrix();
        glRotated(-1.25*wave_rot,0,1,0);
        // main part of hand
        new_cube(0,0,0, 1,1,1, 0, 1, 1, 0, color, texture);
        // heel of hand
        glPushMatrix();
          glTranslated(0,0,1);
          glRotated(90,1,0,0);
          glScaled(1,.5,1);
          partial_cylinder(0,0,0, 1, 1, &heel_angle, 1, color, texture);
        glPopMatrix();
        // thumb
        glPushMatrix();
          glTranslated(-1,0,.2);
          glRotated(45,0,1,0);
          glScaled(.4,1,.4);
          // base
          new_cube(0,0,0, 1,1,1, 0, -1, 1, 2, color, texture);
          // shaft
          new_cube(0,0,-2, 1,1,1, 0, 1, 1, 0, color, texture);
          // tip
          glRotated(-90,1,0,0);
          partial_cylinder(0,3,0, 1, 1, &finger_angle, 1, color, texture);
        glPopMatrix();
        // three fingers
        // one
        glPushMatrix();
          glTranslated(-.70,0,-1.66);
          glScaled(.3,1,.33);
          // shaft
          new_cube(0,0,0, 1,1,2, 0, 1, 1, 0, color, texture);
          //tip
          glRotated(-90,1,0,0);
          partial_cylinder(0,2,0, 1, 1, &finger_angle, 1, color, texture);
        glPopMatrix();
        // two
        glPushMatrix();
          glTranslated(0,0,-1.6);
          glScaled(.3,1,.3);
          // shaft
          new_cube(0,0,0, 1,1,2, 0, 1, 1, 0, color, texture);
          //tip
          glRotated(-90,1,0,0);
          partial_cylinder(0,2,0, 1, 1, &finger_angle, 1, color, texture);
        glPopMatrix();
        // three
        glPushMatrix();
          glTranslated(.70,0,-1.54);
          glScaled(.3,1,.27);
          // shaft
          new_cube(0,0,0, 1,1,2, 0, 1, 1, 0, color, texture);
          //tip
          glRotated(-90,1,0,0);
          partial_cylinder(0,2,0, 1, 1, &finger_angle, 1, color, texture);
        glPopMatrix();
      glPopMatrix();
    //  Undo transformations
    glPopMatrix();
}

/* closed. balloon holding hand */
void hand2(double pos[3], double transl[3], double rot[3], double scale[3], unsigned char color, char texture)
{
    //  Set specular color to white
    float white[] = {1,1,1,1};
    if (color<2 && drawing_renny==0) emission = 100;
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    emission = 0;
    int heel_angle = 180;
    int finger_angle = 180;


    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

    //  Save transformation
    glPushMatrix();
      glRotated(wave_rot,1,0,0);
      glTranslated(pos[0]-1,pos[1]-.5,pos[2]+wave_pos);
      glRotated(180,1,0,0);
      glRotated(90,0,0,1);
      glScaled(.7,.14,.7);
      // wrist/arm
      new_cube(0,0,1, .25,.5,2.5, 0, 1, 1, 0, color, texture);
      glPushMatrix();
        glRotated(-1.25*wave_rot,0,1,0);
        // main part of hand
        new_cube(0,0,0, 1,1,1, 0, 1, 1, 0, color, texture);
        // heel of hand
        glPushMatrix();
          glTranslated(0,0,1);
          glRotated(90,1,0,0);
          glScaled(1,.5,1);
          partial_cylinder(0,0,0, 1, 1, &heel_angle, 1, color, texture);
        glPopMatrix();
        // thumb
        glPushMatrix();
          glTranslated(-.75,1.0,0.38);
          glScaled(.25,5.7143,1.14286);
          glRotated(45,1,0,0);
          glRotated(-90,0,0,1);
          glScaled(.32,1,.32);
          // base
          new_cube(0,0,0, 1,1,1, 0, -1, 1, 2, color, texture);
          // shaft
          new_cube(0,0,-2, 1,1,1, 0, 1, 1, 0, color, texture);
          // tip
          glRotated(-90,1,0,0);
          partial_cylinder(0,3,0, 1, 1, &finger_angle, 1, color, texture);
        glPopMatrix();
        // three fingers
        // one
        glPushMatrix();
          glTranslated(-.70,0,-1.66);
          glScaled(.3,1,.33);
          // shaft
          new_cube(0,1,1.25, 1,2,.75, 0, 1, 1, 0, color, texture);
          //tip
          glRotated(90,1,0,0);
          partial_cylinder(0,2,-2, 1, 1, &finger_angle, 1, color, texture);
        glPopMatrix();
        // two
        glPushMatrix();
          glTranslated(0,0,-1.66);
          glScaled(.3,1,.33);
          // shaft
          new_cube(0,1,1.25, 1,2,.75, 0, 1, 1, 0, color, texture);
          //tip
          glRotated(90,1,0,0);
          partial_cylinder(0,2,-2, 1, 1, &finger_angle, 1, color, texture);
        glPopMatrix();
        // three
        glPushMatrix();
          glTranslated(.70,0,-1.66);
          glScaled(.3,1,.33);
          // shaft
          new_cube(0,1,1.25, 1,2,.75, 0, 1, 1, 0, color, texture);
          //tip
          glRotated(90,1,0,0);
          partial_cylinder(0,2,-2, 1, 1, &finger_angle, 1, color, texture);
        glPopMatrix();
      glPopMatrix();
    //  Undo transformations
    glPopMatrix();
}


/*
 * An improved version of Birdsong Teapot (better handle and spout)
 *     position (x, y, z)
 *     scale/ratio (r)
 *     surface color (color, based on FunkyCullahs() in cyl_shapes.c)
 *     surface texture (texture, can be disabled
 */
void body(double x, double y, double z, double r, unsigned char color, char texture, double walk_scale)
{
    //  Save transformation
    glPushMatrix();
    //  Offset and scale
    glTranslated(x,y,z);
    glScaled(r,r,r);
    int phi_min=-90;
    int phi_max=-30;
    int theta_min=0;
    int theta_max=360;
    int phi=0;
    int theta=360;
    // scale vectors
    double lid_hndl[3] = {0.1, 0.05, 0.1};
    double scale_spout[3] = {0.8, 0.8, 0.8};
    double torii1[3] = {1.0, 1.0, 1.0};
    double pipe_xc[3] = {0.4, 0.4};
    //double torii2[3] = {0.95, 0.95, 0.95};

    // draw short base cylinder
    glPushMatrix();
      glRotated(90,1,0,0);
      glTranslated(0,0,0.725);
      cylinder(0,0,0, 0.7, 0.025, 1, color, texture);
    glPopMatrix();

    // draw bottom partial torus
    partial_torus(0,0,0,torii1,0.7,0.7, &theta_min, &phi_min, &theta_max, &phi_max, 0, color, texture);
    // draw middle partial torus
    phi_min=-30; phi_max=45;
    glPushMatrix();
      glTranslated(0,-0.25,0);
      partial_torus(0,0,0,torii1,1.133,0.2, &theta_min, &phi_min, &theta_max, &phi_max, 0, color, texture);
    glPopMatrix();
    // upper part of body chopped cone
    glPushMatrix();
      glRotated(90,1,0,0);
      glTranslated(0,0,-0.2301);
      chopped_cone(0,0,0,1.2744, 0.3387, 0.4708, 0, color, texture);
    glPopMatrix();
    // top rim torus
    glPushMatrix();
      glTranslated(0,0.50,0);
      torus(0,0,0,torii1,.77,.05, color, texture);
    glPopMatrix();
    // main part of lid chopped cone
    glPushMatrix();
      glRotated(90,1,0,0);
      glTranslated(0,0,-0.65);
      chopped_cone(0,0,0,.8, .15, .069, 0, color, texture);
    glPopMatrix();
    // base of lid handle chopped cone
    glPushMatrix();
      glRotated(-90,1,0,0);
      glTranslated(0,0,0.80);
      glScaled(1.25,1.25,1);
      chopped_cone(0,0,0,.1, .08, .4, 0, color, texture);
    glPopMatrix();
    // top of lid handle partial (half) sphere
    glPushMatrix();
      glRotated(180,1,0,0);
      glTranslated(0,-.88,0);
      glScaled(1.25,1,1.25);
      partial_sphere(0,0,0,lid_hndl, &theta, &phi, 0, color, texture);
    glPopMatrix();
    // spout, patched tapered partial torii
    phi_min = 0; phi_max = 360;
    theta_min = -30; theta_max = 90;
    glPushMatrix();
      glTranslated(-1,.1,0);
      glRotated(-90,0,0,1);
      glRotated(90,1,0,0);
      glScaled(.6,.7,.6);
      taper_partial_torus(0,0,0,scale_spout,.9,.7, .35, &theta_min,&phi_min, &theta_max,&phi_max, 0, color, texture);
      glPushMatrix();
        glTranslated(0,0,1.44);
        glRotated(180,1,0,0);
        glRotated(-90,0,1,0);
        theta_min = 0;
        taper_partial_torus(0,0,0, scale_spout,.9,.35, .15, &theta_min,&phi_min, &theta_max,&phi_max, 0,  color, texture);
      glPopMatrix();
    glPopMatrix();
    // handle patched partial torii
    phi_min = 0; phi_max = 360;
    theta_min = 10; theta_max = 80;
    glPushMatrix();
      glTranslated(1.55,.25,0);
      glRotated(-90,1,0,0);
      glScaled(.95,2.5,1);
      partial_torus(0,0,0, torii1, .2, .04, &theta_min,&phi_min, &theta_max,&phi_max, 0, color, texture);
      theta_min = -120; theta_max = 10;
      glPushMatrix();
        glTranslated(-.6*Cos(10),0,-.6*Sin(10));
        partial_torus(0,0,0, torii1, .8, .04, &theta_min,&phi_min, &theta_max,&phi_max, 1, color, texture);
      glPopMatrix();
      theta_min = 80; theta_max = 130;
      glPushMatrix();
        glTranslated(-.8*Cos(80),0,-.8*Sin(80));
        partial_torus(0,0,0, torii1, 1.0, .04, &theta_min,&phi_min, &theta_max,&phi_max, 1, color, texture);
      glPopMatrix();
    glPopMatrix();
    // wind-up key
    glPushMatrix();
      if (renny_taking_off == 0  && renny_flying == 0 && renny_touching_down == 0)
          key_angle = walk_scale*rny;
      if (key_angle > 360.0) key_angle -= 360.0;
      glTranslated(0,-.2,-1.5);
      glRotated(key_angle,0,0,1);
      glScaled(.7,.7,.7);
      // origin of this object is connector at handle to shaft
      // shaft
      new_cube(0,0,.5, .03,.03,.5, 0, 1,1, 3, color, texture);
      // base of handle
      new_cube(0,0,-.4, .8,.03,.45, 0, .3,1, 0, color, texture);
      // connector
      cylinder(0,0,0, .10, .15, 3, color, texture);
      // top of handle
      glPushMatrix();
        glTranslated(.42,0,-1.);
        glRotated(-90,1,0,0);
        cyl_pipe(0,0,0, pipe_xc, .3,1.1, .032, color, texture);
      glPopMatrix();
      glPushMatrix();
        glTranslated(-.42,0,-1.);
        glRotated(-90,1,0,0);
        cyl_pipe(0,0,0, pipe_xc, .3,1.1, .032, color, texture);
      glPopMatrix();

    glPopMatrix();

    //  Undo transformations
    glPopMatrix();
}
