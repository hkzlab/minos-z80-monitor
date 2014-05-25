#include "xmodem.h"

#include <stdlib.h>
#include <string.h>

#include "boards/n8vem_serio.h"


#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define MAXRETRANS 25

static uint8_t xbuff[1030];

static uint8_t check(const uint8_t *buf, uint16_t sz);
static void flushinput(void);

uint8_t xmodem_receive(uint8_t* dest, uint16_t maxSize) {
	unsigned char *p;
	uint16_t bufsz;
	uint8_t trychar = NAK;
	//uint8_t trychar = 'C';
	uint8_t packetno = 1;
	uint16_t i, c, len = 0;
	uint8_t retry, retrans = MAXRETRANS;
	uint16_t count;
	
	for(;;) {
		for(retry = 0; retry < 16; ++retry) {
			if (trychar) n8vem_serio_putch(trychar);
			c = n8vem_serio_getch()<<1;
			
			switch (c) {
				case SOH:
					bufsz = 128;
					goto start_recv;
				case STX:
					bufsz = 1024;
					goto start_recv;
				case EOT:
					flushinput();
					n8vem_serio_putch(ACK);
					return len; /* normal end */
				case CAN:
					if ((c = n8vem_serio_getch()) == CAN) {
						flushinput();
						n8vem_serio_putch(ACK);
						return 1; /* canceled by remote */
					}
					break;
				default:
					break;
				}
		}
		//if (trychar == 'C') { trychar = NAK; continue; }
		flushinput();
		n8vem_serio_putch(CAN);
		n8vem_serio_putch(CAN);
		n8vem_serio_putch(CAN);
		return 2; /* sync error */

	start_recv:
		trychar = 0;
		p = xbuff;
		*p++ = c;
		for (i = 0;  i < (bufsz+3); ++i) {
			c = n8vem_serio_getch();
			*p++ = c;
		}

		if (xbuff[1] == (uint8_t)(~xbuff[2]) && 
			(xbuff[1] == packetno || xbuff[1] == (uint8_t)packetno-1) &&
			check( &xbuff[3], bufsz)) {
			if (xbuff[1] == packetno)	{
				count = maxSize - len;
				if (count > bufsz) count = bufsz;
				if (count > 0) {
					memcpy (&dest[len], &xbuff[3], count);
					len += count;
				}
				++packetno;
				retrans = MAXRETRANS+1;
			}
			if (--retrans <= 0) {
				flushinput();
				n8vem_serio_putch(CAN);
				n8vem_serio_putch(CAN);
				n8vem_serio_putch(CAN);
				return 3; /* too many retry error */
			}
			n8vem_serio_putch(ACK);
			continue;
		}
	reject:
		flushinput();
		n8vem_serio_putch(NAK);
	}
}

#if 0
static uint16_t crc16_ccitt(const uint8_t *buf, int sz) {
	uint8_t i;
	uint16_t crc = 0;
   
	while (--sz >= 0) {
		crc ^= (uint16_t) *buf++ << 8;
		
		for (i = 0; i < 8; i++)
			if (crc & 0x8000)
				crc = crc << 1 ^ 0x1021;
			else
				crc <<= 1;
	}
	
	return crc;
}
#endif

static uint8_t check(const uint8_t *buf, uint16_t sz) {
	uint16_t i;
	uint8_t cks;

	cks = 0;
	for (i = 0; i < sz; ++i) {
		cks += buf[i];
	}
		
	if (cks == buf[sz])
		return 1;
	else 
		return 0;
}

static void flushinput(void) {
	volatile uint8_t stat = 1;

	while (stat)
		 n8vem_serio_getch_nb(&stat);
}


