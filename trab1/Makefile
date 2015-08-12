CC=g++
CFLAGS= -c -Wall
EXEC=trab1
SRCS=tinyxml2.cpp main.cpp
OBJ=$(SRCS:.cpp=.o)

.PHONY: clean

all: $(SRCS) $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean: 
	rm *.o
	rm $(EXEC)