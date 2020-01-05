/*==========================================================
  File:  input.c
  Author:  _pragma

  Description:  Handles pheripheral input (keyboard, mouse)
  ==========================================================*/
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "cvar.h"
#include "a3dmath.h"
#include "bsp.h"
#include "ms3d.h"
#include "model.h"
#include "entity.h"
#include "input.h"
#include "camera.h"
#include "util.h"
#include "texture.h"
#include "console.h"
#include "kbnames.h"
#include "interp.h"
#include "commands.h"
#include "game.h"
#include "gui_elements.h"

#define MAX_KEYSTATES  (SDLK_RSUPER+1)  // see kbnames.c

kb_state_t kb_state[MAX_KEYSTATES];

extern console_t gc_console;

void I_initKeyStates(void)
{
  memset(kb_state, 0, sizeof kb_state);

  /* hard coded bindings */
  
  I_bindKey("escape", "gamemenu");
  I_bindKey("~", "console");

  I_bindKey("w", "+forward");
  I_bindKey("s", "+back");
  I_bindKey("a", "+moveleft");
  I_bindKey("d", "+moveright");

  I_bindKey("space", "+moveup");
  I_bindKey("enter", "+movedown");
  I_bindKey("pageup", "+moveup");
  I_bindKey("pagedown", "+movedown");

  I_bindKey("f1", "help");
}

void I_destroyKeyStates(void)
{
  int i;

  for(i = 0; i < MAX_KEYSTATES; i++)
    if(kb_state[i].command)
      free(kb_state[i].command);
}

void I_keyPressed(SDL_keysym *keysym)
{
  if(keysym->sym >= MAX_KEYSTATES)
    return;
 
  kb_state[keysym->sym].state = KEY_PRESSED;

  if(CON_processKeystroke(keysym))
    return;

  if(kb_state[keysym->sym].command)
    CON_addCommand(kb_state[keysym->sym].command);
}

void I_keyReleased(SDL_keysym *keysym)
{
  char command[CON_BCS];

  if(keysym->sym >= MAX_KEYSTATES)
    return;

  kb_state[keysym->sym].state = KEY_RELEASED;

  if(kb_state[keysym->sym].command && *kb_state[keysym->sym].command == '+')
  {
    sprintf(command, "-%s", kb_state[keysym->sym].command + 1);
    CON_addCommand(command);    
  }
}

void I_mouseButtonClicked(SDL_MouseButtonEvent *button)
{
  extern int game_state;
  extern gui_element_t *gui_element_list;

  if(game_state == GAME_STATE_GUI)
    gui_processMouseClick(button->x, button->y, button->state, gui_element_list);
}

void I_mouseButtonReleased(SDL_MouseButtonEvent *button)
{
}

void I_mouseMotion(SDL_MouseMotionEvent *motion)
{
  extern int game_state;
  extern ent_t *ge_player;

  if(game_state == GAME_STATE_PLAYING)
    I_mouseLook(ge_player, motion->x, motion->y); 
}

void I_mouseLook(ent_t *entity, int mouseX, int mouseY)
{
  extern cvar_t *cvar_scr_width, *cvar_scr_height;
  int middleX = atoi(cvar_scr_width->value) >> 1, middleY = atoi(cvar_scr_height->value) >> 1;
  float angleY = 0.0, angleZ = 0.0, r[3], a[3];
  extern cvar_t *cvar_mpitchSpeed, *cvar_myawSpeed, *cvar_msensitivity,
                *cvar_thirdperson;
  int thirdperson = atoi(cvar_thirdperson->value) ? -1 : 1;

  SDL_GetMouseState(&mouseX, &mouseY);

  if( (mouseX == middleX) && (mouseY == middleY)) return;

  SDL_WarpMouse(middleX, middleY);

  angleY = (float)(middleX - mouseX) / 1000.0;
  angleZ = (float)(middleY - mouseY) / 1000.0 * thirdperson;

  angleY = atof(cvar_myawSpeed->value) * angleY * atof(cvar_msensitivity->value);
  angleZ = atof(cvar_mpitchSpeed->value) * angleZ * atof(cvar_msensitivity->value);

  M_vsubtract(r, entity->view, entity->position);
  M_vcross(a, r, entity->up);
  M_vnormalize(a);

  E_rotateEntity(entity, angleZ, a[0], a[1], a[2]);
  E_rotateEntity(entity, angleY, 0, 1, 0);
}

int I_findKey(char *key)
{
  int i;
  extern const kb_names_t keyNames[];
 
  for(i = 0; *keyNames[i].name != '\n'; i++)
    if(!stricmp(key, keyNames[i].name))
      break;

  return keyNames[i].id;
}

int I_findKeyName(char *key)
{
  int i;
  extern const kb_names_t keyNames[];
 
  for(i = 0; *keyNames[i].name != '\n'; i++)
    if(!stricmp(key, keyNames[i].name))
      break;

  return i;
}

char *I_findKeyNameByID(int id)
{
  int i;
  extern const kb_names_t keyNames[];
 
  for(i = 0; *keyNames[i].name != '\n'; i++)
    if(id == keyNames[i].id)
      return keyNames[i].name;

  return 0;
}

void I_bindKey(char *key, char *command)
{
  int i;
  extern const kb_names_t keyNames[];

  for(i = 0; keyNames[i].id != -1; i++)
    if(!stricmp(key, keyNames[i].name))
    {
      if(kb_state[keyNames[i].id].command)
        free(kb_state[keyNames[i].id].command);
      kb_state[keyNames[i].id].command = strdup(command);
      return;
    }
}

COMMAND(CMD_keynames)
{
  int i;
  extern const kb_names_t keyNames[];

  for(i = 1; keyNames[i].id != -1; i++)
    CON_printf("  %s", keyNames[i].name);
  CON_printf("%d keys registered", i);
}

COMMAND(CMD_bind)
{
  extern const kb_names_t keyNames[];
  char key[CON_BCS], command[CON_BCS];
  int id, name;

  I_halfChop(arguments, key, command);

  if(!*key)
  {
    CON_printf("Usage:  bind <key> <command>");
    CON_printf("        (bind escape gamemenu)");
    return;
  }

  if(!*command)
  {
    if((id = I_findKey(key)) == -1)
    {
      CON_printf("\'%s\' is not a valid key.  To list keys, type: keynames", key);
      return;
    }

    name = I_findKeyName(key);

    if(!kb_state[id].command)
    {
      CON_printf("^3'^14;%s^3'^15 is not bound.", key);
      return;
    }

    CON_printf("^3'^14;%s^3' ^15is bound to ^3'^14;%s^3'", keyNames[name].name, kb_state[id].command);
    return;
  }
 
  I_bindKey(key, command);
}

COMMAND(CMD_unbind)
{
  int id;

  if(!*arguments)
  {
    CON_printf("Usage:  unbind <key>");
    return;
  }

  if((id = I_findKey(arguments)) == -1)
  {
    CON_printf("^3'^14;%s^3' ^15is not a valid key.  To list keys, type: ^6keynames", arguments);
    return;
  }
  
  if(!kb_state[id].command)
    return;

  free(kb_state[id].command);
  kb_state[id].command = 0;
}

COMMAND(CMD_bindlist)
{
  int i, j;

  for(i = 0, j = 0; i < MAX_KEYSTATES; i++)
    if(kb_state[i].command)
    {
      CON_printf("^3'^14\%s^3' ^15== ^3'^14;%s^3'", I_findKeyNameByID(i), kb_state[i].command); 
      j++;
    }
  CON_printf("%d keys bound", j);
}
