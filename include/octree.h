typedef struct octree_struct
{
  char isSubdivided;

  float width;

  int numTriangles;
  int *triangleIndices;
  
  float center[3];

  struct octree_struct *nodes[8];
} octree_t;

#define OCT_TLF  0 // top left front
#define OCT_TLB  1 // top left back
#define OCT_TRF  2 // top right front
#define OCT_TRB  3 // top right back
#define OCT_BLF  4 // bottom left front
#define OCT_BLB  5 // bottom left back
#define OCT_BRF  6 // bottom right front
#define OCT_BRB  7 // bottom right back

