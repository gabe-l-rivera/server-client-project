CFLAGS = -Wall
LDLIBS = -lpthread

SERVER = filed
CLIENTS = newKey fileGet fileDigest fileRun
CSAPP = csapp.h csapp.c

all: $(CSAPP) $(SERVER) $(CLIENTS)

csapp.h:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/include/csapp.h

csapp.c:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/src/csapp.c

csapp.o: csapp.h csapp.c

$(SERVER): csapp.o
$(CLIENTS): csapp.o

.PHONY: clean
clean:
	/bin/rm -rf csapp.h csapp.c *.o filed newKey fileGet fileDigest fileRun

zip:
	zip project5.zip *.c README
