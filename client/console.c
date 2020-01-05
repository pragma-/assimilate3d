/*==========================================================
  File:  console.c
  Author:  _pragma

  Description:  In game console routines (rendering, input).
  ==========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "util.h"
#include "sdl_stuff.h"
#include "gl.h"
#include "interp.h"
#include "font.h"
#include "texture.h"
#include "console.h"
#include "cvar.h"

console_t gc_console; // our global console object

/*==========================================================
  Function:  CON_initConsole( void )
  Parameters:  none
  Returns:  true  - console was successfully initialized
            false - failed to initialize console

  Description:  Initializes the game console.
  ==========================================================*/

int CON_initConsole( void )
{
  extern cvar_t *cvar_scr_width, *cvar_scr_height;
  extern font_struct_t gf_fonts[];
  int width, height;

  fprintf(stderr, "----------- Init Console -----------\n");

  // obtain screen width and height with failsafe ?: operation
  width =  cvar_scr_width  ? atoi(cvar_scr_width->value)  : SCREEN_WIDTH; 
  height = cvar_scr_height ? atoi(cvar_scr_height->value) : SCREEN_HEIGHT;

  // set all members of gc_console to 0
  memset(&gc_console, 0, sizeof(gc_console));

  // allocate all console buffers
  if(!(gc_console.buffer_commands = malloc(CON_BUFFER_COMMANDS_SIZE)))
    return 0;
  if(!(gc_console.buffer_keys = malloc(CON_BUFFER_KEYSTROKE_SIZE)))
    return 0;
  if(!(gc_console.texture = malloc(sizeof(texture_t))))
    return 0;

  // initialize buffers to 0
  memset(gc_console.buffer_commands, 0, CON_BUFFER_COMMANDS_SIZE);
  memset(gc_console.buffer_keys, 0, CON_BUFFER_KEYSTROKE_SIZE);
  memset(&gc_console.buffer_out, 0, sizeof(struct console_output_buffer_s));

  gc_console.x = width;
  gc_console.y = 0.75;

  gc_console.chars_width  = width  / gf_fonts[FONT_CONSOLE].char_xstep;
  gc_console.chars_height = (height / gf_fonts[FONT_CONSOLE].char_height) / 2;

  // initialize some animation variables
  gc_console.velocity = 0;  
  gc_console.acceleration = 3500;
  gc_console.alpha_max = 0.75;

  CON_printf("console.width:          ^14;%5.2f", gc_console.x);
  CON_printf("console.chars_width:    ^14;%5d", gc_console.chars_width);
  CON_printf("console.chars_height:   ^14;%5d", gc_console.chars_height);
  CON_printf("console.stop:           ^14;%5.2f", (height/2.0));

  CON_printf("------------------------------------");

  // success
  return 1;
}

/*==========================================================
  Function: CON_loadTexture(void)
  Parameters: none
  Returns:  void

  Description:  Loads console background texture.
  ==========================================================*/
void CON_loadTexture(void)
{
  if(!(gc_console.texture->id = T_loadTextureSDL("data/textures/system/console.tga", 1)))
    CON_printf("failed to load console texture");
  else
    CON_printf("Console texture loaded.");
}

/*==========================================================
  Function: CON_destroyConsole(void)
  Parameters: none
  Returns: void

  Description: Frees all memory allocated for console.
  ==========================================================*/
void CON_destroyConsole(void)
{
  console_output_node_t *node, *next;

  fprintf(stderr, "-------- Destroying Console --------\n");

  for(node = gc_console.buffer_out.head; node; node = next)
  {
    next = node->next;
    free(node->text);
    free(node);
  }

  if(gc_console.buffer_commands)
    free(gc_console.buffer_commands);
  if(gc_console.buffer_keys)
    free(gc_console.buffer_keys);

  if(gc_console.texture)
  {
    glDeleteTextures(1, &gc_console.texture->id);
    free(gc_console.texture);
  }
  fprintf(stderr, "console destroyed\n");
  fprintf(stderr, "------------------------------------\n");
}

/*==========================================================
  Function: CON_executeCommands(void)
  Parameters: none
  Returns:  void

  Description:  If any commands are present in the console
                command buffer, they are extracted and sent
                to the interpreter.
  ==========================================================*/
