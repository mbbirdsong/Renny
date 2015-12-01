/*
 * basic_shapes.c
 *
 *  Created on: Oct 15, 2011
 *  Drastically modified on: Oct 24, 2011
 *      Author: michaelb
 *
 *  based on and greatly expanded from code in the 'ex8.c' example in
 *  CSCI 5229, Fall 2011, Prof. W. Schreuder
 */

#include "CSCIx229.h"
#include "final_project_mbb.h"
#include "basic_shapes.h"
#include "cyl_shapes.h"
#include "renny.h"
#include "terrain.h"

float norm0[3] = {1,0,0};  float norm1[3] = {-1,0,0};
float norm2[3] = {0,1,0};  float norm3[3] = {0,-1,0};
float norm4[3] = {0,0,1};  float norm5[3] = {0,0,-1};

float vert0[3] = {1,1,1};  float vert1[3] = {-1,1,1};
float vert2[3] = {1,-1,1}; float vert3[3] = {-1,-1,1};
float vert4[3] = {1,1,-1}; float vert5[3] = {-1,1,-1};
float vert6[3] = {1,-1,-1};float vert7[3] = {-1,-1,-1};

void clear_cube(double x,double y,double z,
                 double dx,double dy,double dz, double th,
                 double scale_x, double scale_y, int bevel,
                 unsigned char color, char texture, int obj_num);
void draw_face(float norm[3], float v0[3],
        float v1[3], float v2[3], float v3[3], unsigned char offset);
void calc_norm (float norm[3],float v0[3], float v1[3], float v2[3]);


void energy_cube(double x,double y,double z,
                 double dx,double dy,double dz, double th,
                 double scale_x, double scale_y, int bevel,
                 unsigned char color, char texture, int obj_num)
{
       //  Save transformation
       glPushMatrix();
           //  Offset, scale and rotate
         glTranslated(0,4,0);
         glRotated(45,1,0,0);
         glRotated(45,0,0,1);
         clear_cube(0,0,0, 2,2,2, th, 1,1,0, color,13, obj_num);
       glPopMatrix();
}

void fog_cube()
{
    /*  // never could get blend of alpha quite right, texture always too faded
    clear_cube((fog_x_range[0]+fog_x_range[1])/2,1.5*fog_mid_el,(fog_z_range[0]+fog_z_range[1])/2,
               (fog_x_range[1]-fog_x_range[0])/2,1.5*fog_max_el,(fog_z_range[1]-fog_z_range[0])/2,
                0, 1,1,0, 9,14, -1);
                */
    new_cube((fog_x_range[0]+fog_x_range[1])/2,fog_mid_el+2,(fog_z_range[0]+fog_z_range[1])/2,
               (fog_x_range[1]-fog_x_range[0])/2,7,(fog_z_range[1]-fog_z_range[0])/2,
                0, 1,1,0, 9,14);
}


/*
 *  Draw a new cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotate about the y axis (th)
 *     the scaling of the front of the cube for bevels (scale_x, scale_y)
 *     one side or symmetric (bevel: 0=sym:x&y, 1=sym:x&one sided:y
 *                                   2=one sided:x&sym:y 3=one sided:x&y
 */
void new_cube(double x,double y,double z,
                 double dx,double dy,double dz, double th,
                 double scale_x, double scale_y, int bevel,
                 unsigned char color, char texture)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   if (color<2 && drawing_renny==0) emission = 100;
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   emission = 0;
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   // bevel or symmetric
   // both x & y one side
   vert0[0] = vert2[0] = scale_x;
   vert0[1] = vert1[1] = scale_y;
   // x symmetric ?
   if (bevel == 0  || bevel == 1)
      vert1[0] = vert3[0] = -scale_x;
   else
      vert1[0] = vert3[0] = -1;
   // y symmetric ?
   if (bevel == 0  ||  bevel == 2)
      vert2[1] = vert3[1] = -scale_y;
   else
      vert2[1] = vert3[1] = -1;

   FunkyCullahs(0, color);
   if (texture > -1)
   {
      //  Set texture
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,textures[(int)texture]);
      glColor3f(1,1,1);
   }
   draw_face(norm4, vert3, vert2, vert0, vert1, 0);    //  Front
   draw_face(norm5, vert6, vert7, vert5, vert4, 0);    //  Back
   draw_face(norm0, vert2, vert6, vert4, vert0, 0);    //  Right
   draw_face(norm1, vert5, vert7, vert3, vert1, 0);    //  Left
   draw_face(norm2, vert0, vert4, vert5, vert1, 0);    //  Top
   draw_face(norm3, vert7, vert6, vert2, vert3, 0);    //  Bottom
   if (texture > -1)
      glDisable(GL_TEXTURE_2D);

   //  Undo transformations and textures
   glPopMatrix();
}

