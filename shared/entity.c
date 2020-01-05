/*==========================================================
  File:  entity.c
  Author:  _pragma

  Description:  Dynamic entity routines.
  ==========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "interp.h"
#include "commands.h"
#include "cvar.h"
#include "game.h"
#include "a3dmath.h"
#include "bsp.h"
#include "ms3d.h"
#include "model.h"
#include "texture.h"
#include "console.h"
#include "entity.h"
#include "config.h"
#include "util.h"

/* This entire file needs to be redone.  The layout and the
   math is all horrible and wrong. */

ent_t *ge_linkedentities = 0;  // only entities in this list will be processed
                               // for movements, collisions, etc

ent_t *ge_entity_list = 0;     // list of all loaded entities

ent_t *ge_player;              // the main player entity
ent_t *ge_entity;              // global entity object (used for loading new entities)

int ge_entity_identifier = 0;  // unique indentifiers for entities

extern BSPmoveData_t gbsp_moveData;
extern BSPLevel_t *gbsp_map;

ent_t *E_createEntity(char *name)
{
  ent_t *entity;

  if(!(entity = malloc(sizeof(ent_t))))
  {
    CON_printf("warning: no memory for ent %s", name);
    return 0;
  }

  memset(entity, 0, sizeof(ent_t));

  if(!(entity->name = strdup(name)))
  {
    CON_printf("warning: no memory for ent %s", name);
    free(entity);
    return 0;
  }

  entity->id = ++ge_entity_identifier;

  return entity;
}

ent_t *E_findEntityById(int id)
{
  ent_t *entity;

  for(entity = ge_entity_list; entity; entity = entity->next)
    if(entity->id == id)
      return entity;

  return 0;
}

void E_bindModelToEntity(m_model_t *model, ent_t *entity)
{
  entity->model = model;
}

void E_destroyEntity(ent_t **entity)
{
  if(!*entity)
    return;

  CON_printf("Destroying entity %d: %s", (*entity)->id, (*entity)->name ? (*entity)->name : "[null]");

  if((*entity)->name)
    free((*entity)->name);
  free(*entity);
  *entity = 0;
}

void E_destroyAllEntities(void)
{
  ent_t *entity, *next;

  CON_printf("------- Destroying Entities --------");

  for(entity = ge_entity_list; entity; entity = next)
  {
    next = entity->next;
    E_destroyEntity(&entity);
  }
  CON_printf("------------------------------------");
}

void E_strafeEntity(ent_t *entity, float speed)
{
  extern cvar_t *cvar_maxSpeed;
  float maxspeed = atof(cvar_maxSpeed->value);

  entity->acceleration[0] = entity->strafe[0] * speed;
  entity->acceleration[2] = entity->strafe[2] * speed;

  entity->velocity[0] += entity->acceleration[0];
  entity->velocity[2] += entity->acceleration[2];

  speed = sqrt(entity->velocity[0] * entity->velocity[0] +
               entity->velocity[2] * entity->velocity[2]);

  if(speed > maxspeed)
  {
    entity->velocity[0] *= maxspeed / speed;
    entity->velocity[2] *= maxspeed / speed;
  }

  entity->move[3] = true;
}

void E_moveEntity(ent_t *entity, float speed)
{
  float v[3];
  extern cvar_t *cvar_maxSpeed;
  float maxspeed = atof(cvar_maxSpeed->value);

  M_vsubtract(v, entity->view, entity->position);
  M_vnormalize(v);

  entity->acceleration[0] = v[0] * speed;
  entity->acceleration[2] = v[2] * speed;

  entity->velocity[0] += entity->acceleration[0];
  entity->velocity[2] += entity->acceleration[2];

  speed = sqrt(entity->velocity[0] * entity->velocity[0] +
               entity->velocity[2] * entity->velocity[2]);

  if(speed > maxspeed)
  {
    entity->velocity[0] *= maxspeed / speed;
    entity->velocity[2] *= maxspeed / speed;
  }

  entity->move[3] = true;
}

