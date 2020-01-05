#ifndef HAS_MULTITEXTURE
	extern PFNGLACTIVETEXTUREARBPROC            glActiveTextureARB;
	extern PFNGLCLIENTACTIVETEXTUREARBPROC      glClientActiveTextureARB;
#endif

void G_initGL(int width, int height);
void G_queryGL(void);
