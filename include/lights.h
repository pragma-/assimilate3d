#define MAX_LIGHTS 8 // Maximum number of lights renderable in a scene.
                     // OpenGL implementation-specific (usually 8).

typedef struct light_s
{
  int id;
  int type;          // LIGHT_DIRECTIONAL, LIGHT_POSITIONAL, LIGHT_SPOTLIGHT

  char enabled;      // true = light is on, false = light is off

  float ambient[4];
  float diffuse[4];
  float specular[4];
  float position[4];

  float scalar[2];   // scalar values associated with this light, see below
                     // for defines used to access the values

  struct light_s *next;
} light_t;

/*================================================
  Light types
  ================================================*/

#define LIGHT_DIRECTIONAL 0
#define LIGHT_POSITIONAL  1
#define LIGHT_SPOTLIGHT   3

/*================================================
  if type != LIGHT_SPOTLIGHT (for scalar[2])
  ================================================*/

// none?

/*================================================
  if type == LIGHT_SPOTLIGHT (for scalar[2])
  ================================================*/

#define LIGHT_SPOT_CUTOFF    0
#define LIGHT_SPOT_EXPONENT  1

void L_initLights(void);
light_t *L_newLight(void);
light_t *L_createLight(void);
void L_destroyLight(light_t *light);
void L_addLight(light_t *light, light_t **list);
void L_removeLight(light_t *light, light_t **list);
