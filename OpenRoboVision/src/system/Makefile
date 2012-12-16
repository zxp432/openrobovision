#
# Makefile
#

CC		:= g++
TARGET	:= 

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

# путь до библиотеки
LIBPATH = ../../include

CFLAGS		:= -I. -I$(LIBPATH) -I$(LIBPATH)/system -DLINUX=1
LIBRARIES	:= -lpthread -lrt

.PHONY: all clean

all: $(OBJECTS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $< $(LIBRARIES)

clean:
	rm -f *.o $(TARGET)

