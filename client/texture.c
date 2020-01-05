/*==========================================================
  File:  texture.c
  Author:  _pragma

  Description:  Loads and maintains texture objects.
  ==========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "SDL_opengl.h"
#include <SDL_image.h>

#include "tga.h"
#include "util.h"
#include "texture.h"
#include "console.h"

texture_t gt_textures[T_MAX_TEXTURES];  // fixme: perhaps a linked list instead
int gt_textureIndex = 0;

void T_initTextures(void)
{
  CON_printf("---------- Init Textures -----------");

  if(!T_loadTextureSDL("data/textures/system/font.tga", true))
    CON_printf("texture:  failed to load font.tga");

  // load console texture
  CON_loadTexture();

  CON_printf("------------------------------------");
}

void T_destroyTextures(void)
{
  int i;
  int a[T_MAX_TEXTURES] = {0};

  CON_printf("------- Destroying Textures --------");

  for(i = 0; i < gt_textureIndex; i++)
  {
    a[i] = gt_textures[i].id;

    if(gt_textures[i].filename)
    {
      CON_printf("Destroying %s", gt_textures[i].filename);
      free(gt_textures[i].filename);
    }
  }

#if 0
  for(;i < T_MAX_TEXTURES; i++)
    a[i] = 0;
#endif

  glDeleteTextures(gt_textureIndex, a);

  CON_printf("------------------------------------");
}

GLuint T_loadTextureSDL(char *filename, char wrap)
{
  GLuint texture;
  GLint bpp;
  GLenum texture_format;
  SDL_Surface *surface;
  int w, h;
  int i;
  char *p = filename;

  while(*p++)
    if(*p == '\\')
      *p = '/';

  for(i = 0; i < gt_textureIndex; i++)
    if(strcmp(gt_textures[i].filename, filename) == 0)
    {
      CON_printf("^10Preloaded texture ^3: ^14%s ^3(^14;%d^3)", filename, gt_textures[i].id);
      return gt_textures[i].id;
    }

  if(!(surface = IMG_Load(filename)))
  {
    CON_printf("^3Warning: ^15could not get image data for \'^14%s^15\'", filename);
    return 0;
  }

  w = surface->w;
  h = surface->h;

  bpp = surface->format->BytesPerPixel;
  if (bpp == 4)     // contains an alpha channel
  {
    if (surface->format->Rmask == 0x000000ff)
      texture_format = GL_RGBA;
    else
      texture_format = GL_BGRA;
  } 
  else if (bpp == 3)     // no alpha channel
  {
    if (surface->format->Rmask == 0x000000ff)
      texture_format = GL_RGB;
    else
      texture_format = GL_BGR;
  } 
  else 
  {
    CON_printf("^3Warning: ^15the image is not truecolor");
    return 0;
  }

  CON_printf("^10Loading texture^3: ^14%s ^3(^14;%d^3x^14;%d^3)", filename, w, h);

  glGenTextures( 1, &texture );
  //printf("gettex: %s\n", gluErrorString(glGetError()));

  gt_textures[gt_textureIndex].filename = strdup(filename);
  gt_textures[gt_textureIndex].width = w;
  gt_textures[gt_textureIndex].height = h;
  gt_textures[gt_textureIndex++].id = texture;
  

  glBindTexture( GL_TEXTURE_2D, texture );

#ifdef DEBUG_TEXTURES
  printf("bindtext: %s\n", gluErrorString(glGetError()));
#endif

  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

#ifdef DEBUG_TEXTURES
  printf("texenv: %s\n", gluErrorString(glGetError()));
#endif

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

#ifdef DEBUG_TEXTURES
  printf("texparam: %s\n", gluErrorString(glGetError()));
#endif

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#ifdef DEBUG_TEXTURES
  printf("texparam: %s\n", gluErrorString(glGetError()));
#endif

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                   wrap ? GL_REPEAT : GL_CLAMP );
#ifdef DEBUG_TEXTURES
  printf("texparam: %s\n", gluErrorString(glGetError()));
#endif

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                   wrap ? GL_REPEAT : GL_CLAMP );

#ifdef DEBUG_TEXTURES
  printf("texparam: %s\n", gluErrorString(glGetError()));
#endif

  glTexImage2D(GL_TEXTURE_2D, 0, bpp, w, h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);

#ifdef DEBUG_TEXTURES
  printf("textimage: %s\n", gluErrorString(glGetError()));
#endif

  free(surface);
  return texture;
}

#if 0
GLuint T_loadTexturePCX(char *filename, char wrap)
{
  // fixme: duh.
  return 0;
}
#endif

/* from craterz@hotmail.com, 
   http://www.geocities.com/SiliconValley/Code/1219/
 */
