/*
 *  Created on: Oct 23, 2011
 *  Latest Update on: Dec 07, 2011
 *      Author: michael birdsong
 *
 *  based on and greatly expanded from code presented throughout lectures
 *  in CSCI 5229, Fall 2011, by Prof. W. Schreuder
 *
 *  Controls
 *  ========
 *
 * Hide/Show View Help: X
 * Rotate right: l or 6(*)
 * Rotate left: h or 4(*)
 * Rotate up: k or 8(*)
 * Rotate down: j or 2(*)
 * Zoom in: i or 3(*)
 * Zoom out: o or 9(*)
 * (*) If you have a Number Pad on the right of your keyboard set 'Num Lock' and use it.
 * Increase View Angle: +
 * Decrease View Angle: -
 * Terrain grid on/off: g/G
 * Exit: ESC
 *
 * Show/Hide Move Help: x
 * Wave: w
 * Back flip: y
 * Spotlight on/off: q
 * Turn Left: left arrow
 * Turn right: right arrow
 * Start/Faster: Up arrow
 * Slower/Stop: Down Arrow
 * Pause Walk: p
 * Resume Walk: f
 * Reset to origin: 0
 * Reset in place: r
 * Takeoff/Ascend: u(**)
 * Descend/Land: d(**)
 * (**) Must have more than 2.0 sec. of Fly Time.
 * Exit: ESC
 *
 *  (more information / explanation)
 *  ------------------
 *  Move of world view keys to 'similar locations':
 *  -----------------------------------------------
 *  If you have an 'extra numeric keypad' to the left on your keyboard, and set
 *  the "Num Lock", the new 'rotate the world' keys seem just like the 'arrow
 *  key' version. a.k.a."
 *  '4'/'6': Rotates your view, CENTERED ON RENNY, left and right
 *  '8'/'2': Rotates your view, CENTERED ON RENNY, up and down
 *  '9'/'3': Zooms your view of the world out and in.
 *  (so it all seems like the 'cursor' and 'page' keys on that number pad)
 *
 *  Move of world view keys to 'vi editor locations':
 *  -------------------------------------------------
 *  IF a user does not have that extra number pad, the above key controls,
 *  the numbers would be a pain, so we also have the old 'vi' cursor version
 *  working:
 *  'h'/'l': Rotates your view, CENTERED ON RENNY, left and right
 *  'k'/'j': Rotates your view, CENTERED ON RENNY, up and down
 *  'o'/'i': Zooms your view of the world out and in.
 *
 */

#include "CSCIx229.h"
#include <unistd.h>

#include "final_project_mbb.h"
#include "cyl_shapes.h"
#include "sphr_shapes.h"
#include "basic_shapes.h"
#include "cmpst_shapes.h"
#include "floor.h"
#include "terrain.h"
#include "renny.h"
#include "game_area.h"

//#define OPENGL_ERROR_CHECK
//#define DRAW_CHECK
//#define DARK_PRINT
//#define BLENDED_SHADOWS

// angle increment for all the objects
const int draw_angle = 15;

// many variable brought forward from Hw2, 3 & 4
int th=90;     // Azimuth of view angle
int ph=15;      // Elevation of view angle
int rh=0;       // Rotation of view angle

int fov=55;       //  Field of view (for perspective)
float cos_fov= 0.866;
double asp=1;     //  Aspect ratio
double dim=20.0;   //  Size of world
double dim_max=64.0;   // Max view of world
double dim_min=16.0;   //  Min view of world

double energy = 108.0;  // initial energy level
int score = 0;          // initial player score
double fly_time = 42.0;  // initial flying time (production val is 0.0)

double Up_x, Up_x_prm, Up_y, Up_y_prm, Up_z;

double t_now = 0.0;   // current time
double t_last = 0.0;  // time of last update
double t_coll = 0.0;  // time to next check of collisions
double t_msg = 0.0;   // when to remove displayed message


int axes=0;       //  Display axes
int mode=0;       //  What to display
int persp=1;      //  Projection Mode (begin in perspective)

int win_hgt = 600, win_wdth = 600;
unsigned char show_move_help = 0; /* 1 = TRUE, 0 = FALSE */
unsigned char show_view_help = 0; /* 1 = TRUE, 0 = FALSE */
unsigned char start_stop = 1; /* control flying martini glasses */
unsigned char show_warn = 1; /* warning set, 1==TRUE, 0==FALSE */

unsigned char glbl_clr = 0; // default object color

int cyl_angle = 135;
int part_theta = 360;
int part_phi = 0;
int theta_min = 0;
int theta_max = 180;
int phi_min = -45;
int phi_max = 45;

double zh1       =  0;  // martini glass rotation

double glb_scale[3] =  {1.0, 1.0, 1.0};

double scale_light[3] =  {8, 8, 8};
double scale1[3] =  {0.1, 0.4, 0.6};
double scale2[3] =  {0.2, 0.4, 0.6};
double scale3[3] =  {0.45, 0.45, 0.45};
double scale4[3] =  {0.5, 0.5, 0.5};
double scale5[3] =  {0.6, 0.5, 0.6};
double scale6[3] =  {1.2, 1.2, 1.2};
double scale7[3] =  {1.2, 0.9, 1.2};


double pos[3] = {0.0, 0.0, 0.0};
double trans[3] = {0.0, 0.0, 0.0};
double rot[3]= {0.0, 0.0, 0.0};
double hand_scale[3] = {1.0, 1.0, 1.0};
double current_normal[3] = {0,1.0,0};
double view_vector[3] = {.707,0,.707};
double view_pos[3] = {-1,.5,-1};

double Ex;
double Ex_prm;
double Ey;
double Ey_prm;
double Ez;

// from ex15.c
//  Textures displayed
int ntex=0;
char txtr = -1;
char saved_txtr = 1;
char flr_txtr = 1;
char flr_show = 1;
char grass_texture = 1;
unsigned char drawing_renny=0;

// from ex12.c
int light=1;      //  Lighting
int move=1;       //  Move light

// fog indicator
int fog_off = 1;

// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)
double zh        =  90;  // Light azimuth
double ylight  =   1.0;    // Elevation of light

float glb_light_level = 1.0;

// from ex15.c
unsigned int textures[15]; // Object Texture names
unsigned int floor_textures[2][4]; // Floor Texture names

