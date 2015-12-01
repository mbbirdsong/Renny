/*
 * sphr_shapes.c
 *
 *  Created on: Oct 15, 2011
 *      Author: michaelb
 *
 *  based on and greatly expanded from code in the 'ex8.c' example in
 *  CSCI 5229, Fall 2011, Prof. W. Schreuder
 */

#include "CSCIx229.h"
#include "final_project_mbb.h"
#include "sphr_shapes.h"
#include "cyl_shapes.h"

/*
 *  Draw vertex in polar coordinates with normal
 */
void Vertex(double th,double ph, unsigned char color, unsigned char set_norm, char texture)
{
   double Cos_th = Cos(th);
   double Sin_th = Sin(th);
   double Cos_ph = Cos(ph);
   double Sin_ph = Sin(ph);

   double x = Sin_th*Cos_ph;
   double y =        Sin_ph;
   double z = Cos_th*Cos_ph;
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   if (set_norm)
      glNormal3d(x,y,z);

   if (texture > -1)
   {
      FunkyCullahs(0, 11);
      glTexCoord2d(th/360.0,ph/180.0+0.5);
   }
   else
   {
      if (color == 0)
         glColor3f(Cos_th*Cos_th , Sin_ph*Sin_ph , Sin_th*Sin_th);
      else
         FunkyCullahs(th, color);
   }
   glVertex3d(x,y,z);
}

/*
 *  Draw vertex in polar coordinates for torus (no normals yet)
 */
void Vertex2(double th,double ph, double rr, double xcr, unsigned char color, unsigned char set_norm, char texture)
{
   double Cos_th = Cos(th);
   double Sin_th = Sin(th);
   double Cos_ph = Cos(ph);
   double Sin_ph = Sin(ph);

   if (set_norm)
      glNormal3d(Cos_th*Cos_ph,Sin_ph,Sin_th*Cos_ph);

   if (texture > -1)
   {
      FunkyCullahs(0, 11);
      glTexCoord2d(((float)th)/360.0,((float)ph)/360.0);
   }
   else
   {
      if (color == 0)
         glColor3f(Cos_th*Cos_th , Sin_ph*Sin_ph , Sin_th*Sin_th);
      else
         FunkyCullahs(th, color);
   }

   glVertex3d(Cos_th*(rr+xcr*Cos_ph) , xcr*Sin_ph , Sin_th*(rr+xcr*Cos_ph));
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
void ball(double x,double y,double z,double* r, unsigned char color, char texture)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   if (color<2 && drawing_renny==0) emission = 100;
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   emission = 0;

   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r[0],r[1],r[2]);

   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   if (texture > -1)
   {
      //  Set texture
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,textures[(int)texture]);
      glColor3f(1,1,1);
   }
   //  Latitude bands
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph, color,1, texture);
         Vertex(th,ph+inc, color,1, texture);
      }
      glEnd();
   }
   if (texture > -1)
      glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a partial sphere ( based on version 2)
 *     at (x,y,z)
 *     radius/scale (r) [now a 3D vector]
 *     latitude angle from pole (phi) >-90 & <-90
 *     longitude angle from 0 (theta) >= d & <360
 *     draw flat surfaces (flats) 0=none, 1=const z @ Sin(phi), 2=to sphere center
 *     surface pattern (color)
 */
void partial_sphere(double x,double y,double z,double* r, int* theta, int* phi,
                    unsigned char flats, unsigned char color, char texture)
{
   const int d=draw_angle;
   int th,ph, loc_theta,loc_phi;
   unsigned char sides = 3;
   double flat_center = 0.0;  // value if flats == 2

   float yellow[] = {1.0,1.0,0.0,1.0};
   if (color<2 && drawing_renny==0) emission = 100;
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   emission = 0;

   // validity checks
   // make sure angles are a product of d (5 in this case)
   if (*theta%d != 0)
   {
      *theta = d*(*theta/d);
      //printf("adjusting partial sphere theta value to %d\n", *theta);
   }
   if (*phi%d != 0)
   {
      *phi = d*(*phi/d);
      //printf("adjusting partial sphere phi to %d\n", *phi);
   }

   // range of angle
   if (d > *theta || *theta > 360 ) {
      *theta = 360;
      //printf("bad theta value in partial sphere angle set to 360\n");
   }
   if ((d-90) > *phi || *phi > 90 ) {
      *phi = 0;
      //printf("bad phi value in partial sphere calling sphere2\n");
      ball(x, y, z, r, color, texture);
      return;
   }

   // make sure angles are products of d (5 in this case)
   if (*theta%d != 0)
   {
      *theta = d*(*theta/d);
      //printf("adjusting partial sphere theta value to %d\n", *theta);
   }
   if (*phi%d != 0)
   {
      *phi = d*(*phi/d);
      //printf("adjusting partial sphere phi value to %d\n", *phi);
   }
   // set validated local angle values and # sides to draw
   loc_theta = *theta;
   loc_phi = *phi;
   if (flats == 0)
   {
       sides = 0;
   }
   else
   {
      if (loc_theta == 360)
         sides = 1;
      else if (loc_phi == 90)
         sides = 2;
   }

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r[0],r[1],r[2]);

   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   if (texture > -1)
   {
      //  Set texture
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,textures[(int)texture]);
      glColor3f(1,1,1);
   }

   //  Latitude bands
   for (ph=-90;ph<loc_phi;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=loc_theta;th+=d)
      {
         Vertex(th,ph, color, 1, texture);
         Vertex(th,ph+d, color, 1, texture);
      }
      glEnd();
   }

   // the flat surfaces
   if (flats == 1  &&  sides != 2)
   {
       flat_center = Sin(loc_phi);
   }

   if (sides > 0)
   {
      if (sides > 1)
      {
          // vertical side at final theta angle
         glBegin(GL_TRIANGLE_FAN);
         FunkyCullahs(0, color);
         glNormal3d(Cos(loc_theta),0,-Sin(loc_theta));
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(0.5,0.5);
         }
         glVertex3d(0,flat_center,0);
         for (ph=loc_phi; ph>=-90; ph-=d)
         {
            glNormal3d(Cos(loc_theta),0,-Sin(loc_theta));
            Vertex(loc_theta,ph, color, 0 ,texture);
         }
         glEnd();

         // vertical side at initial theta angle
         glBegin(GL_TRIANGLE_FAN);
         FunkyCullahs(0, color);
         glNormal3d(-1,0,0);
         glVertex3d(0,flat_center,0);
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(0.5,0.5);
         }
         for (ph=-90; ph<=loc_phi; ph+=d)
         {
            glNormal3d(-1,0,0);
            Vertex(0,ph, color, 0, texture);
         }
         glEnd();
      }
      if (sides != 2)
      {
          glBegin(GL_TRIANGLE_FAN);
          FunkyCullahs(0, color);
          glNormal3d(0,1,0);
          if (texture > -1)
          {
             FunkyCullahs(0, 11);
             glTexCoord2d(0.5,0.5);
          }
          glVertex3d(0,flat_center,0);
         // do the "top"
         for (th=0; th<=loc_theta; th+=d)
         {
            glNormal3d(0,1,0);
            Vertex(th,loc_phi, color, 0, texture);
         }
         glEnd();
      }
   }
   if (texture > -1)
      glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a torus (derived from sphere2)
 *     at (x,y,z)
 *     scale/ratio (r) [now a 3D vector]
 *     ring radius (rr)
 *     cross section radius (xcr)
 *     surface pattern (color)
 */
void torus(double x,double y,double z,double* r,double rr,double xcr, unsigned char color, char texture)
{
   const int d=draw_angle;
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   if (color<2 && drawing_renny==0) emission = 100;
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   emission = 0;

   // validation
   if (0 > rr || rr > 2.0)
   {
       rr = 1.0;
       //printf("Bad value entered for ring radius, reset rr = 1.0");
   }
   if (rr < xcr || xcr > 2.0)
   {
       xcr = 0.5;
       //printf("Bad value entered for ring cross section radius, reset xcr = 0.5");
   }

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r[0],r[1],r[2]);

   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

   if (texture > -1)
   {
      //  Set texture
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,textures[(int)texture]);
      glColor3f(1,1,1);
   }

   glBegin(GL_QUAD_STRIP);

   for (th=0;th<360;th+=d)
   {
      for (ph=0;ph<=360;ph+=d)
      {
         Vertex2(th,ph,rr,xcr, color, 1, texture);
         Vertex2(th+d,ph,rr,xcr, color, 1, texture);
      }
   }
   glEnd();
   if (texture > -1)
      glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a partial torus (derived from torus, partial_cylinder & partial_sphere)
 *     at (x,y,z)
 *     scale/ratio (r) [now a 3D vector]
 *     ring radius (rr)
 *     cross section radius (xcr)
 *     beginning angle of ring (theta_min)
 *     beginning angle of ring cross section (phi_min)
 *     ending angle of ring (theta_max)
 *     ending angle of ring cross section (phi_max)
 *     draw flat surfaces (flats) 0=none, 1=ends only,
 *                                2=planes for edges from cyl edge to center
 *                                3=ends & planes for edges from cyl edge to center
 *     surface pattern (color)
 */
