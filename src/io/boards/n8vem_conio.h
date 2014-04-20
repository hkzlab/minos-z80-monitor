#ifndef _N8VEMCONIO_HEADER_
#define _N8VEMCONIO_HEADER_

#include <common_datatypes.h>

void n8vem_conio_init(void);

uint8_t n8vem_conio_getch(void);
void n8vem_conio_putch(uint8_t ch);

#endif /* _N8VEMCONIO_HEADER_ */
