/*
 * terrain.c
 *
 *  Created on: Nov 15, 2011
 *      Author: michaelb
 */

#include "CSCIx229.h"
#include "final_project_mbb.h"
#include "terrain.h"
#include "renny.h"

typedef struct
{
    double center[2];
    double x_range[2];
    double z_range[2];
    int ridge_dir;
    float Es[2][3];
    float Ns[2][3];
} AreaData;

int print_kount = 0;
int print_norm = 0;

float dims[2] = {X_DIM,Z_DIM};
int i_dim=NUM_X_AREAS+1, j_dim=NUM_Z_AREAS+1;
int i_cntr = (int)((NUM_X_AREAS+1)/2);
int j_cntr = (int)((NUM_Z_AREAS+1)/2);

int side_height = NUM_Z_AREAS*Z_DIM/2;
float x_offset =((float)(X_DIM*NUM_X_AREAS))/2.0;
float z_offset =((float)(Z_DIM*NUM_Z_AREAS))/2.0;
float max_x_length = ((float)(Z_DIM*NUM_X_AREAS));
float max_z_length = ((float)(Z_DIM*NUM_Z_AREAS));
float dist_energy = 0;
float vrtx_els[NUM_X_AREAS+1][NUM_Z_AREAS+1];
float vrtx_clrs[NUM_X_AREAS+1][NUM_Z_AREAS+1][3];
float srfc_els[NUM_X_AREAS][NUM_Z_AREAS];
float srfc_norms[NUM_X_AREAS+1][NUM_Z_AREAS+1][3];
float vrtx_norms[NUM_X_AREAS+1][NUM_Z_AREAS+1][3];
AreaData area_data[NUM_X_AREAS][NUM_Z_AREAS];
float current_elev = 0, last_elev = 0;

float peak_dims[NUM_HILLS][3];
int peak_locs_int[NUM_HILLS][2];
float peak_locs_dbl[NUM_HILLS][2];

float fog_x_range[2]={0,0};
int fog_center_min_el[2]={0,0};
float fog_z_range[2]={0,0};
float fog_center_loc[2]={0.0, 0.0};
int max_el_x_z[2]={0,0};
int dark_center[2]={0,0};
float dark_center_loc[2] = {0,0};
float dark_center_range[2] = {X_DIM*X_DARK_RANGE,Z_DIM*Z_DARK_RANGE};
float dark_range_sq = (X_DIM*X_DARK_RANGE)*(X_DIM*X_DARK_RANGE);
float dark_range =X_DIM*X_DARK_RANGE;
float dark_center_inner[2] = {X_DIM*X_TOTAL_DARK,Z_DIM*Z_TOTAL_DARK};
float dark_inner_sq = (X_DIM*X_TOTAL_DARK)*(X_DIM*X_TOTAL_DARK);
float dark_inner =X_DIM*X_TOTAL_DARK;
float min_el = 42.0;
float max_el = 0.0;

float fog_mid_el = 0.0;
float fog_max_el = 0.0;

/*
 * generate terrain random values
 * (NUM_HILLS major hills of varying height, width and length)
 */
