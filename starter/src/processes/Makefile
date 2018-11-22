# Makefile, ECE254 lab5 Linux Concurrency Control
# Jonathan Shahen, 2018/09/03
# Yiqing Huang, 2014/07/16

CC = gcc
CFLAGS = -Wall -ggdb -std=c99
LD = gcc
LDFLAGS = -ggdb
LDLIBS = -lrt -lpthread -lm

SRCS   = produce.c 
OBJS   = produce.o
TARGETS= produce

all: ${TARGETS}


produce: $(OBJS) 
	$(LD) $(LDFLAGS) -o $@.out $^ $(LDLIBS) 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	gcc -MM -MF $@ $<

.PHONY: clean
clean:
	rm -f *.d *.o *.out
