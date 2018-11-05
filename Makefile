PROGRAM= streljana
CC= gcc
CFLAGS= -Wall 
LDFLAGS= -lglut -lGLU -lGL -lm

$(PROGRAM): main.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)	

main.o: main.c
	$(CC) -c $< $(CFLAGS)

.PHONY: clean

clean:
	-rm *.o $(PROGRAM)