void terrain_init()
{
   unsigned int iseed = (unsigned int)time(NULL);
   srand (iseed);
   int i=0, j=0, ii=0, jj=0;
   float sq_normalize =0.0, normalize = 0.0;
   unsigned char bump_the_origin = 0;

   // Renny can walk across the world in the X or Z direction and using 80% of his initial energy
   dist_energy = 0.0125*max_x_length;
   if (max_x_length <= max_z_length)
       dist_energy = 0.0125*max_z_length;

   /* get hill factors */
   for (i=0; i<NUM_HILLS; i+=1)
   {
       int offset[2] = {2,2};
       int range = NUM_X_AREAS-4;

       /* peaks physical dimensions */
       /* 60 <= peak_height <= 180 */
       peak_dims[i][0] = 20.0*((float)(rand()%5))+50;
       /* 100 <= peak_x dim and peak_x dim <= 300,  10 unit increments*/
       peak_dims[i][1] = 10.0*((float)(rand()%21))+100;
       peak_dims[i][2] = 10.0*((float)(rand()%21))+100;

       /* make sure hills are spread around */
       if (i%5 != 0)
       {
           ii = i%5;
           range = i_cntr-5;
           if (ii%2 == 1)
               offset[0] += i_cntr;
           if (ii%4 == 0  ||  (ii+1)%4 == 0)
               offset[1] += i_cntr;
       }
#ifdef MIN_DEBUG_PRINT_TERRAIN
       printf("%d: %d %d %d\n", i, range, offset[0], offset[1]);
#endif
       /* peaks physical locations w.r.t. x,z vertices*/
       for (j=0; j<2; j+=1)
       {
           peak_locs_int[i][j] = (rand()%range)+offset[j];
           peak_locs_dbl[i][j] = dims[j]*peak_locs_int[i][j];
       }
   }
#ifdef DEBUG_TERRAIN
   peak_dims[0][0] = 150;
   peak_dims[0][1] = 150;
   peak_dims[0][2] = 150;
   peak_locs_int[0][0] = X_0;
   peak_locs_int[0][1] = Z_0;
   peak_locs_dbl[0][0] = dims[0]*peak_locs_int[0][0];
   peak_locs_dbl[0][1] = dims[1]*peak_locs_int[0][1];
   peak_dims[1][0] = 250;
   peak_dims[1][1] = 150;
   peak_dims[1][2] = 150;
   peak_locs_int[1][0] = X_1;
   peak_locs_int[1][1] = Z_1;
   peak_locs_dbl[1][0] = dims[0]*peak_locs_int[1][0];
   peak_locs_dbl[1][1] = dims[1]*peak_locs_int[1][1];
#endif
   /* current mid terrain hack at the time, pt. 1*/
   for (i=0; i<NUM_HILLS; i+=1)
   {
       int i_diff = peak_locs_int[i][0] - i_cntr;
       int j_diff = peak_locs_int[i][1] - j_cntr;
       if (i_diff < 0) i_diff*=-1;
       if (j_diff < 0) j_diff*=-1;
       peak_dims[i][0] *= 1.0;
//       peak_dims[i][0] *= sqrt((float)(j_diff*j_diff + i_diff*i_diff))/(float)i_cntr;
   }

   /* generate vertex elevations */
   for (ii=0; ii<i_dim; ii+=1)
   {
       for (jj=0; jj<j_dim; jj+=1)
       {
		   float elev_decay = 0.0;
           float loc[2]={ii,jj};
		   float cntr[2]={i_cntr,j_cntr};
           vrtx_els[ii][jj] = 0.0;
           for (i=0; i<NUM_HILLS; i+=1)
           {
               elev_decay = 0.0;
               for (j=0; j<2; j+=1)
                   elev_decay += pow(dims[j]*(peak_locs_int[i][j]-loc[j])/peak_dims[i][j+1],2);
               vrtx_els[ii][jj] += peak_dims[i][0]*exp(-(elev_decay));
           }

		   /* make sure the origin will be 'above the fog' */
		   /* */
		   elev_decay = 0.0;
		   for (j=0; j<2; j+=1)
			   elev_decay += pow(dims[j]*(cntr[j]-loc[j])/100.0,2);
		   
		   vrtx_els[ii][jj] += 15.0*exp(-(elev_decay));
		   /* */
       }
   }

   /* generate vertex colors white to forest green 10 to 74 elevation */
   for (ii=0; ii<i_dim; ii+=1)
   {
       for (jj=0; jj<j_dim; jj+=1)
       {
         if ( vrtx_els[ii][jj] < 10)
         {
             vrtx_clrs[ii][jj][0]= 1;
             vrtx_clrs[ii][jj][1] = 1;
             vrtx_clrs[ii][jj][2]=1;
         }
         else if ( vrtx_els[ii][jj]>74)
         {
             vrtx_clrs[ii][jj][0] = 0.1333;
             vrtx_clrs[ii][jj][2]=0.1333;
             vrtx_clrs[ii][jj][1]= 0.5451;
         }
         else
         {
             vrtx_clrs[ii][jj][0] = 1.1354-0.0135*vrtx_els[ii][jj];
             vrtx_clrs[ii][jj][2] = 1.1354-0.0135*vrtx_els[ii][jj];
             vrtx_clrs[ii][jj][1] = 1.0711-0.0071*vrtx_els[ii][jj];
         }
       }
   }

   /* current mid terrain hack for now, pt. 2*/
//   vrtx_els[i_cntr][j_cntr] = vrtx_els[i_cntr-1][j_cntr] =vrtx_els[i_cntr][j_cntr-1] =vrtx_els[i_cntr-1][j_cntr-1] = 0.0;

	/* generate surface midpoint elevations */
	for (ii=0; ii<NUM_X_AREAS; ii+=1)
		for (jj=0; jj<NUM_Z_AREAS; jj+=1)
			srfc_els[ii][jj] = 0.25 *(vrtx_els[ii][jj] + vrtx_els[ii+1][jj] + vrtx_els[ii][jj+1] + vrtx_els[ii+1][jj+1]);

	/* get man and min els, away from edges */
	for (ii=X_EDGE; ii<NUM_X_AREAS-X_EDGE; ii+=1)
	{
		for (jj=Z_EDGE; jj<NUM_Z_AREAS-Z_EDGE; jj+=1)
		{
			/* calcs for fog */
			if (srfc_els[ii][jj]<=min_el)
			{
				min_el=srfc_els[ii][jj];
				fog_center_min_el[0]=ii;
				fog_center_min_el[1]=jj;
			}
			else if (srfc_els[ii][jj]>=max_el)
			{
				/* not used, yet */
				max_el=srfc_els[ii][jj];
				max_el_x_z[0]=ii;
				max_el_x_z[1]=jj;
			}
		}	
	}
	
   /* fill area data values, used both to view terrain and calc elevation */
   for (ii=0; ii<NUM_X_AREAS; ii+=1)
   {
       for (jj=0; jj<NUM_Z_AREAS; jj+=1)
       {
           area_data[ii][jj].x_range[0]=ii*dims[0]-x_offset;
           area_data[ii][jj].z_range[0]=jj*dims[1]-z_offset;
           area_data[ii][jj].center[0]=area_data[ii][jj].x_range[0] + dims[0]/2;
           area_data[ii][jj].center[1]=area_data[ii][jj].z_range[0] + dims[1]/2;
           area_data[ii][jj].x_range[1]=area_data[ii][jj].x_range[0]+dims[0];
           area_data[ii][jj].z_range[1]=area_data[ii][jj].z_range[0]+dims[1];
           if (vrtx_els[ii][jj]+vrtx_els[ii+1][jj+1] > vrtx_els[ii+1][jj]+vrtx_els[ii][jj+1])
               area_data[ii][jj].ridge_dir =1; /* ridge along z=x */
           else
               area_data[ii][jj].ridge_dir =-1; /* ridge along z=-x */

           if (area_data[ii][jj].ridge_dir == 1)
           {
               /* in this 'upper' or NW triangle */
               area_data[ii][jj].Es[0][0]=area_data[ii][jj].x_range[1];
               area_data[ii][jj].Es[0][1]=vrtx_els[ii+1][jj];
               area_data[ii][jj].Es[0][2]=area_data[ii][jj].z_range[0];
               area_data[ii][jj].Ns[0][0]=-dims[1]*(vrtx_els[ii][jj]-vrtx_els[ii+1][jj]);
               area_data[ii][jj].Ns[0][1]=-dims[0]*dims[1];
               area_data[ii][jj].Ns[0][2]=-dims[0]*(vrtx_els[ii+1][jj]-vrtx_els[ii+1][jj+1]);
                  /* normalize */
               normalize = sqrt(area_data[ii][jj].Ns[0][0]*area_data[ii][jj].Ns[0][0] +
                                area_data[ii][jj].Ns[0][1]*area_data[ii][jj].Ns[0][1] +
                                area_data[ii][jj].Ns[0][2]*area_data[ii][jj].Ns[0][2]);
               area_data[ii][jj].Ns[0][0] /= normalize;
               area_data[ii][jj].Ns[0][1] /= normalize;
               area_data[ii][jj].Ns[0][2] /= normalize;


               /* in this 'lower' or SE triangle */
               area_data[ii][jj].Es[1][0]=area_data[ii][jj].x_range[0];
               area_data[ii][jj].Es[1][1]=vrtx_els[ii][jj+1];
               area_data[ii][jj].Es[1][2]=area_data[ii][jj].z_range[1];
               area_data[ii][jj].Ns[1][0]=dims[1]*(vrtx_els[ii+1][jj+1]-vrtx_els[ii][jj+1]);
               area_data[ii][jj].Ns[1][1]=-dims[0]*dims[1];
               area_data[ii][jj].Ns[1][2]=dims[0]*(vrtx_els[ii][jj+1]-vrtx_els[ii][jj]);
                  /* normalize */
               normalize = sqrt(area_data[ii][jj].Ns[1][0]*area_data[ii][jj].Ns[1][0] +
                                area_data[ii][jj].Ns[1][1]*area_data[ii][jj].Ns[1][1] +
                                area_data[ii][jj].Ns[1][2]*area_data[ii][jj].Ns[1][2]);
               area_data[ii][jj].Ns[1][0] /= normalize;
               area_data[ii][jj].Ns[1][1] /= normalize;
               area_data[ii][jj].Ns[1][2] /= normalize;
           }
           else if (area_data[ii][jj].ridge_dir == -1)
           {
               /* in this 'upper' or NE triangle */
               area_data[ii][jj].Es[0][0]=area_data[ii][jj].x_range[1];
               area_data[ii][jj].Es[0][1]=vrtx_els[ii+1][jj+1];
               area_data[ii][jj].Es[0][2]=area_data[ii][jj].z_range[1];
               area_data[ii][jj].Ns[0][0]=dims[1]*(vrtx_els[ii+1][jj+1]-vrtx_els[ii][jj+1]);
                area_data[ii][jj].Ns[0][1]=-dims[0]*dims[1];
               area_data[ii][jj].Ns[0][2]=-dims[0]*(vrtx_els[ii+1][jj]-vrtx_els[ii+1][jj+1]);
                  /* normalize */
               normalize = sqrt(area_data[ii][jj].Ns[0][0]*area_data[ii][jj].Ns[0][0] +
                                area_data[ii][jj].Ns[0][1]*area_data[ii][jj].Ns[0][1] +
                                area_data[ii][jj].Ns[0][2]*area_data[ii][jj].Ns[0][2]);
               area_data[ii][jj].Ns[0][0] /= normalize;
               area_data[ii][jj].Ns[0][1] /= normalize;
               area_data[ii][jj].Ns[0][2] /= normalize;

               /* in this 'lower' or SW triangle */
               area_data[ii][jj].Es[1][0]=area_data[ii][jj].x_range[0];
               area_data[ii][jj].Es[1][1]=vrtx_els[ii][jj];
               area_data[ii][jj].Es[1][2]=area_data[ii][jj].z_range[0];
               area_data[ii][jj].Ns[1][0]=-dims[1]*(vrtx_els[ii][jj]-vrtx_els[ii+1][jj]);
                area_data[ii][jj].Ns[1][1]=-dims[0]*dims[1];
               area_data[ii][jj].Ns[1][2]=dims[0]*(vrtx_els[ii][jj+1]-vrtx_els[ii][jj]);
                  /* normalize */
               normalize = sqrt(area_data[ii][jj].Ns[1][0]*area_data[ii][jj].Ns[1][0] +
                                area_data[ii][jj].Ns[1][1]*area_data[ii][jj].Ns[1][1] +
                                area_data[ii][jj].Ns[1][2]*area_data[ii][jj].Ns[1][2]);
               area_data[ii][jj].Ns[1][0] /= normalize;
               area_data[ii][jj].Ns[1][1] /= normalize;
               area_data[ii][jj].Ns[1][2] /= normalize;

            }
       }
   }

   /* more calcs for fog */
   fog_mid_el = min_el + 5.0;
   fog_max_el = fog_mid_el + 5.0;
   fog_center_loc[0]=area_data[fog_center_min_el[0]][fog_center_min_el[1]].center[0];
   fog_center_loc[1]=area_data[fog_center_min_el[0]][fog_center_min_el[1]].center[1];

   do {
       /* get center of dark area */
       dark_center[0]=(rand()%i_dim);
       dark_center[1]=(rand()%j_dim);
   } while ((abs(dark_center[0]-fog_center_min_el[0])<10 &&  abs(dark_center[1]-fog_center_min_el[1])<10)
           || (abs(dark_center[0]-i_cntr)<X_DARK_RANGE && abs(dark_center[1]-j_cntr)<Z_DARK_RANGE));
   dark_center_loc[0]=area_data[dark_center[0]][dark_center[1]].center[0];
   dark_center_loc[1]=area_data[dark_center[0]][dark_center[1]].center[1];


#ifdef MIN_DEBUG_PRINT_TERRAIN
   printf("min_el: %.2f, %d, %d;  %.2f, %.2f;  dark_center:  %d, %d;  %.2f, %.2f\n", min_el,
           fog_center_min_el[0], fog_center_min_el[1], fog_center_loc[0],fog_center_loc[1],
           dark_center[0],dark_center[1], dark_center_loc[0], dark_center_loc[1]);
#endif



   /* generate surface normals */
   /* get most of field */
   for (ii=0; ii<NUM_X_AREAS; ii+=1)
   {
       for (jj=0; jj<NUM_Z_AREAS; jj+=1)
       {
           sq_normalize =0.0; normalize = 0.0;
           srfc_norms[ii][jj][0] = dims[1]*(vrtx_els[ii][jj]-vrtx_els[ii+1][jj]);
           srfc_norms[ii][jj][1] = dims[0]*dims[1];
           srfc_norms[ii][jj][2] = -dims[0]*(vrtx_els[ii][jj+1]-vrtx_els[ii][jj]);
           for (i=0; i<3; i+=1)
               sq_normalize += srfc_norms[ii][jj][i]*srfc_norms[ii][jj][i];
           normalize = sqrt(sq_normalize);

           for (i=0; i<3; i+=1)
               srfc_norms[ii][jj][i] /= normalize;
       }
   }

   /* get final row */
   for (ii=0; ii<NUM_X_AREAS; ii+=1)
   {
      jj=NUM_Z_AREAS;
      sq_normalize =0.0; normalize = 0.0;

      srfc_norms[ii][jj][0] = -dims[1]*(vrtx_els[ii+1][jj]-vrtx_els[ii][jj]);
      srfc_norms[ii][jj][1] = dims[0]*dims[1];
      srfc_norms[ii][jj][2] = -dims[0]*(vrtx_els[ii][jj]-vrtx_els[ii][jj-1]);
      for (i=0; i<3; i+=1)
         sq_normalize += srfc_norms[ii][jj][i]*srfc_norms[ii][jj][i];
       normalize = sqrt(sq_normalize);

       for (i=0; i<3; i+=1)
          srfc_norms[ii][jj][i] /= normalize;
   }
   /* get final column */
   for (jj=0; jj<NUM_Z_AREAS; jj+=1)
   {
      ii=NUM_X_AREAS;
      sq_normalize =0.0; normalize = 0.0;

      srfc_norms[ii][jj][0] = dims[1]*(vrtx_els[ii-1][jj]-vrtx_els[ii][jj]);
      srfc_norms[ii][jj][1] = dims[0]*dims[1];
      srfc_norms[ii][jj][2] = -dims[0]*(vrtx_els[ii][jj]-vrtx_els[ii][jj+1]);
      for (i=0; i<3; i+=1)
         sq_normalize += srfc_norms[ii][jj][i]*srfc_norms[ii][jj][i];
       normalize = sqrt(sq_normalize);

       for (i=0; i<3; i+=1)
          srfc_norms[ii][jj][i] /= normalize;
   }
   /* get final corner */
   sq_normalize =0.0; normalize = 0.0;

   ii=NUM_X_AREAS;
   jj=NUM_Z_AREAS;
   srfc_norms[ii][jj][0] = -dims[1]*(vrtx_els[ii][jj]-vrtx_els[ii-1][jj]);
   srfc_norms[ii][jj][1] = dims[0]*dims[1];
   srfc_norms[ii][jj][2] = dims[0]*(vrtx_els[ii][jj-1]-vrtx_els[ii][jj]);
   for (i=0; i<3; i+=1)
       sq_normalize += srfc_norms[ii][jj][i]*srfc_norms[ii][jj][i];
   normalize = sqrt(sq_normalize);

   for (i=0; i<3; i+=1)
       srfc_norms[ii][jj][i] /= normalize;

   /* generate vertex normals */
   for (ii=0; ii<i_dim; ii+=1)
   {
       for (jj=0; jj<j_dim; jj+=1)
       {
           sq_normalize =0.0; normalize = 0.0;
           for (i=0; i<3; i+=1)
               vrtx_norms[ii][jj][i] = srfc_norms[ii][jj][i];
           if (ii > 0)
               for (i=0; i<3; i+=1)
                   vrtx_norms[ii][jj][i] += srfc_norms[ii-1][jj][i];
           if (jj > 0)
               for (i=0; i<3; i+=1)
                   vrtx_norms[ii][jj][i] += srfc_norms[ii][jj-1][i];
           if (jj > 0 && ii > 0)
               for (i=0; i<3; i+=1)
                   vrtx_norms[ii][jj][i] += srfc_norms[ii][jj-1][i];

           for (i=0; i<3; i+=1)
               sq_normalize += vrtx_norms[ii][jj][i]*vrtx_norms[ii][jj][i];
           normalize = sqrt(sq_normalize);

           for (i=0; i<3; i+=1)
               vrtx_norms[ii][jj][i] /= normalize;
       }
   }

#ifdef DEBUG_PRINT_TERRAIN
   /* sanity check */
   for (i=0; i<i_dim; i+=1)
   {
      for (j=0; j<j_dim; j+=1)
         printf("%4.1f ",vrtx_els[i][j]);
      printf("\n");
   }
   printf("\n");
   /* sanity check */
   for (ii=0; ii<i_dim; ii+=1)
   {
       for (i=0; i<3; i++)
      {
           for (jj=0; jj<j_dim; jj+=1)
             printf("%6.3f ",srfc_norms[ii][jj][i]);
          printf("\n");
      }
      printf("\n");
   }
   printf("\n");
   for (ii=0; ii<i_dim; ii+=1)
   {
       for (i=0; i<3; i++)
      {
           for (jj=0; jj<j_dim; jj+=1)
             printf("%5.2f ",vrtx_norms[ii][jj][i]);
          printf("\n");
      }
      printf("\n");
   }
   printf("\n");
#endif /* DEBUG_PRINT_TERRAIN */
#ifdef MIN_DEBUG_PRINT_TERRAIN1
   for (i=0; i<NUM_HILLS; i+=1)
   {
      for (j=0; j<3; j+=1)
         printf("%.1f ",peak_dims[i][j]);
      printf("   ");
      for (j=0; j<2; j+=1)
         printf("%.1f ",peak_locs_dbl[i][j]);
      printf("   ");
      for (j=0; j<2; j+=1)
         printf("%d ",peak_locs_int[i][j]);
      printf("\n");
   }
#endif /* DEBUG_PRINT_TERRAIN */
}

