CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-value -Iinclude -Ivendor/hash -Ivendor/vector


SRC_DIR = src
VENDOR_DIR = vendor
HASH_DIR = $(VENDOR_DIR)/hash
VECTOR_DIR = $(VENDOR_DIR)/vector
INCLUDE_DIR = include

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/database.c \
       $(SRC_DIR)/parser.c \
       $(HASH_DIR)/hash.c \
       $(VECTOR_DIR)/vector.c


OBJS = $(SRCS:.c=.o)


TARGET = database

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking executable..."
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


%.o: %.c
	@echo "Compiling $< ..."
	$(CC) $(CFLAGS) -c $< -o $@
	
run: $(TARGET)
	@echo "Running program..."
	./$(TARGET)

clean:
	@echo "Cleaning up..."
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean