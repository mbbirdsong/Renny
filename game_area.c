/*
 * game_area.c
 *
 *  Created on: Nov 21, 2011
 *      Author: michaelb
 */

#include "CSCIx229.h"
#include "game_area.h"
#include <time.h>

//#define DEBUG_QUADS
//#define FOG_QUAD_PRINT


/*
typedef struct
{
    int type;
    char name[21];
    double radius;
    int reward;
    int penalty;
} ShapeData;
*/
const ShapeData shapes[8] = {
        {0, "Evil Teapot", 1.333, 0, -20 },
        {1, "Sugarbowl", 1.2, 20, -8 },
        {2, "Creamer", 1.2, 25, -10 },
        {3, "Wineglass", 1., 15, -6 },
        {4, "Coffee Mug", 1.0, 20, -8 },
        {5, "Martini Glass", 2.5, 15, -6 },
        {6, "Sugarcube", 1.2, 0., 0 },
        {7, "Pine Tree", 4, 0, -12}
};


/* at least four objects per quad, (w/ other bonus objs, but less than 1 per quad) */
Object full_objects[MAX_NUM_OBJS];
QuadData quads[NUM_X_QUADS][NUM_Z_QUADS];
int num_prizes = 0;
int prizes_collected = 0;

int num_trees = 0;

const float martini_alt = 15.0;
const float martini_rot = 10.0;

float martini_angle = 0;
float martini_angle_rate = 15;

float quad_dims[2] ={0,0};
float half_dims[2] ={0,0};

void generate_game_quads();
void populate_game_quad(int i, int j, unsigned int iseed);
void place_object(int i);

