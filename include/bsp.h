typedef struct BSPOverdrawBitset
{
  int size;
  int *bits;
} BSPOverdrawBitset_t;

typedef struct BSPmoveData_s
{
  float fraction;
  float endPoint[3];
  float collisionPlaneNormal[3];
  float collisionPlaneD;
  int collisionPlaneIndex;
  int startOut;
  int allSolid;
} BSPmoveData_t;

#define BSP_FACE_POLYGON  1
#define BSP_BEZIER_PATCH  2
#define BSP_MESH_POLYGON  3
#define BSP_BILLBOARD     4

#define EPSILON  0.03125f

struct BSPHeader
{
  char strID[4];
  int  version;
};

struct BSPLump
{
  int offset;
  int length;
};

struct BSPVertex
{
  float position[3];
  float texCoord[2];
  float lightmapTexCoord[2];
  float normal[3];
  char color[4];  // rgba for vertex
};

struct BSPFace
{
  int textureID;
  int effect;
  int type;
  int startVertIndex;
  int numOfVerts;
  int meshVertIndex;
  int numMeshVerts;
  int lightmapID;
  int lMapCorner[2];
  int lMapSize[2];
  float lMapPos[3];
  float lMapVecs[2][3];
  float normal[3];
  int size[2]; // Bezier patch dimensions
};

struct BSPTexture
{
  char strName[64];
  int flags;
  int contents;
};

struct BSPLightmap
{
  unsigned char imageBits[128][128][3];
};

struct BSPNode
{
  int plane;
  int front;
  int back;
  int min[3];
  int max[3];
};

struct BSPLeaf
{
  int cluster;
  int area;
  int min[3];
  int max[3];
  int startLeafFaceIndex;
  int numOfLeafFaces;
  int startLeafBrushIndex;
  int numOfLeafBrushes;
};

struct BSPPlane
{
  float normal[3];
  float d;
};

struct BSPBrush
{
  int startBrushSideIndex;
  int numOfBrushSides;
  int textureID;
};

struct BSPBrushSide
{
  int plane;
  int textureID;
};

struct BSPVisData
{
  int numOfClusters;
  int bytesPerCluster;
  char *bitsets;
};

enum eLumps
{
  kEntities,
  kTextures,                                  // Stores texture information
  kPlanes,                                // Stores the splitting planes
  kNodes,                                             // Stores the BSP nodes
  kLeafs,                                             // Stores the leafs of the nodes
  kLeafFaces,                                 // Stores the leaf's indices into the fac$
  kLeafBrushes,                               // Stores the leaf's indices into the bru
  kModels,                                    // Stores the info of world models
  kBrushes,                                   // Stores the brushes info (for collision)
  kBrushSides,                                // Stores the brush surfaces info
  kVertices,                                  // Stores the level vertices
  kMeshVerts,                                 // Stores the model vertices offsets
  kShaders,                                   // Stores the shader files (blending, ani
  kFaces,                                             // Stores the faces for the level
  kLightmaps,                                 // Stores the lightmaps for the level
  kLightVolumes,                              // Stores extra world lighting information
  kVisData,                                   // Stores PVS and cluster info (visibilit$
  kMaxLumps                                   // number of lumps (18, apparently)
};

typedef struct BSPLevel
{
  int lengthOfEntities;
  char *entities;

  int numOfVerts;
  struct BSPVertex *vertices;

  int numOfFaces;
  struct BSPFace *faces;

  int numOfTextures;
  struct BSPTexture *textures;
  GLuint textureID[200];

  int numOfMeshes;
  int *meshes;

  int numOfNodes;
  struct BSPNode *nodes;

  int numOfLeafs;
  struct BSPLeaf *leafs;

  int numOfLeafFaces;
  int *leafFaces;

  int numOfPlanes;
  struct BSPPlane *planes;

  int numOfLeafBrushes;
  int *leafBrushes;

  int numOfBrushes;
  struct BSPBrush *brushes;

  int numOfBrushSides;
  struct BSPBrushSide *brushSides;

  struct BSPVisData clusters;

  int numOfLightmaps;
  GLuint lightmaps[200];

} BSPLevel_t;

BSPLevel_t *BSP_loadLevel(char *filename);
void BSP_destroyLevel(BSPLevel_t *bsplevel);
void BSP_drawFace(BSPLevel_t *bsplevel, int faceIndex);
void BSP_drawLevel(BSPLevel_t *bsplevel, float *position);
int BSP_findLeaf(BSPLevel_t *bsplevel, float *position);
void BSP_drawTree(BSPLevel_t *bsplevel, int nodeId, int checkFrustum);
int BSP_isClusterVisible(BSPLevel_t *bsplevel, int current, int test);
void BSP_drawLeaf(BSPLevel_t *bsplevel, int leafIndex, int checkFrustum);
void BSP_resizeOverdrawBitset(int count);
void BSP_destroyOverdrawBitset(void);
void BSP_resetOverdrawBitset(void);
int BSP_OverdrawBitsetBitOn(unsigned int i);
void BSP_OverdrawBitsetSetBit(unsigned int i);
void BSP_rayTrace(BSPLevel_t *bsplevel, float *start, float *end);
void BSP_rayCheckNode(BSPLevel_t *bsplevel, int nodeIndex, float startFraction, float endFraction,
                      float *start, float *end);
void BSP_rayCheckBrush(BSPLevel_t *bsplevel, struct BSPBrush *brush, float *start, float *end);
void BSP_rayTraceSphere(BSPLevel_t *bsplevel, float *start, float *end, float radius);
GLuint CreateLightmapTexture(unsigned char *imageBits, int width, int height);
