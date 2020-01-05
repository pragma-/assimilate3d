/*==========================================================
  File:  lights.c
  Author:  _pragma

  Description:  Handles lighting and lighting effects.
  ==========================================================*/

#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interp.h"
#include "commands.h"
#include "util.h"
#include "lights.h"

light_t *gl_light_list;          // List of all our lights
light_t gl_lights[MAX_LIGHTS];   // OpenGL lights (usually 8)

/*==========================================================
  Function:  L_initLights(void)
  Parameters:  none
  Returns:  void

  Description:  Initializes OpenGL lights.
  ==========================================================*/
void L_initLights(void)
{
  float lightAmbient[] = { 0.45, 0.45, 0.45, 1.0 };
  float lightDiffuse[] = { 0.00, 0.00, 1.00, 1.0 };
  float lightPosition[] = { 0.0, 0.0, 0.0, LIGHT_POSITIONAL };

  glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

light_t *L_newLight(void)
{
  light_t *light;

  if(!(light = L_createLight()))
    return 0;

  L_addLight(light, &gl_light_list);
  return light;
}

light_t *L_createLight(void)
{
  light_t *light;

  if(!(light = malloc(sizeof(light_t))))
  {
    perror("L_createLight");
    return 0;
  }

  memset(light, 0, sizeof(light_t));

  return light;
}

void L_destroyLight(light_t *light)
{
  if(light)
  {
    L_removeLight(light, &gl_light_list);
    free(light);
  }
}

void L_addLight(light_t *light, light_t **list)
{
  if(light)
  {
    light->next = *list;
    *list = light;
  }
}

void L_removeLight(light_t *light, light_t **list)
{
  light_t *temp;

  if(light == *list)
    *list = light->next;
  else
  {
    temp = *list;

    while(temp && (temp->next != light))
      temp = temp->next;

    if(temp)
      temp->next = light->next;
  }
}

COMMAND(CMD_addLight)
{
}

COMMAND(CMD_positionLight)
{
}

COMMAND(CMD_setLightProperty)
{
  char buf[1024];
  char arg1[24], arg2[24], arg3[24], p[1024];
  float values[4];
  extern command_t gi_commands[];

  I_halfChop(arguments, arg1, p);
  I_halfChop(p, arg2, arg3);

  values[0] = atof(arg1);
  values[1] = atof(arg2);
  values[2] = atof(arg3);
  values[3] = 1.0;

  if(CMD_IS("lightambient"))
  {
    CON_printf("Setting ambient to (%.2f, %.2f, %.2f)", values[0],
                values[1], values[2]);
    glLightfv(GL_LIGHT0, GL_AMBIENT, values);
  }
}
