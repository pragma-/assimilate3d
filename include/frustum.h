#define FRUSTUM_OUT       0
#define FRUSTUM_INTERSECT 1
#define FRUSTUM_IN        2

void F_initFrustum(void);
int F_boxInFrustum(float frustum[6][4], float x, float y, float z, float x1, float y1, float z1);
void F_normalizePlane(float frustum[6][4], int side);
void F_calculateFrustum(float frustum[6][4]);