void create_terrain(char terrain_texture)
{
   int i, j;
   int x = (int)(view_pos[0] + x_offset)/dims[0];
   int z = (int)(view_pos[2] + z_offset)/dims[1];
//  int xx = (int)(renny_pos[0] + x_offset)/dims[0];
//   int zz = (int)(renny_pos[2] + z_offset)/dims[1];
   float vect_to_area[2] = {0,0};
   float dot_prod = 0;
   float dark_factor = 0.9*glb_light_level + 0.1;
   //float forest_clr[3]={1,1,1};

   //  Set specular color to white
   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};

   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glColor3f(1,1,1);  // color below 10
   // glColor3f(0.1333,0.5451,0.13333); at 74, linear grade between
   // other forest green: #2E8B57


   /* scale and translate terrain */
   glPushMatrix();
     glTranslated(-NUM_X_AREAS*dims[0]/2.0,0,-NUM_Z_AREAS*dims[0]/2.0);
     glScaled(dims[0],1,dims[1]);
     for (i=0; i<NUM_X_AREAS; i+=1)
     {
        for (j=0; j<NUM_Z_AREAS; j+=1)
        {
            // ignoring y dim, for now
            if (abs(i-x)<2 && abs(j-z)<2)
            {
                dot_prod = 1;
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(1,1);
            }
            else
            {
                vect_to_area[0] = area_data[i][j].center[0]-view_pos[0];
                vect_to_area[1] = area_data[i][j].center[1]-view_pos[2];
                dot_prod = vect_to_area[0]*view_vector[0] + vect_to_area[1]*view_vector[2];
                /* the following prob more calcs than worth results we get */
//                dot_prod /= sqrt((vect_to_area[0]*vect_to_area[0] + vect_to_area[1]*vect_to_area[1])*
//                                 (view_vector[0]*view_vector[0] + view_vector[2]*view_vector[2]));
            }
//            if (i==xx && j==zz && fog_off==1)
//                glEnable(GL_POLYGON_OFFSET_FILL);
//            if (i==xx && j==zz && fog_off==1)
//                dot_prod = 0;

#ifdef DEBUG_PRINT_TERRAIN
            if (j%10==0 &&  i%10==0)
            {
                printf("area_data[%d][%d] is %s view\n",i,j,(dot_prod>0.5?"in":"out"));
                printf("vect_to_area: %.1f, %.1f;  view_vector: %.1f, %.1f\n",vect_to_area[0],vect_to_area[1],view_vector[0], view_vector[1]);
            }
#endif
            if (dot_prod > 0.0)
            {
                if (srfc_els[i][j]<100)
                    glBindTexture(GL_TEXTURE_2D,textures[10*grass_texture]);
                else
                    glBindTexture(GL_TEXTURE_2D,textures[11+grass_texture]);
                if (area_data[i][j].ridge_dir == 1) /* ridge in x=z direction */
                {
                    /* the 'NW triangle' */
                    glBegin(GL_TRIANGLES);
                      glNormal3f(vrtx_norms[i][j][0]/dims[0],vrtx_norms[i][j][1],vrtx_norms[i][j][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i][j][0],dark_factor*vrtx_clrs[i][j][1],dark_factor*vrtx_clrs[i][j][2]);
                      glTexCoord2f(0,0); glVertex3f(i,vrtx_els[i][j],j);
                      glNormal3f(vrtx_norms[i+1][j+1][0]/dims[0],vrtx_norms[i+1][j+1][1],vrtx_norms[i+1][j+1][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i+1][j+1][0],dark_factor*vrtx_clrs[i+1][j+1][1],dark_factor*vrtx_clrs[i+1][j+1][2]);
                      glTexCoord2f(1,1); glVertex3f(i+1,vrtx_els[i+1][j+1],j+1);
                      glNormal3f(vrtx_norms[i+1][j][0]/dims[0],vrtx_norms[i+1][j][1],vrtx_norms[i+1][j][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i+1][j][0],dark_factor*vrtx_clrs[i+1][j][1],dark_factor*vrtx_clrs[i+1][j][2]);
                      glTexCoord2f(1,0); glVertex3f(i+1,vrtx_els[i+1][j],j);
                    glEnd();
                    /* the 'SE triangle' */
                    glBegin(GL_TRIANGLES);
                      glNormal3f(vrtx_norms[i][j][0]/dims[0],vrtx_norms[i][j][1],vrtx_norms[i][j][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i][j][0],dark_factor*vrtx_clrs[i][j][1],dark_factor*vrtx_clrs[i][j][2]);
                      glTexCoord2f(0,0); glVertex3f(i,vrtx_els[i][j],j);
                      glNormal3f(vrtx_norms[i][j+1][0]/dims[0],vrtx_norms[i][j+1][1],vrtx_norms[i][j+1][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i][j+1][0],dark_factor*vrtx_clrs[i][j+1][1],dark_factor*vrtx_clrs[i][j+1][2]);
                      glTexCoord2f(0,1); glVertex3f(i,vrtx_els[i][j+1],j+1);
                      glNormal3f(vrtx_norms[i+1][j+1][0]/dims[0],vrtx_norms[i+1][j+1][1],vrtx_norms[i+1][j+1][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i+1][j+1][0],dark_factor*vrtx_clrs[i+1][j+1][1],dark_factor*vrtx_clrs[i+1][j+1][2]);
                      glTexCoord2f(1,1); glVertex3f(i+1,vrtx_els[i+1][j+1],j+1);
                    glEnd();
                }
                else if (area_data[i][j].ridge_dir == -1) /* ridge in x=-z direction */
                {
                    /* the 'NE triangle' */
                    glBegin(GL_TRIANGLES);
                      glNormal3f(vrtx_norms[i][j+1][0]/dims[0],vrtx_norms[i][j+1][1],vrtx_norms[i][j+1][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i][j+1][0],dark_factor*vrtx_clrs[i][j+1][1],dark_factor*vrtx_clrs[i][j+1][2]);
                      glTexCoord2f(0,1); glVertex3f(i,vrtx_els[i][j+1],j+1);
                      glNormal3f(vrtx_norms[i+1][j+1][0]/dims[0],vrtx_norms[i+1][j+1][1],vrtx_norms[i+1][j+1][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i+1][j+1][0],dark_factor*vrtx_clrs[i+1][j+1][1],dark_factor*vrtx_clrs[i+1][j+1][2]);
                      glTexCoord2f(1,1); glVertex3f(i+1,vrtx_els[i+1][j+1],j+1);
                      glNormal3f(vrtx_norms[i+1][j][0]/dims[0],vrtx_norms[i+1][j][1],vrtx_norms[i+1][j][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i+1][j][0],dark_factor*vrtx_clrs[i+1][j][1],dark_factor*vrtx_clrs[i+1][j][2]);
                      glTexCoord2f(1,0); glVertex3f(i+1,vrtx_els[i+1][j],j);
                    glEnd();
                    /* the 'SW triangle' */
                    glBegin(GL_TRIANGLES);
                      glNormal3f(vrtx_norms[i][j][0]/dims[0],vrtx_norms[i][j][1],vrtx_norms[i][j][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i][j][0],dark_factor*vrtx_clrs[i][j][1],dark_factor*vrtx_clrs[i][j][2]);
                      glTexCoord2f(0,0); glVertex3f(i,vrtx_els[i][j],j);
                      glNormal3f(vrtx_norms[i][j+1][0]/dims[0],vrtx_norms[i][j+1][1],vrtx_norms[i][j+1][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i][j+1][0],dark_factor*vrtx_clrs[i][j+1][1],dark_factor*vrtx_clrs[i][j+1][2]);
                      glTexCoord2f(0,1); glVertex3f(i,vrtx_els[i][j+1],j+1);
                      glNormal3f(vrtx_norms[i+1][j][0]/dims[0],vrtx_norms[i+1][j][1],vrtx_norms[i+1][j][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i+1][j][0],dark_factor*vrtx_clrs[i+1][j][1],dark_factor*vrtx_clrs[i+1][j][2]);
                      glTexCoord2f(1,0); glVertex3f(i+1,vrtx_els[i+1][j],j);
                    glEnd();
                }
                else /* error for some reason, just draw the quad */
                {
                    glBegin(GL_QUADS);
                      glNormal3f(vrtx_norms[i][j][0]/dims[0],vrtx_norms[i][j][1],vrtx_norms[i][j][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i][j][0],dark_factor*vrtx_clrs[i][j][1],dark_factor*vrtx_clrs[i][j][2]);
                      glTexCoord2f(0,0); glVertex3f(i,vrtx_els[i][j],j);
                      glNormal3f(vrtx_norms[i+1][j][0]/dims[0],vrtx_norms[i][j+1][1],vrtx_norms[i][j+1][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i][j+1][0],dark_factor*vrtx_clrs[i][j+1][1],dark_factor*vrtx_clrs[i][j+1][2]);
                      glTexCoord2f(0,1); glVertex3f(i,vrtx_els[i][j+1],j+1);
                      glNormal3f(vrtx_norms[i+1][j+1][0]/dims[0],vrtx_norms[i+1][j+1][1],vrtx_norms[i+1][j+1][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i+1][j+1][0],dark_factor*vrtx_clrs[i+1][j+1][1],dark_factor*vrtx_clrs[i+1][j+1][2]);
                      glTexCoord2f(1,1); glVertex3f(i+1,vrtx_els[i+1][j+1],j+1);
                      glNormal3f(vrtx_norms[i+1][j][0]/dims[0],vrtx_norms[i+1][j][1],vrtx_norms[i+1][j][2]/dims[1]);
                      glColor3f(dark_factor*vrtx_clrs[i+1][j][0],dark_factor*vrtx_clrs[i+1][j][1],dark_factor*vrtx_clrs[i+1][j][2]);
                      glTexCoord2f(1,0); glVertex3f(i+1,vrtx_els[i+1][j],j);
                    glEnd();
                }
            }
            if (abs(i-x)<2 && abs(j-z)<2)
                glDisable(GL_POLYGON_OFFSET_FILL);
        }
     }
     // low tech first version of the sky  get the orientations correct
      glColor3f(dark_factor,dark_factor,dark_factor);
     glBindTexture(GL_TEXTURE_2D,textures[9]);
     glNormal3f( -1, 0, 0);
     glBegin(GL_QUADS); // or quad strip?
       glTexCoord2f(0,0); glVertex3f(NUM_X_AREAS,0,0);
       glTexCoord2f(0.5,0); glVertex3f(NUM_X_AREAS,0,((float)NUM_Z_AREAS)/2.0);
       glTexCoord2f(0.5,1); glVertex3f(NUM_X_AREAS,side_height,((float)NUM_Z_AREAS)/2.0);
       glTexCoord2f(0,1); glVertex3f(NUM_X_AREAS,side_height,0);
     glEnd();
     glBegin(GL_QUADS); // or quad strip?
       glTexCoord2f(0.5,0); glVertex3f(NUM_X_AREAS,0,((float)NUM_Z_AREAS)/2.0);
       glTexCoord2f(1,0); glVertex3f(NUM_X_AREAS,0,NUM_Z_AREAS);
       glTexCoord2f(1,1); glVertex3f(NUM_X_AREAS,side_height,NUM_Z_AREAS);
       glTexCoord2f(0.5,1); glVertex3f(NUM_X_AREAS,side_height,((float)NUM_Z_AREAS)/2.0);
     glEnd();
     glNormal3f( 0, 0, -1);
     glBegin(GL_QUADS); // or quad strip?
       glTexCoord2f(0,0); glVertex3f(0,0,NUM_Z_AREAS);
       glTexCoord2f(0.5,0); glVertex3f(((float)NUM_X_AREAS)/2.0,0,NUM_Z_AREAS);
       glTexCoord2f(0.5,1); glVertex3f(((float)NUM_X_AREAS)/2.0,side_height,NUM_Z_AREAS);
       glTexCoord2f(0,1); glVertex3f(0,side_height,NUM_Z_AREAS);
     glEnd();
     glBegin(GL_QUADS); // or quad strip?
       glTexCoord2f(0.5,0); glVertex3f(((float)NUM_X_AREAS)/2.0,0,NUM_Z_AREAS);
       glTexCoord2f(1,0); glVertex3f(NUM_X_AREAS,0,NUM_Z_AREAS);
       glTexCoord2f(1,1); glVertex3f(NUM_X_AREAS,side_height,NUM_Z_AREAS);
       glTexCoord2f(0.5,1); glVertex3f(((float)NUM_X_AREAS)/2.0,side_height,NUM_Z_AREAS);
     glEnd();
     glNormal3f( 1, 0, 0);
     glBegin(GL_QUADS); // or quad strip?
       glTexCoord2f(1,0); glVertex3f(0,0,0);
       glTexCoord2f(0.5,0); glVertex3f(0,0,((float)NUM_Z_AREAS)/2.0);
       glTexCoord2f(0.5,1); glVertex3f(0,side_height,((float)NUM_Z_AREAS)/2.0);
       glTexCoord2f(1,1); glVertex3f(0,side_height,0);
     glEnd();
     glBegin(GL_QUADS); // or quad strip?
       glTexCoord2f(0.5,0); glVertex3f(0,0,((float)NUM_Z_AREAS)/2.0);
       glTexCoord2f(0,0); glVertex3f(0,0,NUM_Z_AREAS);
       glTexCoord2f(0,1); glVertex3f(0,side_height,NUM_Z_AREAS);
       glTexCoord2f(0.5,1); glVertex3f(0,side_height,((float)NUM_Z_AREAS)/2.0);
     glEnd();
     glNormal3f( 0, 0, 1);
     glBegin(GL_QUADS); // or quad strip?
       glTexCoord2f(1,0); glVertex3f(0,0,0);
       glTexCoord2f(0.5,0); glVertex3f(((float)NUM_X_AREAS)/2.0,0,0);
       glTexCoord2f(0.5,1); glVertex3f(((float)NUM_X_AREAS)/2.0,side_height,0);
       glTexCoord2f(1,1); glVertex3f(0,side_height,0);
     glEnd();
     glBegin(GL_QUADS); // or quad strip?
       glTexCoord2f(0.5,0); glVertex3f(((float)NUM_X_AREAS)/2.0,0,0);
       glTexCoord2f(0,0); glVertex3f(NUM_X_AREAS,0,0);
       glTexCoord2f(0,1); glVertex3f(NUM_X_AREAS,side_height,0);
       glTexCoord2f(0.5,1); glVertex3f(((float)NUM_X_AREAS)/2.0,side_height,0);
     glEnd();

   glPopMatrix();

   glDisable(GL_TEXTURE_2D);
}