int right_margin = 220;
double walk_scale[2]={1.0, 1.0};
double rny   =  90;  // Renny motion angle
double rny_last   =  90;  // Renny motion angle last step
double rny_speed = 10; // = 5; old value

// Warning message
char* warning ="Not a party until martini glasses start flying.";

// population values
int num_objects = 0;
int num_cubes = 0;

int draw_kount=0;

// from ex32.c Simple shadows
//the following are based on the area in which Renny or the flying martini glass is located.
float N[] = {0, -1, 0}; // Normal vector for the plane
float E[] = {0, 0, 0 }; // Point of the plane (vertex off of the diagonal)

// Local display functions
void DrawScene();
void ShowHelp();
void PrintStatus();
float CalcAmbLightLevel();
void ShadowProjection(float L[4], float E[4], float N[4]);
void martini_shadows();


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
    GLenum errCode;
    const GLubyte *errString;
    int step = 0;
    int view_ang = 90-th;
    int current_quad[2] = {NUM_X_AREAS/(2*X_QUAD_DIM),NUM_Z_AREAS/(2*Z_QUAD_DIM)};
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   //  Undo previous transformations
   glLoadIdentity();

   //  Perspective - set eye position
   // need to add rh or Z-axis rot.
   Ex = Ex_prm = -2*dim*Sin(th+renny_rot[0])*Cos(ph);
   Ey = Ey_prm = +2*dim        *Sin(ph);
   Ez = +2*dim*Cos(th+renny_rot[0])*Cos(ph);
   if (rh != 0)
   {
      Ex = Ex_prm*Cos(rh) + Ey_prm*Sin(rh);
      Ey = -Ex_prm*Sin(rh) + Ey_prm*Cos(rh);
   }
   Up_x = Up_x_prm = Sin(ph)*Sin(th+renny_rot[0]);
   Up_y = Up_y_prm = Cos(ph);
   Up_z = -Sin(ph)*Cos(th+renny_rot[0]);
   if (rh != 0)
   {
      Up_x = Up_x_prm*Cos(rh) + Up_y_prm*Sin(rh);
      Up_y = -Up_x_prm*Sin(rh) + Up_y_prm*Cos(rh);
   }
   // do this ONCE per time step, it calcs current and last elevations of Renny (for positioning and energy use)
   get_current_elevation();

   // used to do dot products to determine if a quad 'in range' is actually in front of Renny
   view_vector[0]=-Ex; view_vector[1]=-Ey; view_vector[2]=-Ez;
   view_pos[0]=Ex+renny_pos[0]*glb_scale[0];
   view_pos[2]=Ez+renny_pos[2]*glb_scale[2];
   view_pos[1]=Ey+renny_pos[1]*glb_scale[1]+renny_alt+get_elevation(view_pos[0], view_pos[2]);

   gluLookAt(view_pos[0],view_pos[1],view_pos[2],renny_pos[0]*glb_scale[0],
           renny_pos[1]*glb_scale[1]+current_elev+renny_alt,renny_pos[2]*glb_scale[2] , Up_x,Up_y,Up_z);

   //  Flat or smooth shading
   glShadeModel(GL_SMOOTH);

   //  Translate intensity to color vectors
   glb_light_level = CalcAmbLightLevel();
   float Ambient[]   = {0.01*ambient*glb_light_level ,0.01*ambient*glb_light_level ,0.01*ambient*glb_light_level ,1.0};
   float Diffuse[]   = {0.01*diffuse*glb_light_level ,0.01*diffuse*glb_light_level ,0.01*diffuse*glb_light_level ,1.0};
   float Specular[]  = {0.01*specular*glb_light_level,0.01*specular*glb_light_level,0.01*specular*glb_light_level,1.0};
   float AmbientOther[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
   float DiffuseOther[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
   float SpecularOther[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
   //  Light position
   float Position[]  = {250*Cos(zh)+renny_pos[0],renny_pos[1]+165+current_elev,250*Sin(zh)+renny_pos[2],1.0};
   //  Draw light position as ball (still no lighting here)
   glColor3f(1,1,1);
   ball(Position[0],Position[1],Position[2] , scale_light, 11, -1);
   //  OpenGL should normalize normal vectors
   glEnable(GL_NORMALIZE);
   //  Enable lighting
   glEnable(GL_LIGHTING);
   //  Location of viewer for specular calculations
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
   //  glColor sets ambient and diffuse color materials
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   //  Enable light 0
   glEnable(GL_LIGHT0);
   //  Set ambient, diffuse, specular components and position of light 0
   if (glb_light_level > 0)
   {
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   }
   else
   {
      Ambient[0] = Ambient[1] = Ambient[2] = Ambient[3] = 0.0;
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   }
   glLightfv(GL_LIGHT0,GL_POSITION,Position);

   // check for prize flash
   if (renny_flashing == 1)
   {
      //float Position1[3]={full_objects[current_flash].pos[0],full_objects[current_flash].pos[1],
      //          full_objects[current_flash].pos[2]};
      float Position1[]={renny_pos[0],renny_pos[1],renny_pos[2],1};
      //  Enable light 1
      glEnable(GL_LIGHT1);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT1,GL_AMBIENT ,AmbientOther);
      glLightfv(GL_LIGHT1,GL_DIFFUSE ,DiffuseOther);
      glLightfv(GL_LIGHT1,GL_SPECULAR,SpecularOther);
      glLightfv(GL_LIGHT1,GL_POSITION,Position1);

      if (t_now > t_renny_flash)
      {
          renny_flashing = 0;
          current_flash = -1;
          t_renny_flash = 0.0;
          glDisable(GL_LIGHT1);
      }
   }
   if (renny_lighting==1)
   {
      float PositionSpot[] = {renny_pos[0]+2*Cos(renny_rot[0]),renny_pos[1]+current_elev+2,renny_pos[2]+2*Sin(renny_rot[0]),1};
      float Direction[] = {Cos(renny_rot[0]),0,Sin(renny_rot[0]),0};  /* no roll and pitch of renny considered for now */

      //  Enable Renny's spot light
      glEnable(GL_LIGHT2);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT2,GL_AMBIENT ,AmbientOther);
      glLightfv(GL_LIGHT2,GL_DIFFUSE ,DiffuseOther);
      glLightfv(GL_LIGHT2,GL_SPECULAR,SpecularOther);
      glLightfv(GL_LIGHT2,GL_POSITION,PositionSpot);
      //  Set spotlight parameters
      glLightfv(GL_LIGHT2,GL_SPOT_DIRECTION,Direction);
      glLightf(GL_LIGHT2,GL_SPOT_CUTOFF,30);
      glLightf(GL_LIGHT2,GL_SPOT_EXPONENT,0);
   }
   else
      glDisable(GL_LIGHT2);

   /* Fog, if Renny in a fog_quad and below the elevation for fog */
   quad_from_current_pos(current_quad);
   if (quads[current_quad[0]][current_quad[1]].in_fog==1  &&  (current_elev+renny_alt)<fog_max_el)
   {
       fog_off = 0;
       /* In the fog */
       glEnable(GL_FOG);
       {
          GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};

          glFogi (GL_FOG_MODE, GL_LINEAR);
          glFogfv (GL_FOG_COLOR, fogColor);
          glFogf (GL_FOG_DENSITY, 0.35);
          glHint (GL_FOG_HINT, GL_DONT_CARE);
          glFogf (GL_FOG_START, 1.0);
          glFogf (GL_FOG_END, X_QUAD_DIM*dims[0]);
       }
       glClearColor(0.5, 0.5, 0.5, 1.0);  /* fog color */
   }
   else
   {
       fog_off=1;
       glDisable(GL_FOG);
   }

   //  Draw scene
   create_terrain(0);
