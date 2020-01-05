/*==========================================================
  File:  model.c
  Author:  _pragma

  Description:  Loads and maintains models.
  ==========================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "ms3d.h"
#include "model.h"
#include "texture.h"
#include "console.h"
#include "util.h"

m_model_t *gm_model_list = 0;  // list of loaded models
long gm_model_identifier = 0;  // unique model identification numbers

void M_destroyModels(void)
{
  m_model_t *model, *next;

  CON_printf("-------- Destroying Models ---------");

  for(model = gm_model_list; model; model = next)
  {
    next = model->next;
    M_destroyModel(model);
  }

  CON_printf("------------------------------------");
}

void M_destroyModel(m_model_t *model)
{
  int i;

  if(model)
  {
    if(model->name)
    {
      CON_printf("Destroying model %s", model->name);
      free(model->name);
    }
    if(model->vertices)
      free(model->vertices);
    if(model->triangles)
      free(model->triangles);
    if(model->groups)
    {
      for(i = 0; i < model->numGroups; i++)
        if(model->groups[i].triangleIndices)
          free(model->groups[i].triangleIndices);
      free(model->groups);
    }
    if(model->materials)
      free(model->materials);
    free(model);
    model = 0;
  }
}

m_model_t *M_findModelByName(char *name)
{
  m_model_t *model;

  for(model = gm_model_list; model; model = model->next)
    if(!strcmp(name, model->name))
      return model;

  return 0;
}

m_model_t *M_findModelById(int id)
{
  m_model_t *model;

  for(model = gm_model_list; model; model = model->next)
    if(model->id == id)
      return model;

  return 0;
}

m_model_t *M_loadModelMS3D(char *filename)
{
  ms3d_model_t *ms3d_model;
  m_model_t *model;

  // determine if the model has been loaded already
  if((model = M_findModelByName(filename)))
  {
    CON_printf("Model \'%s\' already loaded as %d", filename, model->id);
    return model;
  }

  if(chdir("data/models") == -1)
  {
    CON_printf("Model data directory does not exist, aborting.");
    return 0;
  }

  if(!(ms3d_model = MS3D_loadModel(filename)))
  {
    CON_printf("Failed to load MS3D model");
    chdir("../..");
    return 0;
  }

  if(!(model = M_convertMS3D(ms3d_model)))
  {
    CON_printf("Failed to conver model");
    MS3D_destroyModel(ms3d_model);
    chdir("../..");
    return 0;
  }
  chdir("../..");

  MS3D_destroyModel(ms3d_model);  

  if(!(model->name = strdup(filename)))
  {
    M_destroyModel(model);
    CON_printf("Out of memory for model name");
    return 0;
  }

  // assigned model an unique identifier 
  // and add to our list of loaded models

  model->id = ++gm_model_identifier;

  model->next = gm_model_list;
  gm_model_list = model;

  return model;
}

m_model_t *M_convertMS3D(ms3d_model_t *ms3d_model)
{
  m_model_t *model;
  int i,j,k;
  char filename[256];

  if(!(model = malloc(sizeof(m_model_t))))
  {
    CON_printf("M_convertMS3D: failed to alloc memory");
    perror("M_convertMS3D malloc");
    return 0;
  }

  memset(model, 0, sizeof(m_model_t));

  model->numVertices = ms3d_model->numVertices;

  if(!(model->vertices = malloc(sizeof(m_vertex_t)*model->numVertices)))
  {
    CON_printf("M_convertMS3D: failed to alloc memory");
    perror("M_convertMS3D malloc");
    free(model);
    return 0;
  }

  for(i = 0; i < model->numVertices; i++)
  {
    model->vertices[i].vertex[0] = ms3d_model->vertices[i].vertex[0];
    model->vertices[i].vertex[1] = ms3d_model->vertices[i].vertex[1];
    model->vertices[i].vertex[2] = ms3d_model->vertices[i].vertex[2];

    model->vertices[i].boneId = ms3d_model->vertices[i].boneId;
  }

  model->numTriangles = ms3d_model->numTriangles;

  if(!(model->triangles = malloc(sizeof(m_triangle_t)*model->numTriangles)))
  {
    CON_printf("M_convertMS3D: failed to alloc memory");
    perror("M_convertMS3D malloc");
    M_destroyModel(model);
    return 0;
  }
  
  for(i = 0; i < model->numTriangles; i++)
  {
    model->triangles[i].vertexIndices[0] = ms3d_model->triangles[i].vertexIndices[0];
    model->triangles[i].vertexIndices[1] = ms3d_model->triangles[i].vertexIndices[1];
    model->triangles[i].vertexIndices[2] = ms3d_model->triangles[i].vertexIndices[2];

    for(j = 0; j < 3; j++)
      for(k = 0; k < 3; k++)
        model->triangles[i].vertexNormals[j][k] = ms3d_model->triangles[i].vertexNormals[j][k];

    for(j = 0; j < 3; j++)
    {
      model->triangles[i].s[j] = ms3d_model->triangles[i].s[j];
      model->triangles[i].t[j] = ms3d_model->triangles[i].t[j];
    }
  }

  model->numGroups = ms3d_model->numGroups;

  if(!(model->groups = malloc(sizeof(m_group_t)*model->numGroups)))
  {
    CON_printf("M_convertMS3D: failed to alloc memory");
    perror("M_convertMS3D malloc");
    M_destroyModel(model);
    return 0;
  }

  for(i = 0; i < model->numGroups; i++)
  {
    model->groups[i].numTriangles = ms3d_model->groups[i].numTriangles;

    if(!(model->groups[i].triangleIndices = malloc(sizeof(int) * model->groups[i].numTriangles)))
    {
      CON_printf("M_convertMS3D: failed to alloc memory");
      perror("M_convertMS3D malloc");
      M_destroyModel(model);
      return 0;
    }

    for(j = 0; j < model->groups[i].numTriangles; j++)
      model->groups[i].triangleIndices[j] = ms3d_model->groups[i].triangleIndices[j];
   
    model->groups[i].materialIndex = ms3d_model->groups[i].materialIndex;
  }

  model->numMaterials = ms3d_model->numMaterials;

  if(!(model->materials = malloc(sizeof(m_material_t)*model->numMaterials)))
  {
    CON_printf("M_convertMS3D: failed to alloc memory");
    perror("M_convertMS3D malloc");
    M_destroyModel(model);
    return 0;
  }
  
  for(i = 0; i < model->numMaterials; i++)
  {
    memcpy(model->materials[i].ambient, 
           ms3d_model->materials[i].ambient,  
           sizeof(float)*4);
    memcpy(model->materials[i].diffuse, 
           ms3d_model->materials[i].diffuse,  
           sizeof(float)*4);
    memcpy(model->materials[i].specular, 
           ms3d_model->materials[i].specular,  
           sizeof(float)*4);
    memcpy(model->materials[i].emissive, 
           ms3d_model->materials[i].emissive,  
           sizeof(float)*4);
    model->materials[i].shininess = ms3d_model->materials[i].shininess;

    if(strlen(ms3d_model->materials[i].texture) > 2 && 
       ms3d_model->materials[i].texture[0] == '.' && 
       ms3d_model->materials[i].texture[1] != '.')
      sprintf(filename, "../textures/%s", ms3d_model->materials[i].texture);
    else
      strcpy(filename, ms3d_model->materials[i].texture);

    model->materials[i].texture = T_loadTextureSDL(filename, true);
  }
  
  return model;
}

void M_renderModel(m_model_t *model)
{
  int i, j, k;

  // fixme:  consider this function for optimization

  for(i = 0; i < model->numGroups; i++)
  {
    if(model->groups[i].materialIndex >= 0)
    {
      glMaterialfv(GL_FRONT, GL_AMBIENT, 
                   model->materials[model->groups[i].materialIndex].ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, 
                   model->materials[model->groups[i].materialIndex].diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, 
                   model->materials[model->groups[i].materialIndex].specular);
      glMaterialfv(GL_FRONT, GL_EMISSION, 
                   model->materials[model->groups[i].materialIndex].emissive);
      glMaterialf(GL_FRONT, GL_SHININESS, 
                   model->materials[model->groups[i].materialIndex].shininess);

      if(model->materials[model->groups[i].materialIndex].texture > 0)
      {
        glBindTexture(GL_TEXTURE_2D, model->materials[model->groups[i].materialIndex].texture);
        glEnable(GL_TEXTURE_2D);
      }
      else
        glDisable(GL_TEXTURE_2D);
    }
    else
      glDisable(GL_TEXTURE_2D);

    glBegin(GL_TRIANGLES);
      for(j = 0; j < model->groups[i].numTriangles; j++)
        for(k = 0; k < 3; k++)
        {
          glNormal3fv(model->triangles[model->groups[i].triangleIndices[j]].vertexNormals[k]);
          glTexCoord2f(model->triangles[model->groups[i].triangleIndices[j]].s[k],
                       model->triangles[model->groups[i].triangleIndices[j]].t[k]);
          glVertex3fv(model->vertices[model->triangles[model->groups[i].triangleIndices[j]].vertexIndices[k]].vertex);
        }
    glEnd();
  }
  glEnable(GL_TEXTURE_2D);
}