void generate_game_quads()
{
    int ii = 0, jj = 0, i=0; //j=0;
    int half_indexes[2] = {((int)X_QUAD_DIM)/2,((int)Z_QUAD_DIM)/2};
    int fog_quad[2] = {0,0};
    int fog_x_rng[2] = {0,0};
    int fog_z_rng[2] = {0,0};

    quad_dims[0] = ((float)X_QUAD_DIM)*dims[0]; quad_dims[1] = ((float)Z_QUAD_DIM)*dims[1];
    half_dims[0] = quad_dims[0]/2; half_dims[1] = quad_dims[1]/2;

    /* location and dimensions of game quads */
    for (ii=0; ii<NUM_X_QUADS; ii++)
    {
        for (jj=0; jj<NUM_Z_QUADS; jj++)
        {
            quads[ii][jj].index[0]= ii*X_QUAD_DIM + half_indexes[0];
            quads[ii][jj].index[1]= jj*Z_QUAD_DIM + half_indexes[1];
            quads[ii][jj].center[0]= ii*quad_dims[0] + half_dims[0]-x_offset;
            quads[ii][jj].center[1]= jj*quad_dims[1] + half_dims[1]-z_offset;
            quads[ii][jj].populated = 0;
            quads[ii][jj].i_range[0]=(quads[ii][jj].index[0]-half_indexes[0] < 0?0:quads[ii][jj].index[0]-half_indexes[0]);
            quads[ii][jj].i_range[1]=(quads[ii][jj].index[0]+half_indexes[0] > NUM_X_AREAS?NUM_X_AREAS:quads[ii][jj].index[0]+half_indexes[0]);
            quads[ii][jj].j_range[0]=(quads[ii][jj].index[1]-half_indexes[1] < 0?0:quads[ii][jj].index[1]-half_indexes[1]);
            quads[ii][jj].j_range[1]=(quads[ii][jj].index[1]+half_indexes[1] > NUM_Z_AREAS?NUM_Z_AREAS:quads[ii][jj].index[1]+half_indexes[1]);
            quads[ii][jj].x_range[0]=dims[0]*((float)quads[ii][jj].i_range[0])-x_offset;
            quads[ii][jj].x_range[1]=dims[0]*((float)quads[ii][jj].i_range[1]+1)-z_offset;
            quads[ii][jj].z_range[0]=dims[1]*((float)quads[ii][jj].j_range[0])-x_offset;
            quads[ii][jj].z_range[1]=dims[1]*((float)quads[ii][jj].j_range[1]+1)-z_offset;
            quads[ii][jj].in_fog=0;
            quads[ii][jj].num_objs=0;
            for (i=0; i<10; i++)
                quads[ii][jj].objs_in_quad[i]=0;
#ifdef DEBUG_QUADS
            printf("index: %d, %d; i_rng: %d, %d; j_rng: %d, %d  cntr: %.1f, %.1f;  x_rng: %.1f, %.1f;  y_rng: %.1f, %.1f;\n",
                    quads[ii][jj].index[0], quads[ii][jj].index[1], quads[ii][jj].i_range[0], quads[ii][jj].i_range[1],
                    quads[ii][jj].j_range[0], quads[ii][jj].j_range[1], quads[ii][jj].center[0], quads[ii][jj].center[1],
                    quads[ii][jj].x_range[0], quads[ii][jj].x_range[1], quads[ii][jj].z_range[0], quads[ii][jj].z_range[1]);
#endif /*DEBUG_QUADS*/
        }
    }
    /* determine quads in fog */
    quad_from_pos(fog_center_loc, fog_quad);
    fog_x_rng[0]=(fog_quad[0]-FOG_QUAD_SPAN<0?0:fog_quad[0]-FOG_QUAD_SPAN);
    fog_x_rng[1]=(fog_quad[0]+FOG_QUAD_SPAN+1>NUM_X_QUADS?NUM_X_QUADS:fog_quad[0]+FOG_QUAD_SPAN+1);
    fog_z_rng[0]=(fog_quad[1]-FOG_QUAD_SPAN<0?0:fog_quad[1]-FOG_QUAD_SPAN);
    fog_z_rng[1]=(fog_quad[1]+FOG_QUAD_SPAN+1>NUM_Z_QUADS?NUM_Z_QUADS:fog_quad[1]+FOG_QUAD_SPAN+1);
    for (ii=fog_x_rng[0]; ii<fog_x_rng[1]; ii++)
    {
        for (jj=fog_z_rng[0]; jj<fog_z_rng[1]; jj++)
        {
            quads[ii][jj].in_fog=1;
        }
    }
    fog_x_range[0]=quads[fog_x_rng[0]][fog_quad[1]].x_range[0];
    fog_x_range[1]=quads[fog_x_rng[1]-1][fog_quad[1]].x_range[1];
    fog_z_range[0]=quads[fog_quad[0]][fog_z_rng[0]].z_range[0];
    fog_z_range[1]=quads[fog_quad[0]][fog_z_rng[1]-1].z_range[1];

#ifdef FOG_QUAD_PRINT
    printf("fog range:   x_rng: %d, %d; z_rng: %d, %d;\n",
            fog_x_rng[0], fog_x_rng[1], fog_z_rng[0], fog_z_rng[1] );
    printf("fog quad data:  quad[%d][%d]; x_range: %.1f, %.1f; z_range: %.1f, %.1f;\n",
            fog_quad[0], fog_quad[1], fog_x_range[0], fog_x_range[1], fog_z_range[0], fog_z_range[1] );
#endif
}

/* for renny */
void quad_from_current_pos(int quad_coords[2])
{
    quad_coords[0] = (int)((renny_pos[0] + x_offset)/quad_dims[0]);
    quad_coords[1] = (int)((renny_pos[2] + z_offset)/quad_dims[1]);
}

void quad_from_pos(float coords[2],int quad_coords[2])
{
    quad_coords[0] = (int)((coords[0] + x_offset)/quad_dims[0]);
    quad_coords[1] = (int)((coords[1] + z_offset)/quad_dims[1]);
}


void populate_game()
{
    unsigned int iseed = (unsigned int)time(NULL);
    int i=0, j=0;
    int halfspan = NUM_X_QUADS/2;  // max is NUM_X_QUADS/2
    int i_begin = NUM_X_QUADS/2-halfspan, i_end = 1+halfspan+NUM_X_QUADS/2;
    int j_begin = NUM_Z_QUADS/2-halfspan, j_end = 1+halfspan+NUM_X_QUADS/2;

    /* set up quad data */
    generate_game_quads();

    /* generate most of object data */
    srand(iseed);
    for (i=i_begin; i<i_end; i++)
    {
        for (j=j_begin; j<j_end; j++)
        {
            unsigned int new_seed = (unsigned int)rand();
            populate_game_quad(i, j, new_seed);
        }
    }

    /* final elevation and roll/pitch data*/
    for (i=0; i<num_objects; i++)
    {
        if (full_objects[i].type==0)      /* evil teapot */
            tea_pot_pursuit(i);
        else if (full_objects[i].type==5) /* flying martini glass */
            martini_glass_fly(i);
        else                              /* all other non moving objects */
            place_object(i);
    }
#ifdef DEBUG_QUADS
    printf("num_objs = %d, num_cubes = %d, num_prizes = %d num_trees = %d\n", num_objects, num_cubes, num_prizes, num_trees);
#endif
}

