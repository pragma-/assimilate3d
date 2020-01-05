/*==========================================================
  File:  commands.c
  Author:  _pragma

  Description:  Misc console command functions.  (Most will
                be in the file they are related to, see 
                commands.h)
  ==========================================================*/

#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "interp.h"
#include "commands.h"
#include "texture.h"
#include "console.h"
#include "ms3d.h"
#include "model.h"
#include "util.h"

extern command_t gi_commands[];

COMMAND(CMD_echo)
{
  CON_printf("%s", arguments);
}

COMMAND(CMD_help)
{
  CON_printf("This is a demonstration of my engine.  '^6commands^15' for");
  CON_printf("a list of commands. '^6bindlist^15' to see what the keys");
  CON_printf("are. '^6cvars^15' to list console variables. '^6aliaslist^15'");
  CON_printf("to list aliases.  ^3Have fun^15!");
}

COMMAND(CMD_qui)
{
  CON_printf("^9Er, did you want to exit? ^15Type the entire command.");
}

COMMAND(CMD_quit)
{
  exit(1); // fixme: implement a better gameloop
}

COMMAND(CMD_list)
{
  int i = 0;
  extern texture_t gt_textures[T_MAX_TEXTURES];
  extern int gt_textureIndex;
  extern m_model_t *gm_model_list;
  m_model_t *model;

  if(CMD_IS("commands"))
  {
    CON_printf("Commands:");

    for(i = 1; *gi_commands[i].command != '\n'; i++)
      CON_printf("  %s", gi_commands[i].command);
    i--;
  }
  else if(CMD_IS("textures"))
  {
    CON_printf("Textures:");

    for(i = 0; i < gt_textureIndex; i++)
      CON_printf("^14  %-25s ^3(^8;%3d^3x^8;%3d^3) ^15id^3: ^14%3d", gt_textures[i].filename, 
                                    gt_textures[i].width,
                                    gt_textures[i].height,
                                    gt_textures[i].id);
  }
  else if(CMD_IS("models"))
  {
    i = 0;
    CON_printf("Models:");
    for(model = gm_model_list; model; model = model->next, i++)
      CON_printf("^14  %-25s %d ^15vertices^3, ^14;%d ^15triangles", model->name,
                                                      model->numVertices,
                                                      model->numTriangles);
  }
  CON_printf("%d total", i);  
}

COMMAND(CMD_gl_info)
{
  char buf[1024];

  if(CMD_IS("gl_vendor"))
    CON_printf("gl_vendor^3: ^14;%s", glGetString(GL_VENDOR));
  else if(CMD_IS("gl_version"))
    CON_printf("gl_version^3: ^14;%s", glGetString(GL_VERSION));
  else if(CMD_IS("gl_extensions"))
  {
    strncpy(buf, glGetString(GL_EXTENSIONS), 1024);
    buf[1024] = 0;
    CON_printf("gl_extensions: %s", buf);
  }
  else if(CMD_IS("gl_renderer"))
    CON_printf("gl_renderer^3: ^14;%s", glGetString(GL_RENDERER));
}

COMMAND(CMD_load)
{
/*
  extern m_model_t *gm_model;

  if(gm_model)
    M_destroyModel(gm_model);
  gm_model = M_loadModelMS3D(arguments);
*/
}

COMMAND(CMD_gamemenu)
{
  extern int game_state, game_init;

  if(!game_init)
    exit(1);

  game_state = !game_state;

  if(game_state)  // 1 = game_playing
  {
    SDL_ShowCursor(0);
    SDL_WM_GrabInput(SDL_GRAB_ON);
  }
  else
  {
    SDL_ShowCursor(1);
    SDL_WM_GrabInput(SDL_GRAB_OFF);
  }
}
