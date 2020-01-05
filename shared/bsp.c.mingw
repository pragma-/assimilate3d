/*==========================================================
  File:  bsp.c
  Author:  _pragma

  Description:  Loads Quake3 BSP maps and handles collision
                detection.

  Note:  Most of this is largely converted from the C++ code
         of the gametutorials.com Quake3 BSP tutorial.
  ==========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "cvar.h"
#include "gl.h"
#include "interp.h"
#include "commands.h"
#include "texture.h"
#include "console.h"
#include "bsp.h"
#include "util.h"
#include "frustum.h"
#include "ms3d.h"
#include "model.h"
#include "entity.h"
#include "server.h"

BSPLevel_t *gbsp_map = 0;
BSPOverdrawBitset_t gbsp_overdraw;

float gbsp_cameraPosition[3];
int gbsp_cameraCluster;

BSPmoveData_t gbsp_moveData;
float gbsp_rayOffset = 0;

extern float gf_frustum[6][4];

BSPLevel_t *BSP_loadLevel(char *filename)
{
  FILE *fp;
  int i, t;
  float f;
  struct BSPHeader header;
  struct BSPLump   lumps[kMaxLumps];
  struct BSPLightmap lightmap;
  BSPLevel_t *bsplevel;

  if(!(fp = fopen(filename, "rb")))
  {
    CON_printf("loadLevel: could not load \'%s\'", filename);
    return false;
  }

  if(!(bsplevel = malloc(sizeof(BSPLevel_t))))
  {
    CON_printf("loadLevel: out of memory");
    fclose(fp);
    return false;
  }

  memset(bsplevel, 0, sizeof(BSPLevel_t));

  CON_printf("-------- Loading BSP Level ---------");
  CON_printf("Filename: %s", filename);

  fread(&header, 1, sizeof(struct BSPHeader), fp);
  fread(&lumps, kMaxLumps, sizeof(struct BSPLump), fp);

  bsplevel->lengthOfEntities = lumps[kEntities].length;
  bsplevel->entities = malloc(lumps[kEntities].length);

  bsplevel->numOfVerts = lumps[kVertices].length / sizeof(struct BSPVertex);
  bsplevel->vertices   = malloc(sizeof(struct BSPVertex) * bsplevel->numOfVerts);

  bsplevel->numOfFaces = lumps[kFaces].length / sizeof(struct BSPFace);
  bsplevel->faces      = malloc(sizeof(struct BSPFace) * bsplevel->numOfFaces);

  bsplevel->numOfTextures = lumps[kTextures].length / sizeof(struct BSPTexture);
  bsplevel->textures      = malloc(sizeof(struct BSPTexture) * bsplevel->numOfTextures);

  bsplevel->numOfLightmaps = lumps[kLightmaps].length / sizeof(struct BSPLightmap);

  bsplevel->numOfMeshes = lumps[kMeshVerts].length / sizeof(int);
  bsplevel->meshes      = malloc(sizeof(int) * bsplevel->numOfMeshes);

  bsplevel->numOfNodes = lumps[kNodes].length / sizeof(struct BSPNode);
  bsplevel->nodes      = malloc(sizeof(struct BSPNode) * bsplevel->numOfNodes);

  bsplevel->numOfLeafs = lumps[kLeafs].length / sizeof(struct BSPLeaf);
  bsplevel->leafs      = malloc(sizeof(struct BSPLeaf) * bsplevel->numOfLeafs);

  bsplevel->numOfLeafFaces = lumps[kLeafFaces].length / sizeof(int);
  bsplevel->leafFaces      = malloc(sizeof(int) * bsplevel->numOfLeafFaces);

  bsplevel->numOfPlanes = lumps[kPlanes].length / sizeof(struct BSPPlane);
  bsplevel->planes      = malloc(sizeof(struct BSPPlane) * bsplevel->numOfPlanes);

  bsplevel->numOfLeafBrushes = lumps[kLeafBrushes].length / sizeof(int);
  bsplevel->leafBrushes      = malloc(sizeof(int) * bsplevel->numOfLeafBrushes);

  bsplevel->numOfBrushes = lumps[kBrushes].length / sizeof(struct BSPBrush);
  bsplevel->brushes      = malloc(sizeof(struct BSPBrush) * bsplevel->numOfBrushes);

  bsplevel->numOfBrushSides = lumps[kBrushSides].length / sizeof(struct BSPBrushSide);
  bsplevel->brushSides      = malloc(sizeof(struct BSPBrushSide) * bsplevel->numOfBrushSides);

  CON_printf("Entities length: %4d", bsplevel->lengthOfEntities);
  CON_printf("Vertices:        %4d", bsplevel->numOfVerts);
  CON_printf("Faces:           %4d", bsplevel->numOfFaces);
  CON_printf("Textures:        %4d", bsplevel->numOfTextures);
  CON_printf("Lightmaps:       %4d", bsplevel->numOfLightmaps);
  CON_printf("Meshes:          %4d", bsplevel->numOfMeshes);
  CON_printf("Nodes:           %4d", bsplevel->numOfNodes);
  CON_printf("Leafs:           %4d", bsplevel->numOfLeafs);
  CON_printf("LeafFaces:       %4d", bsplevel->numOfLeafFaces);
  CON_printf("Brushes:         %4d", bsplevel->numOfBrushes);
  CON_printf("LeafBrushes:     %4d", bsplevel->numOfLeafBrushes);
  CON_printf("BrushSides:      %4d", bsplevel->numOfBrushSides);
  CON_printf("Planes:          %4d", bsplevel->numOfPlanes);

  if(!bsplevel->vertices || !bsplevel->faces || !bsplevel->textures ||
     !bsplevel->meshes || !bsplevel->nodes || !bsplevel->leafs ||
     !bsplevel->leafFaces || !bsplevel->planes || !bsplevel->leafBrushes ||
     !bsplevel->brushes || !bsplevel->brushSides || !bsplevel->entities)
  {
    CON_printf("loadLevel: out of memory");
    fclose(fp);
    BSP_destroyLevel(bsplevel);
    return false;
  }

  fseek(fp, lumps[kEntities].offset, SEEK_SET);
  fread(bsplevel->entities, bsplevel->lengthOfEntities, sizeof(char), fp);

  bsplevel->entities[bsplevel->lengthOfEntities - 1] = 0;

  CON_printf("Entities: %s", bsplevel->entities);

  fseek(fp, lumps[kFaces].offset, SEEK_SET);
  fread(bsplevel->faces, bsplevel->numOfFaces, sizeof(struct BSPFace), fp);

  fseek(fp, lumps[kTextures].offset, SEEK_SET);
  fread(bsplevel->textures, bsplevel->numOfTextures, sizeof(struct BSPTexture), fp);

  fseek(fp, lumps[kMeshVerts].offset, SEEK_SET);
  fread(bsplevel->meshes, bsplevel->numOfMeshes, sizeof(int), fp);
  
  // Quake has Z-axis pointing up so we convert data
  // so that the Y-axis is pointing up.

  fseek(fp, lumps[kVertices].offset, SEEK_SET);

  for(i = 0; i < bsplevel->numOfVerts; i++)
  {
    fread(&bsplevel->vertices[i], 1, sizeof(struct BSPVertex), fp);

    f = bsplevel->vertices[i].position[1];
    bsplevel->vertices[i].position[1] = bsplevel->vertices[i].position[2];
    bsplevel->vertices[i].position[2] = -f;

    bsplevel->vertices[i].texCoord[1] *= -1;
  }

  CON_printf("Loading %d textures:", bsplevel->numOfTextures);

  chdir("data/");

  for(i = 0; i < bsplevel->numOfTextures; i++)
  {
    strcat(bsplevel->textures[i].strName, ".tga");
    bsplevel->textureID[i] = T_loadTextureSDL(bsplevel->textures[i].strName, true);
    CON_drawConsole(0);
  }

  chdir("..");

  fseek(fp, lumps[kLightmaps].offset, SEEK_SET);

  for(i = 0; i < bsplevel->numOfLightmaps; i++)
  {
    fread(&lightmap, 1, sizeof(struct BSPLightmap), fp);

    bsplevel->lightmaps[i] = CreateLightmapTexture((unsigned char *)lightmap.imageBits, 128, 128); 
    CON_printf("lightmap texture: %d", bsplevel->lightmaps[i]); 
  }

  fseek(fp, lumps[kNodes].offset, SEEK_SET);
  fread(bsplevel->nodes, bsplevel->numOfNodes, sizeof(struct BSPNode), fp);

  for(i = 0; i < bsplevel->numOfNodes; i++)
  {
    t = bsplevel->nodes[i].max[1];
    bsplevel->nodes[i].max[1] = bsplevel->nodes[i].max[2];
    bsplevel->nodes[i].max[2] = -t;

    t = bsplevel->nodes[i].min[1];
    bsplevel->nodes[i].min[1] = bsplevel->nodes[i].min[2];
    bsplevel->nodes[i].min[2] = -t;
  }

  fseek(fp, lumps[kLeafs].offset, SEEK_SET);
  fread(bsplevel->leafs, bsplevel->numOfLeafs, sizeof(struct BSPLeaf), fp);

  for(i = 0; i < bsplevel->numOfLeafs; i++)
  {
    t = bsplevel->leafs[i].min[1];
    bsplevel->leafs[i].min[1] = bsplevel->leafs[i].min[2];
    bsplevel->leafs[i].min[2] = -t;

    t = bsplevel->leafs[i].max[1];
    bsplevel->leafs[i].max[1] = bsplevel->leafs[i].max[2];
    bsplevel->leafs[i].max[2] = -t;
  }

  fseek(fp, lumps[kLeafFaces].offset, SEEK_SET);
  fread(bsplevel->leafFaces, bsplevel->numOfLeafFaces, sizeof(int), fp);

  fseek(fp, lumps[kPlanes].offset, SEEK_SET);
  fread(bsplevel->planes, bsplevel->numOfPlanes, sizeof(struct BSPPlane), fp);

  for(i = 0; i < bsplevel->numOfPlanes; i++)
  {
    f = bsplevel->planes[i].normal[1];
    bsplevel->planes[i].normal[1] = bsplevel->planes[i].normal[2];
    bsplevel->planes[i].normal[2] = -f;
  }

  if(lumps[kVisData].length)
  {
    fseek(fp, lumps[kVisData].offset, SEEK_SET);
    fread(&(bsplevel->clusters.numOfClusters), 1, sizeof(int), fp);
    fread(&(bsplevel->clusters.bytesPerCluster), 1, sizeof(int), fp);

    CON_printf("PVS: clusters: %d, bytes per cluster: %d", bsplevel->clusters.numOfClusters,
                                                           bsplevel->clusters.bytesPerCluster);

    bsplevel->clusters.bitsets = malloc(sizeof(char) * bsplevel->clusters.numOfClusters * bsplevel->clusters.bytesPerCluster);

    if(!bsplevel->clusters.bitsets)
      CON_printf("warning: no memory for clusters");
    else
      fread(bsplevel->clusters.bitsets, 1, sizeof(char) * bsplevel->clusters.numOfClusters * bsplevel->clusters.bytesPerCluster, fp);
  }
  else
  {
    CON_printf("No PVS data.");
    bsplevel->clusters.numOfClusters = 0;
    bsplevel->clusters.bytesPerCluster = 0;
    bsplevel->clusters.bitsets = 0;
  }

  fseek(fp, lumps[kLeafBrushes].offset, SEEK_SET);
  fread(bsplevel->leafBrushes, bsplevel->numOfLeafBrushes, sizeof(int), fp);

  fseek(fp, lumps[kBrushes].offset, SEEK_SET);
  fread(bsplevel->brushes, bsplevel->numOfBrushes, sizeof(struct BSPBrush), fp);

  fseek(fp, lumps[kBrushSides].offset, SEEK_SET);
  fread(bsplevel->brushSides, bsplevel->numOfBrushSides, sizeof(struct BSPBrushSide), fp);

  CON_printf("------------------------------------");

  fclose(fp);

  BSP_resizeOverdrawBitset(bsplevel->numOfFaces);

  return bsplevel;
}

void BSP_destroyLevel(BSPLevel_t *bsplevel)
{
  if(!bsplevel)
    return;

  if(bsplevel->entities)
    free(bsplevel->entities);
  if(bsplevel->vertices)
    free(bsplevel->vertices);
  if(bsplevel->faces)
    free(bsplevel->faces);
  if(bsplevel->textures)
    free(bsplevel->textures);
  if(bsplevel->meshes)
    free(bsplevel->meshes);
  if(bsplevel->nodes)
    free(bsplevel->nodes);
  if(bsplevel->leafs)
    free(bsplevel->leafs);
  if(bsplevel->leafFaces)
    free(bsplevel->leafFaces);
  if(bsplevel->planes)
    free(bsplevel->planes);
  if(bsplevel->leafBrushes)
    free(bsplevel->leafBrushes);
  if(bsplevel->brushes)
    free(bsplevel->brushes);
  if(bsplevel->brushSides)
    free(bsplevel->brushSides);
  if(bsplevel->clusters.bitsets)
    free(bsplevel->clusters.bitsets);
  free(bsplevel);
}

void BSP_drawFace(BSPLevel_t *bsplevel, int faceIndex)
{
  struct BSPFace *face = &(bsplevel->faces[faceIndex]); 
  float ambient[] = { 0.75, 0.75, 0.75, 1.0 };
  float diffuse[] = { 0.75, 0.75, 0.75, 1.0 };

  glActiveTextureARB(GL_TEXTURE0_ARB);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, bsplevel->textureID[face->textureID]);

  glActiveTextureARB(GL_TEXTURE1_ARB);

  if(face->lightmapID)
  {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bsplevel->lightmaps[face->lightmapID]);
  }
  else
    glDisable(GL_TEXTURE_2D);
 
  glVertexPointer(3, GL_FLOAT, sizeof(struct BSPVertex), 
                  &(bsplevel->vertices[face->startVertIndex].position));

  glClientActiveTextureARB(GL_TEXTURE0_ARB);
  glTexCoordPointer(2, GL_FLOAT, sizeof(struct BSPVertex), 
                    &(bsplevel->vertices[face->startVertIndex].texCoord));


  glClientActiveTextureARB(GL_TEXTURE1_ARB);
  glTexCoordPointer(2, GL_FLOAT, sizeof(struct BSPVertex), 
                    &(bsplevel->vertices[face->startVertIndex].lightmapTexCoord));

  glMaterialfv(GL_BACK, GL_AMBIENT, ambient);
  glMaterialfv(GL_BACK, GL_DIFFUSE, diffuse);

  glDrawElements(GL_TRIANGLES, face->numMeshVerts, GL_UNSIGNED_INT,
                 &bsplevel->meshes[face->meshVertIndex]);
}

void BSP_drawLevel(BSPLevel_t *bsplevel, float *position)
{
  int leafIndex;

  glEnableClientState(GL_VERTEX_ARRAY);

  glClientActiveTextureARB(GL_TEXTURE0_ARB);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnable(GL_TEXTURE_2D);

  glClientActiveTextureARB(GL_TEXTURE1_ARB);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnable(GL_TEXTURE_2D);

  gbsp_cameraPosition[0] = position[0];
  gbsp_cameraPosition[1] = position[1];
  gbsp_cameraPosition[2] = position[2];

  leafIndex = BSP_findLeaf(bsplevel, gbsp_cameraPosition);
  gbsp_cameraCluster = bsplevel->leafs[leafIndex].cluster;

  BSP_resetOverdrawBitset();
  BSP_drawTree(bsplevel, 0, false);

  glClientActiveTextureARB(GL_TEXTURE1_ARB);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);

  glClientActiveTextureARB(GL_TEXTURE0_ARB);
}

int BSP_findLeaf(BSPLevel_t *bsplevel, float *position)
{
  int i = 0;
  float distance = 0.0;
  struct BSPNode *node;
  struct BSPPlane *plane;

  while (i >= 0)
  {
    node = &(bsplevel->nodes[i]);
    plane = &(bsplevel->planes[node->plane]);

    distance = plane->normal[0] * position[0] +
               plane->normal[1] * position[1] +
               plane->normal[2] * position[2] - plane->d;

    if(distance >= 0)
      i = node->front;
    else
      i = node->back;
  }
  return ~i;
}

void BSP_drawTree(BSPLevel_t *bsplevel, int nodeId, int checkFrustum)
{
  struct BSPNode *node = &(bsplevel->nodes[nodeId]);
  struct BSPPlane *plane = &(bsplevel->planes[node->plane]);

  if(checkFrustum)
    switch(F_boxInFrustum(gf_frustum, node->min[0], node->min[1], node->min[2],
                          node->max[0], node->max[1], node->max[2]))
    {
      case FRUSTUM_OUT:  return;
      case FRUSTUM_IN:   checkFrustum = false;
    }

  if( (plane->normal[0] * gbsp_cameraPosition[0] +
       plane->normal[1] * gbsp_cameraPosition[1] +
       plane->normal[2] * gbsp_cameraPosition[2] - plane->d) > 0)
  {
    if(node->front >= 0)
      BSP_drawTree(bsplevel, node->front, checkFrustum);
    else
      BSP_drawLeaf(bsplevel, ~node->front, checkFrustum);

    if(node->back >= 0)
      BSP_drawTree(bsplevel, node->back, checkFrustum);
    else
      BSP_drawLeaf(bsplevel, ~node->back, checkFrustum);
  }
  else
  {
    if(node->back >= 0)
      BSP_drawTree(bsplevel, node->back, checkFrustum);
    else
      BSP_drawLeaf(bsplevel, ~node->back, checkFrustum);

    if(node->front >= 0)
      BSP_drawTree(bsplevel, node->front, checkFrustum);
    else
      BSP_drawLeaf(bsplevel, ~node->front, checkFrustum);
  }
}

void BSP_drawLeaf(BSPLevel_t *bsplevel, int leafIndex, int checkFrustum)
{
  struct BSPLeaf *leaf = &(bsplevel->leafs[leafIndex]);
  int numOfFaces = leaf->numOfLeafFaces;
  int i;

  if(!BSP_isClusterVisible(bsplevel, gbsp_cameraCluster, leaf->cluster))
    return;

  if(checkFrustum &&
     !F_boxInFrustum(gf_frustum, leaf->min[0], leaf->min[1], leaf->min[2],
                     leaf->max[0], leaf->max[1], leaf->max[2]))
    return;

  while(numOfFaces--)
  {
    i = bsplevel->leafFaces[leaf->startLeafFaceIndex + numOfFaces];

    if(bsplevel->faces[i].type == BSP_BEZIER_PATCH ||
       bsplevel->faces[i].type == BSP_BILLBOARD)
      continue;

    if(!BSP_OverdrawBitsetBitOn(i))
    {
      BSP_drawFace(bsplevel, i);
      BSP_OverdrawBitsetSetBit(i);
    }
  }
}

int BSP_isClusterVisible(BSPLevel_t *bsplevel, int current, int test)
{
  char visSet;

  if(!bsplevel->clusters.bitsets || current < 0)
    return true;

  visSet = bsplevel->clusters.bitsets[(current * bsplevel->clusters.bytesPerCluster) + (test / 8)];

  return (visSet & (1 << (test & 7)));
}

COMMAND(CMD_map)
{
  BSPLevel_t *bsplevel;
  extern cvar_t *cvar_serverPort;

  if(!*arguments)
  {
    CON_printf("Usage:  map <mapname>");
    return;
  }

  bsplevel = BSP_loadLevel(arguments);

  if(bsplevel)
  {
    BSP_destroyLevel(gbsp_map);
    gbsp_map = bsplevel;

    CON_printf("%s loaded", arguments);

    CON_printf("Initializing server");
    initServer(atoi(cvar_serverPort->value));
  }
  else
    CON_printf("Failed to load \'%s\'", arguments);
}

void BSP_resizeOverdrawBitset(int count)
{
  gbsp_overdraw.size = count / 32 + 1;

  if(gbsp_overdraw.bits)
    free(gbsp_overdraw.bits);

  gbsp_overdraw.bits = malloc(sizeof(unsigned int) * gbsp_overdraw.size);
  BSP_resetOverdrawBitset();
}

void BSP_destroyOverdrawBitset(void)
{
  if(gbsp_overdraw.bits)
    free(gbsp_overdraw.bits);
  gbsp_overdraw.bits = 0;
}

void BSP_resetOverdrawBitset(void)
{
  memset(gbsp_overdraw.bits, 0, sizeof(unsigned int) * gbsp_overdraw.size);
}

int BSP_OverdrawBitsetBitOn(unsigned int i)
{
  return gbsp_overdraw.bits[i >> 5] & (1 << (i & 31));
}

void BSP_OverdrawBitsetSetBit(unsigned int i)
{
  gbsp_overdraw.bits[i >> 5] |= (1 << (i & 31));
}

void BSP_rayTrace(BSPLevel_t *bsplevel, float *start, float *end)
{
  gbsp_moveData.startOut = true;
  gbsp_moveData.allSolid = false;
  gbsp_moveData.fraction = 1.0;

  gbsp_rayOffset = 0;

  BSP_rayCheckNode(bsplevel, 0, 0.0, 1.0, start, end);

  if(gbsp_moveData.fraction == 1.0)
  {
    gbsp_moveData.endPoint[0] = end[0];
    gbsp_moveData.endPoint[1] = end[1];
    gbsp_moveData.endPoint[2] = end[2];
  }
  else
  {
    gbsp_moveData.endPoint[0] = start[0] + (end[0] - start[0]) * gbsp_moveData.fraction;
    gbsp_moveData.endPoint[1] = start[1] + (end[1] - start[1]) * gbsp_moveData.fraction;
    gbsp_moveData.endPoint[2] = start[2] + (end[2] - start[2]) * gbsp_moveData.fraction;
  }
}

void BSP_rayCheckNode(BSPLevel_t *bsplevel, int nodeIndex, float startFraction, float endFraction, 
                      float *start, float *end)
{
  struct BSPLeaf *leaf;
  struct BSPBrush *brush;
  struct BSPNode *node;
  struct BSPPlane *plane;
  int i, sideA, sideB;
  float startDistance, endDistance, fractionA, fractionB, 
        middle[3], middleFraction, inverseDistance;

  if(gbsp_moveData.fraction <= startFraction)
    return;

  if(nodeIndex < 0)
  {
    leaf = &(bsplevel->leafs[~nodeIndex]);

    for(i = 0; i < leaf->numOfLeafBrushes; i++)
    {
      brush = &(bsplevel->brushes[bsplevel->leafBrushes[leaf->startLeafBrushIndex + i]]);
      
      if(brush->numOfBrushSides > 0 && (bsplevel->textures[brush->textureID].contents & 1))
        BSP_rayCheckBrush(bsplevel, brush, start, end);
    }
    return;
  }

  node  = &(bsplevel->nodes[nodeIndex]);
  plane = &(bsplevel->planes[node->plane]);

  startDistance = plane->normal[0] * start[0] +
                  plane->normal[1] * start[1] +   
                  plane->normal[2] * start[2] - plane->d;

  endDistance = plane->normal[0] * end[0] +
                plane->normal[1] * end[1] +
                plane->normal[2] * end[2] - plane->d;

  if(startDistance >= gbsp_rayOffset && endDistance >= gbsp_rayOffset)
    BSP_rayCheckNode(bsplevel, node->front, startFraction, endFraction, start, end);
  else if(startDistance < -gbsp_rayOffset && endDistance < -gbsp_rayOffset)
    BSP_rayCheckNode(bsplevel, node->back, startFraction, endFraction, start, end);
  else
  {
    if(startDistance < endDistance)
    {
      sideA = node->back;
      sideB = node->front;
      inverseDistance = 1.0 / (startDistance - endDistance);
      fractionA = (startDistance - EPSILON - gbsp_rayOffset) * inverseDistance;
      fractionB = (startDistance + EPSILON + gbsp_rayOffset) * inverseDistance;
    }
    else if(endDistance < startDistance)
    {
      sideA = node->front;
      sideB = node->back;
      inverseDistance = 1.0 / (startDistance - endDistance);
      fractionA = (startDistance + EPSILON + gbsp_rayOffset) * inverseDistance;
      fractionB = (startDistance - EPSILON - gbsp_rayOffset) * inverseDistance;
    }
    else
    {
      sideA = node->front;
      sideB = node->back;
      fractionA = 1.0;
      fractionB = 0.0;
    }

    if(fractionA < 0.0)
      fractionA = 0.0;
    else if(fractionA > 1.0)
      fractionA = 1.0;

    if(fractionB < 0.0)
      fractionB = 0.0;
    else if(fractionB > 1.0)
      fractionB = 1.0;

    middle[0] = start[0] + (end[0] - start[0]) * fractionA;
    middle[1] = start[1] + (end[1] - start[1]) * fractionA;
    middle[2] = start[2] + (end[2] - start[2]) * fractionA;
    middleFraction = startFraction + (endFraction - startFraction) * fractionA;
    BSP_rayCheckNode(bsplevel, sideA, startFraction, middleFraction, start, middle);

    middle[0] = start[0] + (end[0] - start[0]) * fractionB;
    middle[1] = start[1] + (end[1] - start[1]) * fractionB;
    middle[2] = start[2] + (end[2] - start[2]) * fractionB;
    middleFraction = startFraction + (endFraction - startFraction) * fractionB;
    BSP_rayCheckNode(bsplevel, sideB, middleFraction, endFraction, middle, end);
  }
}

void BSP_rayCheckBrush(BSPLevel_t *bsplevel, struct BSPBrush *brush, float *start, float *end)
{
  float startFraction = -1.0, endFraction = 1.0, startDistance, endDistance;
  float fraction;
  int startsOut = false;
  int endsOut = false;
  float collisionNormal[3], collisionD;
  int i;
  struct BSPBrushSide *brushSide;
  struct BSPPlane *plane;

  for(i = 0; i < brush->numOfBrushSides; i++)
  {
    brushSide = &(bsplevel->brushSides[brush->startBrushSideIndex + i]);
    plane = &(bsplevel->planes[brushSide->plane]);

    startDistance = plane->normal[0] * start[0] +
                    plane->normal[1] * start[1] +
                    plane->normal[2] * start[2] - plane->d - gbsp_rayOffset;

    endDistance = plane->normal[0] * end[0] +
                  plane->normal[1] * end[1] +
                  plane->normal[2] * end[2] - plane->d - gbsp_rayOffset;

    if(startDistance > 0)
      startsOut = true;

    if(endDistance > 0)
      endsOut = true;

    if(startDistance > 0 && endDistance > 0)
      return;

    if(startDistance <= 0 && endDistance <= 0)
      continue;

    if(startDistance > endDistance)
    {
      fraction = (startDistance - EPSILON) / (startDistance - endDistance);

      if(fraction > startFraction)
      {
        startFraction = fraction;
        collisionNormal[0] = plane->normal[0];
        collisionNormal[1] = plane->normal[1];
        collisionNormal[2] = plane->normal[2];
        collisionD = plane->d;
      }
    }
    else
    {
      fraction = (startDistance + EPSILON) / (startDistance - endDistance);
      if(fraction < endFraction)
        endFraction = fraction;
    }
  }
  
  if(startsOut == false)
  {
    gbsp_moveData.startOut = false;
    
    if(endsOut == false)
      gbsp_moveData.allSolid = true;

    return;
  }

  if(startFraction < endFraction &&
     startFraction > -1 && startFraction < gbsp_moveData.fraction)
  {
    if(startFraction < 0)
      startFraction = 0;

    gbsp_moveData.fraction = startFraction;
    gbsp_moveData.collisionPlaneNormal[0] = collisionNormal[0];
    gbsp_moveData.collisionPlaneNormal[1] = collisionNormal[1];
    gbsp_moveData.collisionPlaneNormal[2] = collisionNormal[2];
    gbsp_moveData.collisionPlaneD = collisionD;
    gbsp_moveData.collisionPlaneIndex = brushSide->plane;
  }
}

void BSP_rayTraceSphere(BSPLevel_t *bsplevel, float *start, float *end, float radius)
{
  gbsp_moveData.startOut = true;
  gbsp_moveData.allSolid = false;
  gbsp_moveData.fraction = 1.0f;

  gbsp_rayOffset = radius;

  BSP_rayCheckNode(bsplevel, 0, 0.0f, 1.0f, start, end);

  if(gbsp_moveData.fraction == 1.0f)
  {
    gbsp_moveData.endPoint[0] = end[0];
    gbsp_moveData.endPoint[1] = end[1];
    gbsp_moveData.endPoint[2] = end[2];
  }
  else
  {
    gbsp_moveData.endPoint[0] = start[0] + (end[0] - start[0]) * gbsp_moveData.fraction;
    gbsp_moveData.endPoint[1] = start[1] + (end[1] - start[1]) * gbsp_moveData.fraction;
    gbsp_moveData.endPoint[2] = start[2] + (end[2] - start[2]) * gbsp_moveData.fraction;
  }
}

GLuint CreateLightmapTexture(unsigned char *imageBits, int width, int height)
{
  int id;
  char buf[256];
  extern int gt_textureIndex;
  extern texture_t gt_textures[T_MAX_TEXTURES];

  T_changeGamma(imageBits, width*height*3, 10.0);

  glGenTextures(1, &id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageBits);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  sprintf(buf, "lightmap %d", id);
  gt_textures[gt_textureIndex].filename = strdup(buf);
  gt_textures[gt_textureIndex].width = width;
  gt_textures[gt_textureIndex].height = height;
  gt_textures[gt_textureIndex++].id = id;

  return id;
}