void populate_game_quad(int i, int j, unsigned int iseed)
{
    int ii=0;
	unsigned char success = 1;
    srand (iseed);
    int obj = num_objects;
    int blockers = 0;
    int num_blockers = 2;
    float x_tree = 0.0, y_tree=0.0, z_tree = 0.0;

    /* create evil teapot data */
    /*
     * random initial values: angle for prelim circular motion, scale
     */
    full_objects[obj].type=0;
    full_objects[obj].rot[0]=((float)(rand()%360));
    full_objects[obj].rot[1]=full_objects[obj].rot[2]=0;
    full_objects[obj].pos[0]=.45*quad_dims[0]*Cos(full_objects[obj].rot[0]-90)+quads[i][j].center[0];
    full_objects[obj].pos[1]=0;
    full_objects[obj].pos[2]=.45*quad_dims[0]*Sin(full_objects[obj].rot[0]-90)+quads[i][j].center[1];
    full_objects[obj].scale[0]=full_objects[obj].scale[1]=
            full_objects[obj].scale[2]=5*(.05*((float)(rand()%11))+.75);
    full_objects[obj].radius=full_objects[obj].scale[0]*shapes[0].radius;
    full_objects[obj].draw=1;
    full_objects[obj].area[0]=(int)(full_objects[obj].pos[0]+ x_offset)/X_DIM;
    full_objects[obj].area[1]=(int)(full_objects[obj].pos[2]+ z_offset)/Z_DIM;
    full_objects[obj].quad[0]=i;
    full_objects[obj].quad[1]=j;
    full_objects[obj].alpha = 0;
    full_objects[obj].color = 11;
    full_objects[obj].texture = ((int)(rand()%8)+1);
    full_objects[obj].pursuit = 0;
    full_objects[obj].reward = shapes[0].reward;
    full_objects[obj].penalty = shapes[0].penalty;

    quads[i][j].objs_in_quad[quads[i][j].num_objs]=obj;
    quads[i][j].num_objs+=1;
    num_objects = obj +=1;

    /* create prize data */
    /*
     * random initial values: angle of x/z rot, scale, x/z location
     * and make sure it is created AWAY from Renny in initial QUAD
     */
    full_objects[obj].type=(rand()%4)+1;
    full_objects[obj].rot[0]=((float)(rand()%360));
    full_objects[obj].rot[1]=full_objects[obj].rot[2]=0;
    full_objects[obj].pos[0]=quad_dims[0]*.05*((float)(rand()%19)+1)+quads[i][j].x_range[0];
    full_objects[obj].pos[1]=0;
    full_objects[obj].pos[2]=quad_dims[1]*.05*((float)(rand()%19)+1)+quads[i][j].z_range[0];
    full_objects[obj].area[0]=(int)(full_objects[obj].pos[0]+ x_offset)/X_DIM;
    full_objects[obj].area[1]=(int)(full_objects[obj].pos[2]+ z_offset)/Z_DIM;
    /* make sure to avoid Renny's initial position */
    if (full_objects[obj].area[0] == i_cntr  &&  full_objects[obj].area[1] == j_cntr)
    {
        if (fabs(full_objects[obj].pos[0]-x_offset)<10  &&  fabs(full_objects[obj].pos[2]-z_offset)<10)
        {
            int move_x = rand()%3-1, move_z = rand()%3-1;
            if (move_x == 0  &&  move_z == 0)
                while (move_z == 0)
                    move_z = rand()%3-1;

            full_objects[obj].pos[0] += move_x*X_DIM;
            full_objects[obj].pos[2] += move_z*Z_DIM;
            full_objects[obj].area[0] += move_x;
            full_objects[obj].area[1] += move_z;
        }
    }
    full_objects[obj].scale[0]=full_objects[obj].scale[1]=
            full_objects[obj].scale[2]=2.5*(.05*((float)(rand()%11))+.75);
    full_objects[obj].radius=full_objects[obj].scale[0]*shapes[full_objects[obj].type].radius;
    full_objects[obj].quad[0]=i;
    full_objects[obj].quad[1]=j;
    full_objects[obj].draw=1;
    full_objects[obj].alpha = 1;
    full_objects[obj].color = rand()%2;
    full_objects[obj].texture = -1;
    full_objects[obj].pursuit = 0;
    full_objects[obj].reward = shapes[full_objects[obj].type].reward;
    full_objects[obj].penalty = shapes[full_objects[obj].type].penalty;

    num_prizes += 1;
    quads[i][j].objs_in_quad[quads[i][j].num_objs]=obj;
    quads[i][j].num_objs+=1;
    num_objects = obj +=1;


    /* create the two blocking objects data */
    /*
     * random initial values: angle of x/z rot, scale, x/z location which is
     * within 0.25 of x/z AREA dim of the prize, but at least 3 units away from the prize
     */
    for (blockers = 0; blockers<num_blockers; blockers +=1)
    {
        int prize = 1+blockers;
        full_objects[obj].type=(rand()%4)+1;
        full_objects[obj].rot[0]=((float)(rand()%360));
        full_objects[obj].rot[1]=full_objects[obj].rot[2]=0;
        full_objects[obj].pos[0]=0.025*X_DIM*((float)(rand()%21)-10);
        full_objects[obj].pos[1]=0;
        full_objects[obj].pos[2]=0.025*X_DIM*((float)(rand()%21)-10);
        /* make sure to avoid prize's position */
        if (fabs(full_objects[obj].pos[0])<10  &&
                fabs(full_objects[obj].pos[2])<10)
        {
            int move_x = rand()%2;
            if (move_x)
            {
                if (full_objects[obj].pos[0] < 0) full_objects[obj].pos[0] -= 10;
                else full_objects[obj].pos[0] += 10;
            }
            else
            {
                if (full_objects[obj].pos[2] < 0) full_objects[obj].pos[2] -= 10;
                else full_objects[obj].pos[2] += 10;
            }
        }
        /* make sure second blocker not on top of first */
        if (blockers == 1)
        {
            if (fabs(full_objects[obj].pos[0]-full_objects[obj-1].pos[0])<4  &&
                    fabs(full_objects[obj].pos[2]-full_objects[obj-1].pos[2])<4)
            {
                int move_x = rand()%2;
                if (move_x)
                {
                    if (full_objects[obj].pos[0] < 0) full_objects[obj].pos[0] -= 5;
                    else full_objects[obj].pos[0] += 5;
                }
                else
                {
                    if (full_objects[obj].pos[2] < 0) full_objects[obj].pos[2] -= 5;
                    else full_objects[obj].pos[2] += 5;
                }
            }

        }
        full_objects[obj].pos[0]+=full_objects[obj-prize].pos[0];
        full_objects[obj].pos[2]+=full_objects[obj-prize].pos[2];
        full_objects[obj].area[0]=(int)(full_objects[obj].pos[0]+ x_offset)/X_DIM;
        full_objects[obj].area[1]=(int)(full_objects[obj].pos[2]+ z_offset)/Z_DIM;
        full_objects[obj].scale[0]=full_objects[obj].scale[1]=
                full_objects[obj].scale[2]=3*(.05*((float)(rand()%11))+.75);
        full_objects[obj].radius=full_objects[obj].scale[0]*shapes[full_objects[obj].type].radius;
        full_objects[obj].draw=1;
        full_objects[obj].quad[0]=i;
        full_objects[obj].quad[1]=j;
        full_objects[obj].alpha = 0;
        full_objects[obj].color = rand()%9 + 2;
        full_objects[obj].texture = -1;
        full_objects[obj].pursuit = 0;
        full_objects[obj].reward = shapes[full_objects[obj].type].reward;
        full_objects[obj].penalty = shapes[full_objects[obj].type].penalty;

        quads[i][j].objs_in_quad[quads[i][j].num_objs]=obj;
        quads[i][j].num_objs+=1;
        num_objects = obj +=1;
    }

    for (ii=0; ii<4; ii++)
    {
    /* draw a few trees, if elev > 100 */
    x_tree=quad_dims[0]*.01*((float)(rand()%93)+4)+quads[i][j].x_range[0];
    z_tree=quad_dims[1]*.01*((float)(rand()%93)+4)+quads[i][j].z_range[0];
    y_tree=get_elevation(x_tree, z_tree);
    if (y_tree > 100)
    {
#ifdef DEBUG_QUADS
        printf("quad[%d][%d] has a tree\n", i, j);
#endif
        full_objects[obj].type=7;
        full_objects[obj].rot[0]=full_objects[obj].rot[1]=full_objects[obj].rot[2]=0;
        full_objects[obj].pos[0]=x_tree;
        full_objects[obj].pos[1]=y_tree;
        full_objects[obj].pos[2]=z_tree;
        full_objects[obj].area[0]=(int)(full_objects[obj].pos[0]+ x_offset)/X_DIM;
        full_objects[obj].area[1]=(int)(full_objects[obj].pos[2]+ z_offset)/Z_DIM;
        full_objects[obj].quad[0]=i;
        full_objects[obj].quad[1]=j;
        /* make sure to avoid Renny's initial position */
        if (full_objects[obj].area[0] == i_cntr  &&  full_objects[obj].area[1] == j_cntr)
        {
            if (fabs(full_objects[obj].pos[0]-x_offset)<15  &&  fabs(full_objects[obj].pos[2]-z_offset)<15)
            {
                int move_x = rand()%3-1, move_z = rand()%3-1;
                if (move_x == 0  &&  move_z == 0)
                    while (move_z == 0)
                        move_z = rand()%3-1;

                full_objects[obj].pos[0] += move_x*X_DIM;
                full_objects[obj].pos[2] += move_z*Z_DIM;
                full_objects[obj].area[0] += move_x;
                full_objects[obj].area[1] += move_z;
            }
        }
        full_objects[obj].scale[0]=full_objects[obj].scale[1]=
                full_objects[obj].scale[2]=.05*((float)(rand()%11))+.75;
        full_objects[obj].radius=full_objects[obj].scale[0]*shapes[full_objects[obj].type].radius;
        full_objects[obj].draw=1;
        full_objects[obj].alpha = 0;
        full_objects[obj].color = 255;
        full_objects[obj].texture = -1;
        full_objects[obj].pursuit = 0;
        full_objects[obj].reward = shapes[full_objects[obj].type].reward;
        full_objects[obj].penalty = shapes[full_objects[obj].type].penalty;

        quads[i][j].objs_in_quad[quads[i][j].num_objs]=obj;
        quads[i][j].num_objs+=1;
        num_trees += 1;
        num_objects = obj +=1;
    }
    }
    /* create sugar energy cube (if allowed, based on random init, 20% chance) */
    /*
     * random initial values: x/z location which is at least 3 units away from
     * each blocking object and the prize
     */
    if (rand()%5 == 0)
    {
#ifdef DEBUG_QUADS
        printf("have energy cube\n");
#endif
        full_objects[obj].type=6;
        full_objects[obj].rot[0]=((float)(rand()%360));
        full_objects[obj].rot[1]=full_objects[obj].rot[2]=0;
        full_objects[obj].pos[0]=quad_dims[0]*.05*((float)(rand()%19)+1)+quads[i][j].x_range[0];
        full_objects[obj].pos[1]=0;
        full_objects[obj].pos[2]=quad_dims[0]*.05*((float)(rand()%19)+1)+quads[i][j].z_range[0];
        full_objects[obj].area[0]=(int)(full_objects[obj].pos[0]+ x_offset)/X_DIM;
        full_objects[obj].area[1]=(int)(full_objects[obj].pos[2]+ z_offset)/Z_DIM;
        full_objects[obj].scale[0]=full_objects[obj].scale[1]=
                full_objects[obj].scale[2]=3;
        full_objects[obj].quad[0]=i;
        full_objects[obj].quad[1]=j;
        /* make sure to avoid Renny's initial position */
        if (full_objects[obj].area[0] == i_cntr  &&  full_objects[obj].area[1] == j_cntr)
        {
            if (fabs(full_objects[obj].pos[0]-x_offset)<15  &&  fabs(full_objects[obj].pos[2]-z_offset)<15)
            {
                int move_x = rand()%3-1, move_z = rand()%3-1;
                if (move_x == 0  &&  move_z == 0)
                    while (move_z == 0)
                        move_z = rand()%3-1;

                full_objects[obj].pos[0] += move_x*X_DIM;
                full_objects[obj].pos[2] += move_z*Z_DIM;
                full_objects[obj].area[0] += move_x;
                full_objects[obj].area[1] += move_z;
            }
        }
        full_objects[obj].scale[0]=full_objects[obj].scale[1]=
                full_objects[obj].scale[2]=2.0; //.05*((float)(rand()%11))+.75;
        full_objects[obj].radius=full_objects[obj].scale[0]*shapes[full_objects[obj].type].radius;
        full_objects[obj].draw=1;
        full_objects[obj].alpha = 1;
        full_objects[obj].color = 11;
        full_objects[obj].texture = -1;
        full_objects[obj].pursuit = 0;
        full_objects[obj].reward = shapes[full_objects[obj].type].reward;
        full_objects[obj].penalty = shapes[full_objects[obj].type].penalty;

        quads[i][j].objs_in_quad[quads[i][j].num_objs]=obj;
        quads[i][j].num_objs+=1;
        num_cubes += 1;
        num_objects = obj +=1;
    }


    /* create flying martini glasses (if allowed, based on random init, 40% chance) */
    /* centered over the quad     */
    if (rand()%5 < 2)
    {
#ifdef DEBUG_QUADS
        printf("have glass\n");
#endif
        full_objects[obj].type=5;
        full_objects[obj].rot[0]=((float)(rand()%360));
        full_objects[obj].rot[1]=((float)(rand()%180));
        full_objects[obj].rot[2]=((float)(rand()%180));
        full_objects[obj].pos[0]=quads[i][j].center[0];
        full_objects[obj].pos[1]=martini_alt;
        full_objects[obj].pos[2]=quads[i][j].center[1];
        full_objects[obj].area[0]=(int)(full_objects[obj].pos[0]+ x_offset)/X_DIM;
        full_objects[obj].area[1]=(int)(full_objects[obj].pos[2]+ z_offset)/Z_DIM;
        full_objects[obj].quad[0]=i;
        full_objects[obj].quad[1]=j;
        full_objects[obj].scale[0]=full_objects[obj].scale[1]=
                full_objects[obj].scale[2]=2.5*(.05*((float)(rand()%11))+.75);
        full_objects[obj].radius=full_objects[obj].scale[0]*shapes[full_objects[obj].type].radius;
        full_objects[obj].draw=1;
        full_objects[obj].alpha = 1;
        full_objects[obj].color = rand()%2;
        full_objects[obj].texture = -1;
        full_objects[obj].pursuit = 0;
        full_objects[obj].reward = shapes[5].reward;
        full_objects[obj].penalty = shapes[5].penalty;

        quads[i][j].objs_in_quad[quads[i][j].num_objs]=obj;
        quads[i][j].num_objs+=1;
        num_objects = obj +=1;

    }


#ifdef DEBUG_QUADS
    for (i=0; i<num_objects; i+=1)
    {
        printf("%d:  %d, %.1f,%.1f,%.1f, %.1f,%.1f,%.1f, %.1f,%.1f,%.1f, %d,%d, %.1f, %d, %d, %d, %d, %d, %d, %d\n",
        i, full_objects[i].type, full_objects[i].pos[0], full_objects[i].pos[1], full_objects[i].pos[2],
        full_objects[i].rot[0], full_objects[i].rot[1], full_objects[i].rot[2], full_objects[i].scale[0], full_objects[i].scale[1], full_objects[i].scale[2],
        full_objects[i].area[0], full_objects[i].area[1], full_objects[i].radius,full_objects[i].draw, full_objects[i].alpha,
        full_objects[i].color, full_objects[i].texture, full_objects[i].pursuit, full_objects[i].reward, full_objects[i].penalty);

    }
#endif
    /* check that all is good on quad */
    if (success == 1)
        quads[i][j].populated = 1;
}

