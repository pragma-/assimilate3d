/********** Milkshape3D *************/

typedef struct ms3d_header_struct
{
  char id[10];
  int version;
} __attribute__((packed)) ms3d_header_t;

typedef struct ms3d_vertex_struct
{
  unsigned char flags;
  float vertex[3];
  char boneId;
  unsigned char referenceCount;
} __attribute__((packed)) ms3d_vertex_t;

typedef struct ms3d_triangle_struct
{
  unsigned short flags;
  unsigned short vertexIndices[3];
  float vertexNormals[3][3];
  float s[3];
  float t[3];
  unsigned char smoothingGroup;
  unsigned char groupIndex;
} __attribute__((packed)) ms3d_triangle_t;

typedef struct ms3d_group_struct
{
  unsigned char flags;
  char name[32];
  unsigned short numTriangles;
  unsigned short *triangleIndices;
  char materialIndex;
} __attribute__((packed)) ms3d_group_t;

typedef struct ms3d_material_struct
{
  char name[32];
  float ambient[4], diffuse[4], specular[4], emissive[4],
        shininess, transparency;
  char mode;
  char texture[128];
  char alphamap[128];
} __attribute__((packed)) ms3d_material_t;

typedef struct ms3d_keyframe_rot_struct
{
  float time;
  float rotation[3];
} __attribute__((packed)) ms3d_keyframe_rot_t;

typedef struct ms3d_keyframe_pos_struct
{
  float time;
  float position[3];
} __attribute__((packed)) ms3d_keyframe_pos_t;

typedef struct ms3d_joint_struct
{
  unsigned char flags;
  char name[32];
  char parentName[32];
  float rotation[3];
  float position[3];
  unsigned short numKeyFramesRot;
  unsigned short numKeyFramesTrans;

  ms3d_keyframe_rot_t *keyFramesRot;
  ms3d_keyframe_pos_t *keyFramesTrans;
} __attribute__((packed)) ms3d_joint_t;

typedef struct ms3d_model_struct
{
  ms3d_header_t header;

  unsigned short numVertices;
  ms3d_vertex_t *vertices;

  unsigned short numTriangles;
  ms3d_triangle_t *triangles;

  unsigned short numGroups;
  ms3d_group_t *groups;

  unsigned short numMaterials;
  ms3d_material_t *materials;

  float animationFPS;
  float currentTime;
  int totalFrames;

  unsigned short numJoints;
  ms3d_joint_t *joints;

} ms3d_model_t;

ms3d_model_t *MS3D_loadModel(char *filename);
void MS3D_destroyModel(ms3d_model_t *model);
