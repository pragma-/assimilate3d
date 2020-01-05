/*==========================================================
  File:  render.c
  Author:  _pragma

  Description:  Draws a frame.
  ==========================================================*/

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "a3dmath.h"
#include "bsp.h"
#include "ms3d.h"
#include "model.h"
#include "entity.h"
#include "camera.h"
#include "gl.h"
#include "interp.h"
#include "font.h"
#include "texture.h"
#include "console.h"
#include "cvar.h"
#include "util.h"
#include "gui_elements.h"
#include "game.h"
#include "lights.h"
#include "sdl_stuff.h"

/*==========================================================
  Function:  R_renderScene(int msec)
  Parameters:  msec - time since last frame in milliseconds
  Returns:  void

  Description:  Renders a frame.
 ==========================================================*/
void R_renderScene(int msec)
{
  char buf[20];
  extern camera_t camera[];
  extern texture_t gt_textures[T_MAX_TEXTURES];
  extern GLuint g_sbList;
  extern int gt_curtime, gt_realtime, gt_fps, gc_current_camera;
  extern cvar_t *cvar_showtime, *cvar_thirdperson,
                *cvar_cameradebug, *cvar_r_fov,
                *cvar_showfps, *cvar_renderBoundingSphere,
                *cvar_scr_width, *cvar_scr_height;
  extern gui_element_t *gui_element_list;
  extern int game_state, game_init;
  extern BSPLevel_t *gbsp_map;
  extern ent_t *ge_player, *ge_entity_list;
  extern BSPmoveData_t gbsp_moveData;
  float fov = atof(cvar_r_fov->value);
  float v[4], f;
  ent_t *entity;
  int width = cvar_scr_width ? atoi(cvar_scr_width->value) : SCREEN_WIDTH;
  int height = cvar_scr_height ? atoi(cvar_scr_height->value) : SCREEN_HEIGHT;
  static GLUquadric *quadric = 0;

  if(quadric == 0) {
    quadric = gluNewQuadric();
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov, width/(float)height, 1.0, 3024.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  C_moveCameraToEntity(&camera[0], camera[0].entity);

  if(atoi(cvar_thirdperson->value) && !gc_current_camera)
    gluLookAt(camera[gc_current_camera].view[0], camera[gc_current_camera].view[1]+20, camera[gc_current_camera].view[2],
              camera[gc_current_camera].position[0], camera[gc_current_camera].position[1], camera[gc_current_camera].position[2],
              camera[gc_current_camera].up[0], camera[gc_current_camera].up[1], camera[gc_current_camera].up[2]);
  else
    gluLookAt(camera[gc_current_camera].position[0], camera[gc_current_camera].position[1]+15, camera[gc_current_camera].position[2],
              camera[gc_current_camera].view[0], camera[gc_current_camera].view[1], camera[gc_current_camera].view[2],
              camera[gc_current_camera].up[0], camera[gc_current_camera].up[1], camera[gc_current_camera].up[2]);

  // Position the main light at the view vector position.
  v[0] = camera[gc_current_camera].view[0];
  v[1] = camera[gc_current_camera].view[1];
  v[2] = camera[gc_current_camera].view[2];
  v[3] = LIGHT_POSITIONAL;

  glLightfv(GL_LIGHT0, GL_POSITION, v);

  // Set color to white.
  glColor4f(1.0, 1.0, 1.0, 1.0);


  // If skybox is loaded, draw it.
  if(g_sbList)
  {
    glDisable(GL_DEPTH_TEST); 
    glDisable(GL_LIGHTING);
//    glDisable(GL_CULL_FACE);

    glPushMatrix();

      glTranslatef(camera[gc_current_camera].position[0],
                   camera[gc_current_camera].position[1],
                   camera[gc_current_camera].position[2]);
      glCallList(g_sbList);

    glPopMatrix();

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST); 
  }

  // If the BSP map is loaded, render it.
  if(gbsp_map)
  {
    glFrontFace(GL_CW);
    glPushMatrix();
    BSP_drawLevel(gbsp_map, camera[gc_current_camera].position);
    glPopMatrix();
    glFrontFace(GL_CCW);
  }

  glActiveTextureARB(GL_TEXTURE1_ARB);
  glDisable(GL_TEXTURE_2D);

  glActiveTextureARB(GL_TEXTURE0_ARB);
  glEnable(GL_TEXTURE_2D);

  // Draw a solid sphere at the position of the view vector
  // Note:  A light with the same color will be positioned here as well.
  glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glTranslatef(camera[0].view[0], camera[0].view[1], camera[0].view[2]);

    glColor3f(0.0, 0.0, 1.0);
    gluSphere(quadric, 0.50, 10, 10);
    // glutSolidSphere(0.50, 10, 10);
  glPopMatrix();

  if(atoi(cvar_thirdperson->value) && ge_player->model && game_init)
  {
    glColor3f(0.8, 0.0, 0.2);
    glPushMatrix();
      glTranslatef(ge_player->position[0], ge_player->position[1], ge_player->position[2]);
      M_renderModel(ge_player->model);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
  }

  if(atoi(cvar_thirdperson->value) && atoi(cvar_renderBoundingSphere->value))
  {
  glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glTranslatef(camera[0].position[0], camera[0].position[1], camera[0].position[2]);

    glColor3f(0.0, 1.0, 0.5);
    gluSphere(quadric, ge_player->boundingSphereRadius, ge_player->boundingSphereRadius, 
                    ge_player->boundingSphereRadius);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
  glPopMatrix();
  }

  glColor4f(1.0, 1.0, 1.0, 1.0);

  for(entity = ge_entity_list; entity; entity = entity->next)
  {
    if(entity->id == 1 || !entity->model) continue;  // skip player entity or entities without model
    glPushMatrix();
      glTranslatef(entity->position[0], entity->position[1], entity->position[2]);
      M_renderModel(entity->model);
    glPopMatrix();
  }

  //============================================//
  // change to ortho for text/console/hud/menus //
  //============================================//

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, width, height, 0.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  glEnable(GL_BLEND);

  glColor4f(1.0, 1.0, 1.0, 1.0);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, gt_textures[0].id);

  if(cvar_showfps && atoi(cvar_showfps->value))
  {
    sprintf(buf, "%d fps", gt_fps);
    F_printf(FONT_DEFAULT, width - 10 * strlen(buf), 0, buf);
  }

  if(cvar_showtime && atoi(cvar_showtime->value))
    F_printf(FONT_DEFAULT, 0, height-(16*6), 
             "^5Actualtime:^4 %d^5, Realtime:^4 %d^5, fps:^4 %d^5, msec:^4 %d", 
                             gt_curtime, gt_realtime, gt_fps, msec);

  if(cvar_cameradebug && atoi(cvar_cameradebug->value))
  {
    F_printf(FONT_DEFAULT, 0, height-(16*7), "start out: %d, all solid: %d", 
          gbsp_moveData.startOut,
          gbsp_moveData.allSolid); 
    F_printf(FONT_DEFAULT, 0, height-(16*5), 
             "Position: x: %5.2f, y: %5.2f, z: %5.2f, ground: %d", 
                                                   camera[gc_current_camera].position[0], 
                                                   camera[gc_current_camera].position[1], 
                                                   camera[gc_current_camera].position[2],
                                                   ge_player->groundPlaneIndex);
    F_printf(FONT_DEFAULT, 0, height-(16*4), "View: x: %5.5f, y: %5.5f, z: %5.5f", 
                                                   camera[gc_current_camera].view[0], 
                                                   camera[gc_current_camera].view[1], 
                                                   camera[gc_current_camera].view[2]);
    F_printf(FONT_DEFAULT, 0, height-(16*3), "Velocity, x: %5.5f, y: %5.5f, z: %5.5f",
                                         ge_player->velocity[0],
                                         ge_player->velocity[1],
                                         ge_player->velocity[2]);
    F_printf(FONT_DEFAULT, 0, height-(16*2), "Move, x: %5.5f, y: %5.5f, z: %5.5f, Move: %1.0f, Jump: %d",
                                         ge_player->move[0],
                                         ge_player->move[1],
                                         ge_player->move[2], ge_player->move[3],
                                         ge_player->action_flags & PAF_JUMPING);
  }

  F_printf(FONT_DEFAULT, 0, height-16, "Press '~' to de/activate the console.");
  F_printf(FONT_DEFAULT, width-(10*7), height-16, "_pragma");

  CON_drawConsole(msec);
  
  if(game_state == GAME_STATE_GUI)
    gui_draw_elements(gui_element_list);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glDisable(GL_BLEND);

  SDL_GL_SwapBuffers();
}
