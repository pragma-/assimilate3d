#define CONSOLE_INDEX_SCREEN_MAX  50

typedef struct console_output_node_s
{
  char *text;
  struct console_output_node_s *prev;
  struct console_output_node_s *next;
} console_output_node_t;

typedef struct console_output_buffer_s
{
  console_output_node_t *paging;
  console_output_node_t *head;
  console_output_node_t *tail;
  int nodes;
} console_output_buffer_t;

typedef struct console_data
{
  int state;     // console state - closed, open, moving up/down
  float x,y;     // lower right x,y coordinates, for raising/lowering 
  float velocity;// velocity for lowering/raising
  float acceleration; // acceleration for lowering/raising
  float alpha;        // degree of transparency, used during animation
  float alpha_max;    // user supplied max value, default: 0.75
  float alpha_mod;    // alpha modifier

  console_output_buffer_t buffer_out; // buffer for output
  char *buffer_commands;  // buffer for completed commands
  char *buffer_keys;      // buffer for incoming keystrokes
  char buffer_screen[CONSOLE_INDEX_SCREEN_MAX][200];    // buffer for displaying text on screen
  int  time_screen;              // time message was sent to buffer_screen

  int index_out;          // index into output buffer line (for '\n')
  int index_keys;         // index into keystroke buffer
  int index_commands;     // index into command buffer
  int index_screen;       // index into screen buffer
  
  int chars_width;        // number of characters we can print
                          // horizontally at this resolution

  int chars_height;       // and vertically 

  texture_t *texture;     // texture for console

} console_t;

#define CON_BUFFER_COMMANDS_SIZE  1024
#define CON_BUFFER_KEYSTROKE_SIZE 256

#define CON_BCS CON_BUFFER_COMMANDS_SIZE
#define CON_BKS CON_BUFFER_KEYSTROKE_SIZE

#define CON_STATE_CLOSED   0
#define CON_STATE_CLOSING  1
#define CON_STATE_OPENING  2
#define CON_STATE_OPEN     3


int CON_initConsole( void );
void CON_destroyConsole(void);
int CON_processKeyRelease(SDL_keysym *keysym);
int CON_processKeystroke(SDL_keysym *keysym);
void CON_executeCommands(void);
int CON_addCommand(char *command);
void CON_processKeybuffer(void);
void CON_updateConsole(int msec);
void CON_drawConsole(int msec);
void CON_loadTexture(void);
void CON_printf(const char *fmt, ...);
char *CON_splitCommand(char *command, char **nextcommand);
void CON_popOutputNode(void);
void CON_pushOutputNode(console_output_node_t *node);
console_output_node_t *CON_createOutputNode(char *text);
