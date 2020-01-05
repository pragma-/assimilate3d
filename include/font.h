typedef struct font_struct_s
{
  float char_width,
        char_height,
        char_xstep;

} font_struct_t;

// void F_buildFont(void);  // no longer used
inline void F_renderFont(int font, char *text);
void F_printf(int font, GLint x, GLint y, const char *fmt, ...);
int F_strlen(char *text);
char *F_color_stripped(char *text);

#define FONT_DEFAULT  0
#define FONT_CONSOLE  0

COMMAND(CMD_showcolors);
