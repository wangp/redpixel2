CC = gcc
CFLAGS = -Wall -O3 -I../xmlparse -I../xmltok
LIBS = -L../lib -lxmlparse -lxmltok

all: elements

elements: elements.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	
clean:
	rm -f elements.o elements
