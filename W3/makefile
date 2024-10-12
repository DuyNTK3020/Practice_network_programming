CC = gcc
CFLAGS = -Wall `xml2-config --cflags`
LDFLAGS = -lcurl `xml2-config --libs`

all: resolver

resolver: resolver.o
	$(CC) $(CFLAGS) -o resolver resolver.o $(LDFLAGS)

resolver.o: resolver.c
	$(CC) $(CFLAGS) -c resolver.c

clean:
	rm -f *.o resolver
