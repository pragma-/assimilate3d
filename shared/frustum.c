/*==========================================================
  File:  frustum.c
  Author:  _pragma

  Description:  Viewing frustum routines.
  ==========================================================*/

#include <math.h>
#include <GL/gl.h>

#include "frustum.h"

float gf_frustum[6][4];

enum frustumSides
{
  RIGHT,
  LEFT,
  BOTTOM,
  TOP,
  BACK,
  FRONT
};

enum planeData
{
  A, B, C, D
};

void F_initFrustum(void)
{
  F_calculateFrustum(gf_frustum);
}

void F_normalizePlane(float frustum[6][4], int side)
{
  float magnitude = sqrtf(frustum[side][A] * frustum[side][A] +
                          frustum[side][B] * frustum[side][B] +
                          frustum[side][C] * frustum[side][C]);

  frustum[side][A] /= magnitude;
  frustum[side][B] /= magnitude;
  frustum[side][C] /= magnitude;
  frustum[side][D] /= magnitude;
}

void F_calculateFrustum(float frustum[6][4])
{
  float proj[16], modl[16], clip[16];

  glGetFloatv(GL_PROJECTION_MATRIX, proj);
  glGetFloatv(GL_MODELVIEW_MATRIX, modl);

   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + 
modl[ 3] * proj[12];
  clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + 
modl[ 3] * proj[13];
  clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + 
modl[ 3] * proj[14];
  clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + 
modl[ 3] * proj[15];

  clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + 
modl[ 7] * proj[12];
  clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + 
modl[ 7] * proj[13];
  clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + 
modl[ 7] * proj[14];
  clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + 
modl[ 7] * proj[15];

  clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + 
modl[11] * proj[12];
  clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + 
modl[11] * proj[13];
  clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + 
modl[11] * proj[14];
  clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + 
modl[11] * proj[15];

  clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + 
modl[15] * proj[12];
  clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + 
modl[15] * proj[13];
  clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + 
modl[15] * proj[14];
  clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + 
modl[15] * proj[15];

  frustum[RIGHT][A] = clip[ 3] - clip[ 0];
  frustum[RIGHT][B] = clip[ 7] - clip[ 4];
  frustum[RIGHT][C] = clip[11] - clip[ 8];
  frustum[RIGHT][D] = clip[15] - clip[12];

  F_normalizePlane(frustum, RIGHT);

  frustum[LEFT][A] = clip[ 3] + clip[ 0];
  frustum[LEFT][B] = clip[ 7] + clip[ 4];
  frustum[LEFT][C] = clip[11] + clip[ 8];
  frustum[LEFT][D] = clip[15] + clip[12];

  F_normalizePlane(frustum, LEFT);

  frustum[BOTTOM][A] = clip[ 3] + clip[ 1];
  frustum[BOTTOM][B] = clip[ 7] + clip[ 5];
  frustum[BOTTOM][C] = clip[11] + clip[ 9];
  frustum[BOTTOM][D] = clip[15] + clip[13];

  F_normalizePlane(frustum, BOTTOM);

  frustum[TOP][A] = clip[ 3] - clip[ 1];
  frustum[TOP][B] = clip[ 7] - clip[ 5];
  frustum[TOP][C] = clip[11] - clip[ 9];
  frustum[TOP][D] = clip[15] - clip[13];

  F_normalizePlane(frustum, TOP);

  frustum[BACK][A] = clip[ 3] - clip[ 2];
  frustum[BACK][B] = clip[ 7] - clip[ 6];
  frustum[BACK][C] = clip[11] - clip[10];
  frustum[BACK][D] = clip[15] - clip[14];

  F_normalizePlane(frustum, BACK);

  frustum[FRONT][A] = clip[ 3] + clip[ 2];
  frustum[FRONT][B] = clip[ 7] + clip[ 6];
  frustum[FRONT][C] = clip[11] + clip[10];
  frustum[FRONT][D] = clip[15] + clip[14];

  F_normalizePlane(frustum, FRONT);
}

#define V_IN        1
#define V_OUT       2
#define V_INTERSECT 3

int F_boxInFrustum(float frustum[6][4], float x, float y, float z, float x1, float y1, float z1)
{
  char mode = 0;
  int i;

  for(i = 0; i < 6; i++)
  {
    mode &= V_OUT;
    if(frustum[i][A] * x + frustum[i][B] * y + frustum[i][C] * z + frustum[i][D] >= 0)
      mode |= V_IN;

    if(mode == V_INTERSECT)
      continue;

    if(frustum[i][A] * x1 + frustum[i][B] * y + frustum[i][C] * z + frustum[i][D] >= 0)
      mode |= V_IN;
    else
      mode |= V_OUT;

    if(mode == V_INTERSECT)
      continue;

    if(frustum[i][A] * x + frustum[i][B] * y1 + frustum[i][C] * z + frustum[i][D] >= 0)
      mode |= V_IN;
    else
      mode |= V_OUT;

    if(mode == V_INTERSECT)
      continue;

    if(frustum[i][A] * x1 + frustum[i][B] * y1 + frustum[i][C] * z + frustum[i][D] >= 0)
      mode |= V_IN; 
    else 
      mode |= V_OUT;
                
    if (mode == V_INTERSECT) 
      continue;

    if(frustum[i][A] * x  + frustum[i][B] * y  + frustum[i][C] * z1 + frustum[i][D] >= 0)
      mode |= V_IN; 
    else 
      mode |= V_OUT;
                
    if (mode == V_INTERSECT) 
      continue;

    if(frustum[i][A] * x1 + frustum[i][B] * y  + frustum[i][C] * z1 + frustum[i][D] >= 0)  
      mode |= V_IN; 
    else 
      mode |= V_OUT;
                
    if (mode == V_INTERSECT) 
      continue;

    if(frustum[i][A] * x  + frustum[i][B] * y1 + frustum[i][C] * z1 + frustum[i][D] >= 0)
      mode |= V_IN; 
    else 
      mode |= V_OUT;
                
    if (mode == V_INTERSECT) 
      continue;

    if(frustum[i][A] * x1 + frustum[i][B] * y1 + frustum[i][C] * z1 + frustum[i][D] >= 0) 
      mode |= V_IN; 
    else 
      mode |= V_OUT;
                
    if (mode == V_INTERSECT || mode == V_IN) 
      continue;

    return FRUSTUM_OUT;
  }

  if(mode == V_INTERSECT)
    return FRUSTUM_INTERSECT;
  else
    return FRUSTUM_IN;
}
