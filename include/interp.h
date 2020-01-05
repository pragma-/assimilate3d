#define COMMAND(name) void (name)(char *arguments, int cmd)

typedef struct command_data
{
  char *command;
  void (*command_pointer)(char *arguments, int cmd);
} command_t;

#define CMD_IS(c)  (strncmp(gi_commands[cmd].command, (c), strlen((c))) == 0)

void I_initCommands(void);
void I_interpretCommand(char *unparsedCommand);
void I_halfChop(char *string, char *command, char *arguments);

