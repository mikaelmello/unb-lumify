CPP      = g++
CC       = gcc
OBJ      = client.o server.o clientudp.o serverudp.o ../src/sockets.o
LINKOBJ  = server.o client.o clientudp.o serverudp.o ../src/sockets.o
LIBS     = 
INCS     = 
CXXINCS  = 
BIN      = lumify
CXXFLAGS = $(CXXINCS) -std=c++11 -g3 -ggdb3
CFLAGS   = $(INCS) -std=c11 -ggdb3
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom client server

all: clean client server clientudp serverudp

tcp: clean client server

udp: clean clientudp serverudp

clean: clean-custom
	${RM} $(OBJ) client server clientudp serverudp

client: client.o ../src/sockets.o
	$(CPP) client.o ../src/sockets.o -o client $(LIBS)

server: server.o ../src/sockets.o
	$(CPP) server.o ../src/sockets.o -o server $(LIBS)

client.o:
	$(CPP) -c client.cpp -o client.o $(CXXFLAGS)

server.o:
	$(CPP) -c server.cpp -o server.o $(CXXFLAGS)

clientudp: clientudp.o ../src/sockets.o
	$(CPP) clientudp.o ../src/sockets.o -o clientudp $(LIBS)

serverudp: serverudp.o ../src/sockets.o
	$(CPP) serverudp.o ../src/sockets.o -o serverudp $(LIBS)

clientudp.o:
	$(CPP) -c clientudp.cpp -o clientudp.o $(CXXFLAGS)

serverudp.o:
	$(CPP) -c serverudp.cpp -o serverudp.o $(CXXFLAGS)

../src/sockets.o: ../src/sockets.cpp
	$(CPP) -c ../src/sockets.cpp -o ../src/sockets.o $(CXXFLAGS)