void E_moveEntityTo(ent_t *entity, float *position)
{
  entity->view[0] += position[0] - entity->position[0];
  entity->view[1] += position[1] - entity->position[1];
  entity->view[2] += position[2] - entity->position[2];

  entity->position[0] = position[0];
  entity->position[1] = position[1];
  entity->position[2] = position[2];
}

void E_rotateEntity(ent_t *entity, float angle, float x, float y, float z)
{
  float view[3], r[3];
  float cosTheta = cos(angle), sinTheta = sin(angle);

  M_vsubtract(view, entity->view, entity->position);

  // Find the new x position for the new rotated point
  r[0]  = (cosTheta + (1 - cosTheta) * x * x)       * view[0];
  r[0] += ((1 - cosTheta) * x * y - z * sinTheta)   * view[1];
  r[0] += ((1 - cosTheta) * x * z + y * sinTheta)   * view[2];

  // Find the new y position for the new rotated point
  r[1]  = ((1 - cosTheta) * x * y + z * sinTheta)   * view[0];
  r[1] += (cosTheta + (1 - cosTheta) * y * y)       * view[1];
  r[1] += ((1 - cosTheta) * y * z - x * sinTheta)   * view[2];

  // Find the new z position for the new rotated point
  r[2]  = ((1 - cosTheta) * x * z - y * sinTheta)   * view[0];
  r[2] += ((1 - cosTheta) * y * z + x * sinTheta)   * view[1];
  r[2] += (cosTheta + (1 - cosTheta) * z * z)       * view[2];

  r[0] += entity->position[0];
  r[1] += entity->position[1];
  r[2] += entity->position[2];

  entity->view[0] = r[0];
  entity->view[1] = r[1];
  entity->view[2] = r[2];
}

void E_lookHoriz(ent_t *entity, float speed)
{
  E_rotateEntity(entity, speed, 0, 1, 0);
}

void E_lookVert(ent_t *entity, float speed)
{
  float r[3], a[3];  // axis vector

  M_vsubtract(r, entity->view, entity->position);
  M_vcross(a, r, entity->up);
  M_vnormalize(a);

  /* rotate around the ZX axis */
  E_rotateEntity(entity, speed, a[0], a[1], a[2]);
}

void E_updateEntity(ent_t *entity, int msec)
{
  float v[3], r[3], time;
  extern int game_state;

  if(game_state != GAME_STATE_PLAYING)
    return;

  M_vsubtract(v, entity->view, entity->position);
  M_vcross(r, v, entity->up);
  M_vnormalize(r);

  entity->strafe[0] = r[0];
  entity->strafe[1] = r[1];
  entity->strafe[2] = r[2];

  time = (float)msec * 0.001f;

  E_updateEntityPhysics(entity, time);

  if(entity->move[3])
    E_moveEntityCollisionBSP(entity, gbsp_map);
}

void E_updateEntityPhysics(ent_t *entity, float time)
{
  float v[3];
  int i;
  extern cvar_t *cvar_noclip, *cvar_friction, *cvar_gravity;
  float friction = atof(cvar_friction->value),
        gravity  = atof(cvar_gravity->value);
  int noclip = atoi(cvar_noclip->value);

 /* Gravity.
  * Checks if we 'fall' down without hitting anything.  If so,
  * we increase the Y axis velocity by the gravity constant * !noclip.
  * Otherwise, if we 'fall' down and strike a plane, we store that plane
  * as our ground plane for future calculations.
  */

  if(!noclip && gbsp_map)
  {
    v[0] = entity->position[0];
    v[1] = entity->position[1] - entity->boundingSphereRadius;
    v[2] = entity->position[2];

    BSP_rayTraceSphere(gbsp_map, entity->position, v, entity->boundingSphereRadius);

    if(gbsp_moveData.fraction > 0)
    {
      // entity is not on the ground
      entity->velocity[1] += gravity;
      entity->move[3] = true;
      entity->groundPlaneIndex = -1;
    }
    else
    {
      // entity is on the ground
      if(entity->action_flags & PAF_JUMPING)
      {
        // entity has landed from a jump
        entity->action_flags &= ~PAF_JUMPING;
//        entity->velocity[1] = 0;
      }
      else if(entity->velocity[1] > 0)
        // entity hasn't landed from jump and is actually moving upwards
        // so we want the engine to move this entity
        entity->move[3] = true;

      entity->groundPlaneIndex = gbsp_moveData.collisionPlaneIndex;
    }
  }

 /* Friction.
  * If no clip is enabled, we force friction on all axises whether
  * entity is on the ground or not (free flying noclip).  Otherwise
  * we apply friction on the X and Z axises if the entity is on the
  * ground.
  */

  if(noclip)
  {
    // noclip is enabled, force friction on all axises
    for(i = 0; i < 3; i++)
      if(entity->velocity[i])
      {
        entity->velocity[i] = friction * entity->velocity[i];
        entity->move[3] = true;
      }
  }
  else
  {
    // noclip is disabled, normal action

    if(entity->groundPlaneIndex >= 0)
    {
      // entity is on the ground
      for(i = 0; i < 3; i++)
      {
        // skip Y axis (up/down) and velocities without force
        if(i == 1 || entity->velocity[i] == 0) 
          continue;

        entity->velocity[i] = friction * entity->velocity[i];

        if(fabs(entity->velocity[i]) < 1)
          entity->velocity[i] = 0;
        else
          entity->move[3] = true;
      }  
    }
    else
    {
      // entity is in the air - todo: air friction?
      // currently no action
    }
  }

 /*
  * Update the move vector for the move we wish to make.
  */

  entity->move[0] = entity->velocity[0] * time;
  entity->move[1] = entity->velocity[1] * time;
  entity->move[2] = entity->velocity[2] * time;
}