/* re-draw area for renny's shadow */
void renny_shadow_area()
{
   int i = (int)(renny_pos[0]+ x_offset)/dims[0];
   int j = (int)(renny_pos[2]+ z_offset)/dims[1];
   if (area_data[i][j].ridge_dir == 1) /* ridge in x=z direction */
   {
         /* the 'NW triangle' */
         glBegin(GL_TRIANGLES);
           glNormal3f(vrtx_norms[i][j][0],vrtx_norms[i][j][1],vrtx_norms[i][j][2]);
           glVertex3f(area_data[i][j].x_range[0],vrtx_els[i][j],area_data[i][j].z_range[0]);
           glNormal3f(vrtx_norms[i+1][j+1][0]/dims[0],vrtx_norms[i+1][j+1][1],vrtx_norms[i+1][j+1][2]/dims[1]);
           glVertex3f(area_data[i][j].x_range[1],vrtx_els[i+1][j+1],area_data[i][j].z_range[1]);
           glNormal3f(vrtx_norms[i+1][j][0]/dims[0],vrtx_norms[i+1][j][1],vrtx_norms[i+1][j][2]/dims[1]);
           glVertex3f(area_data[i][j].x_range[1],vrtx_els[i+1][j],area_data[i][j].z_range[0]);
         glEnd();
         /* the 'SE triangle' */
         glBegin(GL_TRIANGLES);
           glNormal3f(vrtx_norms[i][j][0]/dims[0],vrtx_norms[i][j][1],vrtx_norms[i][j][2]/dims[1]);
           glVertex3f(area_data[i][j].x_range[0],vrtx_els[i][j],area_data[i][j].z_range[0]);
           glNormal3f(vrtx_norms[i][j+1][0]/dims[0],vrtx_norms[i][j+1][1],vrtx_norms[i][j+1][2]/dims[1]);
           glVertex3f(area_data[i][j].x_range[0],vrtx_els[i][j+1],area_data[i][j].z_range[1]);
           glNormal3f(vrtx_norms[i+1][j+1][0]/dims[0],vrtx_norms[i+1][j+1][1],vrtx_norms[i+1][j+1][2]/dims[1]);
           glVertex3f(area_data[i][j].x_range[1],vrtx_els[i+1][j+1],area_data[i][j].z_range[1]);
         glEnd();
   }
   else if (area_data[i][j].ridge_dir == -1) /* ridge in x=-z direction */
   {
         /* the 'NE triangle' */
         glBegin(GL_TRIANGLES);
           glNormal3f(vrtx_norms[i][j+1][0]/dims[0],vrtx_norms[i][j+1][1],vrtx_norms[i][j+1][2]/dims[1]);
           glVertex3f(area_data[i][j].x_range[0],vrtx_els[i][j+1],area_data[i][j].z_range[1]);
           glNormal3f(vrtx_norms[i+1][j+1][0]/dims[0],vrtx_norms[i+1][j+1][1],vrtx_norms[i+1][j+1][2]/dims[1]);
           glVertex3f(area_data[i][j].x_range[1],vrtx_els[i+1][j+1],area_data[i][j].z_range[1]);
           glNormal3f(vrtx_norms[i+1][j][0]/dims[0],vrtx_norms[i+1][j][1],vrtx_norms[i+1][j][2]/dims[1]);
           glVertex3f(area_data[i][j].x_range[1],vrtx_els[i+1][j],area_data[i][j].z_range[0]);
         glEnd();
         /* the 'SW triangle' */
         glBegin(GL_TRIANGLES);
           glNormal3f(vrtx_norms[i][j][0]/dims[0],vrtx_norms[i][j][1],vrtx_norms[i][j][2]/dims[1]);
           glVertex3f(area_data[i][j].x_range[0],vrtx_els[i][j],area_data[i][j].z_range[0]);
           glNormal3f(vrtx_norms[i][j+1][0]/dims[0],vrtx_norms[i][j+1][1],vrtx_norms[i][j+1][2]/dims[1]);
           glVertex3f(area_data[i][j].x_range[0],vrtx_els[i][j+1],area_data[i][j].z_range[1]);
           glNormal3f(vrtx_norms[i+1][j][0]/dims[0],vrtx_norms[i+1][j][1],vrtx_norms[i+1][j][2]/dims[1]);
           glVertex3f(area_data[i][j].x_range[1],vrtx_els[i+1][j],area_data[i][j].z_range[0]);
         glEnd();
   }
}

