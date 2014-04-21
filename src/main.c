#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common_datatypes.h>

#ifdef __USE_N8VEM_CONSOLE__
#include <io/boards/n8vem_conio.h>
#endif

static const char title_str[] = "ITHACA AUDIO Z80 CPU BOARD";

void sys_init(void) {
	// Cleanup the ram segment used for the monitor
	memset((uint8_t*)__DLOC__, 0x00, __CLOC__-__DLOC__);

#ifdef __USE_N8VEM_CONSOLE__
	n8vem_conio_init();
#endif
}

void main(void) {
	// Do basic system initialization
	sys_init();

	while(1);
}