/*
 *  Draw a clear cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotate about the y axis (th)
 *     the scaling of the front of the cube for bevels (scale_x, scale_y)
 *     one side or symmetric (bevel: 0=sym:x&y, 1=sym:x&one sided:y
 *                                   2=one sided:x&sym:y 3=one sided:x&y
 */
void clear_cube(double x,double y,double z,
                 double dx,double dy,double dz, double th,
                 double scale_x, double scale_y, int bevel,
                 unsigned char color, char texture, int obj_num)
{
   //  Set specular color to white
   int offset = 1;
   float white[] = {1,1,1,1};
   if (color<2 && drawing_renny==0) emission = 100;
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   emission = 0;
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   // bevel or symmetric
   // both x & y one side
   vert0[0] = vert2[0] = scale_x;
   vert0[1] = vert1[1] = scale_y;
   // x symmetric ?
   if (bevel == 0  || bevel == 1)
      vert1[0] = vert3[0] = -scale_x;
   else
      vert1[0] = vert3[0] = -1;
   // y symmetric ?
   if (bevel == 0  ||  bevel == 2)
      vert2[1] = vert3[1] = -scale_y;
   else
      vert2[1] = vert3[1] = -1;

   //FunkyCullahs(0, color);
   if (texture > -1)
   {
      //  Set texture
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);//(obj_num<0?GL_REPLACE:GL_MODULATE));
      if (obj_num<0)
          glColor4f(1,1,1,0.1);
      else
          glColor4f(1,1,1,1);

      glBindTexture(GL_TEXTURE_2D,textures[(int)texture]);
   }
   if (obj_num < 0)
       offset = 0;
   //  Enable blending
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   // draw faces
   // set offset here based on time
   if (renny_prizing == 1 && obj_num == current_prize)
       offset += (t_now - (t_renny_prize - 0.5))*20.0;
   draw_face(norm4, vert3, vert2, vert0, vert1, offset);    //  Front
   draw_face(norm5, vert6, vert7, vert5, vert4, offset);    //  Back
   draw_face(norm0, vert2, vert6, vert4, vert0, offset);    //  Right
   draw_face(norm1, vert5, vert7, vert3, vert1, offset);    //  Left
   draw_face(norm2, vert0, vert4, vert5, vert1, offset);    //  Top
   draw_face(norm3, vert7, vert6, vert2, vert3, offset);    //  Bottom

   //  Undo transformations and textures
   glPopMatrix();
   if (texture > -1)
      glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);
}


void draw_face(float norm[3], float v0[3],
        float v1[3], float v2[3], float v3[3], unsigned char offset)
{

    if (offset != 0)
    {
        float face_push = 0.25*offset;
        glPushMatrix();
          //  Offset
          glTranslated(face_push*norm[0],face_push*norm[1],face_push*norm[2]);
    }
   glBegin(GL_QUADS);
   //printf("norm in: %.4f, %.4f, %.4f\n", norm[0], norm[1], norm[2]);
   calc_norm (norm, v0, v1, v2);
   //printf("norm calc'ed: %.4f, %.4f, %.4f\n", norm[0], norm[1], norm[2]);
   glNormal3fv(norm);
   glTexCoord2f(0,0); glVertex3f(v0[0], v0[1], v0[2]);
   glTexCoord2f(1,0); glVertex3f(v1[0], v1[1], v1[2]);
   glTexCoord2f(1,1); glVertex3f(v2[0], v2[1], v2[2]);
   glTexCoord2f(0,1); glVertex3f(v3[0], v3[1], v3[2]);
   glEnd();
    if (offset != 0)
        glPopMatrix();
}