void get_area_data(int coords[2], float center[2])
{
    center[0]=area_data[coords[0]][coords[1]].center[0];
    center[1]=area_data[coords[0]][coords[1]].center[1];
}

/* find current normal of terrain based on Renny's X/Z plane location */
void get_current_normal(float normal[])
{
    int x = (int)(renny_pos[0]+ x_offset)/dims[0];
    int z = (int)(renny_pos[2]+ z_offset)/dims[1];
    normal[0]=srfc_norms[x][z][0]; normal[1]=srfc_norms[x][z][1]; normal[2]=srfc_norms[x][z][2];
    int ii = 0;
    float x_area = renny_pos[0]+ x_offset - (float)x*dims[0];
    float z_area = renny_pos[2]+ z_offset - (float)z*dims[1];
    float norm_i[3]= {0,1,0};
    float norm_ip1[3]= {0,1,0};
    float norm_sq = 0;
    for (ii=0; ii<3; ii+=1)
    {
        norm_i[ii]=(vrtx_norms[x+1][z][ii]-vrtx_norms[x][z][ii])*x_area/dims[0] + vrtx_norms[x][z][ii];
        norm_ip1[ii]=(vrtx_norms[x+1][z+1][ii]-vrtx_norms[x][z+1][ii])*x_area/dims[0] + vrtx_norms[x][z+1][ii];
        normal[ii]=(norm_ip1[ii] - norm_i[ii])*z_area/dims[1] + norm_i[ii];
        norm_sq += normal[ii]*normal[ii];
    }
    for (ii=0; ii<3; ii+=1)
    {
        normal[ii] /= norm_sq;
        current_normal[ii] = normal[ii];
    }

#ifdef MIN_DEBUG_PRINT_TERRAIN1
    if (print_norm > 60 && renny_moving==1)
    {
        printf("%d norm: %.4f, %.4f, %.4f\n", print_norm, normal[0], normal[1], normal[2]);
        print_norm = 0;
    }
    else
        print_norm += 1;
#endif

}

