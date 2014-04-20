#include "console.h"

// Propeller driven console I/O
static __sfr __at 0x00 PropCIO_STAT;
static __sfr __at 0x01 PropCIO_DATA;
