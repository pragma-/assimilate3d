/*==========================================================
  File:  kbnames.c
  Author:  _pragma

  Description:  List of keyboard key identification numbers
                matched to stringified identifiers (for bind).
  ==========================================================*/

#include "SDL.h"

#include "kbnames.h"
#include "util.h"

/*==========================================================
  This is a "hash" connecting keyboard keystroke identification
  numbers with text string indentifiers.
  ==========================================================*/

/* Note: new keys must be added in the exact order
 *       shown in SDL_keysym.h!  If you add new keys
 *       beyond the last element in the list, you must
 *       set the MAX_KEYSTATES define accordingly (input.c).
 *       :pragma
 */

const kb_names_t keyNames[] =
    {
        /* see SDL_keysym.h  */
        /* ASCII mapped keys */

        {  0,               "UNUSED"      },
        {  SDLK_BACKSPACE,  "backspace"   },
        {  SDLK_TAB,        "tab"         },
        {  SDLK_CLEAR,      "clear"       },
        {  SDLK_RETURN,     "return"      },
        {  SDLK_RETURN,     "enter"       },
        {  SDLK_PAUSE,      "pause"       },
        {  SDLK_ESCAPE,     "escape"      },
        {  SDLK_SPACE,      "space"       },
        {  SDLK_EXCLAIM,    "!"           },
        {  SDLK_QUOTEDBL,   "\""          },
        {  SDLK_HASH,       "#"           },
        {  SDLK_DOLLAR,     "$"           },
        {  SDLK_AMPERSAND,  "&"           },
        {  SDLK_QUOTE,      "\'"          },
        {  SDLK_LEFTPAREN,  "("           },
        {  SDLK_RIGHTPAREN, ")"           },
        {  SDLK_ASTERISK,   "*"           },
        {  SDLK_PLUS,       "+"           },
        {  SDLK_COMMA,      ","           },
        {  SDLK_MINUS,      "-"           },
        {  SDLK_PERIOD,     "."           },
        {  SDLK_SLASH,      "/"           },
        {  SDLK_0,          "0"           },
        {  SDLK_1,          "1"      },
        {  SDLK_2,          "2"      },
        {  SDLK_3,          "3"      },
        {  SDLK_4,          "4"      },
        {  SDLK_5,          "5"      },
        {  SDLK_6,          "6"      },
        {  SDLK_7,          "7"      },
        {  SDLK_8,          "8"      },
        {  SDLK_9,          "9"      },
        {  SDLK_COLON,      ":"      },
        {  SDLK_SEMICOLON,  ";"      },
        {  SDLK_LESS,       "<"      },
        {  SDLK_EQUALS,     "="      },
        {  SDLK_GREATER,    ">"      },
        {  SDLK_QUESTION,   "?"      },
        {  SDLK_AT,         "@"      },

        /* skip uppercase letters */

        {  SDLK_LEFTBRACKET,     "["      },
        {  SDLK_BACKSLASH,       "\\"     },
        {  SDLK_RIGHTBRACKET,    "]"      },
        {  SDLK_CARET,           "^"      },
        {  SDLK_UNDERSCORE,      "_"      },
        {  SDLK_BACKQUOTE,       "`"      },
        {  SDLK_BACKQUOTE,       "~"      },
        {  SDLK_a, "a"      },
        {  SDLK_b, "b"      },
        {  SDLK_c, "c"      },
        {  SDLK_d, "d"      },
        {  SDLK_e, "e"      },
        {  SDLK_f, "f"      },
        {  SDLK_g, "g"      },
        {  SDLK_h, "h"      },
        {  SDLK_i, "i"      },
        {  SDLK_j, "j"      },
        {  SDLK_k, "k"      },
        {  SDLK_l, "l"      },
        {  SDLK_m, "m"      },
        {  SDLK_n, "n"      },
        {  SDLK_o, "o"      },
        {  SDLK_p, "p"      },
        {  SDLK_q, "q"      },
        {  SDLK_r, "r"      },
        {  SDLK_s, "s"      },
        {  SDLK_t, "t"      },
        {  SDLK_u, "u"      },
        {  SDLK_v, "v"      },
        {  SDLK_w, "w"      },
        {  SDLK_x, "x"      },
        {  SDLK_y, "y"      },
        {  SDLK_z, "z"      },
        {  SDLK_DELETE, "delete"      },

        /* end of ASCII */

        /* numpad/keypad */

        { SDLK_KP0, "kp_0"      },
        { SDLK_KP1, "kp_1"      },
        { SDLK_KP2, "kp_2"      },
        { SDLK_KP3, "kp_3"      },
        { SDLK_KP4, "kp_4"      },
        { SDLK_KP5, "kp_5"      },
        { SDLK_KP6, "kp_6"      },
        { SDLK_KP7, "kp_7"      },
        { SDLK_KP8, "kp_8"      },
        { SDLK_KP9, "kp_9"      },
        { SDLK_KP_PERIOD,   "kp_period"      },
        { SDLK_KP_DIVIDE,   "kp_divide"      },
        { SDLK_KP_MULTIPLY, "kp_multiply"    },
        { SDLK_KP_MINUS,    "kp_minus"       },
        { SDLK_KP_PLUS,     "kp_plus"        },
        { SDLK_KP_ENTER,    "kp_enter"       },
        { SDLK_KP_EQUALS,   "kp_equals"      },

        /* Arrows + Home/End pad */

        { SDLK_UP,       "up"         },
        { SDLK_DOWN,     "down"       },
        { SDLK_RIGHT,    "right"      },
        { SDLK_LEFT,     "left"       },
        { SDLK_INSERT,   "insert"     },
        { SDLK_HOME,     "home"       },
        { SDLK_END,      "end"        },
        { SDLK_PAGEUP,   "pageup"     },
        { SDLK_PAGEDOWN, "pagedown"   },

        /* Function keys */

        { SDLK_F1,  "f1"       },
        { SDLK_F2,  "f2"       },
        { SDLK_F3,  "f3"       },
        { SDLK_F4,  "f4"       },
        { SDLK_F5,  "f5"       },
        { SDLK_F6,  "f6"       },
        { SDLK_F7,  "f7"       },
        { SDLK_F8,  "f8"       },
        { SDLK_F9,  "f9"       },
        { SDLK_F10, "f10"      },
        { SDLK_F11, "f11"      },
        { SDLK_F12, "f12"      },
        { SDLK_F13, "f13"      },
        { SDLK_F14, "f14"      },
        { SDLK_F15, "f15"      },

        /* Key state modifier keys */

        { SDLK_NUMLOCK,   "numlock"     },
        { SDLK_CAPSLOCK,  "capslock"    },
        { SDLK_SCROLLOCK, "scroll_lock" },
        { SDLK_RSHIFT,    "rshift"     },
        { SDLK_LSHIFT,    "lshift"     },
        { SDLK_RCTRL,     "rctrl"       },
        { SDLK_LCTRL,     "lctrl"       },
        { SDLK_RALT,      "ralt"        },
        { SDLK_LALT,      "lalt"        },
        { SDLK_LSUPER,    "lwinkey"     },
        { SDLK_RSUPER,    "rwinkey"     },       

        /* End of hash */

        { -1,             "\n"          }
};
