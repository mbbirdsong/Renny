/*
 * cmpst_shape.c
 *
 *  Created on: Oct 15, 2011
 *      Author: michaelb
 */

#include "CSCIx229.h"
#include "final_project_mbb.h"
#include "sphr_shapes.h"
#include "cyl_shapes.h"
#include "renny.h"
#include "cmpst_shapes.h"


/*
 * Birdsong Teapot
 *     scale/ratio (r)
 *     surface pattern (color)
 */

double crash_spin = 0;

void BirdsongTeapot(double x, double y, double z, double r, unsigned char color, char texture)
{
   int phi_min=-90;
   int phi_max=-30;
   int theta_min=0;
   int theta_max=360;
   int phi=0;
   int theta=360;

   if (renny_crashing == 0) crash_spin = 0;
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   if (renny_crashing == 1)
      glRotated(crash_spin+=renny_crashing*520*(t_now-t_last), 0,1,0);
   glScaled(r,r,r);
   // scale vectors
   double lid_hndl[3] = {0.1, 0.05, 0.1};
   double torii1[3] = {1.0, 1.0, 1.0};
   double torii2[3] = {0.95, 0.95, 0.95};

   glPushMatrix();
   glTranslated(0,0.725,0);

   // draw short cylinder
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
   chopped_cone(0,0,0,.1, .08, .5, 0, color, texture);
   glPopMatrix();
   // top of lid handle partial (half) sphere
   glPushMatrix();
   glRotated(180,1,0,0);
   glTranslated(0,-.88,0);
   partial_sphere(0,0,0,lid_hndl, &theta, &phi, 0, color, texture);
   glPopMatrix();
   // spout, chopped cone
   glPushMatrix();
   glRotated(90,1,0,0);
   glRotated(45,0,1,0);
   glTranslated(-0.60,0,-1.0);
   chopped_cone(0,0,0,.33, 0.4, .25, 0, color, texture);
   glPopMatrix();
   // starter handle torus
   glPushMatrix();
   glRotated(90,1,0,0);
   glTranslated(1.2,0,0);
   torus(0,0,0,torii2,.5,.1, color, texture);
   glPopMatrix();

   glPopMatrix();

   //  Undo transformations
   glPopMatrix();
}

/*
 * Birdsong Sugarbowl / Cream Pitcher
 *    at (x,y,z)
 *    scale/ratio (r)
 *    which object (which) 0=sugarbowl, 1=cream pitcher
 *    surface pattern (color)
 */
void BirdsongSugarbowlCreamer(double x, double y, double z,double r,
                              unsigned char which, unsigned char color, char texture)
{
   int phi_min=-90;
   int phi_max=-30;
   int theta_min=0;
   int theta_max=360;
   int phi=0;
   int theta=360;
   double lid_hndl[3] = {0.1, 0.1, 0.1};

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   glPushMatrix();
   glTranslated(0,0.725,0);
   // draw short cylinder
   //  Save transformation
   glPushMatrix();
   glRotated(90,1,0,0);
   glTranslated(0,0,0.725);
   cylinder(0,0,0, 0.7, 0.025, 1, color, texture);
   glPopMatrix();
   // scale vectors
   double torii1[3] = {1.0, 1.0, 1.0};
   double torii2[3] = {0.95, 0.95, 0.95};

   // draw bottom partial torus
   partial_torus(0,0,0,torii1,0.7,0.7, &theta_min, &phi_min, &theta_max, &phi_max, 0, color, texture);
   // draw middle partial torus
   phi_min=-30; phi_max=30;
   glPushMatrix();
   glTranslated(0,-0.25,0);
   partial_torus(0,0,0,torii1,1.133,0.2, &theta_min, &phi_min, &theta_max, &phi_max, 0, color, texture);
   glPopMatrix();
   // upper part of body chopped cone or notched cone
   glPushMatrix();
   glRotated(90,1,0,0);
   if (which == 0)
   {
      // shorter version
//      glTranslated(0,0,-0.1152);
//      chopped_cone(0,0,0,1.3062, 0.2652, 0.7656, 0, color, texture);
      glTranslated(0,0,-0.3327);
      chopped_cone(0,0,0,1.3062, 0.4827, 0.5742, 0, color, texture);
   }
   else
   {
      glTranslated(0,0,-0.3327);
      notched_cylinder(0,0,0,1.3062, 0.4827, 0.5742, 0.5, 0.6, 60, 180, 0, color, texture);
   }
   glPopMatrix();

   if (which == 0)
   {
      // top rim torus
      glPushMatrix();
      glTranslated(0,0.80,0);
      torus(0,0,0,torii1,.85,.05, color, texture);
      glPopMatrix();
      // main part of lid chopped cone
      glPushMatrix();
      glRotated(90,1,0,0);
      glTranslated(0,0,-0.95);
      chopped_cone(0,0,0,.89, .15, .062, 0, color, texture);
      glPopMatrix();
      // base of lid handle chopped cone
      glPushMatrix();
      glRotated(-90,1,0,0);
      glTranslated(0,0,1.10);
      chopped_cone(0,0,0,.1, .08, .5, 0, color, texture);
      glPopMatrix();
      // top of lid handle partial (half) sphere
      glPushMatrix();
      glRotated(180,1,0,0);
      glTranslated(0,-1.18,0);
      partial_sphere(0,0,0,lid_hndl, &theta, &phi, 0, color, texture);
      glPopMatrix();
   }
   // starter handle torus 1
   glPushMatrix();
   glRotated(90,1,0,0);
   glTranslated(1.2,0,-0.1);
   if (which == 0)
   {
      torus(0,0,0,torii2,.5,.1, color, texture);
      torus(-2.4,0,0,torii2,.5,.1, color, texture);
   }
   else
   {
      theta_min = -125;
      phi_min = 0;
      theta_max = 90;
      phi_max = 360;
      partial_torus(0,0,0,torii2,.5,.1, &theta_min, &phi_min, &theta_max, &phi_max, 1, color, texture);
   }

   glPopMatrix();

   glPopMatrix();

   //  Undo transformations
   glPopMatrix();
}

