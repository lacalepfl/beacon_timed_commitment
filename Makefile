CC=g++
CFLAGS=-Wall -I/usr/local/include -O3
CFLAGSLINK=-lgmp 
SOURCES=

all: timed_pass clean
timed_pass: timed_commit.o main.o
	$(CC) timed_commit.o main.o $(CFLAGS) $(CFLAGSLINK) -o timed_pass
main.o:
	$(CC) main.cpp $(SOURCES) $(CFLAGS) -c
timed_commit.o:
	$(CC) timed_commit.cpp timed_commit.h $(SOURCES) $(CFLAGS) -c
clean:
	rm -rf *.o
	rm -rf *.h.gch
	rm -rf *.h.gch
