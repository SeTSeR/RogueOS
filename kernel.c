/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/
#include "keyboard_map.h"
#include "text_colors.h"
    
/* there are 25 lines each of 80 columns; each element takes 2 bytes */
#define LINES 25
#define COLS 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLS * LINES

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

extern keytype keyboard_map[128];
extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

/* current cursor location */
unsigned int current_loc = 0;
/* video memory begins at address 0xb8000 */
char *vidptr = (char*)0xb8000;
/* current key */
unsigned char current_keycode = 0;
keytype current_key = 0, current_mod = 0;

struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];


void idt_init(void)
{
	unsigned long keyboard_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	/* populate IDT entry of keyboard's interrupt */
	keyboard_address = (unsigned long)keyboard_handler;
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

	/*     Ports
	*	 PIC1	PIC2
	*Command 0x20	0xA0
	*Data	 0x21	0xA1
	*/

	/* ICW1 - begin initialization */
	write_port(0x20 , 0x11);
	write_port(0xA0 , 0x11);

	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	write_port(0x21 , 0x20);
	write_port(0xA1 , 0x28);

	/* ICW3 - setup cascading */
	write_port(0x21 , 0x00);
	write_port(0xA1 , 0x00);

	/* ICW4 - environment info */
	write_port(0x21 , 0x01);
	write_port(0xA1 , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);

	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}

void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	write_port(0x21 , 0xFD);
}

void kprint(const char *str)
{
	unsigned int i = 0;
	while (str[i] != '\0') {
		vidptr[current_loc++] = str[i++];
		vidptr[current_loc++] = 0x07;
	}
}

void kprint_newline(void)
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLS;
	current_loc = current_loc + (line_size - current_loc % (line_size));
}

void clear_screen(void)
{
	unsigned int i = 0;
	while (i < SCREENSIZE) {
		vidptr[i++] = ' ';
		vidptr[i++] = 0x07;
	}
}

void keyboard_handler_main(void)
{
	unsigned char status;

	/* write EOI */
	write_port(0x20, 0x20);

	status = read_port(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		current_keycode = read_port(KEYBOARD_DATA_PORT);
                /* check if this key is a modificator */
                keytype key = keyboard_map[current_keycode & (~0x80)];
                if (key & (KEY_SHIFT + KEY_CTRL + KEY_ALT))
                {
                    if (current_keycode & 0x80)
                        current_mod &= (~key);
                    else
                        current_mod |= key;
                    current_keycode = 0;
                }
	}
}

keytype getch()
{
    while ((current_keycode == 0) || (current_keycode & 0x80));
    current_key |= keyboard_map[current_keycode];
            
    keytype t = current_key;
    current_keycode = 0;
    current_key = 0;
    return t | current_mod;
}

void move(int x, int y)
{
    if (x < 0 || y < 0 || x >= COLS || y >= LINES)
        return;
    if ((y*COLS+x) * BYTES_FOR_EACH_ELEMENT < SCREENSIZE)
        current_loc = (y*COLS+x) * BYTES_FOR_EACH_ELEMENT;
    else
        current_loc = x*BYTES_FOR_EACH_ELEMENT;
}

void addch(char c)
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLS;
	if (c == '\n')
        {
            if (current_loc / line_size < LINES-1)
		current_loc = current_loc - (current_loc % line_size) + line_size;
            else
            {
                current_loc = 0;
                clear_screen();
            }
        }
	else if (c == '\t')
		for (int i = 0; i<4; ++i)
		{
			vidptr[current_loc++] = ' ';
			vidptr[current_loc++] = 0x07;
		}	
	else if (c == '\b')
	{
		vidptr[--current_loc] = 0x07;
		vidptr[--current_loc] = ' ';
	}
	else
	{
		vidptr[current_loc++] = c;
		vidptr[current_loc++] = 0x07;
	}
}

void mvaddch(int x, int y, unsigned char c)
{
    move(x,y);
    addch(c);
}

void addstr(const char* str)
{
	int i = 0;
	while (str[i] !=  '\0')
		addch(str[i++]);
}


void kmain(void)
{
	const char *str = "my first kernel with keyboard support";
	clear_screen();
	kprint(str);
	kprint_newline();
	kprint_newline();

	idt_init();
	kb_init();

        int x = 0, y = 0;
        while (1)
        {
            keytype c = getch();
            addch(c);
            
            switch (c)
            {
                case KEY_UP:
                    if (y > 0)
                        --y;
                    break;
                case KEY_RIGHT:
                    if (x < COLS-1)
                        ++x;
                    break;
                    
                case KEY_DOWN:
                    if (y < LINES-1)
                        ++y;
                    break;
                    
                case KEY_LEFT:
                    if (x > 0)
                        --x;
                    break;
               
                case (KEY_LEFT | KEY_SHIFT):
                    x = 0;
                    break;
                    
                case (KEY_RIGHT | KEY_SHIFT):
                    x = COLS-1;
                    break;
                    
                case (KEY_UP | KEY_SHIFT):
                    y = 0;
                    break;
                    
                case (KEY_DOWN | KEY_SHIFT):
                    y = LINES-1;
                    break;
                    
                case (KEY_ALT | KEY_CTRL | ' '):
                    x = y = 0;
                    break;
            }

            clear_screen();
            mvaddch(x,y,'x');
        }
}
