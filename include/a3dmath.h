typedef float vec3_t[3];
typedef float vec2_t[2];

#define DEG2RAD(a) ((a) * (M_PI/180.0))
#define RAD2DEG(a) ((a) * (180.0/M_PI))

typedef struct m_plane_struct
{
  vec3_t normal;
  float  distance;  
} plane_t;

void M_vnormalize(float *n);
void M_vcross(float *r, float *y, float *z);
float M_vmagnitude(float *v);
void M_vsubtract(float *r, float *y, float *z);
float M_vdot(float *u, float *v);
