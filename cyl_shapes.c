/*
 * cyl_shapes.c
 *
 *  Created on: Oct 15, 2011
 *      Author: michael birdsong
 *
 *  based on and greatly expanded from code in the 'ex12.c' example in
 *  CSCI 5229, Fall 2011, Prof. W. Schreuder
 *
 */

#include "CSCIx229.h"
#include "final_project_mbb.h"
#include "cyl_shapes.h"

double rd = 0.5;
double gr = 0.5;
double bl = 0.5;

/*
 *  Setting Colors for cylinders (based only on theta) or just solid
 */
void FunkyCullahs(int th, unsigned char option)
{
   double dark_factor = 1.0;

   if (option == 255)
       return;

   if (option == 0)
   {
      // Funk #49
      rd = 0.3*(Cos(th) + 1.0 );
      gr = 0.4*(Cos(2.0*th) + 1.0 );
      bl = 0.3*(Cos(0.5*th) + 1.0 );
   }
   else if (option == 1)
   {
      // Funk #42
      rd = 0.3*(Cos(th) + 1.0 );
      gr = 0.4*(Cos(2.0*th) + 1.0 );
      bl = 0.3*(Sin(0.5*th) + 1.0 );
   }
   else if (option == 2)
   {
      rd = 1; gr = 0; bl = 0;
   }
   else if (option == 3)
   {
      rd = 1; gr = 1; bl = 0;
   }
   else if (option == 4)
   {
      rd = 0; gr = 1; bl = 0;
   }
   else if (option == 5)
   {
      rd = 0; gr = 1; bl = 1;
   }
   else if (option == 6)
   {
      rd = 0; gr = 0; bl = 1;
   }
   else if (option == 7)
   {
      rd = 1; gr = 0; bl = 1;
   }
   else if (option == 8)
   {
      rd = 0.1; gr = 0.1; bl = 0.1;
   }
   else if (option == 9)
   {
      rd = 0.4; gr = 0.4; bl = 0.4;
   }
   else if (option == 10)
   {
      rd = 0.7; gr = 0.7; bl = 0.7;
   }
   else if (option == 11)
   {
      rd = 1.0; gr = 1.0; bl = 1.0;
   }

   // set selected color
   if (glb_light_level < 0.5  &&  option > 1)
       dark_factor = 1.96*glb_light_level + 0.02;

   glColor3f(dark_factor*rd, dark_factor*gr, dark_factor*bl);
}

/*
 *  Draw a cone
 *     at (x,y,z)
 *     radius (r)
 *     x & y ratio/scale (r)
 *     z ratio/scale (l)
 *     draw base (flat) 0=FALSE, 1=TRUE
 *     surface pattern (color)
 */
void cone(double x,double y,double z,double r, double l, unsigned char flat, unsigned char color, char texture)
{
    chopped_cone(x, y, z, r, l, 0, flat, color, texture);
}


/*
 *  Draw a chopped cone / non-parallel cylinder
 *     at (x,y,z)
 *     radius (r)
 *     length (l)
 *     diameter of second end (d2) [first end has diameter==1.0]
 *     draw ends (flats) 0=none, 1=first end, 2=second end, 3=both ends
 *     surface pattern (color)
 */
