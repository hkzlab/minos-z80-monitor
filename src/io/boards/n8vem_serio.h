#ifndef  _N8VEMSERIO_HEADER_
#define _N8VEMSERIO_HEADER_

#include <common_datatypes.h>

void n8vem_serio_init(void);
char n8vem_serio_getch();
char n8vem_serio_getch_nb(uint8_t *stat, uint8_t secs);
void n8vem_serio_putch(char ch);

#endif /* _N8VEMSERIO_HEADER_ */
