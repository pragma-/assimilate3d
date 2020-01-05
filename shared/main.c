/*==========================================================
  File:  main.c
  Author:  _pragma

  Description:  Main entry point.
  ==========================================================*/

#include <stdio.h>
#include <stdlib.h>

#ifdef POSIX
  #include <unistd.h>
  #include <fcntl.h>
#endif

#include "SDL.h"
#include "SDL_opengl.h"

#include "sdl_stuff.h"
#include "gl.h"
#include "render.h"
#include "ms3d.h"
#include "model.h"
#include "bsp.h"
#include "entity.h"
#include "camera.h"
#include "texture.h"
#include "console.h"
#include "interp.h"
#include "font.h"
#include "cvar.h"
#include "input.h"
#include "config.h"
#include "util.h"
#include "game.h"
#include "gui.h"
#include "frustum.h"
#include "network.h"

int game_state = GAME_STATE_GUI;  // start up in gui mode
int game_init  = false;           // game engine not yet initialized

int main(int argc, char **argv)
{
  #ifdef POSIX
  // disable delay on stdout
  fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) | FNDELAY);
  #endif

  // when engine terminates, S_die() will be called to clean up
  atexit(S_die);

  // Initialize console variables.
  // Several console variables must be initialized before
  // further initialization of the engine may continue.

  if(!(cvar_init()))   
  {                    
    perror("cvar_init");
    return 1;
  }

  // Initialize console.
  // Console buffers must be allocated and prepared
  // before CON_printf() calls will succeed.
  // TODO,FIXME:  add console_init variable for CON_printf() failsafe.

  if(!(CON_initConsole()))
  {
    perror("init console");
    return 1;
  }

  // Initialize SDL.
  // Initialize SDL and OpenGL context specifications.
  // Sets video mode, window caption, and hides mouse cursor.

  S_initSDL(SCREEN_WIDTH, SCREEN_HEIGHT);

  // Initialize OpenGL.
  // Initialize certain default OpenGL states for the engine.
  // Obtain addresses of ARB extensions and verify support.
  // Display information about the renderer/driver.

  G_initGL(SCREEN_WIDTH, SCREEN_HEIGHT);

  // Initialize viewing frustum.
  // Now that G_initGL() initialized the projection matrix,
  // we can calculate the viewing frustum.

  F_initFrustum();

  // Initialize console commands.
  // Initialize the console commands.

  I_initCommands();

  // Initialize console aliases.
  // Initialize the alias structures and create some 
  // default aliases.

  if(!(alias_init()))
  {
    perror("alias_init");
    return 1;
  }

  // Initialize keyboard bindings.
  // Set up some default key bindings.

  I_initKeyStates();

  // Initialize textures.
  // Load some system textures (console texture, fonts).

  T_initTextures();

  // Initialize player entity.
  // Initialize the player entity and position it in the world.
  // FIXME,TODO: move this to bsp loader and read entity positioning
  // information from the entity definitions in the bsp map as well.

  if(!E_initPlayerEntity())
  {
    perror("E_initPlayerEntity");
    return 1;
  }

  // Initialize the camera.
  // Now that the player entity has been created and positioned,
  // we can position the main camera at the entity's "head".

  C_initCamera();  

  // Initialize graphical user interface.
  // Initialize the gui system.

  gui_init();

  // Configuration.
  // Read and execute console commands as listed in 'mygame.cfg' in
  // the current working directory.

  config_init();

  // Initialize network system
 
  NET_init();

  // Main loop.
  // Do you believe in miracles?

  S_mainLoop();

  /* Never reached. */
  return 0;
}
