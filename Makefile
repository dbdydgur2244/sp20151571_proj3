CC = gcc
CFLAGS = -Wall
SRCS = function.c 20151571.c sp_proj1.c assemble.c memory.c loader.c string_mani.c opcode.c run.c debug.c
OBJECTS=$(SRCS:.c=.o)

TARGET = 20151571.out

all: $(SRCS) $(TARGET)
	
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm $(OBJECTS) $(TARGET)

