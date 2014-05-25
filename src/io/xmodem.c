#include "xmodem.h"

#include <stdlib.h>
#include <string.h>

#include "boards/n8vem_serio.h"
#include "console.h"

#define SOH  0x01
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18

#define CTRLZ 0x1A
#define MAXERR 25


static void flush(void);
static uint8_t checksum(uint8_t *buf);
static uint8_t wait_getData(uint8_t *data);
static uint8_t xmodem_mngPkt(uint8_t *dest);

static uint8_t expected_pkt;

uint8_t xmodem_receive(uint8_t* dest) {
	uint8_t retries = MAXERR;
	uint8_t ch = 0;
	uint8_t res;

	expected_pkt = 0x01; // Setting to the first packet

	n8vem_serio_putch(NAK);
	while(retries--) {
		if(wait_getData(&ch)) {
			switch(ch) {
				case SOH:
					res = xmodem_mngPkt(dest);
					if (res == 1) { // New block
						dest+=128; // Next block
						expected_pkt++;
						n8vem_serio_putch(ACK);
						retries = MAXERR;
					} else if (res == 2) { // Repeated block
						flush();
						n8vem_serio_putch(ACK);
						retries = MAXERR;
					} else { // Garbage
						flush();
						n8vem_serio_putch(NAK);
					}
					break;
				case EOT:
					n8vem_serio_putch(ACK);
					return 0;
				default: // Reading garbage?
					flush();
					n8vem_serio_putch(NAK);
					break;
			}
		} else {
			n8vem_serio_putch(NAK);
		}
	}

	return 0xFF;
}

static void flush(void) {
	uint8_t val = 1;
	uint8_t del_loop;

	while(val) {
		del_loop = 0xFF;
		while(del_loop--) {
			__asm
				nop
			__endasm;
		}
		n8vem_serio_getch_nb(&val);
	}
}

static uint8_t checksum(uint8_t *buf) {
	uint8_t val = 0, idx;

	for (idx = 0; idx < 128; idx++)
		val += buf[idx];

	return val;
}

static uint8_t wait_getData(uint8_t *data) {
	uint16_t retries = 0xFFFF;
	uint8_t stat = 0;	

	while (retries-- && !stat) {
		*data = n8vem_serio_getch_nb(&stat);		
	}

	// Read nothing...
	if (!stat)
		return 0;
	else
		return 1;
}

static uint8_t xmodem_mngPkt(uint8_t *dest) {
	uint8_t pktID, npktID, chk, idx;

	console_printString("\r\nMNG\r\n");
	if (!wait_getData(&pktID)) { // Trying to get pktID
		return 0;
	}

	if (!wait_getData(&npktID)) { // Trying to get npktID
		return 0;
	}

	if(npktID != (0xFF - pktID)) { // Verify we got the packet number correctly
		return 0;
	}
	
	if (pktID == (expected_pkt - 1)) // Already got this...
		return 2;
	else if (pktID == expected_pkt) {
		for(idx = 0; idx < 128; idx++) {
			console_printString("\r\nRD\r\n");
			if (!wait_getData(&dest[idx])) {
				console_printString("\r\nFREAD\r\n");
				return 0;
			}
		}
	
		if (!wait_getData(&chk)) { // Trying to get checksum 
			return 0;
		}

		if(checksum(dest) == chk) { // OK!
			return 1;
		}
	}
	
	return 0;
}