void partial_torus(double x,double y,double z,double* r,double rr,double xcr,
                   int* theta_min, int* phi_min, int* theta_max, int* phi_max,
                   unsigned char flats, unsigned char color, char texture)
{
   const int d=draw_angle;
   int d_th=10;
   int th,ph, loc_th_min,loc_ph_min, loc_th_max,loc_ph_max;
   double temp_swap;

   float yellow[] = {1.0,1.0,0.0,1.0};
   if (color<2 && drawing_renny==0) emission = 100;
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   emission = 0;

   // validation
   if (0 > rr || rr > 2.0)
   {
       rr = 0.5;
       //printf("Bad value entered for ring radius, reset rr = 0.5\n");
   }
   if (rr < xcr || xcr > 2.0)
   {
       xcr = 0.25;
       //printf("Bad value entered for ring cross section radius, reset xcr = 0.25\n");
   }

   // make sure angles are a product of d (5 in this case)
   if (*theta_max%d_th != 0)
   {
      *theta_max = d_th*(*theta_max/d_th);
      //printf("adjusting partial torus theta_max value to %d\n", *theta_max);
   }
   if (*theta_min%d_th != 0)
   {
      *theta_min = d_th*(*theta_min/d_th);
      //printf("adjusting partial torus theta_min value to %d\n", *theta_min);
   }
   if (*phi_max%d != 0)
   {
      *phi_max = d*(*phi_max/d);
      //printf("adjusting partial torus phi_max value to %d\n", *phi_max);
   }
   if (*phi_min%d != 0)
   {
      *phi_min = d*(*phi_min/d);
      //printf("adjusting partial torus phi_min value to %d\n", *phi_min);
   }

   if (*theta_max < *theta_min)
   {
      temp_swap = *theta_max;
      *theta_max = *theta_min;
      *theta_min = temp_swap;
      //printf("theta_max was less than theta_min, swapped values\n");
   }

   if (*phi_max < *phi_min)
   {
      temp_swap = *phi_max;
      *phi_max = *phi_min;
      *phi_min = temp_swap;
      //printf("phi_max was less than phi_min, swapped values\n");
   }

   // do not draw unneeded flat surfaces
   if (*phi_max == *phi_min)
   {
       *phi_max = 360;
       *phi_min = 0;
       if (flats == 2) flats = 0;
       if (flats == 3) flats = 1;
       //printf("phi_max equal to phi_min, drawing full ring cross section\n");
   }
   if (*theta_max == *theta_min)
   {
       *theta_max = 360;
       *theta_min = 0;
       if (flats == 1) flats = 0;
       if (flats == 3) flats = 2;
       //printf("theta_max equal to theta_min, drawing full ring\n");
   }

   loc_th_min = *theta_min;
   loc_ph_min = *phi_min;
   loc_th_max = *theta_max;
   loc_ph_max = *phi_max;

   if ((loc_th_max-loc_th_min) == 360)
       d_th = draw_angle;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r[0],r[1],r[2]);

   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   if (texture > -1)
   {
      //  Set texture
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,textures[(int)texture]);
      glColor3f(1,1,1);
   }
