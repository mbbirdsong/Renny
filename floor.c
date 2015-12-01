/*
 * floor.c
 *
 *  Created on: Oct 18, 2011
 *      Author: michaelb
 */

#include "CSCIx229.h"
#include "final_project_mbb.h"
#include "floor.h"

double boards_widths[40];
double floor_dims[40][20];
int floor_text[40][20];
int num_boards_wide;
int num_boards_long[40];
int print_dims;
double floor_width = 12.0;
double floor_length = 12.0;

/*
 * generate floor board dimensions with random values
 */
void floor_init()
{
   unsigned int iseed = (unsigned int)time(NULL);
   srand (iseed);
   int i=0, j=0;
   double width_total = 0.0;
   double length_total = 0.0;
   num_boards_wide = 0;
#ifdef DEBUG_PRINT
   printf("floor re-calc begin\n");
#endif /* DEBUG_PRINT */
   print_dims = 1;
   for (i=0; i<40; i+=1)
   {
      boards_widths[i] = 0.0;
      num_boards_long[i] = 0;
      length_total = 0.0;
      /* board widths */
      if (width_total < floor_width)
      {
         /* 0.20 <= board_width <= 0.7 */
         boards_widths[i] = 0.10*((double)(rand()%5))+0.20;
         width_total += boards_widths[i];
         num_boards_wide += 1;
         /* board length of this row */
         for (j=0; j<20; j+=1)
         {
             floor_text[i][j] = 0;
             if (length_total < floor_length)
             {
                /* 1.0 <= board_lengths <= 2.0 */
                floor_dims[i][j] = 0.10*((double)(rand()%10))+1.00;
                floor_text[i][j] = rand()%4;
                length_total += floor_dims[i][j];
                num_boards_long[i] += 1;
                if (length_total > floor_length)
                {
                    floor_dims[i][j] = floor_dims[i][j] - (length_total-floor_length);
                    length_total = floor_length;
                }
             }
         }
      }
   }
#ifdef DEBUG_PRINT
   printf("floor re-calc done\n");
   printf("\n%d\n", num_boards_wide);
   for (i=0; i<40; i+=1)
   {
      printf("%d, %.2f:  ",num_boards_long[i], boards_widths[i]);
      for (j=0; j<20; j+=1)
      {
         printf("%.2f ",floor_dims[i][j]);
      }
      printf("\n");
   }
#endif /* DEBUG_PRINT */
}

void create_floor(char flr_texture)
{
   int i, j;
   double flr_hgt=-0.05;
   double flr_gap=0.008;
   double x_pnt=floor_width/2.0, z_pnt=floor_length/2.0,
          x_new=floor_width/2.0, z_new=floor_length/2.0;
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
#ifdef DEBUG_PRINT
   if (print_dims == 1)
      printf("floor re-set begin\n");
#endif /* DEBUG_PRINT */

   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glColor3f(0.9,1,1);

   for (i=0; i<num_boards_wide; i+=1)
   {
      x_new-=boards_widths[i]; z_pnt=floor_length/2.0; z_new=floor_length/2.0;
#ifdef DEBUG_PRINT
      if (print_dims == 1) printf("%.1f, %.1f: ", x_pnt, x_new);
#endif /* DEBUG_PRINT */
      for (j=0; j<num_boards_long[i]; j+=1)
      {
         z_new-=floor_dims[i][j];
#ifdef DEBUG_PRINT
         if (print_dims == 1) printf("%.1f %.1f, ", z_pnt, z_new);
#endif /* DEBUG_PRINT */
         /* floor_text[i][j] = rand()%4; (a funky mistake) */
         glBindTexture(GL_TEXTURE_2D,floor_textures[(int)flr_texture][floor_text[i][j]]);
         glBegin(GL_QUADS);
         glNormal3f( 0,+1, 0);
         glTexCoord2f(0,0); glVertex3f(x_pnt,flr_hgt,z_pnt);
         glTexCoord2f(0,1); glVertex3f(x_pnt,flr_hgt,z_new+flr_gap);
         glTexCoord2f(1,1); glVertex3f(x_new+flr_gap,flr_hgt,z_new+flr_gap);
         glTexCoord2f(1,0); glVertex3f(x_new+flr_gap,flr_hgt,z_pnt);
         glEnd();
            z_pnt=z_new;
      }
#ifdef DEBUG_PRINT
      if (print_dims == 1) printf("\n");
#endif /* DEBUG_PRINT */
      x_pnt=x_new;
   }
#ifdef DEBUG_PRINT
   if (print_dims == 1) printf("floor re-set done\n");
#endif /* DEBUG_PRINT */
   print_dims = 0;

   /* sub floor */
   double sub_floor_edge = .5+floor_length/2.0;
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glVertex3f(sub_floor_edge,flr_hgt-10,sub_floor_edge);
   glVertex3f(sub_floor_edge,flr_hgt-10,-sub_floor_edge);
   glVertex3f(-sub_floor_edge,flr_hgt-10,-sub_floor_edge);
   glVertex3f(-sub_floor_edge,flr_hgt-10,sub_floor_edge);
   glEnd();
   glDisable(GL_POLYGON_OFFSET_FILL);
   glDisable(GL_TEXTURE_2D);

}
