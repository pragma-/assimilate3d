/*==========================================================
  File:  config.c
  Author:  _pragma

  Description:  Reads in and execute commands from a text file.
  ==========================================================*/

#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "texture.h"
#include "console.h"
#include "config.h"
#include "interp.h"
#include "commands.h"
#include "util.h"

void config_init(void)
{
  config_read(CONFIG_DEFAULT_FILE);
}

/*
 *  Quick and dirty for right now ... 
 */

void config_read(char *filename)
{
  FILE *fp;
  char buf[CON_BCS];

#ifdef CONFIG_DEBUG
  extern console_t gc_console;
  int i;
#endif

  if(!(fp = fopen(filename, "r")))
  {
    CON_printf("warning: failed to read config file: %s", filename);
    return;
  }

  while(fgets(buf, CON_BCS, fp))
  {
    buf[strlen(buf)-1] = 0;
    if(*buf && *buf != '/')
    {

#ifdef CONFIG_DEBUG
      printf("Executing: \'%s\'\n", buf);
      printf("bc before: %d: ", gc_console.index_commands);
      for(i = 0; i < CON_BCS; i++)
        printf("[%c]", gc_console.buffer_commands[i]);
      printf("\n");
#endif

      CON_addCommand(buf);

#ifdef CONFIG_DEBUG
      printf("bc between: %d: ", gc_console.index_commands);
      for(i = 0; i < CON_BCS; i++)
        printf("[%c]", gc_console.buffer_commands[i]);
      printf("\n");
#endif

      CON_executeCommands();

#ifdef CONFIG_DEBUG
      printf("bc after: %d: ", gc_console.index_commands);
      for(i = 0; i < CON_BCS; i++)
        printf("[%c]", gc_console.buffer_commands[i]);
      printf("\n");
#endif
    }
  }
  fclose(fp);  
}

COMMAND(CMD_exec)
{
  if(!*arguments)
  {
    CON_printf("Usage: exec <filename>");
    return;
  }
  config_read(arguments);
}
