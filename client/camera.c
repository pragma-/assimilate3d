/*==========================================================
  File:  camera.c
  Author:  _pragma

  Description:  Basic camera handling.
  ==========================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "interp.h"
#include "bsp.h"
#include "ms3d.h"
#include "model.h"
#include "entity.h"
#include "camera.h"
#include "a3dmath.h"
#include "cvar.h"
#include "texture.h"
#include "console.h"
#include "util.h"
#include "game.h"

camera_t camera[2];
int gc_current_camera = 0;

void C_initCamera(void)
{
  extern ent_t *ge_player;

  memset(&camera[0], 0, sizeof(camera_t));
  memset(&camera[1], 0, sizeof(camera_t));

  C_positionCamera(&camera[0], 0.0, 0.0, 250.0, 
                         0.0, 0.0, 225.0, 
                         0.0, 1.0, 0.0);

  C_bindCameraToEntity(&camera[0], ge_player);

  C_positionCamera(&camera[1], 0.0, 200.0, 250.0, 
                         0.0, 0.0, 240.0, 
                         0.0, 1.0, 0.0);
}

void C_bindCameraToEntity(camera_t *camera, ent_t *entity)
{
  camera->entity = entity;
}

void C_moveCameraToEntity(camera_t *camera, ent_t *entity)
{
  int i;

  for(i = 0; i < 3; i++)
    camera->position[i] = entity->position[i];
  for(i = 0; i < 3; i++)
    camera->view[i] = entity->view[i];
  for(i = 0; i < 3; i++)
    camera->up[i] = entity->up[i];
}

void C_positionCamera(camera_t *camera, float px, float py, float pz, 
                                        float vx, float vy, float vz, 
                                        float ux, float uy, float uz)
{
  camera->position[0] = px;
  camera->position[1] = py;
  camera->position[2] = pz;

  camera->view[0] = vx;
  camera->view[1] = vy;
  camera->view[2] = vz;

  camera->up[0] = ux;
  camera->up[1] = uy;
  camera->up[2] = uz;
}

COMMAND(CMD_camera)
{
  gc_current_camera = !gc_current_camera;
}
