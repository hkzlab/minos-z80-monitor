#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common_datatypes.h>
#include <io/console.h>

#include "utilities.h"

#ifdef __USE_N8VEM_CONSOLE__
#include <io/boards/n8vem_conio.h>
#endif

#ifdef __USE_N8VEM_SERIO__
#include <io/boards/n8vem_serio.h>
#include <io/xmodem.h>
#endif

#define CMD_BUF_SIZE 10

#define MONITOR_HEAD "\x1B[2J MINOS 1.1\a\r\n"

#define MONITOR_CMD_PROMPT "\r\n] "
#define MONITOR_ERR_MSG "\r\nERR\r\n"

static const char monitor_cmds[] =	"\r\n"
									"  O - OUT I - IN  J - JP  W - WRM R - RDM X - XFR\r\n"
									"  H - Help\r\n";

static char cmd_buffer[CMD_BUF_SIZE];

/******/

void monitor_parse_command(char *cmd, uint8_t idx);

/**/
void monitor_outp(uint8_t port, uint8_t data);
uint8_t monitor_inp(uint8_t port);
void monitor_jmp(uint8_t *addr);


/** Here lies the code **/
void sys_init(void) {
	//
	// Initialize the function pointers
	uint8_t f_counter = 0;
	 *(uint16_t*)(__ROMADDR__ + 0x800 + f_counter) = (uint16_t)getchar;
	 f_counter+=2;
	 *(uint16_t*)(__ROMADDR__ + 0x800 + f_counter) = (uint16_t)putchar;
	 f_counter+=2;
	 *(uint16_t*)(__ROMADDR__ + 0x800 + f_counter) = (uint16_t)console_printString;
	 f_counter+=2;

#ifdef __USE_N8VEM_CONSOLE__
//	n8vem_conio_init();
#endif

#ifdef __USE_N8VEM_SERIO__
	n8vem_serio_init();
#endif
}

void main(void) {
	uint8_t buf_idx = 0, cmd_read_loop = 1;
	char ch; // Char buffer for data read from console

	// Do basic system initialization
	sys_init();

	console_printString(MONITOR_HEAD);
	console_printString(monitor_cmds + 2);

	while(1) { // Endless loop
		console_printString(MONITOR_CMD_PROMPT);

		cmd_read_loop = 1;
		buf_idx = 0;
		while(cmd_read_loop) {
			ch = getchar(); // Read a char

			// Turn the letter uppercase!
			if (ch >= 0x61 && ch <= 0x7A) {
				ch -= 0x20;
			}

			putchar(ch); // Print it

			switch(ch) {
				case 0x0D: // CR
					monitor_parse_command(cmd_buffer, buf_idx);	
					cmd_read_loop = 0;
					break;
				default:
					if(buf_idx >= CMD_BUF_SIZE) {
						cmd_read_loop = 0;

						console_printString(MONITOR_ERR_MSG);

					} else {
						cmd_buffer[buf_idx++] = ch;
					}
					break;
			}

		}
	}
}

/***/


#define STR_BUFF_LEN 11
void monitor_parse_command(char *cmd, uint8_t idx) {
	char buff[STR_BUFF_LEN];
	uint8_t val;

	if (!idx) return;

	memset(buff, ' ', sizeof(buff));
	buff[sizeof(buff)-1] = 0;

	switch(cmd[0]) {
		case 'H': // Help
			console_printString(monitor_cmds);
			break;
#ifdef __USE_N8VEM_SERIO__
		case 'X': // XModem transfer
			xmodem_receive((uint8_t*)monitor_parseU16(&cmd[1]));
			break;
#endif
		case 'I': // IN
			val = monitor_inp(monitor_parseU8(&cmd[1]));
			
			// Copy the address
			memcpy(buff, &cmd[1], 2);
			monitor_printU8(val, &buff[4]);

			console_printString("\r\n");
			console_printString(buff);
			break;
		case 'R': // READ
			val = *((uint8_t*)monitor_parseU16(&cmd[1]));

			// Copy the address
			memcpy(buff, &cmd[1], 4);
			monitor_printU8(val, &buff[6]);

			console_printString("\r\n");
			console_printString(buff);
			break;
		case 'W': // WRITE
			*((uint8_t*)monitor_parseU16(&cmd[1])) = monitor_parseU8(&cmd[6]);
			break;
		case 'J': // JP
			monitor_jmp((uint8_t*)monitor_parseU16(&cmd[1]));
			break;
		
		case 'O': // OUT
			monitor_outp(monitor_parseU8(&cmd[1]), monitor_parseU8(&cmd[4]));
			break;
		default:
			console_printString(MONITOR_ERR_MSG);
			break;
	}

	return;
}

/*** Monitor Commands ***/
void monitor_jmp(uint8_t *addr) __naked {
	addr;

	__asm
		pop bc
		pop hl
		jp (hl)

		ret
	__endasm;
}

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
