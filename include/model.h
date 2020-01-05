typedef struct m_vertex_struct
{
  float vertex[3];
  int boneId;
} m_vertex_t;

typedef struct m_triangle_struct
{
  int vertexIndices[3];
  float vertexNormals[3][3];
  float s[3];
  float t[3];
} m_triangle_t;

typedef struct m_group_struct
{
  int numTriangles;
  int *triangleIndices;
  int materialIndex;
} m_group_t;

typedef struct m_material_struct
{
  float ambient[4], diffuse[4], specular[4], emissive[4],
        shininess;
  GLuint texture;
} m_material_t;

typedef struct m_keyframe_rot_struct
{
  float time;
  float rotation[3];
} m_keyframe_rot_t;

typedef struct m_keyframe_pos_struct
{
  float time;
  float position[3];
} m_keyframe_pos_t;

typedef struct m_joint_struct
{
  float rotation[3];
  float position[3];
  int numKeyFramesRot;
  int numKeyFramesTrans;

  m_keyframe_rot_t *keyFramesRot;
  m_keyframe_pos_t *keyFramesTrans;
} m_joint_t;

typedef struct m_model_struct
{
  int id;
  float position[3];
  float orientation[3];
  float velocity[3];

  char *name;

  int numVertices;
  m_vertex_t *vertices;

  int numTriangles;
  m_triangle_t *triangles;

  int numGroups;
  m_group_t *groups;

  int numMaterials;
  m_material_t *materials;

  float animationFPS;
  float currentTime;
  int totalFrames;

  int numJoints;
  m_joint_t *joints;

  struct m_model_struct *next;

} m_model_t;

void M_destroyModels(void);
m_model_t *M_findModelByName(char *name);
m_model_t *M_findModelById(int id);
m_model_t *M_loadModelMS3D(char *filename);
m_model_t *M_convertMS3D(ms3d_model_t *model);
void M_destroyModel(m_model_t *model);
void M_renderModel(m_model_t *model);
