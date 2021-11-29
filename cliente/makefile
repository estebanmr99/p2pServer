SOURCEC1 = main.c Peer.c
TARGETC1 = client

.PHONY: all

all: c1

c1:
	$(CC) $(SOURCEC1) -o $(TARGETC1) -lpthread -lm

clean:
	-rm -f *.o
	#-rm -f *.txt
	-rm -f $(TARGETC)