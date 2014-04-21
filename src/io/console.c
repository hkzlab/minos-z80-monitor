#include "console.h"

#ifdef __USE_N8VEM_CONSOLE__
#include <io/boards/n8vem_conio.h>
#endif

// We need to implement this for stdio...
void putchar(char ch);
char getchar(void);

void console_printString(char *str) {
	while(*str != '\0') {
		putchar(*str);
		str++;
	}
}

void putchar(char ch) {
#ifdef __USE_N8VEM_CONSOLE__
	n8vem_conio_putch(ch);
#endif
}

char getchar(void) {
#ifdef __USE_N8VEM_CONSOLE__
	return n8vem_conio_getch();
#endif
}