void place_object(int i)
{
    float obj_normal[3] = {0,1,0};
    /* set object's (at least initial) elevation */
    if (full_objects[i].type !=7 )
       get_normal(full_objects[i].area[0], full_objects[i].area[1], obj_normal);
    full_objects[i].rot[1] = RADS_TO_DEGS*atan(obj_normal[2]/obj_normal[1]);
    full_objects[i].rot[2] = -RADS_TO_DEGS*atan(obj_normal[0]/obj_normal[1]);

    /* set object's (at least initial) pitch and roll on the terrain */
    full_objects[i].pos[1] = get_elevation(full_objects[i].pos[0], full_objects[i].pos[2]);
}

void tea_pot_pursuit(int i)
{
    /* logic to move and place teapot */
    /* pursuit = 0:  prelim circling
     * pursuit = 1:  chasing renny  (backing up id renny is crashing)
     * pusuit = 2:  rotating slowly in the last place it was when renny left quad
     *
     * (they spin faster in celebration when renny is crashing)
     */
    if (full_objects[i].pursuit == 0)
    {
        full_objects[i].rot[0]-=30*(t_now-t_last);
        if (full_objects[i].rot[0]>360) full_objects[i].rot[0] -=360;
        full_objects[i].pos[0]=.45*quad_dims[0]*Cos(full_objects[i].rot[0]-90)+
                quads[full_objects[i].quad[0]][full_objects[i].quad[1]].center[0];
        full_objects[i].pos[2]=.45*quad_dims[0]*Sin(full_objects[i].rot[0]-90)+
                quads[full_objects[i].quad[0]][full_objects[i].quad[1]].center[1];
        full_objects[i].area[0]=(int)(full_objects[i].pos[0]+ x_offset)/X_DIM;
        full_objects[i].area[1]=(int)(full_objects[i].pos[2]+ z_offset)/Z_DIM;
        place_object(i);
    }
    else if (full_objects[i].pursuit == 2)
    {
        full_objects[i].rot[0]+=30*(t_now-t_last);
        if (full_objects[i].rot[0]>360) full_objects[i].rot[0] -=360;

    }

}