void CON_executeCommands(void)
{
  char command[CON_BUFFER_KEYSTROKE_SIZE];
  int length;

#ifdef CON_EXEC_DEBUG
  int i;
#endif

  if(!*gc_console.buffer_commands)
    return;

  do
  {
    length = strlen(strcpy(command, gc_console.buffer_commands)) + 1;
    gc_console.index_commands -= length;

#ifdef CON_EXEC_DEBUG
    printf("Executing command: %s, %d\n", command, length);
    for(i = 0; i < CON_BCS; i++)
      printf("[%c]", gc_console.buffer_commands[i]);
    printf("\n----------------\n");
#endif
  
    memmove(gc_console.buffer_commands, 
            gc_console.buffer_commands + length, 
            gc_console.index_commands);
    memset(gc_console.buffer_commands + gc_console.index_commands, 0, length);
 
    I_interpretCommand(command);
  } while (*gc_console.buffer_commands);
}

/*==========================================================
  Function: CON_splitCommand(char *command, char **nextcommand)
  Parameters:  command     - command string being processed
               nextcommand - pointer to next command after ';'
  Returns: pointer to 'command'

  Description:  This takes a command string such as:

                  "set foo bar;echo foo is $foo" 

                and splits it into commands seperated by the
                semi-colon by finding the first semi-colon and
                setting it to '\0' and pointing 'nextcommand'
                to the character following it.
  ==========================================================*/
char *CON_splitCommand(char *command, char **nextcommand)
{
  int quoted = 0;

  if(!command || !*command)
    return 0;

  *nextcommand = command;

  while(**nextcommand)
  {
    if(**nextcommand == '\"')
    {
      quoted = !quoted;
      **nextcommand = ' ';
    }

    if(**nextcommand == ';' && (quoted == false))
      break;

    (*nextcommand)++;
  }

  if(**nextcommand)  // non-zero, implies ';'
  {
    **nextcommand = 0;
    (*nextcommand)++;
  }
  return command;
}

/*==========================================================
  Function: CON_addCommand(char *command)
  Parameters: command - command to be added
  Returns: true if command was successfully added to the buffer
           or false otherwise

  Description: Preprocesses a command string fresh off the input
               buffer and adds the commands to the command
               buffer.
  ==========================================================*/
int CON_addCommand(char *command)
{
  int length;
  char *cmd, *nextcommand = 0, buf[CON_BCS];

  strcpy(buf, command);
  command = buf;

  while((cmd = CON_splitCommand(command, &nextcommand)))
  {
    length = strlen(cmd)+1;

    if(length == 1 || 
       gc_console.index_commands + length >= CON_BUFFER_COMMANDS_SIZE)
    {
      // fixme: recycle instead of ignoring
      fprintf(stderr, "addCommand warning: ignoring \'%s\'\n", cmd);
      command = nextcommand;
      continue;
    }

    memcpy(gc_console.buffer_commands + gc_console.index_commands, 
         cmd, length);
    gc_console.index_commands += length;
    command = nextcommand;
  }
  return 1;
}

/*==========================================================
  Function: CON_processKeybuffer(void)
  Parameters: none
  Returns: void

  Description: The user has just finished typing a command in
               the console and has pressed 'enter'.
  ==========================================================*/
void CON_processKeybuffer(void)
{
  CON_printf("^3>^7%s", gc_console.buffer_keys);
  CON_addCommand(gc_console.buffer_keys);
  memset(gc_console.buffer_keys, 0, gc_console.index_keys+1);
  gc_console.index_keys = 0;
}

/*==========================================================
  Function: CON_processKeyRelease(SDL_keysym *keysym)
  Parameters: keysym - information about the keystroke
  Returns: true if we do not want the engine to continue processing
           this keystroke after the console has processed it,
           or false if we do.

  Description: Processes a keystroke that was released.
  ==========================================================*/
int CON_processKeyRelease(SDL_keysym *keysym)
{
  SDLKey key = keysym->sym;
 
  if(gc_console.state == CON_STATE_CLOSED)
    return 0;

  if(key >= 32 && key <= 122) /* ASCII mapped keystrokes */
    return 1;    

  switch(key)
  { 
    case SDLK_BACKQUOTE:
    case SDLK_PAGEUP:
    case SDLK_PAGEDOWN:
    case SDLK_END:
    case SDLK_HOME:
    case SDLK_RETURN:
    case SDLK_BACKSPACE:
    case SDLK_DELETE:
      return 1;
      break;
    default: return 0;
  }
}