void chopped_cone(double x,double y,double z,double r, double l, double d2,
                  unsigned char flats, unsigned char color, char texture)
{
   const int d=draw_angle;
   int th;
   double Cos_th = 0;
   double Sin_th = 0;
   double cosConeAngle = 0;
   double sinConeAngle = 1;
   float yellow[] = {1.0,1.0,0.0,1.0};
   if (color<2 && drawing_renny==0) emission = 100;
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   emission = 0;


   // validity check
   if (0 > d2)
   {
      //printf("bad value for smaller diameter end of chopped cone, reset to 0.5\n");
      d2 = 0.5;
   }
   cosConeAngle = cos(atan(2/(1-d2)));
   sinConeAngle = sin(atan(2/(1-d2)));
   //printf("Chopped Cone Side normals: x = %.3f, y = %.3f\n", cosConeAngle, sinConeAngle);

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,l);

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
   //  One end
   if (flats==1 || flats==3)
   {
      glBegin(GL_TRIANGLE_FAN);
      FunkyCullahs(0, color);
      glNormal3d(0,0,1);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(0.5,0.5);
      }
      glVertex3d(0,0,1);
      for (th=0;th<=360;th+=d)
      {
          Cos_th = Cos(th);
          Sin_th = Sin(th);
          FunkyCullahs(th, color);
          glNormal3d(0,0,1);
          if (texture > -1)
          {
             FunkyCullahs(0, 11);
             glTexCoord2d(0.5*(Cos_th+1),0.5*(Sin_th+1));  //flat mapping, correct?
          }
          glVertex3d(Cos_th,Sin_th,1);
      }
      glEnd();
   }

   //  the cylinder
   glBegin(GL_QUAD_STRIP);
   for (th=0;th<=360;th+=d)
   {
       Cos_th = Cos(th);
       Sin_th = Sin(th);
       FunkyCullahs(th, color);

       glNormal3d(sinConeAngle*Cos_th,sinConeAngle*Sin_th,-cosConeAngle);
       if (texture > -1)
       {
          FunkyCullahs(0, 11);
          //glTexCoord2d(0.5*(Cos_th+1),0.5*(Sin_th+1));  //flat mapping, correct?
          glTexCoord2d(((float)th)/360,0);
       }
       glVertex3d(Cos_th,Sin_th,1);
       if (texture > -1)
          //glTexCoord2d(0.5*(d2*Cos_th+1),d2*0.5*(Sin_th+1));  //flat mapping, correct?
          glTexCoord2d(((float)th)/360,1);
       glVertex3d(d2*Cos_th,d2*Sin_th,-1);
   }
   glEnd();

   //  Other end
   if (flats==2 || flats==3)
   {
      glBegin(GL_TRIANGLE_FAN);
      glNormal3d(0,0,-1);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(0.5,0.5);
      }
      glVertex3d(0,0,-1);
      for (th=360;th>=0;th-=d)
      {
         Cos_th = Cos(th);
         Sin_th = Sin(th);
         FunkyCullahs(th, color);
         glNormal3d(0,0,-1);
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(d2*0.5*(Cos_th+1),d2*0.5*(Sin_th+1));  //flat mapping, correct?
         }
         glVertex3d(d2*Cos_th,d2*Sin_th,-1);
      }
      glEnd();
   }
   if (texture > -1)
      glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a cylinder
 *     at (x,y,z)
 *     radius (r)
 *     length (l)
 *     draw ends (flats) 0=none, 1=first end, 2=second end, 3=both ends
 *     surface pattern (color)
 */
void cylinder(double x,double y,double z,double r, double l, unsigned char flats,
              unsigned char color, char texture)
{
   const int d=draw_angle;
   int th;
   double Cos_th = 0;
   double Sin_th = 0;
   float yellow[] = {1.0,1.0,0.0,1.0};
   if (color<2 && drawing_renny==0) emission = 100;
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   emission = 0;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,l);

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
   //  One end
   if (flats==1 || flats==3)
   {
      glBegin(GL_TRIANGLE_FAN);
      FunkyCullahs(0, color);
      glNormal3d(0,0,1);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(0.5,0.5);
      }
      glVertex3d(0,0,1);
      for (th=0;th<=360;th+=d)
      {
         Cos_th = Cos(th);
         Sin_th = Sin(th);
         FunkyCullahs(th, color);
         glNormal3d(0,0,1);
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(0.5*(Cos_th+1),0.5*(Sin_th+1));  //flat mapping, correct?
         }
         glVertex3d(Cos_th,Sin_th,1);
      }
      glEnd();
   }

   //  the cylinder
   glBegin(GL_QUAD_STRIP);
   for (th=0;th<=360;th+=d)
   {
      FunkyCullahs(th, color);
      Cos_th = Cos(th);
      Sin_th = Sin(th);

      glNormal3d(Cos_th,Sin_th,0);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(((float)th)/360,0);
      }
      glVertex3d(Cos_th,Sin_th,1);
      if (texture > -1)
         glTexCoord2d(((float)th)/360,1);
      glVertex3d(Cos_th,Sin_th,-1);
   }
   glEnd();

   //  Other end
   if (flats==2 || flats==3)
   {
      glBegin(GL_TRIANGLE_FAN);
      glNormal3d(0,0,-1);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(0.5,0.5);
      }
      glVertex3d(0,0,-1);
      for (th=360;th>=0;th-=d)
      {
         Cos_th = Cos(th);
         Sin_th = Sin(th);
         FunkyCullahs(th, color);
         glNormal3d(0,0,-1);
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(0.5*(Cos_th+1),0.5*(Sin_th+1));  //flat mapping, correct?
         }
         glVertex3d(Cos_th,Sin_th,-1);
      }
      glEnd();
   }
   if (texture > -1)
      glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a pipe or ring (depending on scaling/aspect ratio)
 *     at (x,y,z)
 *     scale (r) [now a 3D vector]
 *     outer radius (ro)
 *     inner radius (ri)
 *     length (l)
 *     surface pattern (color)
 */
