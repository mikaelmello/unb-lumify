CPP      = g++
CC       = gcc
OBJ      = client.o server.o ../src/sockets.o
LINKOBJ  = server.o client.o ../src/sockets.o
LIBS     = 
INCS     = 
CXXINCS  = 
BIN      = lumify
CXXFLAGS = $(CXXINCS) -std=c++11 -g3 -ggdb3
CFLAGS   = $(INCS) -std=c11 -ggdb3
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom client server

all: clean client server

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

clean: clean-custom
	${RM} $(OBJ) client server

client: client.o ../src/sockets.o
	$(CPP) client.o ../src/sockets.o -o client $(LIBS)

server: server.o ../src/sockets.o
	$(CPP) server.o ../src/sockets.o -o server $(LIBS)

client.o:
	$(CPP) -c client.cpp -o client.o $(CXXFLAGS)

server.o:
	$(CPP) -c server.cpp -o server.o $(CXXFLAGS)

src/main.o: src/main.cpp
	$(CPP) -c src/main.cpp -o src/main.o $(CXXFLAGS)

../src/sockets.o: ../src/sockets.cpp
	$(CPP) -c ../src/sockets.cpp -o ../src/sockets.o $(CXXFLAGS)