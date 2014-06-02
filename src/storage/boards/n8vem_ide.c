#include "n8vem_ide.h"

#define IDE_BASE_ADDR 0x30

// IDE I/O ports
static __sfr __at (IDE_BASE_ADDR+0x03) IDE_Ctrl;
static __sfr __at (IDE_BASE_ADDR+0x00) IDE_PortA;
static __sfr __at (IDE_BASE_ADDR+0x01) IDE_PortB;
static __sfr __at (IDE_BASE_ADDR+0x02) IDE_PortC;

// DEFINES:
// SHD Config
// IDE SHD register structure
// LBA -> Enables LBA mode
// DEV -> Device select
// hea -> Head number
// obs | LBA | obs | DEV | hea | hea | hea | hea
#define IDE_SHD_CFG			0b11100000

// 8255 I/O Modes
#define MODE_8255_INPUT		0b10010010
#define MODE_8255_OUTPUT	0b10000000

// IDE lines
#define IDE_LINE_A0		0x01
#define IDE_LINE_A1		0x02
#define IDE_LINE_A2		0x04
#define IDE_LINE_CS0	0x08
#define IDE_LINE_CS1	0x10
#define IDE_LINE_WR		0x20
#define IDE_LINE_RD		0x40
#define IDE_LINE_RST	0x80

// IDE REGISTERS
#define IDE_REG_DATA	(IDE_LINE_CS0)
#define IDE_REG_ERR		(IDE_LINE_CS0 | IDE_LINE_A0)
#define IDE_REG_SECCNT	(IDE_LINE_CS0 | IDE_LINE_A1)
#define IDE_REG_SHD		(IDE_LINE_CS0 | IDE_LINE_A1 | IDE_LINE_A2)
#define IDE_REG_SEC		(IDE_LINE_CS0 | IDE_LINE_A0 | IDE_LINE_A1)
#define IDE_REG_CYLL	(IDE_LINE_CS0 | IDE_LINE_A2)
#define IDE_REG_CYLH	(IDE_LINE_CS0 | IDE_LINE_A0 | IDE_LINE_A2)
#define IDE_REG_CMD		(IDE_LINE_CS0 | IDE_LINE_A0 | IDE_LINE_A1 | IDE_LINE_A2)
#define IDE_REG_STAT	(IDE_LINE_CS0 | IDE_LINE_A0 | IDE_LINE_A1 | IDE_LINE_A2)
#define IDE_REG_CTRL	(IDE_LINE_CS1 | IDE_LINE_A1 | IDE_LINE_A2)
#define IDE_REG_ASTAT	(IDE_LINE_CS1 | IDE_LINE_A0 | IDE_LINE_A1 | IDE_LINE_A2)

// IDE COMMANDS
#define IDE_CMD_RECAL	0x10
#define IDE_CMD_RD		0x20
#define IDE_CMD_WR		0x30
#define IDE_CMD_INIT	0x91
#define IDE_CMD_ID		0xEC
#define IDE_CMD_SPDOWN	0xE0
#define IDE_CMD_SPUP	0xE1

uint8_t n8vem_ide_brd(uint8_t reg);
void n8vem_ide_bwr(uint8_t reg, uint8_t val);

uint8_t n8vem_ide_init(void) {
	uint8_t delay = 0xFF;

	// Set 8255 to input
	IDE_Ctrl = MODE_8255_INPUT;

	// Reset the drive
	IDE_PortC = IDE_LINE_RST;

	while(delay--) { 
		__asm
			nop
		__endasm;
	};
	delay = 0xFF;
	
	IDE_PortC = 0;

	n8vem_ide_bwr(IDE_REG_SHD, IDE_SHD_CFG);
	
	while(delay--) {
		__asm
			nop
		__endasm;
		if (!n8vem_ide_brd(IDE_REG_STAT & 0x80)) return 0; // Check the busy flag is off
	}

	// If we got here, the init timed out
	return 0xFF;
}

uint8_t n8vem_ide_brd(uint8_t reg) {
	uint8_t reg_val = 0;

	IDE_PortC = reg;
	IDE_PortC = reg | IDE_LINE_RD;

	reg_val = IDE_PortA;

	IDE_PortC = reg;
	IDE_PortC = 0;

	return reg_val;
}

void n8vem_ide_bwr(uint8_t reg, uint8_t val) {
	IDE_Ctrl = MODE_8255_OUTPUT;
	
	IDE_PortA  = val;

	IDE_PortC = reg;
	IDE_PortC = reg | IDE_LINE_WR;
	IDE_PortC = reg;
	IDE_PortC = 0;

	IDE_Ctrl = MODE_8255_INPUT;
}