/* find current normal of terrain based on objects X/Z plane area  number */
void get_normal(int i, int j, float normal[])
{
    normal[0]=srfc_norms[i][j][0]; normal[1]=srfc_norms[i][j][1]; normal[2]=srfc_norms[i][j][2];
    /* Note:  Do linear interp. of vertex norms based on actual location as proposed for
     * get_current_normal[]?
     */
}

/* find and SET Renny's elevation on the terrain, based on Renny's X/Z plane location */
void get_current_elevation()
{
    last_elev = current_elev;
    current_elev = get_elevation(renny_pos[0], renny_pos[2]);

}

/* find and return object's elevation on the terrain, based on it's X/Z plane location */
float get_elevation(float x_obj, float z_obj)
{
    float  current_elev_new = 0;
#ifdef MIN_DEBUG_PRINT_TERRAIN
    float  current_elev_old = 0;

    int el_case = 0;
#endif
    int x = (int)(x_obj+ x_offset)/X_DIM;
    int z = (int)(z_obj+ z_offset)/Z_DIM;
    float x_area = x_obj - area_data[x][z].x_range[0];//(float)x*dims[0];
    float z_area = z_obj - area_data[x][z].z_range[0];//(float)z*dims[1];

    /* (old way) linear double interpolation in the area */
    float x_elev_i = (vrtx_els[x+1][z]-vrtx_els[x][z])*x_area/X_DIM + vrtx_els[x][z];
    float x_elev_ip1 = (vrtx_els[x+1][z+1]-vrtx_els[x][z+1])*x_area/X_DIM + vrtx_els[x][z+1];

    /* new way, deal with the two triangle planes of the area*/
    float z_elev_i = 0;
    float z_elev_ip1 = 0;

    if (area_data[x][z].ridge_dir == 1)
    {
        if (x_area>z_area)
        {
            /* in this 'upper' or NW triangle */
#ifdef MIN_DEBUG_PRINT_TERRAIN
            el_case = 1;
#endif
            z_elev_i = vrtx_els[x][z] + x_area*(vrtx_els[x+1][z]-vrtx_els[x][z])/dims[0];
            z_elev_ip1 = vrtx_els[x][z] + x_area*(vrtx_els[x+1][z+1]-vrtx_els[x][z])/dims[0];
            current_elev_new = z_elev_i + z_area*(z_elev_ip1 - z_elev_i)/x_area;
        }
        else
        {
            /* in this 'lower' or SE triangle */
#ifdef MIN_DEBUG_PRINT_TERRAIN
            el_case = 2;
#endif
            z_elev_i = vrtx_els[x][z] + x_area*(vrtx_els[x+1][z+1]-vrtx_els[x][z])/dims[0];
            z_elev_ip1 = vrtx_els[x][z+1] + x_area*(vrtx_els[x+1][z+1]-vrtx_els[x][z+1])/dims[0];
            current_elev_new = z_elev_i + (z_area-x_area)*(z_elev_ip1 - z_elev_i)/(dims[1]-x_area);
        }
    }
    else if (area_data[x][z].ridge_dir == -1)
    {
        if (x_area> dims[0]-z_area)
        {
            /* in this 'upper' or NE triangle */
#ifdef MIN_DEBUG_PRINT_TERRAIN
            el_case = 3;
#endif
            z_elev_i = vrtx_els[x][z+1] + x_area*(vrtx_els[x+1][z]-vrtx_els[x][z+1])/dims[0];
            z_elev_ip1 = vrtx_els[x][z+1] + x_area*(vrtx_els[x+1][z+1]-vrtx_els[x][z+1])/dims[0];
            current_elev_new = z_elev_i + (z_area-(dims[1]-x_area))*(z_elev_ip1 - z_elev_i)/x_area;
        }
        else
        {
            /* in this 'lower' or SW triangle */
#ifdef MIN_DEBUG_PRINT_TERRAIN
            el_case = 4;
#endif
            z_elev_i = vrtx_els[x][z] + x_area*(vrtx_els[x+1][z]-vrtx_els[x][z])/dims[0];
            z_elev_ip1 = vrtx_els[x][z+1] + x_area*(vrtx_els[x+1][z]-vrtx_els[x][z+1])/dims[0];
            current_elev_new = z_elev_i + z_area*(z_elev_ip1 - z_elev_i)/(dims[1]-x_area);
        }

    }
    else
    {
        /* do it the old way */
        current_elev_new = (x_elev_ip1 - x_elev_i)*z_area/Z_DIM + x_elev_i;
    }

#ifdef MIN_DEBUG_PRINT_TERRAIN
    /* set the globals, old way  for comparison/debug */
    current_elev_old = (x_elev_ip1 - x_elev_i)*z_area/Z_DIM + x_elev_i;

    if (print_kount > 60 && renny_moving==1 && x_obj==renny_pos[0]  && z_obj==renny_pos[2])
    {
        switch(el_case)
        {
        case 1:
            printf("case 1: ridge_dir = 1, x_area>z_area\n");
            break;
        case 2:
            printf("case 2: ridge_dir = 1, x_area<z_area\n");
            break;
        case 3:
            printf("case 3: ridge_dir = -1, x_area>dims[0]-z_area\n");
            break;
        case 4:
            printf("case 4: ridge_dir = -1, x_area<dims[0]-z_area\n");
            break;
        default:
            printf("error: failed, did the old way\n");
            break;
        }
        printf("verts: %.2f, %.2f, %.2f, %.2f\n", vrtx_els[x][z], vrtx_els[x][z+1], vrtx_els[x+1][z], vrtx_els[x+1][z+1]);
        printf("elev compare: %.2f, %.2f\n",  current_elev_old, current_elev);
        if (current_elev_old > current_elev)
            printf("WTF?\n");
        if (current_elev - current_elev_old > 1)
            printf("huh?\n");
        print_kount = 1;
    }
    else
        print_kount += 1;
#endif
    return current_elev_new;
}