#ifdef OPENGL_ERROR_CHECK
   if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        printf ("%d MAIN OpenGL Error: %s\n", step, errString);
    }
   step+=1;
#endif /* OPENGL_ERROR_CHECK */
   DrawScene();
#ifdef OPENGL_ERROR_CHECK
   if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        printf ("%d MAIN OpenGL Error: %s\n", step, errString);
    }
   step+=1;
#endif /* OPENGL_ERROR_CHECK */
   /* draw fog */
   if (fog_off == 1)
      fog_cube();

   //  draw Renny
   switch (mode)
   {
      //  "renny" walking
      case 0:
         walk_renny(glbl_clr, txtr, walk_scale);
         sleep_now = 0;
         break;

      // stop the walking
      case 1:
           stop_renny(glbl_clr, txtr, walk_scale);
         break;

      // start the walking
      case 2:
          start_renny(glbl_clr, txtr, walk_scale);
         break;

      // Bounce back after collision
      case 3:
         crash_renny(glbl_clr, txtr, walk_scale);
         break;

      // Wave at user
      case 4:
            wave_renny(glbl_clr, txtr, walk_scale);
         break;

      // Flip Renny
      case 5:
         flip_renny(glbl_clr, txtr, walk_scale);
         break;

      // Renny takeoff
      case 6:
         takeoff_renny(glbl_clr, txtr, walk_scale);
         break;

      // Renny fly
      case 7:
         fly_renny(glbl_clr, txtr, walk_scale);
         break;

      // Renny land
      case 8:
         land_renny(glbl_clr, txtr, walk_scale);
         break;

   }
#ifdef OPENGL_ERROR_CHECK
   if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        printf ("%d MAIN OpenGL Error: %s\n", step, errString);
    }
   step+=1;
