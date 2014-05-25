#include "xmodem.h"

#include <stdlib.h>
#include <string.h>

#include "boards/n8vem_serio.h"

#define SOH  0x01
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18

#define CTRLZ 0x1A
#define MAXRETRANS 25


static void flush(void);
static uint8_t checksum(uint8_t *buf);

uint8_t xmodem_receive(uint8_t* dest) {
	return 0xFF;
}

static void flush(void) {
	uint8_t val = 1;

	while(val)
		n8vem_serio_getch_nb(&val);
}

static uint8_t checksum(uint8_t *buf) {
	uint8_t val = 0, idx;

	for (idx = 0; idx < 128; idx++)
		val += buf[idx];

	return val;
}
