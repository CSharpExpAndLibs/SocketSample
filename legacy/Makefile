CC = gcc
CFLAGS = -g3

TARGETS = TcpSockServer TcpSockClient DomainSockServer

all: $(TARGETS)

TcpSockServer: TcpSockServer.o
	$(CC) $(CFLAGS) -o $@ $^

TcpSockClient: TcpSockClient.o
	$(CC) $(CFLAGS) -o $@ $^

DomainSockServer: DomainSockServer.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *~ *.o $(TARGETS)
