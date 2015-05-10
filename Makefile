.PHONY=all clean
CFLAGS += -g
all: scan
clean:
	rm -f *.o scan
scan: ip.o main.o scaner.o
	gcc -o $@ $^
