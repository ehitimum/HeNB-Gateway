# Compiler and flags
CC = gcc
CFLAGS = -D_DEFAULT_SOURCE -I lib
LDFLAGS = -lsctp

# Source files
LIB_DIR = lib
LIB_SOURCES = $(shell find $(LIB_DIR) -name "*.c" ! -name "converter-example.c")
MAIN_SOURCES = henb-gw.c s1ap-modifier.c s1ap-handler.c
SRC_FILES = $(MAIN_SOURCES) $(LIB_SOURCES)

# Output executable
TARGET = henb-gw

# Build rules
all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

