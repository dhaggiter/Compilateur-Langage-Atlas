CC      = gcc
CFLAGS  = -Wall -Wno-unused-function -g
LDFLAGS = -lm

TARGET  = atlas

SRCS    = main.c symtable.c codegen.c simulator.c atlas.tab.c lex.yy.c
HDRS    = symtable.h codegen.h simulator.h atlas.tab.h

all: $(TARGET)

atlas.tab.c atlas.tab.h: atlas.y
	bison -d atlas.y

lex.yy.c: atlas.l atlas.tab.h
	flex atlas.l

$(TARGET): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET) atlas.tab.c atlas.tab.h lex.yy.c *.map *.o

.PHONY: all clean
