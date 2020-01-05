typedef struct console_variable
{
  char *key;
  char *value;
  int flags;
  struct console_variable *next;
} cvar_t;

#define CVAR_LOCKED     (1<<0)  /* value cannot be modified */
#define CVAR_PRIVATE    (1<<1)  /* cvar cannot be unset/destroyed */
#define CVAR_LISTED     (1<<2)  /* cvar is in global list */
#define CVAR_TOGGLE     (1<<3)  /* cvar's key as command in console will toggle instead of display value */

int cvar_init(void);
int alias_init(void);
void cvar_destroyCvars(void);
void cvar_destroyAliases(void);
cvar_t *cvar_new(char *key, char *value, int flags);
cvar_t *alias_new(char *key, char *value, int flags);
cvar_t *cvar_create(char *key, char *value, int flags);
void cvar_destroy(cvar_t *cvar);
void alias_destroy(cvar_t *cvar);
void cvar_add(cvar_t *cvar, cvar_t **list);
void cvar_remove(cvar_t *cvar, cvar_t **list);
cvar_t *cvar_findKey(char *key);
cvar_t *alias_findKey(char *key);
cvar_t *cvar_findValue(void *value);
int cvar_parseCommand(char *command, char *arguments);
int alias_parseCommand(char *command, char *arguments);

