# Prefixes
COMPILER_PREFIX = /opt/local/
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

# Tool flags
# Pad the image to 2048 bytes
H2B_FLAGS = -s F000 -l F800

# Project directories
SRC_DIR = src/
BIN_DIR = bin/

INCLUDE_DIR = $(SRC_DIR)/include

CLOC = 0xF000
DLOC = 0x8000

# Compilation / Assembly / Linking flags
CCC_FLAGS = -c -mz80 -D__SDCC__=1 -D__CLOC__=$(CLOC) -D__DLOC__=$(DLOC) -I $(INCLUDE_DIR)
CAS_FLAGS = -plosff 
CLD_FLAGS = 

# Here begins the actual creation of destination files
TARGET = main

#all: $(BIN_DIR)/$(TARGET).hex
all: $(BIN_DIR)/$(TARGET).bin

$(BIN_DIR)/$(TARGET).bin:	$(BIN_DIR)/$(TARGET).hex
	$(QUIET)$(ECHO) Converting $(TARGET).hex to bin
	$(H2B) $(H2B_FLAGS) $(BIN_DIR)/$(TARGET).hex

$(BIN_DIR)/$(TARGET).hex:	$(BIN_DIR)/$(TARGET).ihx
	$(QUIET)$(ECHO) Generating $(TARGET).ihx
	$(QUIET)$(COPY)	$(BIN_DIR)/$(TARGET).ihx $(BIN_DIR)/$(TARGET).hex

$(BIN_DIR)/$(TARGET).ihx:	$(BIN_DIR)/$(TARGET).rel $(BIN_DIR)/$(TARGET).arf \
		$(BIN_DIR)/xmodem.rel $(BIN_DIR)/console.rel $(BIN_DIR)/n8vem_serio.rel $(BIN_DIR)/n8vem_conio.rel
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/$(TARGET).arf
	$(QUIET)$(MOVE) $(TARGET).ihx $(BIN_DIR)
	$(QUIET)$(MOVE) $(TARGET).map $(BIN_DIR)
	$(QUIET)$(MOVE) $(TARGET).noi $(BIN_DIR)

$(BIN_DIR)/$(TARGET).rel: $(SRC_DIR)/$(TARGET).c
	$(CCC) $(CCC_FLAGS) -o $(BIN_DIR) $(SRC_DIR)/$(TARGET).c

$(BIN_DIR)/xmodem.rel: $(SRC_DIR)/io/xmodem.c
	$(CCC) $(CCC_FLAGS) -o $(BIN_DIR) $(SRC_DIR)/io/xmodem.c

$(BIN_DIR)/console.rel: $(SRC_DIR)/io/console.c
	$(CCC) $(CCC_FLAGS) -o $(BIN_DIR) $(SRC_DIR)/io/console.c

$(BIN_DIR)/n8vem_serio.rel: $(SRC_DIR)/io/boards/n8vem_serio.c
	$(CCC) $(CCC_FLAGS) -o $(BIN_DIR) $(SRC_DIR)/io/boards/n8vem_serio.c

$(BIN_DIR)/n8vem_conio.rel: $(SRC_DIR)/io/boards/n8vem_conio.c
	$(CCC) $(CCC_FLAGS) -o $(BIN_DIR) $(SRC_DIR)/io/boards/n8vem_conio.c

$(BIN_DIR)/$(TARGET).arf:
	$(QUIET)$(ECHO) Generating $(TARGET).arf
	$(QUIET)$(ECHO) -mjx > $(BIN_DIR)/$(TARGET).arf
	$(QUIET)$(ECHO) -b _CODE=$(CLOC) >> $(BIN_DIR)/$(TARGET).arf
	$(QUIET)$(ECHO) -b _DATA=$(DLOC) >> $(BIN_DIR)/$(TARGET).arf
	$(QUIET)$(ECHO) -i $(TARGET).ihx >> $(BIN_DIR)/$(TARGET).arf
	$(QUIET)$(ECHO) -k $(COMPILER_LIBS) >> $(BIN_DIR)/$(TARGET).arf
	$(QUIET)$(ECHO) -l z80 >> $(BIN_DIR)/$(TARGET).arf
	$(QUIET)$(ECHO) $(BIN_DIR)/$(TARGET).rel >> $(BIN_DIR)/$(TARGET).arf
	$(QUIET)$(ECHO) $(BIN_DIR)/console.rel >> $(BIN_DIR)/$(TARGET).arf
	$(QUIET)$(ECHO) $(BIN_DIR)/xmodem.rel >> $(BIN_DIR)/$(TARGET).arf
	$(QUIET)$(ECHO) $(BIN_DIR)/n8vem_conio.rel >> $(BIN_DIR)/$(TARGET).arf
	$(QUIET)$(ECHO) $(BIN_DIR)/n8vem_serio.rel >> $(BIN_DIR)/$(TARGET).arf
	$(QUIET)$(ECHO) -u >> $(BIN_DIR)/$(TARGET).arf
	$(QUIET)$(ECHO) -e >> $(BIN_DIR)/$(TARGET).arf

clean:
	rm $(BIN_DIR)/*