#endif /* OPENGL_ERROR_CHECK */
   /* only draw fog cube and shadow when Renny not in fog */
   if (fog_off == 1)
   {
      /* and light level is high enough to see a shadow */
      if (glb_light_level > 0.2)
      {
          /*  VERY CLOSE for stencil!!  (It does not clear frame to frame). Works in z-buffer blended */
         //  Save what is glEnabled
         glPushAttrib(GL_ENABLE_BIT);

         glDisable(GL_LIGHTING);
#ifdef BLENDED_SHADOWS
         //  Enable stencil operations
         glEnable(GL_STENCIL_TEST);

         //  Step 1:  Set stencil buffer to 1 where there are shadows
         //  Existing value of stencil buffer doesn't matter
         glStencilFunc(GL_ALWAYS,1,0xFFFFFFFF);
         //  Set the value to 1 (REF=1 in StencilFunc)
         //  only if Z-buffer would allow write
         glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
         //  Make Z-buffer and color buffer read-only
         glDepthMask(0);
         glColorMask(0,0,0,0);
#else
         //  Draw blended
         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
         glColor4f(0,0,0,0.6);
         //  Make Z-buffer read-only
         glDepthMask(0);
#endif
         // Do Renny
         //  Draw flattened scene (must get Position, E, & N)
         shadow_normal_and_plane(renny_pos[0], renny_pos[2], N, E);
         glPushMatrix();
           ShadowProjection(Position,E,N);
           renny_shadow();
         glPopMatrix();

         // Do any martini glasses in visible quads
         martini_shadows();

#ifdef BLENDED_SHADOWS
         //  Make Z-buffer and color buffer read-write
         glDepthMask(1);
         glColorMask(1,1,1,1);

         //  Step 2:  Draw shadow masked by stencil buffer
         //  Set the stencil test draw where stencil buffer is > 0
         glStencilFunc(GL_LESS,0,0xFFFFFFFF);
         //  Make the stencil buffer read-only
         glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
         //  Enable blending
         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
         glColor4f(0,0,0,0.6);

         //  Draw the shadow over the area in question
         renny_shadow_area();
#else
         //  Make Z-buffer read-write
         glDepthMask(1);
#endif
         //  Undo glEnables
         glPopAttrib();
      }
   }

   //  Generate display - no lighting from here on
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Score=%d", score);
   // need to do a background color for energy display
   glWindowPos2i(105,5);
   Print("Energy=%.0f", energy);
   glWindowPos2i(215,5);
   Print("Speed=%.0f", (renny_moving||renny_flying)?rny_speed:0);
   glWindowPos2i(305,5);
   Print("Zoom=%.2f", dim_min/dim);
   glWindowPos2i(405,25);
   Print("Loc=%d,%d", (int)renny_pos[0],(int)renny_pos[2]);
   glWindowPos2i(405,5);
   if (view_ang > 180) view_ang -=360;
   else if (view_ang < -180) view_ang +=360;
   Print("Head/View=%d/%d", (int)renny_rot[0], view_ang); // fix th display
   if (fly_time > 0.0)
   {
       glWindowPos2i(150,25);
       Print("Fly Time=%.1f", fly_time);
       glWindowPos2i(305,25);
       Print("Alt.=%.1f", renny_alt);
   }
   glWindowPos2i(5,25);
   if (renny_lighting == 1)
       Print("Spot light ON.");

   ShowHelp();

   //  Red for warning
   if (glb_light_level > 0.5)
       glColor3f(1,0,0);
   else
       glColor3f(1,.5,.5);
   glWindowPos2i(5,45);
   Print(warning);
   if (show_warn == 1)
   {
      show_warn=0;
      t_msg = 3.0 + glutGet(GLUT_ELAPSED_TIME)/1000.0;
   }

   if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        printf ("%d MAIN OpenGL Error: %s\n", step, errString);
    }
   step+=1;

   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
   //if (sleep_now == 1)
       //sleep(2);
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   unsigned char redisplay = 0;

   t_last=t_now;
   t_now = glutGet(GLUT_ELAPSED_TIME)/1000.0;

   if (start_stop == 1)
   {
      martini_angle += fmod(martini_angle_rate*(t_now-t_last),360);
      if (martini_angle > 360) martini_angle -= 360;
      redisplay = 1;
   }

   if (renny_moving == 1 && renny_rocking==0)
   {
      rny_last = rny;
      rny +=42*rny_speed*(t_now-t_last);
      if (rny > 360) rny -= 360;
      redisplay = 1;
   }
   if (move == 1)
   {
      zh += fmod(6*inc*(t_now-t_last),360);
      if (zh > 360) zh -= 360;
      redisplay = 1;
   }

   if (redisplay == 1)
   {
      if (t_msg != 0)
      {
         if (t_now > t_msg)
         {
            t_msg = 0;
            warning ="                              ";
         }
      }
      glutPostRedisplay();
   }
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   if (key == GLUT_KEY_RIGHT)     //  Right arrow key - Turn Renny right by 5 degrees
   {
      renny_rot[0]+=5;
      if (renny_rot[0] > 180) renny_rot[0] -=360;
   }
   else if (key == GLUT_KEY_LEFT) //  Left arrow key - Turn Renny left by 5 degrees
   {
       renny_rot[0]-=5;
       if (renny_rot[0] < -180) renny_rot[0] +=360;
   }
   else if (key == GLUT_KEY_UP  && (stopping != 1 && starting != 1 &&
            renny_crashing != 1 && renny_waving != 1 && renny_flipping != 1) &&
            renny_taking_off == 0  && renny_touching_down == 0)   // Up arrow key - Increase Renny Walking Speed
   {
       if (renny_flying == 0)
       {
          if (renny_rocking == 1)
          {
             mode = 2;
             warning = "OK, go.";
          }
          else
          {
             if (rny_speed < 24.9)
                rny_speed += 1;
             else
                warning = "Already @ max speed there, Mario.";
          }
       }
       else
       {
           if (rny_speed < 29.9)
              rny_speed += 1;
           else
              warning = "Already @ max speed there, Chuck Yeager.";
       }
   }
   else if (key == GLUT_KEY_DOWN  && (stopping != 1 && starting != 1 &&
            renny_crashing != 1 && renny_waving != 1 && renny_flipping != 1) &&
            renny_taking_off == 0  && renny_touching_down == 0) // Down arrow key - Decrease Renny Walking Speed
   {
       if (renny_flying == 0)
       {
          if (renny_rocking == 0)
          {
             if (rny_speed < 10.1)
             {
                mode = 1;
                warning = "Pausing.";
             }
             else
                rny_speed -= 1;
          }
       }
       else
       {
           if (rny_speed < -4.9)
           {
              warning = "Already in full reverse there, Rep. Bachmann";
           }
           else
              rny_speed -= 1;
       }
   }
   else if (key == GLUT_KEY_PAGE_UP)    // PageUp key - increase dim, within reason
       if ( dim<dim_max)
          if (dim < 1.5*dim_min)
             dim += 1;
          else if (dim < 4*dim_min)
             dim += 4.0;
          else
             dim += 8.0;
      else
         warning = "WARNING! @ Zoom max. reasonable value there, Galileo.";
   else if (key == GLUT_KEY_PAGE_DOWN)  //  PageDown key - decrease dim, within reason
   {
       if ( dim>dim_min)
           if (dim < 1.5*dim_min)
              dim -= 1.;
           else if (dim < 4*dim_min)
              dim -= 4.0;
           else
              dim -= 8.0;
      else
         warning = "WARNING! @ Zoom min. reasonable value there, van Leeuwenhoek.";
   }

   if (warning[0] != ' ')
       show_warn = 1;

   //  Update projection
   Project(persp?fov:0,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   //PrintStatus();
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
       //  Theta, X-axis rot., a.k.a. "Roll" for 'propeller-heads'
   if (ch == '6'  ||  ch == 'l')     //  Right arrow key (w/ Num Lock) - increase angle by 5 degrees
   {
       th -= 5;
       if (th < -180) th +=360;
   }
   else if (ch == '4'  ||  ch == 'h') //  Left arrow key (w/ Num Lock) - decrease angle by 5 degrees
   {
       th += 5;
       if (th > 180) th -=360;
   }
   //  Phi, Y-axis rot., a.k.a. "Yaw" for 'propeller-heads'
   else if (ch == '8'  ||  ch == 'k')   // Up arrow key (w/ Num Lock) - increase elevation by 5 degrees
      if (ph < 41)
          ph += 5;
      else
          warning = "WARNING! @ Highest view angle there, Lindbergh";
   else if (ch == '2'  ||  ch == 'j')   // Down arrow key (w/ Num Lock) - decrease elevation by 5 degrees
      if (ph > 11)
          ph -= 5;
      else
          warning = "WARNING! @ Lowest view angle there, grave digger";
   // Dimensions of the 'world'
   else if (ch == '9'   ||  ch == 'o')    // PageUp key (w/ Num Lock) - increase dim, within reason
       if ( dim<dim_max)
          if (dim < 1.5*dim_min)
             dim += 0.5;
          else if (dim < 10.0*dim_min)
             dim += 5.0;
          else
             dim += 10.0;
      else
         warning = "WARNING! @ Zoom max. reasonable value there, Galileo.";
   else if (ch == '3'  ||  ch == 'i')  //  PageDown key (w/ Num Lock) - decrease dim, within reason
       if ( dim>dim_min)
           if (dim < 1.5*dim_min)
              dim -= 0.5;
           else if (dim < 10.0*dim_min)
              dim -= 5.0;
           else
              dim -= 10.0;
      else
         warning = "WARNING! @ Zoom min. reasonable value there, van Leeuwenhoek.";
   //  Smooth color model
   else if (ch == 27)  //  Exit on ESC
      exit(0);
   else if (ch == '0') //  Reset view
   {
      th = 90;
      ph = 15;
      rh = 0;
      fov = 55;
      dim = 20.0;
      persp = 1;
      ylight=0.0;
      ambient=30;
      diffuse=100;
      specular=0;
      emission=0;
      shininess=0;
      distance=5;
      inc=10;
      local=1;
      renny_pos[0]=renny_pos[1]=renny_pos[2]=0;
      renny_rot[0]=45;
      renny_rot[1]=renny_rot[2]=0;
      renny_x_last = 0.0;
      renny_z_last = 0.0;

      mode=0;
      rny_speed = 5;
      renny_alt = 0;
      renny_moving = 0;
      renny_rocking = 1;
      renny_crashing = 0;
      renny_flipping = 0;
      renny_waving = 0;
      starting = 0;
      stopping = 0;
      renny_taking_off = 0;
      renny_touching_down = 0;
      renny_flying = 0;
      show_hand = 0;
      show_balloon=0;
   }
   else if (ch == 'q')
      renny_lighting = 1-renny_lighting;  /* turn on renny's spot light */
   else if (ch == 'r')
   {
      /* reset in place */
      mode=0;
      rny_speed = 5;
      renny_alt = 0;
      renny_moving = 0;
      renny_rocking = 1;
      renny_crashing = 0;
      renny_flipping = 0;
      renny_waving = 0;
      starting = 0;
      stopping = 0;
      renny_taking_off = 0;
      renny_touching_down = 0;
      renny_flying = 0;
      show_hand = 0;
      show_balloon=0;
   }
   else if (ch == 'w'  && (stopping != 1 && starting != 1 &&
            renny_crashing != 1 && renny_waving != 1 && renny_flipping != 1) &&
            renny_taking_off == 0  && renny_touching_down == 0  && renny_flying == 0)     // Renny waves
   {
       mode = 4;
       warning="\'Hi there.\'";
   }
   else if (ch == 'y'  && (stopping != 1 && starting != 1 &&
            renny_crashing != 1 && renny_waving != 1 && renny_flipping != 1) &&
            renny_taking_off == 0  && renny_touching_down == 0  && renny_flying == 0)     // Flip Renny
       mode = 5;
   else if (ch == 'x')               // Show/Hide "Moving Renny Help Menu"
      show_move_help = 1-show_move_help;
   else if (ch == 'X')               // Show/Hide "View the World Help Menu"
      show_view_help = 1-show_view_help;
   else if (ch == 'f' && (stopping != 1 && starting != 1 &&
           renny_crashing != 1 && renny_waving != 1 && renny_flipping != 1) &&
           renny_taking_off == 0  && renny_touching_down == 0  && renny_flying == 0) // Insta Restart Renny (same as prev speed)
      mode = 2;
   else if (ch == 'p' && (stopping != 1 && starting != 1 &&
           renny_crashing != 1 && renny_waving != 1 && renny_flipping != 1) &&
           renny_taking_off == 0  && renny_touching_down == 0  && renny_flying == 0) // Insta Stop Rennny (save speed)
      mode = 1;
   //  Rho, Z-axis rot., a.k.a. "Pitch" for 'propeller-heads'
   else if (ch == 'Z')               // Increase Rotation about Z - Axis by 5 degrees
      rh += 5;
   else if (ch == 'z')               // Decrease Rotation about Z - Axis by 5 degrees
      rh -= 5;
   else if (ch == 'g' || ch == 'G')  //  Grass Textures
       grass_texture = 1 - grass_texture;
   else if (ch == 'u'  && (stopping != 1 && starting != 1 &&
            renny_crashing != 1 && renny_waving != 1 && renny_flipping != 1) &&
            renny_taking_off == 0  && renny_touching_down == 0)
   {
       if (fly_time > 2.0)
       {
          // Up, 'u'  - Increase Renny flying alt or take off
          if (renny_alt == 0.0)
          {
             mode = 6;
             warning = "Away we go!";
          }
          else
          {
             if (renny_alt < 24.9)
                renny_alt += 1;
             else
                warning = "Already @ max altitude there, Buzz.";
          }
       }
       else
       {
           show_warn = 1;
           warning = "You have no fly time.  Go back flip and catch a flying martini glass.";
       }
   }
   else if (ch == 'd'  && (stopping != 1 && starting != 1 &&
            renny_crashing != 1 && renny_waving != 1 && renny_flipping != 1) &&
            renny_taking_off == 0  && renny_touching_down == 0  && fly_time > 2.0)
   {
       // Down, 'u' - Decrease Renny flying alt or land
       if (renny_alt < 5.1)
       {
          mode = 8;
          warning = "Landing sir!.";
       }
       else
           renny_alt -= 1;
   }

   if (persp==1)
   {
      if (ch == '-')      // Decrease field of view angle
         if (fov>40)
         {
            fov--;
            cos_fov = Cos(fov+5);
         }
         else
            warning = "WARNING! @ FOV min. value, Mr. Hitchcock";
      else if (ch == '+') // Increase field of view angle
      {
         if (fov<90)
         {
            fov++;
            cos_fov = Cos(fov+5);
         }
         else
            warning = "WARNING! @ FOV max. value, Mr. DeMille";
      }
   }

   if (warning[0] != ' ')
      show_warn = 1;

   //  Translate shininess power to value (-1 => 0)
   if (glb_light_level == 0) shininess = -1;
   else shininess = 0;
   shinyvec[0] = shininess<0 ? 0 : pow(2.0,shininess);

   //  Update projection
   Project(persp?fov:0,asp,dim);

   //  Tell GLUT it is necessary to redisplay the scene
   //PrintStatus();
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   // Save these new values to reposition text/legends
   win_wdth = width;
   win_hgt = height;

   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(persp?fov:0,asp,dim);
}

