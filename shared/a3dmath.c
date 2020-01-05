/*==========================================================
  File:  a3dmath.c
  Author:  _pragma

  Description:  3D math routines.
  ==========================================================*/

#include <math.h>

#include "a3dmath.h"
#include "util.h"

/** Vectors **/

float M_vdot(vec3_t u, vec3_t v)
{
  return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

void M_vcross(float *r, float *y, float *z)
{
  r[0] = (y[1] * z[2]) - (y[2] * z[1]);
  r[1] = (y[0] * z[2]) - (y[2] * z[0]);
  r[2] = (y[0] * z[1]) - (y[1] * z[0]);
}

float M_vmagnitude(float *v)
{
  return (float)sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
}

void M_vsubtract(float *r, float *y, float *z)
{
  r[0] = y[0] - z[0];
  r[1] = y[1] - z[1];
  r[2] = y[2] - z[2];
}

void M_vnormalize(float *n)
{
  float mag = M_vmagnitude(n);

  if(mag != 0.0)
  {
    n[0] /= mag;
    n[1] /= mag;
    n[2] /= mag;
  }
}

/** Planes **/

float M_distanceToPlane(plane_t plane, vec3_t point)
{
  return M_vdot(plane.normal, point) + plane.distance;
}

void M_raytracePlane(vec3_t *r, plane_t plane, vec3_t rpos, vec3_t rdir)
{
  float a = M_vdot(plane.normal, rdir), d;

  if(a == 0) // ray is parallel to plane
  {
    *r[0] = rpos[0];
    *r[1] = rpos[1];
    *r[2] = rpos[2];
  }

  M_vsubtract(*r, rpos, rdir);

  d = M_distanceToPlane(plane, rpos)/a;
 
  *r[0] *= d;
  *r[1] *= d;
  *r[2] *= d;  
} 