void shadow_normal_and_plane(float x_obj, float z_obj, float N[], float E[])
{
    int n_e_case = 0;
    int x = (int)(x_obj+ x_offset)/X_DIM;
    int z = (int)(z_obj+ z_offset)/Z_DIM;
    float x_area = x_obj - area_data[x][z].x_range[0];//(float)x*dims[0];
    float z_area = z_obj - area_data[x][z].z_range[0];//(float)z*dims[1];
    float shadow_vert_offset_cheat = 0.25;

//    if

    /* now done in init_terrain() in area_data initalization */
    if (area_data[x][z].ridge_dir == 1)
    {
        if (x_area>z_area)
            n_e_case = 0; /* in this 'upper' or NW triangle */
        else
            n_e_case = 1; /* in this 'lower' or SE triangle */
    }
    else if (area_data[x][z].ridge_dir == -1)
    {
        if (x_area> dims[0]-z_area)
            n_e_case = 0; /* in this 'upper' or NE triangle */
        else
            n_e_case = 1; /* in this 'lower' or SW triangle */
    }
    N[0] = area_data[x][z].Ns[n_e_case][0];
    N[1] = area_data[x][z].Ns[n_e_case][1];
    N[2] = area_data[x][z].Ns[n_e_case][2];

    if (renny_flying == 1)
    {
        /* major offset  larger ratio at lower alts*/
        shadow_vert_offset_cheat = renny_alt*0.1 + 1/(renny_alt+1);
    }
    else
    {
        // on steep terrain keep shadow better visible on surrounding quads.
        if (N[1]<0.98) shadow_vert_offset_cheat = 0.50;
        else if (N[1]>0.995) shadow_vert_offset_cheat = 0.05;
		
		//unless at REALLY low view angles
		if (ph < 15)
			shadow_vert_offset_cheat = 0.01;
    }

    E[0] = area_data[x][z].Es[n_e_case][0];
    E[1] = area_data[x][z].Es[n_e_case][1]+shadow_vert_offset_cheat;
    E[2] = area_data[x][z].Es[n_e_case][2];


    if (area_data[x][z].ridge_dir == -1)
        n_e_case += 2;

//    if


#ifdef SHADOW_DEBUG
    if (print_kount > 60 && renny_moving==1 && x_obj==(float)renny_pos[0]  && z_obj==(float)renny_pos[2])
    {
        switch(n_e_case)
        {
        case 0:
            printf("case 0: ridge_dir = 1, x_area>z_area\n");
            break;
        case 1:
            printf("case 1: ridge_dir = 1, x_area<z_area\n");
            break;
        case 2:
            printf("case 2: ridge_dir = -1, x_area>dims[0]-z_area\n");
            break;
        case 3:
            printf("case 3: ridge_dir = -1, x_area<dims[0]-z_area\n");
            break;
        }
        printf("x/z: %.2f, %.2f\n", x_obj, z_obj);
        printf("E[]: %.2f, %.2f, %.2f\n", E[0], E[1], E[2]);
        printf("N[]: %.2f, %.2f, %.2f\n",  N[0], N[1], N[2]);
        print_kount = 1;
    }
    else
        print_kount += 1;
#endif

}