void cyl_pipe(double x,double y,double z,double* r, double ri, double ro, double l,
          unsigned char color, char texture)
{
   const int d=draw_angle;
   int th;
   double swap = 0;
   double Cos_th = 0;
   double Sin_th = 0;
   float yellow[] = {1.0,1.0,0.0,1.0};
   if (color<2 && drawing_renny==0) emission = 100;
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   emission = 0;

   // validation
   if (ro == ri)
   {
       ri = 0.5*ro;
       //printf("outer and inner radii were equal, now inner radius = 0.5 * outer radius\n");
   }
   else if (ri > ro )
   {
       swap = ri;
       ri = ro;
       ro = swap;
       //printf("outer radius >  inner radius now swapped\n");
   }

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r[0],r[1],l);

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

   //  One end
   glBegin(GL_QUAD_STRIP);
   for (th=0;th<=360;th+=d)
   {
      Cos_th = Cos(th);
      Sin_th = Sin(th);
      FunkyCullahs(th, color);
      glNormal3d(0,0,1);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(ri*0.5*(Cos_th+1),ri*0.5*(Sin_th+1));  //flat mapping, correct?
      }
      glVertex3d(ri*Cos_th,ri*Sin_th,1);
      if (texture > -1)
         glTexCoord2d(ro*0.5*(Cos_th+1),ro*0.5*(Sin_th+1));  //flat mapping, correct?
      glVertex3d(ro*Cos_th,ro*Sin_th,1);
   }
   glEnd();

   //  the outer cylinder
   glBegin(GL_QUAD_STRIP);
   for (th=0;th<=360;th+=d)
   {
      Cos_th = Cos(th);
      Sin_th = Sin(th);

      glNormal3d(Cos_th,Sin_th,0);
      FunkyCullahs(th, color);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(((float)th)/360,0);
      }
      glVertex3d(ro*Cos_th,ro*Sin_th,1);
      if (texture > -1)
         glTexCoord2d(((float)th)/360,1);
      glVertex3d(ro*Cos_th,ro*Sin_th,-1);
   }
   glEnd();

   //  the inner cylinder
   glBegin(GL_QUAD_STRIP);
   for (th=360;th>=0;th-=d)
   {
      Cos_th = Cos(th);
      Sin_th = Sin(th);

      FunkyCullahs(th, color);
      glNormal3d(-Cos_th,-Sin_th,0);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(((float)th)/360,0);
      }
      glVertex3d(ri*Cos_th,ri*Sin_th,1);
      if (texture > -1)
         glTexCoord2d(((float)th)/360,1);
      glVertex3d(ri*Cos_th,ri*Sin_th,-1);
   }
   glEnd();

   //  Other end
   glBegin(GL_QUAD_STRIP);
   for (th=360;th>=0;th-=d)
   {
      Cos_th = Cos(th);
      Sin_th = Sin(th);
      FunkyCullahs(th, color);
      glNormal3d(0,0,-1);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(ri*0.5*(Cos_th+1),ri*0.5*(Sin_th+1));  //flat mapping, correct?
      }
      glVertex3d(ri*Cos_th,ri*Sin_th,-1);
      if (texture > -1)
         glTexCoord2d(ro*0.5*(Cos_th+1),ro*0.5*(Sin_th+1));  //flat mapping, correct?
      glVertex3d(ro*Cos_th,ro*Sin_th,-1);
   }
   glEnd();
   if (texture > -1)
      glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw cylinder with spout/notch (parallel or non-parallel cylinder)
 *     at (x,y,z)
 *     radius (r)
 *     length (l)
 *     diameter of second end (d2) [first end has diameter==1.0]
 *     depth of spout (spt_x) ratio w.r.t. d2
 *     length of spout (spt_z) ratio w.r.t. l
 *     width of spout (spt_w) angle
 *     location of spout (spt_th) angle to spout centerline
 *     draw ends (flats) 0=none, 1=first end, 2=second end, 3=both ends
 *     surface pattern (color)
 */