/*
 * Birdsong Wineglass
 *    at (x,y,z)
 *    scale/ratio (r) [now a 3D vector]
 *    surface pattern (color)
 */
void BirdsongWineglass(double x, double y, double z, float* r, unsigned char color, char texture)
{
   int theta = 360;
   int phi = 0;
   double bulb_scale[3] = {.6,1.2,.6};
   double base_scale[3] = {.5,.1,.5};

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r[0],r[1],r[2]);

   glPushMatrix();
   glTranslated(0,1,0);

   // create wineglass base
   glPushMatrix();
   glRotated(180,1,0,0);
   glTranslated(0,1,0);
   partial_sphere(0,0,0,base_scale, &theta, &phi, 0, color, texture);
   glPopMatrix();

   // wine glass stem
   glPushMatrix();
   glRotated(-90,1,0,0);
   glTranslated(0,0,-.45);
   cylinder(0,0,0,.1, .45, 0, color, texture);
   glPopMatrix();

   // create wineglass bulb
   phi=30;
   glPushMatrix();
   glRotated(0,1,0,0);
   glTranslated(0,1.175,0);
   partial_sphere(0,0,0,bulb_scale, &theta, &phi, 0, color, texture);
   glPopMatrix();

   glPopMatrix();
   //  Undo transformations
   glPopMatrix();
}

/*
 * Birdsong Coffee Mug
 *    at (x,y,z)
 *    scale/ratio (r) [now a 3D vector]
 *    surface pattern (color)
 */
void BirdsongCoffeeMug(double x, double y, double z, float* r, unsigned char color, char texture)
{
   int theta = 360;
   int phi = 0;
   double base_scale[3] = {.8,0.15,0.8};
   double sides_scale[3] = {1,1,1};
   double hndl_scale[3] = {.6,1,1};

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r[0],r[1],r[2]);

   // draw short cylinder
   glPushMatrix();
   glTranslated(0,0.025,0);
   glRotated(90,1,0,0);
   cylinder(0,0,0, 0.5, 0.025, 1, color, texture);
   glPopMatrix();

   // create coffee mug base
   glPushMatrix();
   glTranslated(0,.16,0);
   partial_sphere(0,0,0,base_scale, &theta, &phi, 1, color, texture);
   glPopMatrix();

   // mug sides
   glPushMatrix();
   glTranslated(0,0.911,0);
   glRotated(-90,1,0,0);
   cyl_pipe(0,0,0,sides_scale, 0.64, 0.8, .75, color, texture);
   glPopMatrix();

   // mug handle
   glPushMatrix();
   glTranslated(1.04,0.9,0);
   glRotated(-90,1,0,0);
   torus(0,0,0,hndl_scale, .5, .16, color, texture);
   glPopMatrix();

   //  Undo transformations
   glPopMatrix();
}


/*
 * Birdsong Martiniglass
 *    at (x,y,z)
 *    scale/ratio (r) [now a 3D vector]
 *    surface pattern (color)
 */
void BirdsongMartiniglass(double x, double y, double z, float* r, unsigned char color, char texture)
{
   double glass_scale[3] = {1.0,.45,1.0};
   double base_scale[3] = {.5,.1,.5};

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r[0],r[1],r[2]);

   // create  martini glass base
   glPushMatrix();
   glRotated(90,1,0,0);
   glTranslated(0,0,.95);
   cone(0,0,0, base_scale[0], base_scale[1], 0, color, texture);
   glPopMatrix();

   // martini glass stem
   glPushMatrix();
   glRotated(-90,1,0,0);
   glTranslated(0,0,-.45);
   cylinder(0,0,0,.1, .45, 0, color, texture);
   glPopMatrix();

   // create martini glass body
   glPushMatrix();
   glRotated(-90,1,0,0);
   glTranslated(0,0,.36);
   cone(0,0,0, glass_scale[0], glass_scale[1], 0, color, texture);
   glPopMatrix();

   //  Undo transformations
   glPopMatrix();
}

/*
 * Birdsong Balloon
 *    at (x,y,z)
 *    scale/ratio (r) [now a 3D vector]
 *    surface pattern (color)
 */
void BirdsongBalloon(double x, double y, double z, float* r, unsigned char color, char texture)
{
   int theta = 360;
   int phi = 0;

   double bottom_scale[3] = {1.5,2.0,1.5};
   double top_scale[3] = {1.5,1.5,1.5};

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r[0],r[1],r[2]);

   //  Draw blended
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   if (shadow_save == 1)
      glColor4f(1,0,0,balloon_fade);

   // create balloon top
   glPushMatrix();
   glTranslated(0,6,0);
   glRotated(180,1,0,0);
   partial_sphere(0,0,0,top_scale, &theta, &phi, 0, 255, texture);
   glPopMatrix();

   // create balloon bottom
   glPushMatrix();
   glTranslated(0,6,0);
   partial_sphere(0,0,0,bottom_scale, &theta, &phi, 0, 255, texture);
   glPopMatrix();

   if (shadow_save == 1)
      glColor4f(1,1,1,balloon_fade);
   // create string
   glPushMatrix();
   glTranslated(0,2,0);
   glRotated(90,1,0,0);
   cylinder(0,0,0, 0.025, 3, 0, 255, -1);
   glPopMatrix();

   glDisable(GL_BLEND);

   //  Undo transformations
   glPopMatrix();
}
