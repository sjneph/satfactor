CC	= g++
SFLAGS	= -static -ansi -Wall -pedantic -O3
OBJDIR	= objects

SOURCE1	= Extract.cpp
SOURCE2	= IEncode.cpp
BIN	= ../bin

NAME1	= extract-sat
NAME2	= iencode

.cpp.o:; $(CC) -c $(SFLAGS) $<

all:
	mkdir -p $(BIN)
	$(CC) -o $(BIN)/$(NAME1) $(SFLAGS) $(SOURCE1)
	$(CC) -o $(BIN)/$(NAME2) $(SFLAGS) $(SOURCE2)

clean:
	rm -f $(BIN)/$(NAME1)
	rm -f $(BIN)/$(NAME2)
