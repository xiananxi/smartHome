CC := aarch64-linux-gnu-gcc
#CC := aarch64-none-linux-gnu-gcc

SRC := $(shell find src -name "*.c")
INC := ./inc \
	./usr/local/include \
	./usr/usr/include \
	./usr/usr/include/python3.10 \
	./usr/usr/include/aarch64-linux-gnu/python3.10/ \
	./usr/usr/include/aarch64-linux-gnu/

OBJ := $(subst src/,obj/,$(SRC:.c=.o))

TARGET := obj/smarthome

CFLAGS := $(foreach item,$(INC),-I $(item))
LIBS_PATH := ./usr/local/lib \
	./usr/lib/aarch64-linux-gnu/ \
	./usr/usr/lib/aarch64-linux-gnu/ \
	./usr/usr/lib/python3.10/ \

LDFLAGS := $(foreach item,$(LIBS_PATH),-L $(item))
LIBS := -lwiringPi -lpython3.10 -lpthread -lexpat -lz -lcrypt
obj/%.o: src/%.c
	mkdir -p obj
	$(CC) -c $< -o  $@ -fPIC $(CFLAGS)
$(TARGET): $(OBJ)
	$(CC) $^ -o $@ -fPIC $(CFLAGS) $(LDFLAGS) $(LIBS)
	scp obj/smarthome src/face.py ini/gdevice.ini orangepi@192.168.31.173:/home/orangepi/ 
all: $(TARGET)
clean:
	rm -rf obj/*
debug:
	echo $(SRC)
	echo $(OBJ)
	echo $(TARGET)
	echo $(CFLAGS)
	echo $(LDFLAGS)
	echo $(LIBS)
	echo $(INC)
	echo $(CC)
.PHONY: all clean debug