//   glBegin(GL_QUAD_STRIP);

   for (th=loc_th_min;th<loc_th_max;th+=d_th)
   {
      glBegin(GL_QUAD_STRIP);
      for (ph=loc_ph_min;ph<=loc_ph_max;ph+=d)
      {
         Vertex2(th,ph,rr,xcr,color, 1, texture);
         Vertex2(th+d,ph,rr,xcr,color, 1, texture);
      }
      glEnd();
      if (flats == 2  ||  flats == 3)
      {
          /* normals correct? */
          glBegin(GL_QUAD_STRIP);
          glNormal3d(-Cos(th)*Sin(loc_ph_max),Cos(loc_ph_max),-Sin(th)*Sin(loc_ph_max));
          Vertex2(th,loc_ph_max,rr,xcr,color, 0, texture);
          Vertex2(th+d,loc_ph_max,rr,xcr,color, 0, texture);
          Vertex2(th,loc_ph_max,rr,0,color, 0, texture);
          Vertex2(th+d,loc_ph_max,rr,0,color, 0, texture);
          glEnd();
          glBegin(GL_QUAD_STRIP);
          glNormal3d(Cos(th)*Sin(loc_ph_min),-Cos(loc_ph_min),Sin(th)*Sin(loc_ph_min));
          Vertex2(th,loc_ph_max,rr,0,color, 0, texture);
          Vertex2(th+d,loc_ph_max,rr,0,color, 0, texture);
          Vertex2(th,loc_ph_min,rr,xcr,color, 0, texture);
          Vertex2(th+d,loc_ph_min,rr,xcr,color, 0, texture);
          glEnd();
      }
   }

   if (flats == 1  ||  flats == 3)
   {
      //  One end
      glBegin(GL_TRIANGLE_FAN);
      FunkyCullahs(0, color);
      glNormal3d(Sin(loc_th_min),0,-Cos(loc_th_min));
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(0.5,0.5);
      }
      glVertex3d(rr*Cos(loc_th_min),0,rr*Sin(loc_th_min));
      for (ph=loc_ph_min;ph<=loc_ph_max;ph+=d)
      {
          Vertex2(loc_th_min,ph,rr,xcr,color, 0, texture);
      }
      glEnd();

      //  Other end
      glBegin(GL_TRIANGLE_FAN);
      FunkyCullahs(0, color);
      glNormal3d(Sin(loc_th_max),0,Cos(loc_th_max));
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(0.5,0.5);
      }
      glVertex3d(rr*Cos(loc_th_max),0,rr*Sin(loc_th_max));
      for (ph=loc_ph_max;ph>=loc_ph_min;ph-=d)
      {
          Vertex2(loc_th_max,ph,rr,xcr,color, 0, texture);
      }
      glEnd();
   }
   if (texture > -1)
      glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a partial torus (derived from torus, partial_cylinder & partial_sphere)
 *     at (x,y,z)
 *     scale/ratio (r) [now a 3D vector]
 *     ring radius (rr)
 *     initial cross section radius (xcr)
 *     final cross section radius (xcr2)
 *     beginning angle of ring (theta_min)
 *     beginning angle of ring cross section (phi_min)
 *     ending angle of ring (theta_max)
 *     ending angle of ring cross section (phi_max)
 *     draw flat surfaces (flats) 0=none, 1=ends only,
 *                                2=planes for edges from cyl edge to center
 *                                3=ends & planes for edges from cyl edge to center
 *     surface pattern (color)
 */