void E_positionEntity(ent_t *entity, float px, float py, float pz, 
                                     float vx, float vy, float vz, 
                                     float ux, float uy, float uz)
{
  entity->position[0] = px;
  entity->position[1] = py;
  entity->position[2] = pz;

  entity->view[0] = vx;
  entity->view[1] = vy;
  entity->view[2] = vz;

  entity->up[0] = ux;
  entity->up[1] = uy;
  entity->up[2] = uz;
}

void E_moveEntityCollisionBSP(ent_t *entity, BSPLevel_t *bsplevel)
{
  extern cvar_t *cvar_noclip;
  float v[3];

  if((atoi(cvar_noclip->value) && entity == ge_player))
  {
    v[0] = entity->position[0] + entity->move[0];
    v[1] = entity->position[1] + entity->move[1];
    v[2] = entity->position[2] + entity->move[2];
    E_moveEntityTo(entity, v);
  }
  else
    E_moveEntityNewPositionBSP(entity, bsplevel);

  entity->move[3] = false;
}

void E_moveEntityNewPositionBSP(ent_t *entity, BSPLevel_t *bsplevel)
{
  float v[3], dot;

  if(!bsplevel)
    return;

  // set up end point of our move attempt
  v[0] = entity->position[0] + entity->move[0];
  v[1] = entity->position[1] + entity->move[1];
  v[2] = entity->position[2] + entity->move[2];

  // trace ray to end point and fill gbsp_moveData structure with results
  BSP_rayTraceSphere(bsplevel, entity->position, v, entity->boundingSphereRadius);

  if(gbsp_moveData.fraction > 0 && !gbsp_moveData.allSolid)
  {
    E_moveEntityTo(entity, gbsp_moveData.endPoint);
    return;
  }

//  CON_printf("Starting slide");

  while(gbsp_moveData.fraction == 0)
  {
    M_vnormalize(gbsp_moveData.collisionPlaneNormal);
    dot = M_vdot(gbsp_moveData.collisionPlaneNormal, entity->move);

    v[0] += gbsp_moveData.collisionPlaneNormal[0] * -dot;
    v[1] += gbsp_moveData.collisionPlaneNormal[1] * -dot;
    v[2] += gbsp_moveData.collisionPlaneNormal[2] * -dot;

    BSP_rayTraceSphere(bsplevel, entity->position, v, entity->boundingSphereRadius);

//    CON_printf("tracing to (%.3f, %.3f, %.3f)", v[0], v[1], v[2]);

    if(gbsp_moveData.fraction == 1 && !gbsp_moveData.allSolid)
    {
//      CON_printf("moving entity to (%.3f, %.3f, %.3f)", v[0], v[1], v[2]);
      E_moveEntityTo(entity, v);
    }
  }

//  CON_printf("End slide");
} 

 //=======================================================================// 
 // Player specifics.                                                     //
 // The following functions are specific to the main local player entity. //
 //=======================================================================//