void notched_cylinder(double x,double y,double z,double r, double l, double d2,
        double spt_x, double spt_z, int spt_w, int spt_th, unsigned char flats,
        unsigned char color, char texture)
{
   const int d=draw_angle;
   int th, th_spt;
   double Cos_th = 0;
   double Sin_th = 0;
   double cosConeAngle = 0;
   double sinConeAngle = 1;
   double A[3] = {0,0,0};
   double A0[3] = {0,0,0};
   double B[3] = {0,0,0};
   double spt_nrml[3] = {0,0,0};
   float yellow[] = {1.0,1.0,0.0,1.0};
   if (color<2 && drawing_renny==0) emission = 100;
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   emission = 0;

   double notch_z = 2*spt_z - 1;
   double slope = 0.5*(1-d2);
   double b = 0.5*(1+d2);
   double r_at_notch = slope*notch_z + b;
   int spout_max = 0;
   int spout_min = 0;

   // validity check
   if (0 > d2  || d2 > 1)
   {
      //printf("bad value for smaller diameter, reset to 0.5\n");
      d2 = 0.5;
   }
   // make sure angles are products of d (5 in this case)
   if (spt_w%d != 0)
   {
      spt_w = d*(spt_w/d);
      //printf("adjusting spout width angle value to %d\n", spt_w);
   }
   if (spt_th%d != 0)
   {
      spt_th = d*(spt_th/d);
      //printf("adjusting spout location angle value to %d\n", spt_th);
   }
   spout_min = d*((spt_th - spt_w/2)/d);
   spout_max = 2*spt_th- spout_min;

   cosConeAngle = cos(atan(2/(1-d2)));
   sinConeAngle = sin(atan(2/(1-d2)));

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,l);

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

   //  One end
   if (flats==1 || flats==3)
   {
      glBegin(GL_TRIANGLE_FAN);
      FunkyCullahs(0, color);
      glNormal3d(0,0,1);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(0.5,0.5);
      }
      glVertex3d(0,0,1);
      for (th=0;th<=360;th+=d)
      {
         FunkyCullahs(th, color);
         Cos_th = Cos(th);
         Sin_th = Sin(th);
         glNormal3d(0,0,1);
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(0.5*(Cos_th+1),0.5*(Sin_th+1));  //flat mapping, correct?
         }
         glVertex3d(Cos_th,Sin_th,1);
      }
      glEnd();
   }

   //  the smooth part of cylinder
   glBegin(GL_QUAD_STRIP);
   for (th=0;th<=360;th+=d)
   {
       Cos_th = Cos(th);
       Sin_th = Sin(th);
       FunkyCullahs(th, color);

       glNormal3d(sinConeAngle*Cos_th,sinConeAngle*Sin_th,-cosConeAngle);
       if (texture > -1)
       {
          FunkyCullahs(0, 11);
          glTexCoord2d(((float)th)/360,0);
       }
       glVertex3d(Cos_th,Sin_th,1);
       if (texture > -1)
          glTexCoord2d(((float)th)/360,(1-spt_z));
       glVertex3d(r_at_notch*Cos_th,r_at_notch*Sin_th,notch_z);
   }
   glEnd();

   //  the notch part of cylinder (single sweep method, working)
   glBegin(GL_QUAD_STRIP);
   for (th=0;th<=360;th+=d)
   {
      Cos_th = Cos(th);
      Sin_th = Sin(th);

      FunkyCullahs(th, color);

      if (th <= spout_min  ||  th >= spout_max)
      {
         glNormal3d(sinConeAngle*Cos_th,sinConeAngle*Sin_th,-cosConeAngle);
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(((float)th)/360,(1-spt_z));
         }
         glVertex3d(r_at_notch*Cos_th,r_at_notch*Sin_th,notch_z);
         if (texture > -1)
            glTexCoord2d(((float)th)/360,1);
         glVertex3d(d2*Cos_th,d2*Sin_th,-1);
      }
      else if (th > spout_min  && th < spt_th)
      {
         glNormal3d(sinConeAngle*Cos_th,sinConeAngle*Sin_th,-cosConeAngle);
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(((float)th)/360,(1-spt_z));
         }
         glVertex3d(r_at_notch*Cos_th,r_at_notch*Sin_th,notch_z);
         if (texture > -1)
            glTexCoord2d(((float)th)/360,1);
         glVertex3d(d2*Cos(spout_min),d2*Sin(spout_min),-1);
         A0[0] = d2*Cos(spout_min);
         A0[1] = d2*Sin(spout_min);
      }
      else if (th < spout_max  && th > spt_th)
      {
         glNormal3d(sinConeAngle*Cos_th,sinConeAngle*Sin_th,-cosConeAngle);
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(((float)th)/360,(1-spt_z));
         }
         glVertex3d(r_at_notch*Cos_th,r_at_notch*Sin_th,notch_z);
         if (texture > -1)
            glTexCoord2d(((float)th)/360,1);
         glVertex3d(d2*Cos(spout_max),d2*Sin(spout_max),-1);
      }
      else if (th == spt_th)
      {
         double spout_r = d2;
         double spout_ang = (double)(spout_max - spt_th);
         double dbl_spt_min = (double) spout_min;
         double spout_cntr = spt_th;
         double spout_theta = 0.0;
         double nrml_mag = 1.0;
         // deal with first th == spt_th here
         glNormal3d(sinConeAngle*Cos_th,sinConeAngle*Sin_th,-cosConeAngle);
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(((float)th)/360,(1-spt_z));
         }
         glVertex3d(r_at_notch*Cos_th,r_at_notch*Sin_th,notch_z);
         if (texture > -1)
            glTexCoord2d(((float)th)/360,1);
         glVertex3d(d2*Cos(spout_min),d2*Sin(spout_min),-1);

         for (th_spt=spout_min; th_spt<=spout_max; th_spt+=d)
         {
             FunkyCullahs(th_spt, color);
             spout_theta = (double) th_spt;
             if (th_spt <= spt_th)
                spout_r = d2*(1+spt_x*(spout_theta-dbl_spt_min)/spout_ang);
             else
                spout_r = d2*(1+spt_x*(1.0+(spout_cntr-spout_theta)/spout_ang));

             // find glNormal3d(x,y,z);
             A[0] = A0[0] - spout_r*Cos(th_spt);
             A[1] = A0[1] - spout_r*Sin(th_spt);
             A[2] = 0;

             B[0] = spout_r*Cos(th_spt) - r_at_notch*Cos_th;
             B[1] = spout_r*Sin(th_spt) - r_at_notch*Sin_th;
             B[2] = -1 - notch_z;

             spt_nrml[0] = A[1]*B[2] - B[1]*A[2];
             spt_nrml[1] = A[2]*B[0] - A[0]*B[2];
             spt_nrml[2] = A[0]*B[1] - A[1]*B[0];

             nrml_mag = sqrt(spt_nrml[0]*spt_nrml[0] + spt_nrml[1]*spt_nrml[1] + spt_nrml[2]*spt_nrml[2]);
             spt_nrml[0] /= nrml_mag;
             spt_nrml[1] /= nrml_mag;
             spt_nrml[2] /= nrml_mag;
             glNormal3d(spt_nrml[0],spt_nrml[1],spt_nrml[2]);

             A0[0] = spout_r*Cos(th_spt);
             A0[1] = spout_r*Sin(th_spt);

             if (texture > -1)
             {
                FunkyCullahs(0, 11);
                glTexCoord2d(((float)th)/360,(1-spt_z));
             }
             glVertex3d(r_at_notch*Cos_th,r_at_notch*Sin_th,notch_z);
             if (texture > -1)
                glTexCoord2d(((float)th_spt)/360,1);
             glVertex3d(spout_r*Cos(th_spt),spout_r*Sin(th_spt),-1);
         }
         // deal with first th == spt_th here
         glNormal3d(sinConeAngle*Cos_th,sinConeAngle*Sin_th,-cosConeAngle);
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(((float)th)/360,(1-spt_z));
         }
         glVertex3d(r_at_notch*Cos_th,r_at_notch*Sin_th,notch_z);
         if (texture > -1)
            glTexCoord2d(((float)th)/360,1);
         glVertex3d(d2*Cos(spout_max),d2*Sin(spout_max),-1);

      }
   }
   glEnd();

   //  Other end
   if (flats==2 || flats==3)
   {
      double spout_r = d2;
      double spout_ang = (double)(spout_max - spt_th);
      double dbl_spt_min = (double) spout_min;
      double spout_cntr = spt_th;
      double spout_theta = 0.0;

      glBegin(GL_TRIANGLE_FAN);
      glNormal3d(0,0,-1);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(0.5,0.5);
      }
      glVertex3d(0,0,-1);
      for (th=360;th>=0;th-=d)
      {
          FunkyCullahs(th, color);
         glNormal3d(0,0,-1);
         Cos_th = Cos(th);
         Sin_th = Sin(th);
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(0.5*(Cos_th+1),0.5*(Sin_th+1));  //flat mapping, correct?
         }
         if (th> spout_min  &&  th < spout_max)
         {
            spout_theta = (double) th;
            if (th <= spt_th)
               spout_r = d2*(1+spt_x*(spout_theta-dbl_spt_min)/spout_ang);
            else
               spout_r = d2*(1+spt_x*(1.0+(spout_cntr-spout_theta)/spout_ang));
         }
         else
            spout_r = d2;

         glVertex3d(spout_r*Cos_th,spout_r*Sin_th,-1);
      }
      glEnd();
   }
   if (texture > -1)
      glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}


