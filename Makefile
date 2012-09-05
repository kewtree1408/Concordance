CC = gcc
CFLAGS =  -c -p -g#-pedantic -Wall -std=c99 -Werror -Wno-sign-conversion -lm -g -pg 
LD = gcc
LDFLAGS = 
PROGNAME1 = idx_builder
PROGNAME2 = idx_searcher
OBJ = 2-3_tree.o parsers.o list_query.o find_and_context.o other_funcs.o


.SUFFIXES: .c .o

all:	$(PROGNAME1) $(PROGNAME2)
clean:
	rm -f *.o $(PROGNAME1) $(PROGNAME2)

$(PROGNAME1): $(OBJ) builder.o
	$(LD) $(LDFLAGS) -o $(PROGNAME1) $(OBJ) builder.o

$(PROGNAME2): $(OBJ) searcher.o
	$(LD) $(LDFLAGS) -o $(PROGNAME2) $(OBJ) searcher.o

.c.o:
	$(CC) $(CFLAGS) $<

2-3_tree.o: 2-3_tree.h
parsers.o: parsers.h
builder.o: builder.h
list_query.o: list_query.h
find_and_context.o: find_and_context.h
searcher.o: searcher.h
other_funcs.o: other_funcs.h
