/*==========================================================
  File:  cvar.c
  Author:  _pragma

  Description:  Handles console variables and aliases.
  ==========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "cvar.h"
#include "util.h"
#include "interp.h"
#include "commands.h"
#include "texture.h"
#include "console.h"
#include "bsp.h"
#include "ms3d.h"
#include "model.h"
#include "entity.h"
#include "server.h"

       /** List of console variables **/

cvar_t *gc_cvar_list = 0, 

       *cvar_cl_name = 0,

       /** Rendering **/

       *cvar_r_maxfps = 0, 
       *cvar_r_fov = 0,

       /** Physics **/

       *cvar_friction = 0,
       *cvar_gravity = 0,

       /** Movement **/

       *cvar_moveSpeed = 0,
       *cvar_maxSpeed = 0,
       *cvar_moveForward = 0, 
       *cvar_moveBack = 0, 
       *cvar_moveLeft = 0, 
       *cvar_moveRight = 0, 
       *cvar_moveUp = 0, 
       *cvar_moveDown = 0, 
       *cvar_noclip = 0,

       /** keyboard view **/

       *cvar_lookLeft = 0,
       *cvar_lookRight = 0,
       *cvar_lookUp = 0,
       *cvar_lookDown = 0,
       *cvar_yawSpeed = 0,
       *cvar_pitchSpeed = 0,

       /** Console **/

       *cvar_paging_size = 0,
       *cvar_con_rottime = 0,
       *cvar_con_rottime_mod = 0,
       *cvar_con_index_screen = 0, 

       /** mouse view **/

       *cvar_myawSpeed = 0,
       *cvar_mpitchSpeed = 0,
       *cvar_msensitivity = 0,
       *cvar_thirdperson = 0,

       /** Debug messages **/

       *cvar_framedebug = 0,
       *cvar_cvardebug = 0, 
       *cvar_cameradebug = 0,
       *cvar_physicsdebug = 0,
       *cvar_movedebug = 0,
       *cvar_condebug = 0,

       /** Visual Debug **/

       *cvar_renderBoundingSphere,

       /** Informative messages **/

       *cvar_showfps = 0,
       *cvar_showtime = 0,

       /** Server variables **/

       *cvar_serverPort = 0;

cvar_t *gc_alias_list = 0;

#define CVAR_INIT(c, k, v, f) if(!( (c) = cvar_new((k), (v),(f)) )) return false
#define ALIAS_INIT(c, k, v, f) if(!( (c) = alias_new((k), (v),(f)) )) return false

