__build: main

BUILD_NAME := MemConfigBinTool
ROOT_DIR   := $(shell pwd)
SRC_FILE   := $(wildcard *.c ./*/*.c)
OBJS       := $(SRC_FILE:.c=.o)
INC_DIR    := -I$(ROOT_DIR)/include
CFLAG      :=

main: $(OBJS)
	$(CC) $^ -o $(BUILD_NAME)

	@./$(BUILD_NAME)
	@rm -f $(OBJS) $(BUILD_NAME)

$(OBJS): %.o: %.c
	$(CC) $(INC_DIR) -c $< -o $@ $(CFLAG)

.PHONY: clean
clean:
	@rm -f $(OBJS) $(BUILD_NAME)
