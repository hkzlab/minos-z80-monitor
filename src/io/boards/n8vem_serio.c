#include "n8vem_serio.h"

#define SERIO_BASE 0xA0

// N8VEM serial I/O
static __sfr __at (SERIO_BASE+0x00) SerIO_CTLB;
static __sfr __at (SERIO_BASE+0x01) SerIO_CTLA;
static __sfr __at (SERIO_BASE+0x02) SerIO_DATB;
static __sfr __at (SERIO_BASE+0x03) SerIO_DATA;

static __sfr __at (SERIO_BASE+0x0B) SerIO_PAR_CTL;
static __sfr __at (SERIO_BASE+0x08) SerIO_PAR_PRTA;
static __sfr __at (SERIO_BASE+0x09) SerIO_PAR_PRTB;
static __sfr __at (SERIO_BASE+0x0A) SerIO_PAR_PRTC;

#define SER_INITDATA_SIZE 16
static uint8_t ser_initdata[SER_INITDATA_SIZE] = { 0x04, 0x44, // WR4: X16 clock,1 Stop,NP
												   0x03, 0xC1, // WR3: Enable reciever, No Auto Enable, Recieve 8 bits
												   0x05, 0xEA, // WR5: Enable, Transmit 8 bits
												   0x0B, 0x56, // WR11: Recieve/transmit clock = BRG
												   0x0C, 0x06, // WR12: BRate Low byte 19,200 Baud
												   //0x0C, 0x0E, // WR12: BRate Low byte 9,600 Baud
												   //0x0C, 0x1E, // WR12: BRate Low byte 4,800 Baud
												   //0x0C, 0x40, // WR12: BRate Low byte 2,400 Baud
												   0x0D, 0x00, // WR13: High byte
												   0x0E, 0x01, // WR14: Use 4.9152 MHz Clock.
												   0x0F, 0x00}; // WR15: Generate Int with CTS going high

void n8vem_serio_init(void) {
	uint8_t idx;
	
	for (idx = 0; idx < SER_INITDATA_SIZE; idx++) {
		SerIO_CTLA = ser_initdata[idx];
	}
	
	for (idx = 0; idx < SER_INITDATA_SIZE; idx++) {
		SerIO_CTLB = ser_initdata[idx];
	}

	SerIO_PAR_CTL = 0x8A; // A input, B output, C(bits 0-3) output, (bits 4-7)input
}

char n8vem_serio_getch(void) {
	SerIO_CTLA = 0x05; // Sel Reg 5
	SerIO_CTLA = 0xEA; // Lower RTS

/*
	__asm
		nop
		nop
	__endasm;
*/
	while(!(SerIO_CTLA & 0x01));

	return SerIO_DATA;
}

char n8vem_serio_getch_nb(uint8_t *stat, uint8_t secs) {
	uint16_t cnt;
	
	SerIO_CTLA = 0x05; // Sel Reg 5
	SerIO_CTLA = 0xEA; // Lower RTS
/*
	__asm
		nop
		nop
	__endasm;
*/
	while(secs--) { 
		//cnt = 0xBBBB;
		cnt = 0x258B;
		while(cnt--) {
			if (SerIO_CTLA & 0x01) {
				*stat = 1;
				return SerIO_DATA;
			}
		}
	}

	*stat = 0;
	return 0;
}

void n8vem_serio_putch(char ch) {
	while (!(SerIO_CTLA & 0x04));

	SerIO_DATA = ch;
	
	SerIO_CTLA = 0x05; // Sel Reg 5
	SerIO_CTLA = 0xE8; // Raise RTS
}