int cvar_init(void)
{
  CVAR_INIT(cvar_cl_name,     "name",   "Client",  CVAR_PRIVATE);

  CVAR_INIT(cvar_condebug,    "con_debug",     "0",   CVAR_PRIVATE | CVAR_TOGGLE);
  CVAR_INIT(cvar_cvardebug,   "cvar_debug",    "0",   CVAR_PRIVATE | CVAR_TOGGLE);
  CVAR_INIT(cvar_framedebug,  "frame_debug",   "0",   CVAR_PRIVATE | CVAR_TOGGLE);
  CVAR_INIT(cvar_cameradebug, "cam_debug",     "0",   CVAR_PRIVATE | CVAR_TOGGLE);
  CVAR_INIT(cvar_physicsdebug,"physics_debug", "0",   CVAR_PRIVATE | CVAR_TOGGLE);
  CVAR_INIT(cvar_movedebug,   "move_debug",    "0",   CVAR_PRIVATE | CVAR_TOGGLE);

  CVAR_INIT(cvar_renderBoundingSphere,   "d_showsphere",    "0",   CVAR_PRIVATE | CVAR_TOGGLE);

  CVAR_INIT(cvar_r_maxfps,     "r_maxfps",   "120",  CVAR_PRIVATE);
  CVAR_INIT(cvar_r_fov,        "r_fov",      "70.0", CVAR_PRIVATE);
  CVAR_INIT(cvar_showtime,     "showtime",   "0",    CVAR_PRIVATE | CVAR_TOGGLE);
  CVAR_INIT(cvar_showfps,      "r_drawfps",  "0",    CVAR_PRIVATE | CVAR_TOGGLE);

  /** Physics **/

  CVAR_INIT(cvar_gravity,      "p_gravity",    "-9.8", CVAR_PRIVATE);
  CVAR_INIT(cvar_friction,     "p_friction",   "0.85",  CVAR_PRIVATE);

  /** Movement **/

  CVAR_INIT(cvar_moveSpeed,   "movespeed",   "75", CVAR_PRIVATE);
  CVAR_INIT(cvar_maxSpeed,    "maxspeed",   "600", CVAR_PRIVATE);
  CVAR_INIT(cvar_moveForward, "moveforward", "0", CVAR_PRIVATE);
  CVAR_INIT(cvar_moveBack,    "moveback",    "0", CVAR_PRIVATE);
  CVAR_INIT(cvar_moveLeft,    "moveleft",    "0", CVAR_PRIVATE);
  CVAR_INIT(cvar_moveRight,   "moveright",   "0", CVAR_PRIVATE);
  CVAR_INIT(cvar_moveUp,      "moveup",      "0", CVAR_PRIVATE);
  CVAR_INIT(cvar_moveDown,    "movedown",    "0", CVAR_PRIVATE);
  CVAR_INIT(cvar_noclip,      "noclip",      "0", CVAR_PRIVATE | CVAR_TOGGLE);

  /** Console **/

  CVAR_INIT(cvar_paging_size,      "paging_size", "5",    CVAR_PRIVATE);
  CVAR_INIT(cvar_con_rottime,      "con_rottime", "2000", CVAR_PRIVATE);
  CVAR_INIT(cvar_con_rottime_mod,  "con_rottime_mod", "0.85", CVAR_PRIVATE);
  CVAR_INIT(cvar_con_index_screen, "con_screen",  "8",    CVAR_PRIVATE);

  /** Server **/

  CVAR_INIT(cvar_serverPort,   "sv_port", SERVER_DEFAULT_PORT, CVAR_PRIVATE);

  /** View **/

  CVAR_INIT(cvar_lookLeft,     "lookleft",     "0",     CVAR_PRIVATE);
  CVAR_INIT(cvar_lookRight,    "lookright",    "0",     CVAR_PRIVATE);
  CVAR_INIT(cvar_yawSpeed,     "kb_yawspeed",  "0.035", CVAR_PRIVATE);

  CVAR_INIT(cvar_lookUp,       "lookup",         "0",     CVAR_PRIVATE);
  CVAR_INIT(cvar_lookDown,     "lookdown",       "0",     CVAR_PRIVATE);
  CVAR_INIT(cvar_pitchSpeed,   "kb_pitchspeed",  "0.035", CVAR_PRIVATE);

  CVAR_INIT(cvar_mpitchSpeed,   "m_pitchspeed",  "0.035", CVAR_PRIVATE);
  CVAR_INIT(cvar_myawSpeed,     "m_yawspeed",    "0.035", CVAR_PRIVATE);
  CVAR_INIT(cvar_msensitivity,  "m_sensitivity", "3",     CVAR_PRIVATE);
  CVAR_INIT(cvar_thirdperson,   "c_thirdperson", "0",     CVAR_PRIVATE | CVAR_TOGGLE);

  return true;
}

int alias_init(void)
{
  cvar_t *alias;

  /** Movement **/

  ALIAS_INIT(alias,  "+forward",   "moveforward 1", CVAR_PRIVATE);
  ALIAS_INIT(alias,  "-forward",   "moveforward 0", CVAR_PRIVATE);
  ALIAS_INIT(alias,  "+back",      "moveback 1",    CVAR_PRIVATE);
  ALIAS_INIT(alias,  "-back",      "moveback 0",    CVAR_PRIVATE);
  ALIAS_INIT(alias,  "+moveleft",  "moveleft 1",    CVAR_PRIVATE);
  ALIAS_INIT(alias,  "-moveleft",  "moveleft 0",    CVAR_PRIVATE);
  ALIAS_INIT(alias,  "+moveright", "moveright 1",   CVAR_PRIVATE);
  ALIAS_INIT(alias,  "-moveright", "moveright 0",   CVAR_PRIVATE);
  ALIAS_INIT(alias,  "+moveup",    "moveup 1",      CVAR_PRIVATE);
  ALIAS_INIT(alias,  "-moveup",    "moveup 0",      CVAR_PRIVATE);
  ALIAS_INIT(alias,  "+movedown",  "movedown 1",    CVAR_PRIVATE);
  ALIAS_INIT(alias,  "-movedown",  "movedown 0",    CVAR_PRIVATE);

  /** View **/
 
  ALIAS_INIT(alias,  "+lookright", "lookright 1",    CVAR_PRIVATE);
  ALIAS_INIT(alias,  "-lookright", "lookright 0",    CVAR_PRIVATE);
  ALIAS_INIT(alias,  "+lookleft",  "lookleft 1",     CVAR_PRIVATE);
  ALIAS_INIT(alias,  "-lookleft",  "lookleft 0",     CVAR_PRIVATE);

  ALIAS_INIT(alias,  "+lookup",    "lookup 1",       CVAR_PRIVATE);
  ALIAS_INIT(alias,  "-lookup",    "lookup 0",       CVAR_PRIVATE);
  ALIAS_INIT(alias,  "+lookdown",  "lookdown 1",     CVAR_PRIVATE);
  ALIAS_INIT(alias,  "-lookdown",  "lookdown 0",     CVAR_PRIVATE);

  return true;
}

void cvar_destroyCvars(void)
{
  cvar_t *cvar, *temp;

  for(cvar = gc_cvar_list; cvar; cvar = temp)
  {
    temp = cvar->next;

    if(cvar->key)
      free(cvar->key);
    if(cvar->value)
      free(cvar->value);
    free(cvar);
  }
}

void cvar_destroyAliases(void)
{
  cvar_t *cvar, *temp;

  for(cvar = gc_alias_list; cvar; cvar = temp)
  {
    temp = cvar->next;

    if(cvar->key)
      free(cvar->key);
    if(cvar->value)
      free(cvar->value);
    free(cvar);
  }
}

cvar_t *alias_new(char *key, char *value, int flags)
{
  cvar_t *cvar;

  if(!(cvar = cvar_create(key, value, flags)))
    return 0;

  cvar_add(cvar, &gc_alias_list);
  return cvar;
}

cvar_t *cvar_new(char *key, char *value, int flags)
{
  cvar_t *cvar;

  if(!(cvar = cvar_create(key, value, flags)))
    return 0;

  cvar_add(cvar, &gc_cvar_list);

  //printf("cvar: %s, %s, %d\n", cvar->key, cvar->value, cvar->flags);

  return cvar;
}

cvar_t *cvar_create(char *key, char *value, int flags)
{
  cvar_t *cvar;

  if(!(cvar = malloc(sizeof(cvar_t))))
  {
    perror("cvar_create");
    return 0;
  }

  cvar->key = strdup(key);
  cvar->value = strdup(value);

  cvar->next = 0;
  cvar->flags = flags;

  if(cvar_cvardebug && atoi(cvar_cvardebug->value))
    CON_printf("New cvar: (\'%s\' = \'%s\')", key, value);

  return cvar;
}

void cvar_destroy(cvar_t *cvar)
{
  if(cvar)
  {
    if(cvar_cvardebug && atoi(cvar_cvardebug->value))
      CON_printf("Destroying cvar: (\'%s\' = \'%s\')", cvar->key, cvar->value);

    if(cvar->key)
      free(cvar->key);
    if(cvar->value)
      free(cvar->value);
 
    if(cvar->flags & CVAR_LISTED)
      cvar_remove(cvar, &gc_cvar_list);

    free(cvar);
    cvar = 0;
  }
}

void alias_destroy(cvar_t *cvar)
{
  if(cvar)
  {
    if(cvar_cvardebug && atoi(cvar_cvardebug->value))
      CON_printf("Destroying alias: (\'%s\' = \'%s\')", cvar->key, cvar->value);

    if(cvar->key)
      free(cvar->key);
    if(cvar->value)
      free(cvar->value);
 
    if(cvar->flags & CVAR_LISTED)
      cvar_remove(cvar, &gc_alias_list);

    free(cvar);
    cvar = 0;
  }
}

void cvar_add(cvar_t *cvar, cvar_t **list)
{
  if(cvar)
  {
    cvar->flags |= CVAR_LISTED;

    cvar->next = *list;
    *list = cvar;
  }
}

void cvar_remove(cvar_t *cvar, cvar_t **list)
{
  cvar_t *temp;

  if(cvar == *list) 
    *list = cvar->next;
  else 
  { 
    temp = *list; 
    
    while (temp && (temp->next != (cvar))) 
      temp = temp->next; 

    if (temp) 
      temp->next = cvar->next; 
  }
}

cvar_t *cvar_findKey(char *key)
{
  cvar_t *c;

  for(c = gc_cvar_list; c; c = c->next)
    if(!stricmp(key, c->key))
      return c;
  return 0;
}

cvar_t *alias_findKey(char *key)
{
  cvar_t *c;

  for(c = gc_alias_list; c; c = c->next)
    if(!stricmp(key, c->key))
      return c;
  return 0;
}

cvar_t *cvar_findValue(void *value)
{
  cvar_t *c;

  for(c = gc_cvar_list; c; c = c->next)
    if(!stricmp(value, c->value))
      return c;
  return 0;
}