void taper_partial_torus(double x,double y,double z,double* r,double rr,double xcr,
                   double xcr2,int* theta_min, int* phi_min, int* theta_max, int* phi_max,
                   unsigned char flats, unsigned char color, char texture)
{
   const int d=draw_angle;
   int th,ph, loc_th_min,loc_ph_min, loc_th_max,loc_ph_max;
   double temp_swap;

   float yellow[] = {1.0,1.0,0.0,1.0};
   if (color<2 && drawing_renny==0) emission = 100;
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   emission = 0;

   // validation
   if (0 > rr || rr > 2.0)
   {
       rr = 0.5;
       //printf("Bad value entered for ring radius, reset rr = 0.5\n");
   }
   if (rr < xcr || xcr > 2.0)
   {
       xcr = 0.25;
       //printf("Bad value entered for ring cross section radius, reset xcr = 0.25\n");
   }

   // make sure angles are a product of d (5 in this case)
   if (*theta_max%d != 0)
   {
      *theta_max = d*(*theta_max/d);
      //printf("adjusting partial torus theta_max value to %d\n", *theta_max);
   }
   if (*theta_min%d != 0)
   {
      *theta_min = d*(*theta_min/d);
      //printf("adjusting partial torus theta_min value to %d\n", *theta_min);
   }
   if (*phi_max%d != 0)
   {
      *phi_max = d*(*phi_max/d);
      //printf("adjusting partial torus phi_max value to %d\n", *phi_max);
   }
   if (*phi_min%d != 0)
   {
      *theta_min = d*(*phi_min/d);
      //printf("adjusting partial torus phi_min value to %d\n", *phi_min);
   }

   if (*theta_max < *theta_min)
   {
      temp_swap = *theta_max;
      *theta_max = *theta_min;
      *theta_min = temp_swap;
      //printf("theta_max was less than theta_min, swapped values\n");
   }

   if (*phi_max < *phi_min)
   {
      temp_swap = *phi_max;
      *phi_max = *phi_min;
      *phi_min = temp_swap;
      //printf("phi_max was less than phi_min, swapped values\n");
   }

   // do not draw unneeded flat surfaces
   if (*phi_max == *phi_min)
   {
       *phi_max = 360;
       *phi_min = 0;
       if (flats == 2) flats = 0;
       if (flats == 3) flats = 1;
       //printf("phi_max equal to phi_min, drawing full ring cross section\n");
   }
   if (*theta_max == *theta_min)
   {
       *theta_max = 360;
       *theta_min = 0;
       if (flats == 1) flats = 0;
       if (flats == 3) flats = 2;
       //printf("theta_max equal to theta_min, drawing full ring\n");
   }

   loc_th_min = *theta_min;
   loc_ph_min = *phi_min;
   loc_th_max = *theta_max;
   loc_ph_max = *phi_max;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r[0],r[1],r[2]);

   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   if (texture > -1)
   {
      //  Set texture
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,textures[(int)texture]);
      glColor3f(1,1,1);
   }

   for (th=loc_th_min;th<loc_th_max;th+=d)
   {
      float xcr_a = xcr - (xcr-xcr2)*(th-loc_th_min)/(loc_th_max-loc_th_min);
      float xcr_b = xcr - (xcr-xcr2)*(th+d-loc_th_min)/(loc_th_max-loc_th_min);
      glBegin(GL_QUAD_STRIP);
      for (ph=loc_ph_min;ph<=loc_ph_max;ph+=d)
      {
         Vertex2(th,ph,rr,xcr_a,color, 1, texture);
         Vertex2(th+d,ph,rr,xcr_b,color, 1, texture);
      }
      glEnd();
      if (flats == 2  ||  flats == 3)
      {
          /* normals correct? */
          glBegin(GL_QUAD_STRIP);
          glNormal3d(-Cos(th)*Sin(loc_ph_max),Cos(loc_ph_max),-Sin(th)*Sin(loc_ph_max));
          Vertex2(th,loc_ph_max,rr,xcr_a,color, 0, texture);
          Vertex2(th+d,loc_ph_max,rr,xcr_b,color, 0, texture);
          Vertex2(th,loc_ph_max,rr,0,color, 0, texture);
          Vertex2(th+d,loc_ph_max,rr,0,color, 0, texture);
          glEnd();
          glBegin(GL_QUAD_STRIP);
          glNormal3d(Cos(th)*Sin(loc_ph_min),-Cos(loc_ph_min),Sin(th)*Sin(loc_ph_min));
          Vertex2(th,loc_ph_max,rr,0,color, 0, texture);
          Vertex2(th+d,loc_ph_max,rr,0,color, 0, texture);
          Vertex2(th,loc_ph_min,rr,xcr_a,color, 0, texture);
          Vertex2(th+d,loc_ph_min,rr,xcr_b,color, 0, texture);
          glEnd();
      }
   }

   if (flats == 1  ||  flats == 3)
   {
      //  One end
      glBegin(GL_TRIANGLE_FAN);
      FunkyCullahs(0, color);
      glNormal3d(Sin(loc_th_min),0,-Cos(loc_th_min));
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(0.5,0.5);
      }
      glVertex3d(rr*Cos(loc_th_min),0,rr*Sin(loc_th_min));
      for (ph=loc_ph_min;ph<=loc_ph_max;ph+=d)
      {
          Vertex2(loc_th_min,ph,rr,xcr,color, 0, texture);
      }
      glEnd();

      //  Other end
      glBegin(GL_TRIANGLE_FAN);
      FunkyCullahs(0, color);
      glNormal3d(-Sin(loc_th_max),0,Cos(loc_th_max));
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(0.5,0.5);
      }
      glVertex3d(rr*Cos(loc_th_max),0,rr*Sin(loc_th_max));
      for (ph=loc_ph_max;ph>=loc_ph_min;ph-=d)
      {
          Vertex2(loc_th_max,ph,rr,xcr2,color, 0, texture);
      }
      glEnd();
   }
   if (texture > -1)
      glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}
