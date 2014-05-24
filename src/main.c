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

static const char title_str[] = "ITHACA AUDIO Z80 CPU BOARD\r\n";

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

	while(1) {
		console_printString(title_str);
	}
}

/*** Monitor Commands ***/

void monitor_outp(uint8_t port, uint8_t data) {
	port; data; // Silence the warning...

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
	__endasm;
}

uint8_t monitor_inp(uint8_t port) {
	port;

	return 0;
}
