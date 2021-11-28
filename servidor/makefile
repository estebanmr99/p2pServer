SOURCEC1 = main.c IPFinder.c Server.c Request.c
TARGETC1 = server

.PHONY: all

all: c1

c1:
	$(CC) $(SOURCEC1) -o $(TARGETC1) -lpthread -lm

clean:
	-rm -f *.o
	#-rm -f *.txt
	-rm -f $(TARGETC)