void calc_norm (float norm[3],float v0[3], float v1[3], float v2[3])
{
   float norm_len = 0;
   // check of zero area faces (only works because of way OUR cube is set above)
   if ((v1[0]-v0[0] == 0  && v1[1]-v0[1] == 0  &&  v1[2]-v0[2] == 0) ||
       (v2[0]-v1[0] == 0  && v2[1]-v1[1] == 0  &&  v2[2]-v1[2] == 0))
       return;

   norm[0] = ((v1[1]-v0[1])*(v2[2]-v1[2]))-((v2[1]-v1[1])*(v1[2]-v0[2]));
   norm[1] = ((v2[0]-v1[0])*(v1[2]-v0[2]))-((v1[0]-v0[0])*(v2[2]-v1[2]));
   norm[2] = ((v1[0]-v0[0])*(v2[1]-v1[1]))-((v2[0]-v1[0])*(v1[1]-v0[1]));
   norm_len = sqrt(norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2]);
   norm[0] /= norm_len;
   norm[1] /= norm_len;
   norm[2] /= norm_len;
}

/*
 *  Draw a 2D-ish tree at (x,y,z)
 */
void TwoDTree(double x,double y,double z)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glScaled(4,4,4);
   // not worrying about normals on trees
   glNormal3f(0.,1.,0.);
   // x/y plane
   //  Trunk
   glColor3f(0.1843,0.3098,0.1843);
   glBegin(GL_POLYGON);
   glVertex3f( 1.0,-1.0, 0.0);
   glVertex3f( 1.0, 2.0, 0.0);
   glVertex3f(-1.0, 2.0, 0.0);
   glVertex3f(-1.0,-1.0, 0.0);
   glEnd();
   glColor3f(0.1333,0.5451,0.1333);
   //  lowest branches
   glBegin(GL_POLYGON);
   glVertex3f( 4.0, 2.0, 0.0);
   glVertex3f( 2.0, 4.0, 0.0);
   glVertex3f(-2.0, 4.0, 0.0);
   glVertex3f(-4.0, 2.0, 0.0);
   glEnd();
   //  middle branches
   glBegin(GL_POLYGON);
   glVertex3f( 3.0, 4.0, 0.0);
   glVertex3f( 0.75, 6.0, 0.0);
   glVertex3f(-0.75, 6.0, 0.0);
   glVertex3f(-3.0, 4.0, 0.0);
   glEnd();
   //  top branches
   glBegin(GL_TRIANGLES);
   glVertex3f( 2.0, 6.0, 0.0);
   glVertex3f( 0.0, 8.0, 0.0);
   glVertex3f(-2.0, 6.0, 0.0);
   glEnd();
   // y/z plane
   //  Trunk
   glColor3f(0.1843,0.3098,0.1843);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0,-1.0, 1.0);
   glVertex3f( 0.0, 2.0, 1.0);
   glVertex3f( 0.0, 2.0,-1.0);
   glVertex3f( 0.0,-1.0,-1.0);
   glEnd();
   glColor3f(0.1333,0.5451,0.1333);
   //  lowest branches
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 2.0, 4.0);
   glVertex3f( 0.0, 4.0, 2.0);
   glVertex3f( 0.0, 4.0,-2.0);
   glVertex3f( 0.0, 2.0,-4.0);
   glEnd();
   //  middle branches
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 4.0, 3.0);
   glVertex3f( 0.0, 6.0, 0.75);
   glVertex3f( 0.0, 6.0,-0.75);
   glVertex3f( 0.0, 4.0,-3.0);
   glEnd();
   //  top branches
   glBegin(GL_TRIANGLES);
   glVertex3f( 0.0, 6.0, 2.0);
   glVertex3f( 0.0, 8.0, 0.0);
   glVertex3f( 0.0, 6.0,-2.0);
   glEnd();
   //  Undo transformations
   glPopMatrix();
}

