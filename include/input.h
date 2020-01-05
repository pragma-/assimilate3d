typedef struct keyboard_state
{
  int state;
  char *command;
} kb_state_t;

#define KEY_PRESSED   1
#define KEY_RELEASED  0

void I_initKeyStates(void);
void I_destroyKeyStates(void);
void I_keyPressed(SDL_keysym *keysym);
void I_keyReleased(SDL_keysym *keysym);
void I_mouseMotion(SDL_MouseMotionEvent *motion);
int I_findKey(char *key);
int I_findKeyName(char *key);
char *I_findKeyNameByID(int id);
void I_bindKey(char *key, char *command);
void I_mouseButtonClicked(SDL_MouseButtonEvent *button);
void I_mouseButtonReleased(SDL_MouseButtonEvent *button);
void I_mouseLook(ent_t *entity, int mouseX, int mouseY);
