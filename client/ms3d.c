/*==========================================================
  File:  ms3d.c
  Author:  _pragma

  Description:  Loads MilkShape3D models.
  ==========================================================*/

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "ms3d.h"
#include "texture.h"
#include "console.h"
#include "util.h"

ms3d_model_t *MS3D_loadModel(char *filename)
{
  FILE *fp;
  char *buffer, *p;
  int size, i, j;
  ms3d_model_t *model = 0;

  if(!(fp = fopen(filename, "rb")))
  {
    CON_printf("loadModel: failed to open '%s'", filename);
    perror("loadModel");
    return 0;
  }

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
 
  if(!(buffer = malloc(size)))
  {
    CON_printf("Failed to alloc memory for model buffer");
    perror("malloc buffer");
    fclose(fp);
    return 0;
  }

  fread(buffer, size, 1, fp);
  fclose(fp);

  p = buffer;

  if(!(model = malloc(sizeof(ms3d_model_t))))
  {
    CON_printf("Failed to allocate memory for model");
    perror("malloc model");
    free(buffer);
    return 0;
  }

  memcpy(&model->header, (ms3d_header_t *)p, sizeof(ms3d_header_t));
  p += sizeof(ms3d_header_t);

  if(strncmp(model->header.id, "MS3D000000", 10) != 0)
  {
    CON_printf("loadModel: '%s' is not a ms3d file", filename);
    free(buffer);
    return 0;
  }

  if(model->header.version < 3 || model->header.version > 4)
  {
     CON_printf("loadModel: '%s' is version %d (requires 3 or 4)",
                filename, model->header.version);
     free(buffer);
     return 0;
  }

  CON_printf("Loading '%s' - %d bytes, version %d", filename, size, model->header.version);

  /************** Vertices ***************/

  model->numVertices = *(unsigned short *)p;
  p += sizeof(unsigned short);
  
  CON_printf("vertices: %d", model->numVertices);  

  if(!(model->vertices = malloc(sizeof(ms3d_vertex_t) * model->numVertices)))
  {
    CON_printf("Failed to allocate memory for vertices");
    perror("malloc vertices");
    free(buffer);
    return 0;
  }

  for(i = 0; i < model->numVertices; i++)
  {
    memcpy(&model->vertices[i],
           (ms3d_vertex_t *)p, sizeof(ms3d_vertex_t));
    p += sizeof(ms3d_vertex_t);

/* fixme: perhaps we can have a console variable 'm_verbose'
          that will display all these CON_printf()s

    CON_printf("read vertex %d: %d, %3.2f, %3.2f, %3.2f, %d, %d",
                i, model->vertices[i].flags,
                model->vertices[i].vertex[0],
                model->vertices[i].vertex[1],
                model->vertices[i].vertex[2],
                model->vertices[i].boneId,
                model->vertices[i].referenceCount);
*/
  }

  /***************************************/
  
  /************* Triangles ***************/

  model->numTriangles = *(unsigned short *)p;
  p += sizeof(unsigned short);
  
  CON_printf("triangles: %d", model->numTriangles);  

  if(!(model->triangles = malloc(sizeof(ms3d_triangle_t) * model->numTriangles)))
  {
    CON_printf("Failed to allocate memory for triangles");
    perror("malloc triangles");
    free(buffer);
    return 0;
  }

  for(i = 0; i < model->numTriangles; i++)
  {
    memcpy(&model->triangles[i],
           (ms3d_triangle_t *)p, sizeof(ms3d_triangle_t));
    p += sizeof(ms3d_triangle_t);

/* fixme: perhaps we can have a console variable 'm_verbose'
          that will display all these CON_printf()s

    CON_printf("read triangle %d: %d, %d, %d, %d, %3.2f, %3.2f, %3.2f",
                i, model->triangles[i].flags,
                model->triangles[i].vertexIndices[0],
                model->triangles[i].vertexIndices[1],
                model->triangles[i].vertexIndices[2],
                model->triangles[i].vertexNormals[0],
                model->triangles[i].vertexNormals[1],
                model->triangles[i].vertexNormals[2]);
    CON_printf("                  %3.2f, %3.2f, %3.2f, %3.2f, %3.2f, %3.2f",
                model->triangles[i].s[0],
                model->triangles[i].s[1],
                model->triangles[i].s[2],
                model->triangles[i].t[0],
                model->triangles[i].t[1],
                model->triangles[i].t[2]);
    CON_printf("                  %d, %d",
                model->triangles[i].smoothingGroup,
                model->triangles[i].groupIndex);
*/
  }

  /***************************************/
  
  /*********** Groups (Meshes) ***********/

  model->numGroups = *(unsigned short *)p;
  p += sizeof(unsigned short);
  
  CON_printf("groups: %d", model->numGroups);  

  if(!(model->groups = malloc(sizeof(ms3d_group_t) * model->numGroups)))
  {
    CON_printf("Failed to allocate memory for groups");
    perror("malloc groups");
    free(buffer);
    return 0;
  }

  for(i = 0; i < model->numGroups; i++)
  {
    memcpy(&model->groups[i],
           (ms3d_group_t *)p, sizeof(ms3d_group_t));
    p += sizeof(ms3d_group_t);
    p -= sizeof(unsigned short *);  // triangleIndices
    p -= sizeof(unsigned char);     // materialIndex

/* fixme: perhaps we can have a console variable 'm_verbose'
          that will display all these CON_printf()s
 */
    CON_printf("read group %d: %d, %s, %d",
                i, model->groups[i].flags,
                model->groups[i].name,
                model->groups[i].numTriangles);
/**/

    model->groups[i].triangleIndices = malloc(sizeof(unsigned short) * model->groups[i].numTriangles);

    for(j = 0; j < model->groups[i].numTriangles; j++)
    {
      model->groups[i].triangleIndices[j] = *(unsigned short *)p;
      p += sizeof(unsigned short);

//      CON_printf("read index %d: %d", j, model->groups[i].triangleIndices[j]);
    }
    model->groups[i].materialIndex = *(unsigned char *)p;
    p += sizeof(unsigned char);
  }

  /***************************************/
  
  /************* Materials ***************/

  model->numMaterials = *(unsigned short *)p;
  p += sizeof(unsigned short);
  
  CON_printf("materials: %d", model->numMaterials);  

  if(!(model->materials = malloc(sizeof(ms3d_material_t) * model->numMaterials)))
  {
    CON_printf("Failed to allocate memory for materials");
    perror("malloc materials");
    free(buffer);
    return 0;
  }

  for(i = 0; i < model->numMaterials; i++)
  {
    memcpy(&model->materials[i],
           (ms3d_material_t *)p, sizeof(ms3d_material_t));
    p += sizeof(ms3d_material_t);

/* fixme: perhaps we can have a console variable 'm_verbose'
          that will display all these CON_printf()s
*/
    CON_printf("read material %d: %s - %s",
                i, model->materials[i].name, model->materials[i].texture);
  }

  /***************************************/
  
#if 0
  /******** Animation information ********/

  model->numVertices = *(unsigned short *)p;
  p += sizeof(unsigned short);
  
  CON_printf("vertices: %d", model->numVertices);  

  if(!(model->vertices = malloc(sizeof(ms3d_vertex_t) * model->numVertices)))
  {
    CON_printf("Failed to allocate memory for model");
    perror("malloc vertices");
    free(buffer);
    return 0;
  }

  for(i = 0; i < model->numVertices; i++)
  {
    memcpy(&model->vertices[i],
           (ms3d_vertex_t *)p, sizeof(ms3d_vertex_t));
    p += sizeof(ms3d_vertex_t);

/* fixme: perhaps we can have a console variable 'm_verbose'
          that will display all these CON_printf()s

    CON_printf("read vertex %d: %d, %3.2f, %3.2f, %3.2f, %d, %d",
                i, model->vertices[i].flags,
                model->vertices[i].vertex[0],
                model->vertices[i].vertex[1],
                model->vertices[i].vertex[2],
                model->vertices[i].boneId,
                model->vertices[i].referenceCount);
*/
  }

  /***************************************/
  
  /************ Bones/Joints *************/

  model->numVertices = *(unsigned short *)p;
  p += sizeof(unsigned short);
  
  CON_printf("vertices: %d", model->numVertices);  

  if(!(model->vertices = malloc(sizeof(ms3d_vertex_t) * model->numVertices)))
  {
    CON_printf("Failed to allocate memory for model");
    perror("malloc vertices");
    free(buffer);
    return 0;
  }

  for(i = 0; i < model->numVertices; i++)
  {
    memcpy(&model->vertices[i],
           (ms3d_vertex_t *)p, sizeof(ms3d_vertex_t));
    p += sizeof(ms3d_vertex_t);

/* fixme: perhaps we can have a console variable 'm_verbose'
          that will display all these CON_printf()s

    CON_printf("read vertex %d: %d, %3.2f, %3.2f, %3.2f, %d, %d",
                i, model->vertices[i].flags,
                model->vertices[i].vertex[0],
                model->vertices[i].vertex[1],
                model->vertices[i].vertex[2],
                model->vertices[i].boneId,
                model->vertices[i].referenceCount);
*/
  }

  /***************************************/
#endif  

  free(buffer);
  return model;
}

ms3d_model_t *MS3D_loadModelASCII(char *filename)
{
  return 0;
}

void MS3D_destroyModel(ms3d_model_t *model)
{
  int i;

  if(model)
  {
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