/*==========================================================
  Function: CON_processKeystroke(SDL_keysym *keysym)
  Parameters: keysym - information about the keystroke
  Returns: true if want the engine to continue processing
           this keystroke after the console has processed it,
           or false if we do not. (FIXME,TODO: this is backwards
           from CON_processKeyRelease() above.)

  Description: Processes a keystroke.
  ==========================================================*/
int CON_processKeystroke(SDL_keysym *keysym)
{
  SDLKey key = keysym->sym;
  extern cvar_t *cvar_paging_size;
  int paging_size = atoi(cvar_paging_size->value), i = 0;

  // fixme: perhaps all these if()'s can be a switch()

  if(key == SDLK_BACKQUOTE)
  {
    if(gc_console.state < CON_STATE_OPENING)
    {
      gc_console.state = CON_STATE_OPENING;
      SDL_EnableUNICODE(1);
    }
    else
      gc_console.state = CON_STATE_CLOSING;
    return 1;
  }

  if(gc_console.state <= CON_STATE_CLOSING)
    return 0;

  if(key == SDLK_PAGEUP)
  {
    while(gc_console.buffer_out.paging->prev && i++ < paging_size)
      gc_console.buffer_out.paging = gc_console.buffer_out.paging->prev;
    return 1;
  }

  if(key == SDLK_END)
  {
    gc_console.buffer_out.paging = gc_console.buffer_out.tail;
    return 1;
  }

  if(key == SDLK_HOME)
  {
    gc_console.buffer_out.paging = gc_console.buffer_out.head;
    return 1; 
  }

  if(key == SDLK_PAGEDOWN)
  {
    while(gc_console.buffer_out.paging->next && i++ < paging_size)
      gc_console.buffer_out.paging = gc_console.buffer_out.paging->next;
    return 1;
  }

  if(key == SDLK_RETURN)
  {
    gc_console.buffer_keys[gc_console.index_keys++] = 0;
    CON_processKeybuffer();
    return 1;
  }

  if(key == SDLK_BACKSPACE || key == SDLK_DELETE)
  {
    if(gc_console.index_keys > 0)
      gc_console.buffer_keys[--gc_console.index_keys] = 0;
    return 1;
  }

  if(key >= 32 && key <= 122) /* ASCII mapped keystrokes, add to keybuffer */
  {
    if(gc_console.index_keys < CON_BUFFER_KEYSTROKE_SIZE - 2)
    {
      key = keysym->unicode & 0x7f;
      gc_console.buffer_keys[gc_console.index_keys++] = key;
    }
    return 1;
  }
  return 0;
}

/*==========================================================
  Function: CON_rotateScreenBuffer(void)
  Parameters: none
  Returns: void

  Description:  Rotates the buffer that is displayed when the
                console is hidden.
  ==========================================================*/
void CON_rotateScreenBuffer(void)
{
  int i;

  for(i = 0; i < gc_console.index_screen; i++)
    strcpy(gc_console.buffer_screen[i], gc_console.buffer_screen[i+1]);
  gc_console.index_screen--;
}

/*==========================================================
  Function:     CON_popOutputNode(void)
  Parameters:   none
  Returns:      void

  Description:  Removes the head node and makes the next node
                the head.
  ==========================================================*/
void CON_popOutputNode(void)
{
  console_output_node_t *node;

  if(!gc_console.buffer_out.head || gc_console.buffer_out.nodes < 200)
    return;

  node = gc_console.buffer_out.head;

  gc_console.buffer_out.head = node->next;
  gc_console.buffer_out.head->prev = 0;

  if(gc_console.buffer_out.paging == node)
    gc_console.buffer_out.paging = node->next;

  gc_console.buffer_out.nodes--;

  free(node->text);
  free(node);
}

/*==========================================================
  Function:     CON_createOutputNode(char *text)
  Parameters:   text -  text to store in this node
  Returns:      newly allocated output node

  Description:  Allocates and initializes a new output node.
  ==========================================================*/
console_output_node_t *CON_createOutputNode(char *text)
{
  console_output_node_t *node;

  node = malloc(sizeof(console_output_node_t));
  if(!node)
    return 0;

  node->text = strdup(text);
  if(!node->text)
  {
    free(node);
    return 0;
  }
  node->next = 0;
  node->prev = 0;
  return node;
}

/*==========================================================
  Function:     CON_pushOutputNode(console_output_node_t *node)
  Parameters:   node -  node to push onto the end of the list
  Returns:      void

  Description:  Adds a new node to the end of the output list.
  ==========================================================*/
