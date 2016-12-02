CC=g++
CFLAGS= -c -Wall -std=c++11
LFLAGS= -lGL -lglut
EXEC=trabalhocg
SRCS=tinyxml2.cpp imageloader.cpp botia.cpp objects.cpp main.cpp
OBJ=$(SRCS:.cpp=.o)

.PHONY: clean

all: $(SRCS) $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean: 
	-rm *.o
	-rm $(EXEC)

celan: clean