GLuint T_loadTextureRAW(char *filename, char wrap)
{
    GLuint texture;
    int width, height;
    unsigned char * data;
    FILE * file;

    // open texture data
    file = fopen( filename, "rb" );
    if ( file == NULL )
    {
      perror(filename);
      return 0;
    }

    // allocate buffer
    width = 256;
    height = 256;
    data = malloc( width * height * 3 );

    // read texture data
    fread( data, width * height * 3, 1, file );
    fclose( file );

    // allocate a texture name
    glGenTextures( 1, &texture );

    // select our current texture
    glBindTexture( GL_TEXTURE_2D, texture );

    // select modulate to mix texture with color for shading
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
//    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
//    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                     wrap ? GL_REPEAT : GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                     wrap ? GL_REPEAT : GL_CLAMP );

    // build our texture mipmaps
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,
                       GL_RGB, GL_UNSIGNED_BYTE, data );

    // free buffer
    free( data );
    return texture;
}

GLuint T_loadTextureTGA(char *filename, char wrap)
{
  GLuint texture;
  unsigned char *data;
  int w, h;
  int i;
  char *p = filename;

  while(*p++)
    if(*p == '\\')
      *p = '/';

  for(i = 0; i < gt_textureIndex; i++)
    if(strcmp(gt_textures[i].filename, filename) == 0)
    {
      CON_printf("^10Preloaded texture ^3: ^14%s ^3(^14;%d^3)", filename, gt_textures[i].id);
      return gt_textures[i].id;
    }

  if(!(data = TGA_getData(filename, &w, &h)))
  {
    CON_printf("^3Warning: ^15could not get TGA data for \'^14%s^15\'", filename);
    return 0;
  }

  CON_printf("^10Loading texture^3: ^14%s ^3(^14;%d^3x^14;%d^3)", filename, w, h);

  glGenTextures( 1, &texture );
  //printf("gettex: %s\n", gluErrorString(glGetError()));

  gt_textures[gt_textureIndex].filename = strdup(filename);
  gt_textures[gt_textureIndex].width = w;
  gt_textures[gt_textureIndex].height = h;
  gt_textures[gt_textureIndex++].id = texture;
  

  glBindTexture( GL_TEXTURE_2D, texture );

#ifdef DEBUG_TEXTURES
  printf("bindtext: %s\n", gluErrorString(glGetError()));
#endif

  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

#ifdef DEBUG_TEXTURES
  printf("texenv: %s\n", gluErrorString(glGetError()));
#endif

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

#ifdef DEBUG_TEXTURES
  printf("texparam: %s\n", gluErrorString(glGetError()));
#endif

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#ifdef DEBUG_TEXTURES
  printf("texparam: %s\n", gluErrorString(glGetError()));
#endif

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                   wrap ? GL_REPEAT : GL_CLAMP );
#ifdef DEBUG_TEXTURES
  printf("texparam: %s\n", gluErrorString(glGetError()));
#endif

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                   wrap ? GL_REPEAT : GL_CLAMP );

#ifdef DEBUG_TEXTURES
  printf("texparam: %s\n", gluErrorString(glGetError()));
#endif

  glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

#ifdef DEBUG_TEXTURES
  printf("textimage: %s\n", gluErrorString(glGetError()));
#endif

  free(data);
  return texture;
}

void T_changeGamma(char *image, int size, float factor)
{
  int i, after;
  float scale = 1.0, temp = 0.0, r = 0.0, g = 0.0, b = 0.0;

  return;

/* whoever wrote this was on crack */

    // Go through every pixel in the lightmap
    for(i = 0; i < size / 3; i++, image += 3)
    {
        // extract the current RGB values
        r = (float)image[0];
        g = (float)image[1];
        b = (float)image[2];

        if(image[0] != 0 && image[1] != 0 && image[2] != 0)
        {
//          CON_printf("before: %d, %d, %d", image[0], image[1], image[2]);
          after = true;
        }
        else
          after = false;

       // Multiply the factor by the RGB values, while keeping it to a 255 ratio
       r = r * factor / 255.0f;
       g = g * factor / 255.0f;
       b = b * factor / 255.0f;

       // Check if the the values went past the highest value
       if(r > 1.0f && (temp = (1.0f/r)) < scale)
           scale=temp;
       if(g > 1.0f && (temp = (1.0f/g)) < scale)
           scale=temp;
       if(b > 1.0f && (temp = (1.0f/b)) < scale)
           scale=temp;

       // Get the scale for this pixel and multiply it by our pixel values
       scale*=255.0f;

       r*=scale;
       g*=scale;
       b*=scale;

        // Assign the new gamma'nized RGB values to our image
        image[0] = (char)r;
        image[1] = (char)g;
        image[2] = (char)b;
/*
        if(after)
          CON_printf(" after: %d, %d, %d", image[0], image[1], image[2]);
*/
    }
}
