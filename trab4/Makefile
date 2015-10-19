CC=g++
CFLAGS= -c -Wall -std=c++11
LFLAGS= -lGL -lglut
EXEC=trabalhocg
SRCS=tinyxml2.cpp objects.cpp main.cpp
OBJ=$(SRCS:.cpp=.o)

.PHONY: clean celan lcean

all: $(SRCS) $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean: 
	-rm *.o
	-rm $(EXEC)

lcean: clean

celan: clean