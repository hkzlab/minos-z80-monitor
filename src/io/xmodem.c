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



uint8_t xmodem_receive(uint8_t* dest) {
	return 0xFF;
}

