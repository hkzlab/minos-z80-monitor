#include "n8vem_serio.h"

// N8VEM serial I/O
static __sfr __at 0xA0 SerIO_CTLB;
static __sfr __at 0xA1 SerIO_CTLA;
static __sfr __at 0xA2 SerIO_DATB;
static __sfr __at 0xA3 SerIO_DATA;

#define SER_INITDATA_SIZE 16
static uint8_t ser_initdata[SER_INITDATA_SIZE] = { 0x04, 0x44, // WR4: X16 clock,1 Stop,NP
												   0x03, 0xC1, // WR3: Enable reciever, No Auto Enable, Recieve 8 bits
												   0x05, 0xEA, // WR5: Enable, Transmit 8 bits
												   0x0B, 0x56, // WR11: Recieve/transmit clock = BRG
												   0x0C, 0x06, // WR12: BRate Low byte 19,200 Baud
												   0x0D, 0x00, // WR13: High byte
												   0x0E, 0x01, // WR14: Use 4.9152 MHz Clock.
												   0x0F, 0x00}; // WR15: Generate Int with CTS going high

void n8vem_serio_init(void) {
	return;
}

