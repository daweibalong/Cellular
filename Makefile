LDFLAGS = -L/usr/local/lib -llua 
INCLUDE = -I/usr/local/include

all:queue box engine testcase 

clean:
	rm *.o testcase testcase2
queue:
	gcc $(INCLUDE) queue.c -c -o queue.o
box:
	gcc $(INCLUDE) box.c -c -o box.o
engine:
	gcc $(INCLUDE) engine.c -c -o engine.o
testcase:
	gcc $(INCLUDE) testcase.c -c -o testcase.o
	gcc $(LDFLAGS) queue.o box.o engine.o testcase.o -o testcase

.PHONY: all clean
