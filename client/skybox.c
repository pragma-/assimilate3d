/*==========================================================
  File:  skybox.c
  Author:  _pragma

  Description:  Initializes and renders skybox.
  ==========================================================*/

#include "SDL.h"
#include "SDL_opengl.h"

#include "interp.h"
#include "commands.h"
#include "skybox.h"
#include "texture.h"
#include "console.h"
#include "util.h"

GLuint g_sbTexture[5] = {0}, g_sbList = 0 ;

void SB_loadSkyBox(char *name)
{
  char path[1024], file[1024];
  int failed = 0;

  sprintf(path, "data/skybox/%s/%s%%c.tga", name, name);

  sprintf(file, path, 'N');
  if(!(g_sbTexture[0] = T_loadTextureSDL(file, false)))
    failed = 1;
  sprintf(file, path, 'E');
  if(!(g_sbTexture[1] = T_loadTextureSDL(file, false)))
    failed = 1;
  sprintf(file, path, 'S');
  if(!(g_sbTexture[2] = T_loadTextureSDL(file, false)))
    failed = 1;
  sprintf(file, path, 'W');
  if(!(g_sbTexture[3] = T_loadTextureSDL(file, false)))
    failed = 1;
  sprintf(file, path, 'U');
  if(!(g_sbTexture[4] = T_loadTextureSDL(file, false)))
    failed = 1;

  sprintf(file, "data/skybox/%s/%s.cfg", name, name);
  config_read(file);

  if(failed)
    CON_printf("^9Failed to load skybox texture");
  else if(!SB_generateSkyBox(1024, 1024, 1024))
    CON_printf("^9Failed to generate skybox list");
  else
    CON_printf("^2Loaded skybox '%s'", name);
}

int SB_generateSkyBox(float width, float height, float length)
{
  float px, py, pz;

  if(g_sbList)
  {
    CON_printf("^3Warning: deleting existing skybox list");
    glDeleteLists(g_sbList, 1);
  }

  px = -width  / 2;
  py = -height / 2;
  pz = -length / 2;

  if(!(g_sbList = glGenLists(1)))
  {
    CON_printf("^9SkyBox:  Failed to generate skybox list");
    return 0;
  }

  glNewList(g_sbList, GL_COMPILE);
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, g_sbTexture[4]);  // Sky
  glBegin(GL_QUADS);
    glTexCoord2f(1, 0);  glVertex3f(px + width, py + height, pz + length);
    glTexCoord2f(0, 0);  glVertex3f(px,         py + height, pz + length);
    glTexCoord2f(0, 1);  glVertex3f(px,         py + height, pz);
    glTexCoord2f(1, 1);  glVertex3f(px + width, py + height, pz);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, g_sbTexture[0]);  // North
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0);  glVertex3f(px,         py + height, pz + length);
    glTexCoord2f(1, 0);  glVertex3f(px + width, py + height, pz + length);
    glTexCoord2f(1, 1);  glVertex3f(px + width, py,          pz + length);
    glTexCoord2f(0, 1);  glVertex3f(px        , py,          pz + length);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, g_sbTexture[1]);  // East
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0);  glVertex3f(px + width, py + height, pz + length);
    glTexCoord2f(1, 0);  glVertex3f(px + width, py + height, pz);
    glTexCoord2f(1, 1);  glVertex3f(px + width, py,          pz);
    glTexCoord2f(0, 1);  glVertex3f(px + width, py,          pz + length);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, g_sbTexture[2]);  // South
  glBegin(GL_QUADS);
    glTexCoord2f(1, 1);  glVertex3f(px        , py,          pz);
    glTexCoord2f(0, 1);  glVertex3f(px + width, py,          pz);
    glTexCoord2f(0, 0);  glVertex3f(px + width, py + height, pz);
    glTexCoord2f(1, 0);  glVertex3f(px,         py + height,         pz);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, g_sbTexture[3]);  // West
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0);  glVertex3f(px, py + height, pz);
    glTexCoord2f(1, 0);  glVertex3f(px, py + height, pz + length);
    glTexCoord2f(1, 1);  glVertex3f(px, py,          pz + length);
    glTexCoord2f(0, 1);  glVertex3f(px, py,          pz);
  glEnd();

  glEndList();

  return 1;
}

COMMAND(CMD_skybox)
{
  SB_loadSkyBox(arguments);
}