void PrintStatus()
{
    printf("Starting=%d, Stopping=%d, Moving=%d, Rocking=%d, Crashing=%d, Waving=%d, Flipping=%d\n",
            starting, stopping, renny_moving, renny_rocking, renny_crashing, renny_waving, renny_flipping);
}

void martini_shadows()
{
    int i=0, j=0, ii=0;
    float vect_to_quad[2] = {0,0};
    float dot_prod = 0;

    /* renny_quad[1] data are either already the initial value or have been updated in
     * 'check_collision()' in renny.c
     */
    /* min x and z quad indexes to draw, renny_quad[0], &
     * max x and z quad indexes to draw, renny_quad[1],
     * set previously in DrawScene()
     */

    /* cycle through all objects, draw shadows of martinti glasses only for those in Renny's quad
     * and the eight surrounding quads and in front of renny (-45 < yaw < 45).
     */
    for (i=renny_quad[0][0]; i<(renny_quad[2][0]+1); i++)
    {
        for (j=renny_quad[0][1]; j<(renny_quad[2][1]+1); j++)
        {
            // ignoring y dim, for now
            if (i==renny_quad[1][0] && j==renny_quad[1][1])
                dot_prod = 1;
            else
            {
                vect_to_quad[0] = quads[i][j].center[0]-view_pos[0];
                vect_to_quad[1] = quads[i][j].center[1]-view_pos[2];
                dot_prod = vect_to_quad[0]*view_vector[0] + vect_to_quad[1]*view_vector[2];
            }

            if (dot_prod > 0.0)
            {
                int num_objs_in_this_quad = 0;
                int * objs_in_this_quad;
                Object obj_in_this_quad;
                /* list of objects in this quad */
                objs_in_this_quad=quads[i][j].objs_in_quad;
                num_objs_in_this_quad = quads[i][j].num_objs;

                /* run through objects in quad and draw them if in FOV*/
                for (ii=0; ii<num_objs_in_this_quad; ii++)
                {
                    /* skip of object invisible (e.g. collected prize) */
                    if (full_objects[objs_in_this_quad[ii]].draw == 0 ||
                        full_objects[objs_in_this_quad[ii]].type != 5)  continue;

                    obj_in_this_quad = full_objects[objs_in_this_quad[ii]];

                    /* see if object in FOV (x/z pos considered) */
                    vect_to_quad[0] = obj_in_this_quad.pos[0]-view_pos[0];
                    vect_to_quad[1] = obj_in_this_quad.pos[2]-view_pos[2];
                    dot_prod = vect_to_quad[0]*view_vector[0] + vect_to_quad[1]*view_vector[2];
                    dot_prod /= sqrt((vect_to_quad[0]*vect_to_quad[0] + vect_to_quad[1]*vect_to_quad[1])*
                                     (view_vector[0]*view_vector[0] + view_vector[2]*view_vector[2]));

                    if (dot_prod < cos_fov) continue;

                    shadow_normal_and_plane(obj_in_this_quad.pos[0], obj_in_this_quad.pos[2], N, E);
                    glPushMatrix();
                      float Position[]  = {obj_in_this_quad.pos[0],obj_in_this_quad.pos[1]+165,obj_in_this_quad.pos[2],1.0};
                      ShadowProjection(Position,E,N);

                    /* all required transforms here, like in place_renny*/
                    glPushMatrix();
                      glScaled(glb_scale[0],glb_scale[1],glb_scale[2]);
                      glTranslated(obj_in_this_quad.pos[0],obj_in_this_quad.pos[1],obj_in_this_quad.pos[2]);
                      glRotated(obj_in_this_quad.rot[1], 1,0,0);
                      glRotated(obj_in_this_quad.rot[2], 0,0,1);
                      glRotated(-obj_in_this_quad.rot[0], 0,1,0);

                      /* then draw object */
                      BirdsongMartiniglass(0, 0, 0, obj_in_this_quad.scale, 255,
                                             (char)obj_in_this_quad.texture);
                    glPopMatrix();
                  glPopMatrix();

                }
            }
        }
    }
}


