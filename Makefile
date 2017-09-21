CPP      = g++
CC       = gcc
OBJ      = src/main.o src/sockets.o
LINKOBJ  = src/main.o src/sockets.o
LIBS     = 
INCS     = 
CXXINCS  = 
BIN      = lumify
CXXFLAGS = $(CXXINCS) -std=c++11 -g3 -ggdb3
CFLAGS   = $(INCS) -std=c11 -ggdb3
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

src/main.o: src/main.cpp
	$(CPP) -c src/main.cpp -o src/main.o $(CXXFLAGS)

src/sockets.o: src/sockets.cpp
	$(CPP) -c src/sockets.cpp -o src/sockets.o $(CXXFLAGS)