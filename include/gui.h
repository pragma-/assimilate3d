#define GUI_STATE_MAINMENU  0   // main start up menu
#define GUI_STATE_OPTIONS   1   // options menu
#define GUI_STATE_CONFIG    2   // config screen
#define GUI_STATE_INGAME    3   // in-game menu
#define GUI_STATE_INVENTORY 4   // item inventory screen


void gui_init(void);
int gui_init_startup(void);
int gui_init_ingamemenu(void);
int gui_init_options(void);
int gui_init_config(void);
void gui_backbutton_on_click(float x, float y, int button);
void gui_options_on_click(float x, float y, int button);
void gui_config_on_click(float x, float y, int button);
void gui_newgame_on_click(float x, float y, int button);
void gui_exitgame_on_click(float x, float y, int button);
void gui_resumegame_on_click(float x, float y, int button);
void gui_switch_state(int state);
