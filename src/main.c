#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common_datatypes.h>
#include <io/console.h>

#ifdef __USE_N8VEM_CONSOLE__
#include <io/boards/n8vem_conio.h>
#endif

#ifdef __USE_N8VEM_SERIO__
#include <io/boards/n8vem_serio.h>
#endif

#define MONITOR_TITLE "ITHACA AUDIO Z80 "
#define MONITOR_VERSION "v0.01"
#define MONITOR_COPYRIGHT " (Fabio Battaglia)"

static const char title_str[] = MONITOR_TITLE MONITOR_VERSION MONITOR_COPYRIGHT " \a\a\r\n";
static const char monitor_cmds[] = "O - OUT to port | I - IN from port | J - JP to addr\r\n"
								   "X - XModem      \r\n";

void monitor_outp(uint8_t port, uint8_t data);

void sys_init(void) {
	// Clear Zero page
	memset((uint8_t*)0x00, 0x00, 0xFF);

#ifdef __USE_N8VEM_CONSOLE__
	n8vem_conio_init();
#endif

#ifdef __USE_N8VEM_SERIO__
	n8vem_serio_init();
#endif
}

void main(void) {
	// Do basic system initialization
	sys_init();

	console_printString(title_str);
	console_printString(monitor_cmds);

	while(1) {
		;
	}
}

/*** Monitor Commands ***/

void monitor_outp(uint8_t port, uint8_t data) __naked {
	port; data;

	__asm
		ld hl, #3
		add hl, sp
		ld a, (hl) // Load data from stack

		ld hl, #2
		add hl, sp

		push bc
		
		ld c, (hl) // Load port from stack
		out (c), a // Output to port

		pop bc

		ret
	__endasm;
}

uint8_t monitor_inp(uint8_t port) __naked {
	port;

	__asm
		ld hl, #2
		add hl, sp

		push bc

		ld c, (hl)
		in l,(c)

		pop bc

		ret
	__endasm;
}
