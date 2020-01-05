typedef struct gui_element
{
  int type;      // button, text, tab, etc

  char *text;    // message on the widget/element

  float x1, y1,
        x2, y2;  // bounding box coordinates

  GLuint textureId;

  void (*on_click)(float x, float y, int button); // duh

  struct gui_element *next;
} gui_element_t;


#define  GUI_BUTTON    1    // button
#define  GUI_TEXTBOX   2    // text box for displaying text
#define  GUI_TEXTENTRY 3    // text box for inputting data

gui_element_t *gui_create_element(int type, char *text, 
                                  float x1, float y1, 
                                  float x2, float y2,
                                  char *texture);
void gui_add_element_to_list(gui_element_t *element, gui_element_t **list);
void gui_remove_element_from_list(gui_element_t *element, gui_element_t **list);
void gui_destroy_element(gui_element_t *element);
void gui_destroy_all_elements(gui_element_t *element_list);
void gui_processMouseClick(int x, int y, int button, gui_element_t *element_list);
void gui_draw_elements(gui_element_t *element_list);