COMMAND(CMD_set)
{
  char key[CON_BCS], value[CON_BCS];
  cvar_t *cvar;

  I_halfChop(arguments, key, value);

  if(!*value)
  {
    CON_printf("Usage: set <variable> <value>");
    return;
  }

  if(!(cvar = cvar_findKey(key)))
  {
    cvar = cvar_new(key, value, 0);
    return;
  }

  if(cvar->flags & CVAR_LOCKED)
  {
    CON_printf("This variable cannot be modified.");
    return;
  }

  if(cvar->value)
    free(cvar->value);

  cvar->value = strdup(value);
}

COMMAND(CMD_unset)
{
  char key[CON_BCS], value[CON_BCS];
  cvar_t *cvar;

  I_halfChop(arguments, key, value);

  if(!(cvar = cvar_findKey(key)))
  {
    CON_printf("No such variable.");
    return;
  }

  if(cvar->flags & CVAR_PRIVATE)
  {
    CON_printf("This variable cannot be unset.");
    return;
  }

  if(cvar->flags & CVAR_LOCKED)
  {
    CON_printf("This variable cannot be modified.");
    return;
  }

  cvar_destroy(cvar);
}

int cvar_parseCommand(char *command, char *arguments)
{
  cvar_t *cvar;
  int value;

  if(!(cvar = cvar_findKey(command)))
    return false;

  if(!*arguments)
  {
    if((cvar->flags & CVAR_TOGGLE) && !(cvar->flags & CVAR_LOCKED))
    {
      value = !atoi(cvar->value);
      sprintf(cvar->value, "%d", value);
      CON_printf("\'%s\' %s.", cvar->key, value ? "enabled" : "disabled");
    }
    else
      CON_printf("\'%s\' == \'%s\'", cvar->key, cvar->value);

    return true;
  }
    
  if(cvar->flags & CVAR_LOCKED)
  {
    CON_printf("This variable cannot be modified.");
   return true;
  }

  if(cvar->value)
    free(cvar->value);

  cvar->value = strdup(arguments);
  return true;
}

COMMAND(CMD_cvars)
{
  cvar_t *c;
  int i;

  for(i = 0, c = gc_cvar_list; c; c = c->next, i++)
    CON_printf("\'%s\' == \'%s\'", c->key, c->value);
  CON_printf("%d console variables", i);
}

COMMAND(CMD_alias)
{
  char key[CON_BCS], value[CON_BCS];
  cvar_t *cvar;

  I_halfChop(arguments, key, value);

  cvar = alias_findKey(key);

  if(!*value)
  {
    if(!cvar)
      CON_printf("Usage: alias <alias> <command>");
    else
      CON_printf("\'%s\' == \'%s\'", cvar->key, cvar->value);
    return;
  }

  if(!cvar)
  {
    cvar = alias_new(key, value, 0);
    return;
  }

  if(cvar->flags & CVAR_LOCKED)
  {
    CON_printf("This alias cannot be modified.");
    return;
  }

  if(cvar->value)
    free(cvar->value);

  cvar->value = strdup(value);
}

COMMAND(CMD_unalias)
{
  char key[CON_BCS], value[CON_BCS];
  cvar_t *cvar;

  I_halfChop(arguments, key, value);

  if(!(cvar = alias_findKey(key)))
  {
    CON_printf("No such alias.");
    return;
  }

  if(cvar->flags & CVAR_PRIVATE)
  {
    CON_printf("This alias cannot be unset.");
    return;
  }

  if(cvar->flags & CVAR_LOCKED)
  {
    CON_printf("This alias cannot be modified.");
    return;
  }

  alias_destroy(cvar);
}

int alias_parseCommand(char *command, char *arguments)
{
  cvar_t *cvar;
//  char buf[CON_BCS];

  if(!(cvar = alias_findKey(command)))
    return false;

  if(!*arguments)
  {
  //  strcpy(buf, cvar->value);
    CON_addCommand(cvar->value);
    return true;
  }
    
  if(cvar->flags & CVAR_LOCKED)
  {
    CON_printf("This variable cannot be modified.");
   return true;
  }

  if(cvar->value)
    free(cvar->value);

  cvar->value = strdup(arguments);
  return true;
}

COMMAND(CMD_aliaslist)
{
  cvar_t *c;
  int i;

  for(i = 0, c = gc_alias_list; c; c = c->next, i++)
    CON_printf("\'%s\' == \'%s\'", c->key, c->value);
  CON_printf("%d aliases", i);
}