void martini_glass_fly(int i)
{
    if (full_objects[i].draw == 1)
    {
        /* move along the terrain flying above it at a set altitude */
        double Rad1 = 50+49.*Cos(2*martini_angle);
        double Rad2 = 50.0-49.*Cos(2*martini_angle);
        //float x_z_pos[2]={0,0};
        full_objects[i].pos[0] = Rad2*Cos(martini_angle) + quads[full_objects[i].quad[0]][full_objects[i].quad[1]].center[0];
        full_objects[i].pos[2] = -Rad1*Cos(martini_angle) + quads[full_objects[i].quad[0]][full_objects[i].quad[1]].center[1];
        full_objects[i].pos[1] = get_elevation(full_objects[i].pos[0], full_objects[i].pos[2]) + martini_alt;
        full_objects[i].rot[2] += 13*martini_angle_rate*(t_now-t_last);
        full_objects[i].rot[1] = 2*full_objects[i].rot[2];
        if (full_objects[i].rot[0]>360) full_objects[i].rot[0] -=360;
        if (full_objects[i].rot[2]>360) full_objects[i].rot[2] -=360;
        full_objects[i].area[0]=(int)(full_objects[i].pos[0]+ x_offset)/X_DIM;
        full_objects[i].area[1]=(int)(full_objects[i].pos[2]+ z_offset)/Z_DIM;
        /* needed for collision checks since it could be out of its quad of origin
         * (not quite right, should leave in its own quad, but check collision if it is being drawn and
         * a martini glass)
         */
        // x_z_pos[0]=full_objects[i].pos[0]; x_z_pos[0]=full_objects[i].pos[2];
        //quad_from_pos(x_z_pos,full_objects[i].quad);
    }
}

void energy_cube_spin(int i)
{
    if (full_objects[i].draw == 1)
    {
        full_objects[i].rot[0]+=216*(t_now-t_last);  //216 = 2*108  The Hatch ("The Swan") was DHARMA Initiative Station Two
        if (full_objects[i].rot[0]>360) full_objects[i].rot[0] -=360;
    }
}
