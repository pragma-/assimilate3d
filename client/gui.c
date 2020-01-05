/*==========================================================
  File:  gui.c
  Author:  _pragma

  Description:  Sets up the layout of the game graphical
                user interface and handles callbacks for
                elements.
  ==========================================================*/

#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "texture.h"
#include "console.h"
#include "cvar.h"
#include "game.h"
#include "gui_elements.h"
#include "gui.h"
#include "util.h"
#include "sdl_stuff.h"

gui_element_t *gui_element_list = 0;
int gui_last_state = 0, gui_state = 0;

extern int game_state;

#define ELEMENT_INIT(c, type, text, x1, y1, x2, y2, texture) \
  if(!((c) = gui_create_element( (type), (text), (x1), (y1), (x2), (y2), (texture)) )) \
    return false

void gui_init(void)
{
  gui_switch_state(GUI_STATE_MAINMENU);
}

int gui_init_startup(void)
{
  gui_element_t *element;
  extern cvar_t *cvar_scr_width, *cvar_scr_height;
  int width = cvar_scr_width ? atoi(cvar_scr_width->value) : SCREEN_WIDTH;
  int height = cvar_scr_height ? atoi(cvar_scr_height->value) : SCREEN_HEIGHT;

  ELEMENT_INIT(element, GUI_BUTTON, 0, 
               (width/2) - 65, 110, (width/2) + 65, 200, 
               "data/textures/gui/gui_newgame.tga");
  gui_add_element_to_list(element, &gui_element_list);
  element->on_click = gui_newgame_on_click;

  ELEMENT_INIT(element, GUI_BUTTON, 0, 
               (width/2) - 65, 210, (width/2) + 65, 300, 
               "data/textures/gui/gui_options.tga");
  gui_add_element_to_list(element, &gui_element_list);
  element->on_click = gui_options_on_click;

  ELEMENT_INIT(element, GUI_BUTTON, 0, 
               (width/2) - 65, 310, (width/2) + 65, 400, 
               "data/textures/gui/gui_quitgame.tga");
  gui_add_element_to_list(element, &gui_element_list);
  element->on_click = gui_exitgame_on_click;

  return true;
}

int gui_init_ingamemenu(void)
{
  gui_element_t *element;
  extern cvar_t *cvar_scr_width, *cvar_scr_height;
  int width = cvar_scr_width ? atoi(cvar_scr_width->value) : SCREEN_WIDTH;
  int height = cvar_scr_height ? atoi(cvar_scr_height->value) : SCREEN_HEIGHT;

  ELEMENT_INIT(element, GUI_BUTTON, 0, 
               (width/2) - 85, 110, (width/2) + 85, 200, 
               "data/textures/gui/gui_resumegame.tga");
  gui_add_element_to_list(element, &gui_element_list);
  element->on_click = gui_resumegame_on_click;

  ELEMENT_INIT(element, GUI_BUTTON, 0, 
               (width/2) - 75, 210, (width/2) + 75, 300, 
               "data/textures/gui/gui_newgame.tga");
  gui_add_element_to_list(element, &gui_element_list);
  element->on_click = gui_newgame_on_click;

  ELEMENT_INIT(element, GUI_BUTTON, 0, 
               (width/2) - 75, 310, (width/2) + 75, 400, 
               "data/textures/gui/gui_quitgame.tga");
  gui_add_element_to_list(element, &gui_element_list);
  element->on_click = gui_exitgame_on_click;

  return true;
}

int gui_init_options(void)
{
  gui_element_t *element;
  extern cvar_t *cvar_scr_width, *cvar_scr_height;
  int width = cvar_scr_width ? atoi(cvar_scr_width->value) : SCREEN_WIDTH;
  int height = cvar_scr_height ? atoi(cvar_scr_height->value) : SCREEN_HEIGHT;

  ELEMENT_INIT(element, GUI_BUTTON, 0, 
               0, height-50, 100, height, 
               "data/textures/gui/gui_back.tga");
  gui_add_element_to_list(element, &gui_element_list);
  element->on_click = gui_backbutton_on_click;

  ELEMENT_INIT(element, GUI_BUTTON, 0, 
                        width/2-65, 175, width/2+65, 245, 
                        "data/textures/gui/gui_config.tga");
  gui_add_element_to_list(element, &gui_element_list);
  element->on_click = gui_config_on_click;  

  return true;
}

int gui_init_config(void)
{
  gui_element_t *element;
  extern cvar_t *cvar_scr_width, *cvar_scr_height;
  int width = cvar_scr_width ? atoi(cvar_scr_width->value) : SCREEN_WIDTH;
  int height = cvar_scr_height ? atoi(cvar_scr_height->value) : SCREEN_HEIGHT;


  ELEMENT_INIT(element, GUI_BUTTON, 0, 
               0, height-50, 100, height, 
               "data/textures/gui/gui_back.tga");
  gui_add_element_to_list(element, &gui_element_list);
  element->on_click = gui_backbutton_on_click;

  return true;
}

void gui_backbutton_on_click(float x, float y, int button)
{
  gui_switch_state(gui_last_state);
}

void gui_options_on_click(float x, float y, int button)
{
  gui_switch_state(GUI_STATE_OPTIONS);
  gui_last_state = GUI_STATE_MAINMENU;
}

void gui_config_on_click(float x, float y, int button)
{
  gui_switch_state(GUI_STATE_CONFIG);
  gui_last_state = GUI_STATE_OPTIONS;
}

void gui_newgame_on_click(float x, float y, int button)
{
  extern int game_init;

  game_state = GAME_STATE_PLAYING;
  SDL_ShowCursor(0);
  SDL_WM_GrabInput(SDL_GRAB_ON);
  gui_switch_state(GUI_STATE_INGAME);
  game_init = true;
  CON_addCommand("newgame");
}

void gui_resumegame_on_click(float x, float y, int button)
{
  CON_addCommand("gamemenu");
}

void gui_exitgame_on_click(float x, float y, int button)
{
  // todo: add 'are you sure?  y/n' box

  gui_destroy_all_elements(gui_element_list);
  gui_element_list = 0;
  exit(1);
}

void gui_switch_state(int state)
{
  gui_state = state;

  gui_destroy_all_elements(gui_element_list);
  gui_element_list = 0;

  switch(state)
  {
    case GUI_STATE_MAINMENU:
      gui_init_startup();
      break;
    case GUI_STATE_OPTIONS:
      gui_init_options();
      break;
    case GUI_STATE_CONFIG:
      gui_init_config();
      break;
    case GUI_STATE_INGAME:
      gui_init_ingamemenu();
      break;
    default:
      CON_printf("Warning: unknown gui_state %d", state);
  }
}
