/*==========================================================
  File:  gui_elements.c
  Author:  _pragma

  Description:  Under the hood of the graphical user interface.
  ==========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "texture.h"
#include "console.h"
#include "cvar.h"
#include "util.h"
#include "gui_elements.h"

gui_element_t *gui_create_element(int type, char *text, 
                                  float x1, float y1, 
                                  float x2, float y2,
                                  char *texture)
{
  gui_element_t *element;

  if(!(element = malloc(sizeof(gui_element_t))))
  {
    CON_printf("warning: failed to create gui element");
    return false;
  }

  element->type = type;
  if(text)
    element->text = strdup(text);
  else
    element->text = 0;
  element->x1 = x1;
  element->y1 = y1;
  element->x2 = x2;
  element->y2 = y2;
  element->next = 0;

  if(!(element->textureId = T_loadTextureSDL(texture, true)))
    CON_printf("Warning failed to load '%s'", texture);

  return element;
}

void gui_add_element_to_list(gui_element_t *element, gui_element_t **list)
{
  element->next = *list;
  *list = element;
}

void gui_remove_element_from_list(gui_element_t *element, gui_element_t **list)
{
  gui_element_t *temp;

  if(element == *list)
    *list = element->next;
  else
  {
    temp = *list;

    while (temp && (temp->next != (element)))
      temp = temp->next;

    if (temp)
      temp->next = element->next;
  }
}

void gui_destroy_element(gui_element_t *element)
{
  if(element)
  {
    if(element->text)
      free(element->text);
    free(element);
    element = 0;
  }
  else
    CON_printf("warning: destroying null gui element");
}

void gui_destroy_all_elements(gui_element_t *element_list)
{
  gui_element_t *element, *next;

  for(element = element_list; element; element = next)
  {
    next = element->next;
    gui_destroy_element(element);
  }
}

void gui_draw_elements(gui_element_t *element_list)
{
  gui_element_t *element;
  extern cvar_t *cvar_scr_width, *cvar_scr_height;
  int width = atoi(cvar_scr_width->value), 
      height = atoi(cvar_scr_height->value);

  glLoadIdentity();

  glDisable(GL_TEXTURE_2D);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor4f(0.0, 0.0, 0.0, 0.5);
  glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(width, 0);
    glVertex2i(width, height);
    glVertex2i(0, height);
  glEnd();

  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glColor4f(1.0, 1.0, 1.0, 1.0);

  for(element = element_list; element; element = element->next)
  {
    glBindTexture(GL_TEXTURE_2D, element->textureId);
    glBegin(GL_QUADS);
      glTexCoord2f(0, 0);  glVertex2f(element->x1, element->y1);
      glTexCoord2f(0, 1);  glVertex2f(element->x1, element->y2);
      glTexCoord2f(1, 1);  glVertex2f(element->x2, element->y2);
      glTexCoord2f(1, 0);  glVertex2f(element->x2, element->y1);
    glEnd();
  }
}

void gui_processMouseClick(int x, int y, int button, gui_element_t *element_list)
{
  gui_element_t *element;

  if((button & SDL_BUTTON(1)) == 0)
    return;

  for(element = element_list; element; element = element->next)
    if(x > element->x1 && x < element->x2 &&
       y > element->y1 && y < element->y2)
    {
      element->on_click(x, y, button);
      return;
    }
}