void DrawScene()
{
    int i=0, j=0, ii=0;
#ifdef DRAW_CHECK
    int num_drawn=0;
#endif
    float vect_to_quad[2] = {0,0};
    float dot_prod = 0;

    /* renny_quad[1] data are either already the initial value or have been updated in
     * 'check_collision()' in renny.c
     */
    /* min x and z quad indexes to draw */
    renny_quad[0][0] =  (renny_quad[1][0]==0?0:renny_quad[1][0]-1);
    renny_quad[0][1] =  (renny_quad[1][1]==0?0:renny_quad[1][1]-1);
    /* max x and z quad indexes to draw */
    renny_quad[2][0] =  (renny_quad[1][0]==NUM_X_QUADS-1?NUM_X_QUADS-1:renny_quad[1][0]+1);
    renny_quad[2][1] =  (renny_quad[1][1]==NUM_Z_QUADS-1?NUM_Z_QUADS-1:renny_quad[1][1]+1);

    /* cycle through all objects that need movement updates */
    for (i=0; i<num_objects; i++)
    {
        if (full_objects[i].type==0)      /* evil teapot */
            tea_pot_pursuit(i);
        else if (full_objects[i].type==5) /* flying martini glass */
            martini_glass_fly(i);
        else if (full_objects[i].type==6) /* sugar energy cube */
            energy_cube_spin(i);
    }

    /* cycle through all objects, draw only those in Renny's quad and the eight surrounding quads
     * (or now do by the quad object lists?)
     * and in front of renny (-45 < yaw < 45).
     */
    for (i=renny_quad[0][0]; i<(renny_quad[2][0]+1); i++)
    {
        for (j=renny_quad[0][1]; j<(renny_quad[2][1]+1); j++)
        {
            // ignoring y dim, for now
            if (i==renny_quad[1][0] && j==renny_quad[1][1])
                dot_prod = 1;
            else
            {
                vect_to_quad[0] = quads[i][j].center[0]-view_pos[0];
                vect_to_quad[1] = quads[i][j].center[1]-view_pos[2];
                dot_prod = vect_to_quad[0]*view_vector[0] + vect_to_quad[1]*view_vector[2];
//                dot_prod /= sqrt((vect_to_quad[0]*vect_to_quad[0] + vect_to_quad[1]*vect_to_quad[1])*
//                                 (view_vector[0]*view_vector[0] + view_vector[2]*view_vector[2]));
            }
            //printf("quad[%d][%d] is %s view\n",i,j,(dot_prod>0.5?"in":"out"));
            if (dot_prod > 0.0)
            {
                int num_objs_in_this_quad = 0;
                int * objs_in_this_quad;
                Object obj_in_this_quad;
                /* list of objects in this quad */
                objs_in_this_quad=quads[i][j].objs_in_quad;
                num_objs_in_this_quad = quads[i][j].num_objs;
                //printf("num_objs_in_this_quad = %d\n", num_objs_in_this_quad);

                /* run through objects in quad and draw them if in FOV*/
                for (ii=0; ii<num_objs_in_this_quad; ii++)
                {
                    /* skip of object invisible (e.g. collected prize) */
                    if (full_objects[objs_in_this_quad[ii]].draw == 0 )  continue;
                    obj_in_this_quad = full_objects[objs_in_this_quad[ii]];
                    /* see if object in FOV (x/z pos considered) */
                    vect_to_quad[0] = obj_in_this_quad.pos[0]-view_pos[0];
                    vect_to_quad[1] = obj_in_this_quad.pos[2]-view_pos[2];
                    dot_prod = vect_to_quad[0]*view_vector[0] + vect_to_quad[1]*view_vector[2];
                    dot_prod /= sqrt((vect_to_quad[0]*vect_to_quad[0] + vect_to_quad[1]*vect_to_quad[1])*
                                     (view_vector[0]*view_vector[0] + view_vector[2]*view_vector[2]));

                    if (dot_prod < cos_fov) continue;
#ifdef DRAW_CHECK
                    num_drawn+=1;
#endif
                    /* all required transforms here, like in place_renny*/
                    glPushMatrix();
                      glScaled(glb_scale[0],glb_scale[1],glb_scale[2]);
                      glTranslated(obj_in_this_quad.pos[0],obj_in_this_quad.pos[1],obj_in_this_quad.pos[2]);
                      glRotated(obj_in_this_quad.rot[1], 1,0,0);
                      glRotated(obj_in_this_quad.rot[2], 0,0,1);
                      glRotated(-obj_in_this_quad.rot[0], 0,1,0);

                      /* then draw object */
                      switch (obj_in_this_quad.type)
                      {
                          case 0:
                              BirdsongTeapot(0, 0, 0, obj_in_this_quad.scale[0], (unsigned char)obj_in_this_quad.color,
                                             (char)obj_in_this_quad.texture);
                              break;
                          case 1:
                              BirdsongSugarbowlCreamer(0, 0, 0, obj_in_this_quad.scale[0], 0, (unsigned char)obj_in_this_quad.color,
                                             (char)obj_in_this_quad.texture);
                              break;
                          case 2:
                              BirdsongSugarbowlCreamer(0, 0, 0, obj_in_this_quad.scale[0], 1, (unsigned char)obj_in_this_quad.color,
                                             (char)obj_in_this_quad.texture);
                              break;
                          case 3:
                              BirdsongWineglass(0, 0, 0, full_objects[i].scale, (unsigned char)obj_in_this_quad.color,
                                             (char)obj_in_this_quad.texture);
                              break;
                          case 4:
                              BirdsongCoffeeMug(0, 0, 0, obj_in_this_quad.scale, (unsigned char)obj_in_this_quad.color,
                                             (char)obj_in_this_quad.texture);
                              break;
                          case 5:
                              BirdsongMartiniglass(0, 0, 0, obj_in_this_quad.scale, (unsigned char)obj_in_this_quad.color,
                                             (char)obj_in_this_quad.texture);
                              break;
                          case 6:
                              energy_cube(0,0,0, 0,0,0, 0, obj_in_this_quad.scale[0], obj_in_this_quad.scale[1],
                                           0,(unsigned char)obj_in_this_quad.color, (char)obj_in_this_quad.texture, objs_in_this_quad[ii]);
                              if (renny_prizing == 1 && t_now > t_renny_prize && objs_in_this_quad[ii] == current_prize)
                              {
                                  renny_prizing = 0;
                                  full_objects[objs_in_this_quad[ii]].draw = 0;
                                  t_renny_prize = 0;
                                  current_prize = -1;
                              }
                              break;
                          case 7:
                        	  TwoDTree(0,0,0);
                              break;
                      }
                    glPopMatrix();

                }
            }
        }
    }
#ifdef DRAW_CHECK
    draw_kount+=1;
    if (draw_kount%10 == 0)
    {
        draw_kount=0;
        printf("num_drawn=%d\n",num_drawn);
    }
#endif
}