int E_initPlayerEntity(void)
{
  if(!(ge_player = E_createEntity("Player")))
    return false;

  ge_player->next = ge_entity_list;
  ge_entity_list = ge_player;

  ge_player->health = 100;
  ge_player->boundingSphereRadius = 20;

  ge_player->model = M_loadModelMS3D("game/player.ms3d");

  E_positionEntity(ge_player, 0, 50, 0, 50, 0, 20, 0, 1, 0); 

  return true;
}

void E_updatePlayerEntity(ent_t *entity, int msec)
{
  float v[3], r[3], time, speed;
  extern int game_state, gt_curtime;
  extern cvar_t *cvar_moveForward,
                *cvar_moveBack,
                *cvar_moveLeft,
                *cvar_moveRight,
                *cvar_moveDown,
                *cvar_moveUp,
                *cvar_moveSpeed,
                *cvar_lookLeft,
                *cvar_lookRight,
                *cvar_yawSpeed,
                *cvar_lookUp,
                *cvar_lookDown,
                *cvar_pitchSpeed,
                *cvar_noclip,
                *cvar_thirdperson;
  int noclip = atoi(cvar_noclip->value),
      thirdperson = atoi(cvar_thirdperson->value) ? -1 : 1;
  float is_on_ground;

  if(game_state != GAME_STATE_PLAYING)
    return;

  M_vsubtract(v, entity->view, entity->position);
  M_vcross(r, v, entity->up);
  M_vnormalize(r);

  entity->strafe[0] = r[0];
  entity->strafe[1] = r[1];
  entity->strafe[2] = r[2];

  time = (float)msec * 0.001f;
  speed = atof(cvar_moveSpeed->value);
  is_on_ground = entity->groundPlaneIndex >= 0;

  entity->acceleration[0] = 0;
  entity->acceleration[1] = 0;
  entity->acceleration[2] = 0;

  if(atoi(cvar_moveUp->value))
  {
    if(noclip)
    {
      // noclip is enabled, allow free flying
      entity->velocity[1] += speed;
      entity->move[3] = true;
    }
    else
    {
      // noclip is disabled, normal world action
      if(!(entity->action_flags & PAF_JUMPING) && is_on_ground && 
         gt_curtime - entity->jump_time > 1000)
      {
        // entity is not jumping and entity is on ground 
        // and at least 1 sec has elapsed since last jump
        entity->velocity[1] = speed;
        entity->action_flags |= PAF_JUMPING;
        entity->groundPlaneIndex = -1;
        entity->jump_time = gt_curtime;
        entity->move[3] = true;
      }
    }
  }

  if(atoi(cvar_moveDown->value))
  {
    if(noclip)
    {
      // noclip is enabled, allow free flying
      entity->velocity[1] -= speed;
      entity->move[3] = true;
    }
    else
    {
      // currently does nothing, todo fixme: crouching
    }
  }

  // if noclip is enabled, pretend the entity is always on the
  // ground to allow momentum
  if(noclip)
    is_on_ground = 1;
  else
  // if the entity is not on the ground and noclip is disabled
  // allow it to move through air, but only at 5% of the speed
  if(!is_on_ground)
    is_on_ground = 0.05f; 

  if(atoi(cvar_moveForward->value)) E_moveEntity(entity, speed * is_on_ground * thirdperson);
  if(atoi(cvar_moveBack->value))    E_moveEntity(entity, -speed * is_on_ground * thirdperson);
  if(atoi(cvar_moveLeft->value))    E_strafeEntity(entity, -speed * is_on_ground * thirdperson);
  if(atoi(cvar_moveRight->value))   E_strafeEntity(entity, speed * is_on_ground * thirdperson);

  if(atoi(cvar_lookRight->value))   E_lookHoriz(entity, -atof(cvar_yawSpeed->value));
  if(atoi(cvar_lookLeft->value))    E_lookHoriz(entity, atof(cvar_yawSpeed->value));
  if(atoi(cvar_lookUp->value))      E_lookVert(entity, atof(cvar_pitchSpeed->value));
  if(atoi(cvar_lookDown->value))    E_lookVert(entity, -atof(cvar_pitchSpeed->value));

  E_updateEntityPhysics(entity, time);

  if(entity->move[3])
    E_moveEntityCollisionBSP(entity, gbsp_map);
}

 //=======================================================================// 
 // Dynamic Entities                                                      //
 // The following functions are specific to the entity editor and whatnot //
 //=======================================================================//