void CON_pushOutputNode(console_output_node_t *node)
{
  if(!gc_console.buffer_out.head)
  {
    gc_console.buffer_out.head   = node;
    gc_console.buffer_out.tail   = node;
    gc_console.buffer_out.paging = node;
  }
  else
  {
    gc_console.buffer_out.tail->next = node;
    node->prev = gc_console.buffer_out.tail;
    if(gc_console.buffer_out.paging == gc_console.buffer_out.tail)
      gc_console.buffer_out.paging = node;
    gc_console.buffer_out.tail = node;
  }
  gc_console.buffer_out.nodes++;  
}

/*==========================================================
  Function: CON_printf(const char *fmt, ...)
  Parameters: fmt - va args format
              ... - va args parameters
  Returns: void

  Description:  Adds a string to the console output buffers.
  ==========================================================*/
void CON_printf(const char *fmt, ...)
{
  char text[1024];  // fixme:  make va() command a-la quake2
  char *p, *q, c;
  va_list ap;
  int length, real_length;
  extern int gt_curtime;
  extern cvar_t *cvar_con_index_screen;
  console_output_node_t *node;

  if(fmt == 0) return;

  va_start(ap, fmt);
    vsnprintf(text, sizeof(text), fmt, ap);
  va_end(ap);

  length = F_strlen(text);        // TODO: make F_strlen(text, &length, &real_length);
  real_length = strlen(text) + 1; // TODO: and eliminate this line

  if(!length)
    return;

  fprintf(stderr, "%s\n", F_color_stripped(text));


  if(length > gc_console.chars_width)
  {
    p = q = text;
    do
    {
      q += (length > gc_console.chars_width) ? gc_console.chars_width : length;
      while(*q && *q != ' ')
        q--;
      *q = 0;
      
      node = CON_createOutputNode(p);
      strcpy(gc_console.buffer_screen[gc_console.index_screen], p);
      gc_console.index_screen++;
      if(gc_console.index_screen > MIN(CONSOLE_INDEX_SCREEN_MAX-1, atoi(cvar_con_index_screen->value)))
        CON_rotateScreenBuffer();
      CON_popOutputNode();  
      CON_pushOutputNode(node);

      if(length < gc_console.chars_width)
        break;

      q++;
      p = q;
      length = F_strlen(p);
      
    } while(length);
  }
  else
  {
    CON_popOutputNode();                // remove head node, make next node head, if applicable
    node = CON_createOutputNode(text);  // allocate and initialize new node
    CON_pushOutputNode(node);           // add this node to tail and reset tail pointer
    strcpy(gc_console.buffer_screen[gc_console.index_screen], text);
    gc_console.index_screen++;
    if(gc_console.index_screen > MIN(CONSOLE_INDEX_SCREEN_MAX-1, atoi(cvar_con_index_screen->value)))
      CON_rotateScreenBuffer();
  }
  gc_console.time_screen = gt_curtime;
}

/*==========================================================
  Function: CON_updateConsole(int msec)
  Parameters: msec - time since last frame in milliseconds
  Returns: void

  Description: Updates certain console variables such as
               animation for opening and closing, screen buffers,
               and whatnot.
  ==========================================================*/
void CON_updateConsole(int msec)
{
  float time = (float)msec/1000.0;
  extern int gt_curtime;  
  extern cvar_t *cvar_scr_width, 
                *cvar_scr_height,
                *cvar_con_rottime,
                *cvar_con_rottime_mod;
  float width  = atof(cvar_scr_width->value), 
        height = atof(cvar_scr_height->value);

  if(gc_console.index_screen && 
     gt_curtime - gc_console.time_screen >= atoi(cvar_con_rottime->value))
  {
    CON_rotateScreenBuffer();
    gc_console.time_screen = gt_curtime - (atof(cvar_con_rottime->value) * 
(atof(cvar_con_rottime_mod->value)));
  }

  if(gc_console.state == CON_STATE_OPENING)
  {
    if(gc_console.y < height/2)             // fixme: add max y to struct
    {
      gc_console.velocity += gc_console.acceleration * time;
      gc_console.y += gc_console.velocity * time;
      gc_console.alpha += gc_console.alpha_max / ((height/2)/(gc_console.velocity * time));
    }
    if(gc_console.y >= height/2)
    {
      gc_console.y = height/2;
      gc_console.state = CON_STATE_OPEN;
      gc_console.alpha = gc_console.alpha_max;
      gc_console.velocity = 0;  // console is at rest
    }
  }
  else if(gc_console.state == CON_STATE_CLOSING)
  {
    if(gc_console.y > 0)
    {
      gc_console.velocity += gc_console.acceleration * time;
      gc_console.y -= gc_console.velocity * time; 
      gc_console.alpha -= gc_console.alpha_max / ((height/2)/(gc_console.velocity * time));
    }
    else
    if(gc_console.y <= 0)
    {
      gc_console.alpha = 0;
      gc_console.y = 0;
      gc_console.state = CON_STATE_CLOSED;
      gc_console.velocity = 0;  // console is at rest
      SDL_EnableUNICODE(0);
    }
  }
}

