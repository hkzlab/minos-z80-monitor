#include "n8vem_conio.h"

// Propeller driven console I/O
static __sfr __at 0x00 PropCIO_STAT;
static __sfr __at 0x01 PropCIO_DATA;

void n8vem_conio_init(void) {
	// Nothing to do...
	return;
}

uint8_t n8vem_conio_getch(void) {
	while(!(PropCIO_STAT & 0x02)); // Loop until we are ready to get the data...
	return PropCIO_DATA;
}

void n8vem_conio_putch(uint8_t ch) {
	while(!(PropCIO_STAT & 0x04)); // Loop until we are ready to send the data...
	PropCIO_DATA = ch; // Send the data
}
