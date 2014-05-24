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

#define CMD_BUF_SIZE 12

#define MONITOR_TITLE "ITHACA AUDIO Z80 "
#define MONITOR_VERSION "v0.01"
#define MONITOR_COPYRIGHT " (Fabio Battaglia)"

static const char title_str[] = MONITOR_TITLE MONITOR_VERSION MONITOR_COPYRIGHT " \a\a\r\n";
static const char monitor_cmds[] = " O - OUT port   | I - IN port    | J - JP to addr \r\n"
								   " W - Write mem  | R - Read mem   | X - XModem trns\r\n"
								   " H - Help \r\n\n";

static const char cmd_prompt[] = "] ";

static const char cmd_notimpl_msg[] = "CMD NOT IMPLEMENTED!\r\n";
static const char cmd_err_msg[] = "CMD ERROR!\r\n";


static char cmd_buffer[CMD_BUF_SIZE];

/******/

void monitor_parse_command(char *cmd, uint8_t idx);

uint8_t monitor_parseU8(char *str);
uint16_t monitor_parseU16(char *str);
/**/
void monitor_outp(uint8_t port, uint8_t data);
uint8_t monitor_inp(uint8_t port);
void monitor_write(uint16_t *addr, uint8_t data);
uint8_t monitor_read(uint16_t *addr);
void monitor_jmp(uint8_t *addr);

uint8_t monitor_parseU8(char *str);

/** Here lies the code **/
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
	uint8_t buf_idx = 0, cmd_read_loop = 1;
	char ch; // Char buffer for data read from console

	// Do basic system initialization
	sys_init();

	console_printString("\r\n");
	console_printString(title_str);
	console_printString(monitor_cmds);

	while(1) { // Endless loop
		console_printString("\r\n");
		console_printString(cmd_prompt);

		cmd_read_loop = 1;
		buf_idx = 0;
		while(cmd_read_loop) {
			ch = getchar(); // Read a char
			putchar(ch); // Print it

			switch(ch) {
				case 0x0D: // CR
					monitor_parse_command(cmd_buffer, buf_idx);	
					cmd_read_loop = 0;
					break;
				default:
					if(buf_idx >= CMD_BUF_SIZE) {
						cmd_read_loop = 0;

						console_printString("\r\n");
						console_printString(cmd_err_msg);

					} else {
						cmd_buffer[buf_idx++] = ch;
					}
					break;
			}

		}
	}
}

/***/

uint8_t monitor_parseU8(char *str) {
	uint8_t val = 0;
	uint8_t idx;
	char ch;

	for (idx = 0; idx < 2; idx++) {
		ch = str[1 - idx];

		if ((ch >= 0x61) && (ch <= 0x66))
			ch -= 0x20;

		if ((ch >= 0x41) && (ch <= 0x46))
			val |= (ch - 55) << (4 * idx); // Convert from ASCII to value
		else if ((ch >= 0x30) && (ch <= 0x39))
			val |= (ch - 48) << (4 * idx); // Convert from ASCII to value
	}

	return val;
}

uint16_t monitor_parseU16(char *str) {
	uint16_t val = 0;

	val |= (monitor_parseU8(str+0) << 8);
	val |= (monitor_parseU8(str+2) << 0);

	return val;
}

void monitor_parse_command(char *cmd, uint8_t idx) {
	if (!idx) return;

	switch(cmd[0]) {
		case 'H': // Help
		case 'h':
			console_printString("\r\n");
			console_printString(monitor_cmds);
			break;
/*
		case 'I': // IN
		case 'i':
		case 'W': // WRITE
		case 'w':
		case 'R': // READ
		case 'r':
		case 'X': // XModem transfer
		case 'x':
*/
		case 'J': // JP
		case 'j':
			monitor_jmp((uint8_t*)monitor_parseU16(&cmd[1]));
			break;
		
		case 'O': // OUT
		case 'o':
			monitor_outp(monitor_parseU8(&cmd[1]), monitor_parseU8(&cmd[4]));
			break;
		default:
			console_printString("\r\n");
			console_printString(cmd_notimpl_msg);
			break;
	}

	return;
}

/*** Monitor Commands ***/
void monitor_write(uint16_t *addr, uint8_t data) {
	addr; data;

	return;
}

uint8_t monitor_read(uint16_t *addr) {
	addr;

	return 0;
}

void monitor_jmp(uint8_t *addr) __naked {
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