/*==========================================================
  Function: CON_drawScreenBuffer(void)
  Parameters: none
  Returns: void

  Description: Displays the buffer that is shown when the console
               is up.
  ==========================================================*/
void CON_drawScreenBuffer(void)
{
  extern font_struct_t gf_fonts[];
  int i;

  for(i = 0; i < gc_console.index_screen; i++)
    F_printf(FONT_CONSOLE, 0, i * gf_fonts[FONT_CONSOLE].char_height, gc_console.buffer_screen[i]);
}

/*==========================================================
  Function: CON_drawConsole(int msec)
  Parameters: msec - time since last frame in milliseconds
  Returns: void

  Description: Draws the console.
  ==========================================================*/
void CON_drawConsole(int msec)
{
  extern texture_t gt_textures[T_MAX_TEXTURES];
  extern cvar_t *cvar_condebug;
  extern font_struct_t gf_fonts[];
  float y = gc_console.y;
  extern int gt_curtime;
  int i,j;
  char *p;
  static int blink = true, blinktime = false;
  console_output_node_t *node;
  extern cvar_t *cvar_scr_width, *cvar_scr_height;
  float width = atof(cvar_scr_width->value), height = atof(cvar_scr_height->value);

  if(cvar_condebug && atoi(cvar_condebug->value))
  {
    F_printf(FONT_DEFAULT, 0, height-(16*11), "Console status");
    F_printf(FONT_DEFAULT, 0, height-(16*10), "x: %4.2f, y: %4.2f, alpha: %4.2f, state: %d", 
  			    gc_console.x,
                            gc_console.y, gc_console.alpha,
                            gc_console.state);
    F_printf(FONT_DEFAULT, 0, height-(16*9), "Screen rotate time: %d, current time: %d, time left: %d", gc_console.time_screen, gt_curtime, gt_curtime - gc_console.time_screen);
  }

  if(gc_console.state == CON_STATE_CLOSED)
  {
    glBlendFunc(GL_ONE, GL_ONE);
    CON_drawScreenBuffer();
    return;
  }

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if(gc_console.texture)
    glBindTexture(GL_TEXTURE_2D, gc_console.texture->id);
 
  glColor4f(0.3, 0.5, 0.7, gc_console.alpha);

  glLoadIdentity();

  glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);    glVertex3f(0.0, y, 0.0);
    glTexCoord2f(1.0, 0.0);    glVertex3f(width, y, 0.0);
    glTexCoord2f(1.0, 1.0);    glVertex3f(width, y-height, 0.0);
    glTexCoord2f(0.0, 1.0);    glVertex3f(0.0, y-height, 0.0);
  glEnd();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glBindTexture(GL_TEXTURE_2D, gt_textures[0].id);
  glColor4f(1.0, 1.0, 1.0, gc_console.alpha + 0.25);

  y = gc_console.y - 22;
  for(node = gc_console.buffer_out.paging, i = 0; node && i < gc_console.chars_height; node = node->prev, i++)
  {
    F_printf(FONT_CONSOLE, 0, y - gf_fonts[FONT_CONSOLE].char_height, node->text);
    y -= gf_fonts[FONT_CONSOLE].char_height;
  }

  blinktime += msec;
  if(blinktime > 350)
  {
    blink = !blink;
    blinktime = 0;
  }

  F_printf(FONT_CONSOLE, 0, gc_console.y-22, "^11%c^15%s^11%c", 
           0 ? '|' : '>',  
           gc_console.buffer_keys,
           blink ? '_' : ' ');

  glColor4f(1.0, 1.0, 1.0, 1.0);

}

COMMAND(CMD_clear)
{
  gc_console.index_screen = 0;
}