/*
 *  Draw a partial cylinder, partial w.r.t. the circular cross-section
 *     at (x,y,z)
 *     radius (r)
 *     length (l)
 *     angle (angle), 5 <= angle <= 355
 *     draw flat surfaces (flats) 0=none, 1=ends only,
 *                                2=ends & planes for edges from cyl edge to center
 *     surface pattern (color)
 */
void partial_cylinder(double x,double y,double z,double r, double l, int* angle,
                      unsigned char flats, unsigned char color, char texture)
{
   const int d=draw_angle;
   int th;
   int loc_ang;
   double Cos_th = 0;
   double Sin_th = 0;
   float yellow[] = {1.0,1.0,0.0,1.0};
   if (color<2 && drawing_renny==0) emission = 100;
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   emission = 0;


   // validity checks
   // range of angle
   if (d > *angle || *angle >= 360 ) {
      *angle = 0;
      //printf("bad partial cylinder angle value, calling cylinder");
      if (flats > 0) flats = 3;
      cylinder(x, y, z, r, l, flats, color, texture);
      return;
   }
   // make sure angle is a product of d (5 in this case)
   if (*angle%d != 0)
   {
      *angle = d*(*angle/d);
      //printf("adjusting partial cylinder angle value to %d\n", *angle);
   }
   loc_ang = *angle;

   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,l);

   if (texture > -1)
   {
      //  Set texture
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,textures[(int)texture]);
      glColor3f(1,1,1);
   }

   if (flats > 0)
   {
      //  One end
      glBegin(GL_TRIANGLE_FAN);
      FunkyCullahs(0, color);
      glNormal3d(0,0,1);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(0.5,0.5);
      }
      glVertex3d(0,0,1);
      for (th=0;th<=loc_ang;th+=d)
      {
         Cos_th = Cos(th);
         Sin_th = Sin(th);
         FunkyCullahs(th, color);
         glNormal3d(0,0,1);
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(0.5*(Cos_th+1),0.5*(Sin_th+1));  //flat mapping, correct?
         }
         glVertex3d(Cos_th,Sin_th,1);
      }
      glEnd();
   }
   glBegin(GL_QUAD_STRIP);
   //  the curved part of cylinder
   for (th=0;th<=loc_ang;th+=d)
   {
       FunkyCullahs(th, color);
       Cos_th = Cos(th);
       Sin_th = Sin(th);

       glNormal3d(Cos_th,Sin_th,0);
       if (texture > -1)
       {
          FunkyCullahs(0, 11);
          glTexCoord2d(((float)th)/360,0);
       }
       glVertex3d(Cos_th,Sin_th,1);
       if (texture > -1)
       {
          FunkyCullahs(0, 11);
          glTexCoord2d(((float)th)/360,1);
       }
       glVertex3d(Cos_th,Sin_th,-1);
   }
   glEnd();
   //  the flat parts of cylinder thru the center
   if (flats==2)
   {
      glBegin(GL_QUAD_STRIP);
      FunkyCullahs(0, color);
      glNormal3d(-Sin(loc_ang),Cos(loc_ang),0);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(0,0);  //flat mapping, correct?
      }
      glVertex3d(Cos(loc_ang),Sin(loc_ang),1);
      if (texture > -1)
         glTexCoord2d(1,0);  //flat mapping, correct?
      glVertex3d(Cos(loc_ang),Sin(loc_ang),-1);
      if (texture > -1)
         glTexCoord2d(0,0.5);  //flat mapping, correct?
      glVertex3d(0,0,1);
      if (texture > -1)
         glTexCoord2d(1,0.5);  //flat mapping, correct?
      glVertex3d(0,0,-1);
      glEnd();
      glBegin(GL_QUAD_STRIP);
      FunkyCullahs(0, color);
      glNormal3d(0,-1,0);
      if (texture > -1)
      {
         FunkyCullahs(0, 11);
         glTexCoord2d(0,0.5);  //flat mapping, correct?
      }
      glVertex3d(0,0,1);
      if (texture > -1)
         glTexCoord2d(1,0.5);  //flat mapping, correct?
      glVertex3d(0,0,-1);
      if (texture > -1)
         glTexCoord2d(0,1);  //flat mapping, correct?
      glVertex3d(1,0,1);
      if (texture > -1)
         glTexCoord2d(1,1);  //flat mapping, correct?
      glVertex3d(1,0,-1);
      glEnd();
   }


   if (flats > 0)
   {
      //  Other end
      glBegin(GL_TRIANGLE_FAN);
      //glColor3f(0, 0, 1);
      FunkyCullahs(0, color);
      glNormal3d(0,0,-1);
      glVertex3d(0,0,-1);
      for (th=loc_ang;th>=0;th-=d)
      {
         Cos_th = Cos(th);
         Sin_th = Sin(th);
         FunkyCullahs(th, color);
         glNormal3d(0,0,-1);
         if (texture > -1)
         {
            FunkyCullahs(0, 11);
            glTexCoord2d(0.5*(Cos_th+1),0.5*(Sin_th+1));  //flat mapping, correct?
         }
         glVertex3d(Cos_th,Sin_th,-1);
      }
      glEnd();
   }
   if (texture > -1)
      glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}

