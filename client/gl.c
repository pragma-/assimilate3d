/*==========================================================
  File:  gl.c
  Author:  _pragma

  Description:  Initializes and queries OpenGL.
  ==========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "util.h"
#include "gl.h"
#include "lights.h"
#include "texture.h"
#include "console.h"

#ifndef HAS_MULTITEXTURE
  PFNGLACTIVETEXTUREARBPROC        glActiveTextureARB       = NULL;
  PFNGLCLIENTACTIVETEXTUREARBPROC  glClientActiveTextureARB = NULL;
#endif

void G_initGL(int width, int height)
{
  CON_printf("------------ Init GL ---------------");
  glShadeModel(GL_SMOOTH);

  glClearColor(0.0, 0.0, 1.0, 0.0);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClearDepth(1.0);

  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  L_initLights();  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glViewport (0, 0, (GLsizei) width, (GLsizei) height); 
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();

  glMatrixMode (GL_MODELVIEW);
  glEnable(GL_COLOR_MATERIAL);

  G_queryGL();

#ifndef HAS_MULTITEXTURE
  glActiveTextureARB = SDL_GL_GetProcAddress("glActiveTextureARB");
  glClientActiveTextureARB = SDL_GL_GetProcAddress("glClientActiveTextureARB");

  // Here we make sure that the functions were loaded properly
  // FIXME/TODO: When we actually use multitexture, we may wish to set a variable indicating such
  if(!glActiveTextureARB || !glClientActiveTextureARB)
    CON_printf("Warning: no multitexture support");
#endif 

  CON_printf("------------------------------------");
}

void G_queryGL(void)
{
  CON_printf("VENDOR:     %s", glGetString(GL_VENDOR));
  CON_printf("RENDERER:   %s", glGetString(GL_RENDERER));
  CON_printf("VERSION:    %s", glGetString(GL_VERSION));

  if(glGetString(GL_EXTENSIONS)) {
    char string[1024];
    strncpy(string, glGetString(GL_EXTENSIONS), 1023);
    string[1023] = 0;
    CON_printf("EXTENSIONS: %s", string);
  }
}