void ShowHelp()
{
       /* show the general move help control keys, if user desires */
       if (show_view_help == 1)
       {
          glWindowPos2i(5,win_hgt-20);
          Print("Hide View Help: X");
          glWindowPos2i(5,win_hgt-40);
          Print("Rotate right: l or 6(*)");
          glWindowPos2i(5,win_hgt-60);
          Print("Rotate left: h or 4(*)");
          glWindowPos2i(5,win_hgt-80);
          Print("Rotate up: k or 8(*)");
          glWindowPos2i(5,win_hgt-100);
          Print("Rotate down: j or 2(*)");
          glWindowPos2i(5,win_hgt-120);
          Print("Zoom in: i or 3(*)");
          glWindowPos2i(5,win_hgt-140);
          Print("Zoom out: o or 9(*)");
          glWindowPos2i(5,win_hgt-160);
          Print("(*) If you have a Number Pad");
          glWindowPos2i(5,win_hgt-180);
          Print("on the right of your keyboard,");
          glWindowPos2i(5,win_hgt-200);
          Print("set \'Num Lock\' and use it.");
          glWindowPos2i(5,win_hgt-240);
          Print("Increase View Angle: +");
          glWindowPos2i(5,win_hgt-260);
          Print("Decrease View Angle: -");
          glWindowPos2i(5,win_hgt-300);
          Print("Terrain grid on/off: g/G");
          glWindowPos2i(5,win_hgt-320);
          Print("Exit: ESC");

       }
       else
       {
          glWindowPos2i(5,win_hgt-20);
          Print("Show View Help: X");
       }

       /* show the general view help control keys, if user desires */
       if (show_move_help == 1)
       {
          glWindowPos2i(win_wdth-right_margin,win_hgt-20);
          Print("Hide Move Help: x");
          glWindowPos2i(win_wdth-right_margin, win_hgt-40);
          Print("Wave: w");
          glWindowPos2i(win_wdth-right_margin, win_hgt-60);
          Print("Back flip: y");
          glWindowPos2i(win_wdth-right_margin, win_hgt-80);
          Print("Spotlight on/off: q");
          glWindowPos2i(win_wdth-right_margin, win_hgt-100);
          Print("Turn Left: left arrow");
          glWindowPos2i(win_wdth-right_margin, win_hgt-120);
          Print("Turn right: right arrow");
          glWindowPos2i(win_wdth-right_margin, win_hgt-140);
          Print("Start/Faster: Up arrow");
          glWindowPos2i(win_wdth-right_margin, win_hgt-160);
          Print("Slower/Stop: Down Arrow");
          glWindowPos2i(win_wdth-right_margin, win_hgt-180);
          Print("Pause Walk: p");
          glWindowPos2i(win_wdth-right_margin, win_hgt-200);
          Print("Resume Walk: f");
          glWindowPos2i(win_wdth-right_margin,win_hgt-220);
          Print("Reset to origin: 0");
          glWindowPos2i(win_wdth-right_margin, win_hgt-240);
          Print("Reset in place: r");
          glWindowPos2i(win_wdth-right_margin, win_hgt-260);
          Print("Takeoff/Ascend: u(**)");
          glWindowPos2i(win_wdth-right_margin, win_hgt-280);
          Print("Descend/Land: d(**)");
          glWindowPos2i(win_wdth-right_margin, win_hgt-300);
          Print("(**) Must have more than");
          glWindowPos2i(win_wdth-right_margin, win_hgt-320);
          Print("2.0 sec. of Fly Time.");
          glWindowPos2i(win_wdth-right_margin, win_hgt-360);
          Print("Exit: ESC");
       }
       else
       {
          glWindowPos2i(win_wdth-right_margin, win_hgt-20);
          Print("Show Move Help: x");
       }
}


