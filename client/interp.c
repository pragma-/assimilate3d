/*=====================================================
  File: interp.c , Console interpreter
  Auther: _pragma

  Description:  Console interpreter and console commands.
  =====================================================*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <ctype.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "interp.h"
#include "commands.h"
#include "texture.h"
#include "console.h"
#include "cvar.h"
#include "util.h"
#include "font.h"

/*=====================================================
  Console commands and the function they point to.
  =====================================================*/

const command_t gi_commands[] =
{
  {  "UNUSED"       ,    0            },
  {  "di"           ,    CMD_qui      },
  {  "die"          ,    CMD_quit     },
  {  "qui"          ,    CMD_qui      },
  {  "quit"         ,    CMD_quit     },
  {  "commands"     ,    CMD_list     },
  {  "textures"     ,    CMD_list     },
  {  "models"       ,    CMD_list     },
  {  "help"         ,    CMD_help     },
  {  "echo"         ,    CMD_echo     },
  {  "clear"        ,    CMD_clear    },
  {  "gl_version"   ,    CMD_gl_info  },
  {  "gl_renderer"  ,    CMD_gl_info  },
  {  "gl_vendor"    ,    CMD_gl_info  },
  {  "gl_extensions",    CMD_gl_info  },
  {  "echo"         ,    CMD_echo     },
  {  "load"         ,    CMD_load     },
  {  "set"          ,    CMD_set      },
  {  "unset"        ,    CMD_unset    },
  {  "cvars"        ,    CMD_cvars    },
  {  "alias"        ,    CMD_alias    },
  {  "unalias"      ,    CMD_unalias  },
  {  "aliaslist"    ,    CMD_aliaslist},
  {  "keynames"     ,    CMD_keynames },
  {  "bind"         ,    CMD_bind     },
  {  "unbind"       ,    CMD_unbind   },
  {  "bindlist"     ,    CMD_bindlist },
  {  "exec"         ,    CMD_exec     },
  {  "camera"       ,    CMD_camera   },
  {  "gamemenu"     ,    CMD_gamemenu },
  {  "map"          ,    CMD_map      },
  {  "entities"     ,    CMD_listEntities   },
  {  "entload"      ,    CMD_loadEntity      },
  {  "entposition"  ,    CMD_positionEntity },
  {  "entmodel"     ,    CMD_setEntityModel },
  {  "entselect"    ,    CMD_selectEntity   },
  {  "lightadd"     ,    CMD_addLight       },
  {  "lightposition",    CMD_positionLight  },
  {  "lightambient" ,    CMD_setLightProperty },
  {  "lightdiffuse" ,    CMD_setLightProperty },
  {  "lightspecular",    CMD_setLightProperty },
  {  "lightshininess",   CMD_setLightProperty },
  {  "colors",           CMD_showcolors       },
  {  "sv_start",         CMD_sv_start         },
  {  "sv_shutdown",      CMD_sv_shutdown      },
  {  "connect",          CMD_connect          },
  {  "disconnect",       CMD_disconnect       },
  {  "say",              CMD_say              },
  {  "sky",              CMD_skybox           },
  {  "\n"           ,    0                  }
};

int gi_total_commands = 0;

void I_initCommands( void )
{
  int i;

  CON_printf("---------- Init Commands -----------");

  for(i = 0; *gi_commands[i].command != '\n'; i++);
  gi_total_commands = i;

  CON_printf("%d commands initialized",i-1); 

  CON_printf("------------------------------------");
}

char *I_expandCommand(char *unparsedCommand)
{
  static char buf[CON_BCS];
  char *p, var[CON_BCS];
  int i, j, flag = 1;
  cvar_t *cvar;
  
  for(i = 0, p = unparsedCommand; *p; p++)
  {
    if(*(p+flag) == '$')
    {
      if(*p == '\\')
        continue;

      buf[i++] = *p;

      p+=1+flag;

      for(j = 0; *p; p++)
      {
        if(!isalnum(*p) && *p != '_')
          break; 
        var[j++] = *p;
      }

      var[j] = 0;
      buf[i] = 0;

      if(!(cvar = cvar_findKey(var)))
      {
        sprintf(buf + i, "$%s", var);
        i += strlen(var) + 1;
        continue;
      }

      strcat(buf, cvar->value);
      i += strlen(cvar->value);
      flag = 0;
    }
    else
      flag = 1;

    buf[i++] = *p;
  }
  buf[i] = 0;
  return buf;
}

void I_interpretCommand(char *unparsedCommand)
{
  int i;
  char command[CON_BUFFER_KEYSTROKE_SIZE],
       arguments[CON_BUFFER_KEYSTROKE_SIZE],
       *s;

  s = I_expandCommand(unparsedCommand);
  I_halfChop(s, command, arguments);

  for(i = 0; i < gi_total_commands; i++)
    if(!(strncmp(gi_commands[i].command, command, strlen(command))))
      break;

  if(i == gi_total_commands) /* command not found */
  {
    if(!cvar_parseCommand(command, arguments))
      if(!alias_parseCommand(command, arguments))
        CON_printf("^6%s^15: ^7command not found", command);
    return;
  }

  if(i > 0 && (gi_commands[i].command_pointer != 0))
    gi_commands[i].command_pointer(arguments, i);
}

void I_halfChop(char *string, char *command, char *arguments)
{
    char *arg_head = arguments;
    
    for (; isspace(*string) || (*string == '/'); string++)
        ;
    for (; *string && !isspace(*command = tolower(*string)); string++, command++)
        ;
    *command = '\0';
    for (; isspace(*string); string++)
        ;
    for (; (*arguments = *string); string++, arguments++)
        ;
    *arguments = '\0';        
    for (arguments--; (arguments != arg_head) && isspace(*arguments); arguments--)
        *arguments = '\0';
    
}
