CC = gcc
CFLAGS = -Wall -O3
LDLIBS = `allegro-config --libs`

EX = ex

$(EX): magic4x4.o ex.o
	$(CC) -o $@ $^ $(LDLIBS)

clean:
	rm -f magic4x4.o
	rm -f $(EX) ex.o
