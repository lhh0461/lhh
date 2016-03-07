CC=gcc
CFLAGS= -g -Wall -I../include -I../3rd/libevent-2.0.22-stable/include -I../3rd/lua/ -L../3rd/lua -llua -lm -ldl -L/usr/lib -levent_core

all: gated

gated : gated.o log.o 
	$(CC) $(CFLAGS) gated.o log.o -o gated

.c.o:                                                                                                                           
	$(CC) $(CFLAGS) -c $<

clean:                                                                                                                          
	rm -f *~
	rm -f *.o
