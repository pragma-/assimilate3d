/*==========================================================
  File:  font.c
  Author:  _pragma

  Description:  Handles fonts (rendering, initialization).
  ==========================================================*/

#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "gl.h"
#include "texture.h"
#include "util.h"
#include "interp.h"
#include "font.h"

GLuint gf_baseFonts = 0;

font_struct_t gf_fonts[] =
{
  { 12, 12, 7 }    // FONT_CONSOLE, FONT_DEFAULT  (0)
};

/*==========================================================
  Function:   F_renderFont(int font, char *text)
  Parameters: font - font texture (id into texture list) 
              text - text to be rendered
  Returns:    nothing

  Description:  Replacement for F_buildFont.  F_renderFont
                renders supplied text using supplied font texture.
  ==========================================================*/
inline void F_renderFont(int font, char *text)
{
  extern texture_t gt_textures[T_MAX_TEXTURES];
  float x=0.0, y=0.0;
  float fwidth = gf_fonts[font].char_width,
        fheight = gf_fonts[font].char_height,
        fxstep = gf_fonts[font].char_xstep;

  while(*text)
  {
    x = (*text - 32) * (1.0 / 16.0);
    if(x >= 1.0)
      y = (int)x * (1.0 / 16.0);
    else
      y = 0.0;

#if 0
    fprintf(stderr, "'%c' - %d - (%f, %f)\n", *text, *text - 32, x, y);
#endif

    glBegin(GL_QUADS);
      glTexCoord2f(x+1.0/16.0, y);
      glVertex2i((int)fwidth, 0);

      glTexCoord2f(x, y);
      glVertex2i(0, 0);

      glTexCoord2d(x, y+1.0/16.0);
      glVertex2i(0, (int)fheight);

      glTexCoord2d(x+1.0/16.0, y+1.0/16.0);
      glVertex2i((int)fwidth, (int)fheight);
    glEnd();
    glTranslated((int)fxstep, 0, 0);

    text++;
  }

  glBindTexture(GL_TEXTURE_2D, gt_textures[font].id);  
}

/*==========================================================
  Function:  F_printf(GLint x, GLint y, const char *fmt, ...)
  Parameters:  x   - position on x coordinate
               y   - position on y coordinate
               fmt - format string
                
  Returns:  nothing

  Description:  F_printf prints text on the display at
                position (x, y).  F_printf is now able to
                colorize the text by using the '^' character
                followed by a number representing the ANSI
                color.  Example: "^1This is red.^2 This is green."
  ==========================================================*/

void F_printf(int font, GLint x, GLint y, const char *fmt, ...)
{
  char text[2048], *p, *b = 0;
  int color;
  va_list ap;
  static float ANSI_colors[][3] = 
  { 
    { 0.0,  0.0,  0.0  },     // 0   black
    { 0.6,  0.0,  0.0  },     //     red
    { 0.0,  0.6,  0.0  },     //     green
    { 0.9,  0.45, 0.0  },     //     brown
    { 0.0,  0.0,  0.6  },     // 4   blue
    { 0.4,  0.0,  0.65 },     //     purple
    { 0.0,  0.4,  0.65 },     //     dark cyan
    { 0.6,  0.6,  0.6  },     //     white/light grey
    { 0.3,  0.3,  0.3  },     // 8   dark grey
    { 1.0,  0.0,  0.0  },     //     bright red
    { 0.0,  1.0,  0.0  },     //     bright green
    { 1.0,  1.0,  0.0  },     //     yellow
    { 0.0,  0.0,  1.0  },     // 12  bright blue
    { 1.0,  0.0,  1.0  },     //     magneta
    { 0.4,  1.0,  1.0  },     //     cyan
    { 1.0,  1.0,  1.0  }      // 15  bright white
  };

  if(fmt == 0) return;

  va_start(ap, fmt);
    vsprintf(text, fmt, ap);
  va_end(ap);

  glLoadIdentity();
  glTranslated(x,y,0);

  glListBase(gf_baseFonts-32);

  glColor4f(1.0, 1.0, 1.0, 1.0);

  if(!(p = strchr(text, '^')))
    F_renderFont(font, text);
  else
  {
    *p = 0;
    F_renderFont(font, text);
    b = p;

    do
    {
      p = b;

      p++;
      color = atoi(p);

      while(isdigit(*p)) p++;
      if(*p == ';') p++;

      if((b = strchr(p, '^')))
        *b = 0;

      if(color < 0 || color > 15)
        color = 15;

      glColor4f(ANSI_colors[color][0], 
                ANSI_colors[color][1], 
                ANSI_colors[color][2], 1.0);

      F_renderFont(font, p);

    } while(b);
  }
}

/*==========================================================
  Function:   F_strlen(char *text)
  Parameters:
              text - text to be counted

  Returns:    integer length of string 

  Description:  Counts number of printable characters in string.
  ==========================================================*/
int F_strlen(char *text)
{
  int i = 0;

  while(*text)
  {
    // skip color codes
    if(*text == '^')
    {
      text++;
      while(isdigit(*text)) 
        text++;
      if(*text == ';')
        text++;
      continue;
    }
    i++;
    text++;
  }
  return i;
}

/*==========================================================
  Function:   F_color_stripped(char *text)
  Parameters:
              text - inputted text to be processed

  Returns:    text without color codes

  Description:  Strips color codes from text for logs.
  ==========================================================*/

char *F_color_stripped(char *text)
{
  static char buf[1024];
  char *p;
  int i = 0;

  p = buf;

  while(*text && i < 1024)
  {
    if(*text == '^')
    {
      text++;
      while(isdigit(*text))
        text++;
      if(*text == ';')
        text++;
      continue;
    }
    *p++ = *text++;
    i++;
  }
  *p = 0;
  return buf;
}

COMMAND(CMD_showcolors)
{
  CON_printf("^15;0: ^0BLACK  ^15; 9: ^9BRIGHT RED");
  CON_printf("^15;1: ^1RED    ^15;10: ^10BRIGHT GREEN");
  CON_printf("^15;2: ^2GREEN  ^15;11: ^11BRIGHT YELLOW");
  CON_printf("^15;3: ^3ORANGE ^15;12: ^12BRIGHT BLUE");
  CON_printf("^15;4: ^4BLUE   ^15;13: ^13MAGNETA");
  CON_printf("^15;5: ^5PURPLE ^15;14: ^14BRIGHT CYAN");
  CON_printf("^15;6: ^6CYAN   ^15;15: ^15BRIGHT WHITE");
  CON_printf("^15;7: ^7GREY ");
  CON_printf("^15;8: ^8DARK GREY ");
}
