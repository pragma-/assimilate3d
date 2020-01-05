typedef struct texture_data
{
  char *filename;
  int width, height;
  GLuint id;
} texture_t;

// fixme: use linked list instead?
#define T_MAX_TEXTURES  200

void T_initTextures(void);
void T_destroyTextures(void);
#if 0
GLuint T_loadTexturePCX(char *filename, char wrap);
#endif
GLuint T_loadTextureRAW(char *filename, char wrap);
GLuint T_loadTextureTGA(char *filename, char wrap);
GLuint T_loadTextureSDL(char *filename, char wrap);
void T_changeGamma(char *image, int size, float factor);
