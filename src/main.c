#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common_datatypes.h>

void sys_init(void) {
	memset((uint8_t*)__DLOC__, 0x00, __CLOC__-__DLOC__);
}

void main(void) {
	// Do basic system initialization
	sys_init();

	while(1);
}

