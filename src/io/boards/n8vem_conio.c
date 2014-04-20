#include "n8vem_conio.h"

// Propeller driven console I/O
static __sfr __at 0x00 PropCIO_STAT;
static __sfr __at 0x01 PropCIO_DATA;

void n8vem_conio_init(void) {
	return;
}
