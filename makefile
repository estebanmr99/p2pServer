SOURCEC1 = main.c IPFinder.c Server.c HTTPRequest.c Queue.c
TARGETC1 = server

SOURCEC2 = client.c
TARGETC2 = client

.PHONY: all

all: c1 c2

c1:
	$(CC) $(SOURCEC1) -o $(TARGETC1) -lpthread -lm

c2:
	$(CC) $(SOURCEC2) -o $(TARGETC2) -lpthread

clean:
	-rm -f *.o
	#-rm -f *.txt
	-rm -f $(TARGETC)