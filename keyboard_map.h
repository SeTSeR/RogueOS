/* The following array is taken from 
    http://www.osdever.net/bkerndev/Docs/keyboard.htm
   All credits where due
*/

enum keys
{
    KEY_ESC = 1,
    
    KEY_F1 = 0x101, 
    KEY_F2, KEY_F3, KEY_F4,
    KEY_F5, KEY_F6, KEY_F7, 
    KEY_F8, KEY_F9, KEY_F10,
    KEY_F11, KEY_F12,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,
    
    KEY_CAPSLOCK,
    KEY_NUMLOCK,
    KEY_SCRLOCK,
    
    KEY_HOME,
    KEY_PGUP,
    KEY_PGDN,
    KEY_END,
    
    KEY_INSERT,
    KEY_DELETE,
    
    KEY_SHIFT = 1<<10,
    KEY_CTRL  = 1<<11,
    KEY_ALT   = 1<<12
};

typedef int keytype;
/* first 8 bits : char of key
 *        9 bit : for non-ascii keys
 *       10 bit : shift
 *       11 bit : ctrl
 *       12 bit : alt
 */

keytype keyboard_map[128] =
{
    0,  KEY_ESC, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    KEY_CTRL,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   KEY_SHIFT,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   KEY_SHIFT,				/* Right shift */
  '*',
    KEY_ALT,	/* Alt */
  ' ',	/* Space bar */
    KEY_CAPSLOCK, /* Caps lock */
    KEY_F1, /* 59 - F1 key ... > */
    KEY_F2,   KEY_F3,   KEY_F4,   KEY_F5,   KEY_F6,   KEY_F7,   KEY_F8,   KEY_F9,
    KEY_F10, /* < ... F10 */
    KEY_NUMLOCK, /* 69 - Num lock*/
    KEY_SCRLOCK, /* Scroll Lock */
    KEY_HOME,	/* Home key */
    KEY_UP,	/* Up Arrow */
    KEY_PGUP,	/* Page Up */
  '-',
    KEY_LEFT,	/* Left Arrow */
    0,
    KEY_RIGHT,	/* Right Arrow */
  '+',
    KEY_END,	/* 79 - End key*/
    KEY_DOWN,	/* Down Arrow */
    KEY_PGDN,	/* Page Down */
    KEY_INSERT,	/* Insert Key */
    KEY_DELETE,	/* Delete Key */
    0,   0,   0,
    KEY_F11,	/* F11 Key */
    KEY_F12,	/* F12 Key */
    0,	/* All other keys are undefined */
};