COMMAND(CMD_listEntities)
{
  ent_t *entity;
  int i = 0;

  for(entity = ge_entity_list; entity; entity = entity->next)
  {
    CON_printf("^14;%-30s ^3[^14;%2d^3] : (^14;%5.3f^3, ^14;%5.3f^3, ^14;%5.3f^3)", entity->name, entity->id, 
                               entity->position[0], entity->position[1], entity->position[2]);
    CON_printf("Model^3: ^14;%s", entity->model ? entity->model->name ? entity->model->name : "^3[^14no name model^3]" : "^3[^14no model^3]"); 
    i++;
  }
  CON_printf("%d entities", i);
}

ent_t *E_loadEntity(char *name)
{
  ent_t *entity;
  FILE *fp;

  // First we make sure the entity file exists... (hackish)
  if(!(fp = fopen(name, "r")))
  {
    CON_printf("loadEntity: Could not open '%s'", name);
    return 0;
  }
  fclose(fp);

  // Now that we know it exists, we can create the
  // entity object.
  if(!(entity = E_createEntity(name)))
  {
    CON_printf("Out of memory for entity '%s'", name);
    return 0;
  }

  // add to linked list of entities
  entity->next = ge_entity_list;
  ge_entity_list = entity;

  // Set the current working entity to the newly created object.
  ge_entity = entity;

  // Read in and execute commands related to this entity
  // from the entity file.
  config_read(name);

  return entity;
}

COMMAND(CMD_loadEntity)
{
  ent_t *entity;

  if(!(entity = E_loadEntity(arguments)))
    CON_printf("^9Failed^15 to load ^3'^14;%s^3'", arguments);
  else
  {
    CON_printf("^3'^14;%s^3' ^15loaded as ^14;%d", arguments, entity->id);
    CON_printf("Selected ^3'^14;%s^3' at ^3(^14;%.3f^3, ^14;%.3f^3, ^14;%.3f^3)", 
                entity->name, entity->position[0], entity->position[1], entity->position[2]);
  }
}

// fixme: model should be a clone
COMMAND(CMD_setEntityModel)
{
  m_model_t *model;

  if(!(model = M_findModelByName(arguments)))
    if(!(model = M_loadModelMS3D(arguments)))
    {
      CON_printf("setEntityModel: could not load ^3'^14;%s^3'", arguments);
      return;
    }

  ge_entity->model = model;
}

COMMAND(CMD_selectEntity)
{
  ent_t *entity;

  if(!(entity = E_findEntityById(atoi(arguments))))
  {
    CON_printf("selectEntity^3:^15 could not find ^14;%d", atoi(arguments));
    return;
  }

  ge_entity = entity;
  CON_printf("Selected ^3'^14%s^3'^15 at ^3(^14;%.3f^3,^14 %.3f^3,^14 %.3f^3)", 
    ge_entity->name, ge_entity->position[0], ge_entity->position[1], ge_entity->position[2]);
}



COMMAND(CMD_positionEntity)
{
  char bufx[100], bufy[100], bufz[100], buf[100];
  float x, y, z;

  I_halfChop(arguments, bufx, buf);
  I_halfChop(buf, bufy, bufz);

  if(!*bufx || !*bufy || !*bufz)
  {
    CON_printf("Usage:  position x y z");
    return;
  }

  x = atof(bufx);
  y = atof(bufy);
  z = atof(bufz);

  ge_entity->velocity[0] = 0;
  ge_entity->velocity[1] = 0;
  ge_entity->velocity[2] = 0;

  E_positionEntity(ge_entity, x, y, z,
             x,
             y,
             z + 60,
             0, 1, 0);
}
