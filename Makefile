# Prefixes
COMPILER_PREFIX = $(SDCC_PREFIX)
COMPILER_LIBS = $(COMPILER_PREFIX)/share/sdcc/lib/z80/

# Options
QUIET = @

# SDCC commands
CCC = $(COMPILER_PREFIX)/bin/sdcc
CAS = $(COMPILER_PREFIX)/bin/sdasz80
CLD = $(COMPILER_PREFIX)/bin/sdldz80

# Misc local commands
ECHO = echo
COPY = cp
MOVE = mv
H2B = hex2bin

ROMADDR = 0xF000

# Tool flags
H2B_FLAGS = -s F000 
#H2B_FLAGS = -s F000

# Project directories
SRC_DIR = src/
BIN_DIR = bin/

INCLUDES = -I$(SRC_DIR)/include -I$(SRC_DIR)

CLOC = 0xF010
CSIZ = 0x79C
DLOC = 0xF800

# Compilation / Assembly / Linking flags
CUST_DEFINES = -D__USE_N8VEM_CONSOLE__ -D__USE_N8VEM_SERIO__ -D__USE_N8VEM_IDE__
CUST_DEFINES += -D__SERIAL_CONSOLE__
CCC_FLAGS = --opt-code-size -mz80 -D__SDCC__=1 -D__ROMADDR__=$(ROMADDR) -D__CLOC__=$(CLOC) -D__DLOC__=$(DLOC) $(CUST_DEFINES) $(INCLUDES)
CAS_FLAGS = -plosff
CLD_FLAGS = --code-loc $(CLOC) --data-loc $(DLOC) --code-size $(CSIZ) --no-std-crt0 --out-fmt-ihx

# Here begins the actual creation of destination files
TARGET = monitor

#all: $(BIN_DIR)/$(TARGET).hex
allpad: $(BIN_DIR)/$(TARGET).bin
	cat $(BIN_DIR)/$(TARGET).bin $(BIN_DIR)/$(TARGET).bin $(BIN_DIR)/$(TARGET).bin $(BIN_DIR)/$(TARGET).bin > $(BIN_DIR)/$(TARGET)_pad.bin

all: $(BIN_DIR)/$(TARGET).bin

$(BIN_DIR)/$(TARGET).bin:	$(BIN_DIR)/$(TARGET).hex
	$(QUIET)$(ECHO) Converting $(TARGET).hex to bin
	$(H2B) $(H2B_FLAGS) $(BIN_DIR)/$(TARGET).hex

$(BIN_DIR)/$(TARGET).hex:	$(BIN_DIR)/$(TARGET).ihx
	$(QUIET)$(ECHO) Generating $(TARGET).ihx
	$(QUIET)$(COPY)	$(BIN_DIR)/$(TARGET).ihx $(BIN_DIR)/$(TARGET).hex

$(BIN_DIR)/$(TARGET).ihx:	$(BIN_DIR)/crt0.rel $(BIN_DIR)/main.rel $(BIN_DIR)/xmodem.rel \
							$(BIN_DIR)/console.rel $(BIN_DIR)/n8vem_serio.rel \
							$(BIN_DIR)/n8vem_conio.rel $(BIN_DIR)/utilities.rel \
							$(BIN_DIR)/n8vem_ide.rel
	$(CCC) $(CLD_FLAGS) $(CCC_FLAGS) $(BIN_DIR)/crt0.rel $(BIN_DIR)/main.rel $(BIN_DIR)/xmodem.rel \
		$(BIN_DIR)/console.rel $(BIN_DIR)/n8vem_serio.rel \
		$(BIN_DIR)/n8vem_conio.rel $(BIN_DIR)/utilities.rel \
		$(BIN_DIR)/n8vem_ide.rel \
		-o $(BIN_DIR)/$(TARGET).ihx

$(BIN_DIR)/crt0.rel: $(SRC_DIR)/crt0.s
	$(CAS) $(CAS_FLAGS) -c -o $(BIN_DIR)/crt0.rel $(SRC_DIR)/crt0.s

$(BIN_DIR)/main.rel: $(SRC_DIR)/main.c
	$(CCC) $(CCC_FLAGS) -c -o $(BIN_DIR) $(SRC_DIR)/main.c

$(BIN_DIR)/utilities.rel: $(SRC_DIR)/utilities.c
	$(CCC) $(CCC_FLAGS) -c -o $(BIN_DIR) $(SRC_DIR)/utilities.c

$(BIN_DIR)/xmodem.rel: $(SRC_DIR)/io/xmodem.c
	$(CCC) $(CCC_FLAGS) -c -o $(BIN_DIR) $(SRC_DIR)/io/xmodem.c

$(BIN_DIR)/console.rel: $(SRC_DIR)/io/console.c
	$(CCC) $(CCC_FLAGS) -c -o $(BIN_DIR) $(SRC_DIR)/io/console.c

$(BIN_DIR)/n8vem_serio.rel: $(SRC_DIR)/io/boards/n8vem_serio.c
	$(CCC) $(CCC_FLAGS) -c -o $(BIN_DIR) $(SRC_DIR)/io/boards/n8vem_serio.c

$(BIN_DIR)/n8vem_conio.rel: $(SRC_DIR)/io/boards/n8vem_conio.c
	$(CCC) $(CCC_FLAGS) -c -o $(BIN_DIR) $(SRC_DIR)/io/boards/n8vem_conio.c

$(BIN_DIR)/n8vem_ide.rel: $(SRC_DIR)/storage/boards/n8vem_ide.c
	$(CCC) $(CCC_FLAGS) -c -o $(BIN_DIR) $(SRC_DIR)/storage/boards/n8vem_ide.c

clean:
	rm $(BIN_DIR)/*
