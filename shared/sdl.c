/*==========================================================
  File:  sdl.c
  Author:  _pragma

  Description:  Initializes SDL and controls main loop.
  ==========================================================*/

#include <stdlib.h>
#include <stdio.h>

#ifdef POSIX
  #include <unistd.h>
  #include <fcntl.h>
#endif

#include "SDL.h"
#include "SDL_opengl.h"

#include "gl.h"
#include "sdl_stuff.h"
#include "render.h"
#include "bsp.h"
#include "ms3d.h"
#include "model.h"
#include "entity.h"
#include "input.h"
#include "camera.h"
#include "time.h"
#include "texture.h"
#include "console.h"
#include "cvar.h"
#include "util.h"
#include "gui_elements.h"
#include "network.h"

#define error(s) fprintf(stderr, (s), SDL_GetError())

cvar_t *cvar_scr_width, *cvar_scr_height;
extern cvar_t *cvar_r_maxfps, *cvar_framedebug;

/* todo: make vid_restart command to change screen resolution in-game */

void S_initSDL(int width, int height)
{
  const SDL_VideoInfo *info = 0;
  int bpp, flags = 0;
  char buf[80];

  CON_printf("------------ Init SDL --------------");  

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    error("SDL init failed: %s\n");
    exit(1);
  } 

  if(!(info = SDL_GetVideoInfo()))
  {
    error("Video query failed: %s\n");
    exit(1);
  }

  bpp = info->vfmt->BitsPerPixel;

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE    , 5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE  , 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE   , 5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE  , 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  flags |= SDL_OPENGL;
  if(SCREEN_FULLSCREEN)
    flags |= SDL_FULLSCREEN;

  if(SDL_SetVideoMode(width, height, bpp, flags) == 0)
  {
    error("SetVideoMode: %s\n");
    exit(1);
  }

  CON_printf("SetVideoMode:  %dx%dx%d", width, height, bpp);

  // SDL_WM_GrabInput(SDL_GRAB_ON);
  SDL_WM_SetCaption("myGame", 0);
  SDL_ShowCursor(1);
  SDL_WarpMouse(width>>1, height>>1);

  sprintf(buf, "%d", width);
  cvar_scr_width = cvar_new("scr_width", buf, CVAR_LOCKED|CVAR_PRIVATE);
  sprintf(buf, "%d", height);
  cvar_scr_height = cvar_new("scr_height", buf, CVAR_LOCKED|CVAR_PRIVATE);

  CON_printf("------------------------------------");
}

void S_mainLoop(void)
{
  SDL_Event event;
  int time, oldtime, newtime;
  extern int gt_fps;

  CON_printf("------- Entering Main Loop ---------");
  CON_printf("Try typing '^10help^15' or pressing ^10F1^15.");

  oldtime = T_getTime();

  while(1)
  {
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case SDL_KEYDOWN:
          I_keyPressed(&event.key.keysym);
          break;
        case SDL_KEYUP:
          I_keyReleased(&event.key.keysym);
          break;
        case SDL_MOUSEMOTION:
          I_mouseMotion(&event.motion);
          break;
        case SDL_MOUSEBUTTONDOWN:
          I_mouseButtonClicked(&event.button);
          break;
        case SDL_MOUSEBUTTONUP:
          I_mouseButtonReleased(&event.button);
          break;
        case SDL_QUIT:
          exit(1);
          break;
      }
    }
    
    do
    {
      SDL_Delay(5);
      newtime = T_getTime();
      time = newtime - oldtime;
    } while (time < 1);

    gt_fps = 1.0 / ((float)time / 1000.0);

    S_runFrame(time);
    oldtime = newtime;
  }
}

void S_die(void)
{
  extern gui_element_t *gui_element_list;
  extern struct BSPLevel *gbsp_map;

  CON_printf("==------- Acckt!  Dying... -------==");

  destroyServer();
  BSP_destroyLevel(gbsp_map);
  E_destroyAllEntities();
  gui_destroy_all_elements(gui_element_list);
  I_destroyKeyStates();
  M_destroyModels();
  T_destroyTextures();
  cvar_destroyCvars();
  cvar_destroyAliases();
  CON_destroyConsole();
  SDL_Quit();

  #ifdef POSIX
  fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) | ~FNDELAY);
  #endif

  fprintf(stderr, "Good-bye, cruel world.\n");

  fprintf(stderr, "==--------------------------------==\n");
}

void S_runFrame(int msec)
{
  static int total_frames = 0, frames_drawn = 0;
  int maxfps;
  extern int gt_curtime, gt_realtime;
  extern ent_t *ge_player;
  extern char net_errorString[];
  extern int net_server, net_client, maxdesc;

  maxfps = atoi(cvar_r_maxfps->value);
  
  total_frames++;
  gt_realtime += msec;

  if(atoi(cvar_framedebug->value))
    CON_printf("maxfps: %d, frame: %d, msec: %d, gt_curtime: %d, gt_realtime: %d", 
                maxfps, total_frames, msec, gt_curtime, gt_realtime); 

  // fixme: following line works ... but it doesn't
  // if(msec < (1000/maxfps)) return;  // framerate too high

  E_updatePlayerEntity(ge_player, msec);
  CON_updateConsole(msec);
  CON_executeCommands();

  if(!NET_checkConnections())
    // CON_printf(net_errorString);
    ;
  else
  {
    if(net_server >= 0)
      checkServerConnections();

    if(net_client >= 0)
      checkClientConnections();
  }

  R_renderScene(msec);

  frames_drawn++;
}