/*
 * Return light level factor (0.0 to 1.0) as a function distance from "dark center"
 */
float CalcAmbLightLevel()
{
    float light_level = 1.0;
    float x_dist = fabs(renny_pos[0]-dark_center_loc[0]);
    float z_dist = fabs(renny_pos[2]-dark_center_loc[1]);

    /* are we in the outer square ?*/
    if (x_dist<dark_center_range[0] && z_dist<dark_center_range[1])
    {
        /* are we in the outer circle */
        if ((x_dist*x_dist + z_dist*z_dist) < dark_range_sq)
        {
            /* are we in the inner circle? */
            if ((x_dist*x_dist + z_dist*z_dist) < dark_inner_sq)
                light_level = 0;
            else 
            {
                light_level = 0.9*(sqrt(x_dist*x_dist + z_dist*z_dist)-dark_inner)/(dark_range-dark_inner)+0.1;
                warning="Lights would be good here.";
                show_warn = 1;
            }
        }
    }

#ifdef DARK_PRINT
    if (draw_kount >=120)
    {
        draw_kount = 0;
        printf("light_level = %.3f, dark_range = %.3f, dark_inner = %.3f, dist = %.3f\n", light_level,
                dark_range, dark_inner, sqrt(x_dist*x_dist + z_dist*z_dist));
    }
    else
        draw_kount += 1;
#endif
    return light_level;
}

/*
 * Multiply the current ModelView-Matrix with a shadow-projetion matrix.
 *
 * L is the position of the light source
 * E is a point within the plane on which the shadow is to be projected.
 * N is the normal vector of the plane.
 *
 * Everything that is drawn after this call is "squashed" down to the plane.
 */
void ShadowProjection(float L[4], float E[4], float N[4])
{
   float mat[16];
   float e = E[0]*N[0] + E[1]*N[1] + E[2]*N[2];
   float l = L[0]*N[0] + L[1]*N[1] + L[2]*N[2];
   float c = e - l;
   //  Create the matrix.
   mat[0] = N[0]*L[0]+c; mat[4] = N[1]*L[0];   mat[8]  = N[2]*L[0];   mat[12] = -e*L[0];
   mat[1] = N[0]*L[1];   mat[5] = N[1]*L[1]+c; mat[9]  = N[2]*L[1];   mat[13] = -e*L[1];
   mat[2] = N[0]*L[2];   mat[6] = N[1]*L[2];   mat[10] = N[2]*L[2]+c; mat[14] = -e*L[2];
   mat[3] = N[0];        mat[7] = N[1];        mat[11] = N[2];        mat[15] = -l;
   //  Multiply modelview matrix
   glMultMatrixf(mat);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   sprintf(app_launch,"%s", argv[0]);
   char * last_slash = strrchr(app_launch,  '/');
   if ( last_slash != NULL)
	*last_slash = '\0';
   else
     app_launch[0] = '\0';
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(win_hgt,win_wdth);
   glutCreateWindow("\'Renny in Funderland\', Birdsong Final Project");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   // Create floor dimensions
   floor_init();
   //Create terrain, must be done before populate_game()
   terrain_init(0);
   // generate game population
   populate_game();
   //  Load Object Textures
   textures[0] = LoadTexBMP("grass.bmp");
   textures[1] = LoadTexBMP("img1.bmp");
   textures[2] = LoadTexBMP("img2.bmp");
   textures[3] = LoadTexBMP("img3.bmp");
   textures[4] = LoadTexBMP("img4.bmp");
   textures[5] = LoadTexBMP("img5.bmp");
   textures[6] = LoadTexBMP("img6.bmp");
   textures[7] = LoadTexBMP("img7.bmp");
   textures[8] = LoadTexBMP("img8.bmp");
   textures[9] = LoadTexBMP("BlueSkyTexture.bmp");
   textures[10] = LoadTexBMP("grass_no_grid.bmp");
   textures[11] = LoadTexBMP("forest_grid.bmp");
   textures[12] = LoadTexBMP("forest_no_grid.bmp");
   textures[13] = LoadTexBMP("teapot.bmp");
   textures[14] = LoadTexBMP("fog.bmp");
   //  Load Floor Textures
   floor_textures[0][0] = LoadTexBMP("floor1a.bmp");
   floor_textures[0][1] = LoadTexBMP("floor1b.bmp");
   floor_textures[0][2] = LoadTexBMP("floor1c.bmp");
   floor_textures[0][3] = LoadTexBMP("floor1d.bmp");
   floor_textures[1][0] = LoadTexBMP("floor2a.bmp");
   floor_textures[1][1] = LoadTexBMP("floor2b.bmp");
   floor_textures[1][2] = LoadTexBMP("floor2c.bmp");
   floor_textures[1][3] = LoadTexBMP("floor2d.bmp");
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
