SOURCEC1 = Client.c Peer.c
TARGETC1 = client

.PHONY: all

all: c1

c1:
	$(CC) $(SOURCEC1) -o $(TARGETC1) -lpthread -lm -lssl -lcrypto

clean:
	-rm -f *.o
	#-rm -f *.txt
	-rm -f $(TARGETC)
