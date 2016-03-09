CFLAGS= -g -Wall
CC=gcc
INCFLAG = -I./include -I./3rd/libevent/include -I./3rd/lua/ 
LDFLAGS = -L./3rd/lua -L/usr/lib 
LDLIBS = -llua -lm -ldl -L/usr/lib -levent_core
BUILD_PATH ?= .

SRC = main.c gated.c gamed.c config.c network.c log.c

all : \
	$(BUILD_PATH)/main

$(BUILD_PATH)/main : $(foreach v, $(SRC), src/$(v)) 
	$(CC) $(CFLAGS) -o $@ $^ $(INCFLAG) $(LDFLAGS) $(LDLIBS)

.c.o:                                                                                                                           
	$(CC) $(CFLAGS) -c $<

clean:                                                                                                                          
	rm $(BUILD_PATH)/main
	rm -f *~
	rm -f *.o

%.o : %.c
	echo "    Compile File $<"
	$(CC) -c -o $@ $< $(CFLAGS) 
