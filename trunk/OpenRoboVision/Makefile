#
# OpenRoboVision
#
#
# Author: 	Vladimir  aka noonv
# email: 	noonv13@gmail.com
# site:	robocraft.ru
#

AR 		:= ar
CC		:= g++
TARGET	:= liborv

# путь до библиотеки
LIBPATH = 

# объекты
ORVSYSTEMOBJ	:= $(patsubst %.cpp, %.o, $(wildcard ./src/system/*.cpp))
ORVROBOOBJ	:= $(patsubst %.cpp, %.o, $(wildcard ./src/robo/*.cpp))
ORVVISIONOBJ	:= $(patsubst %.cpp, %.o, $(wildcard ./src/vision/*.cpp))

OBJECTS 	:= $(ORVSYSTEMOBJ) $(ORVROBOOBJ) $(ORVVISIONOBJ)

LIBRARIES	:= 


.PHONY: all clean

all: $(TARGET)

$(TARGET):
	make -C ./src
	$(AR) cr $(TARGET).a $(OBJECTS)
	
shared:	
	make -C ./src
	$(CC) -shared -fPIC -o $(TARGET).so $(OBJECTS)

clean:
	rm -f *.o $(TARGET)
	make -C ./src clean

rebuild:
	make clean
	make

objects: $(OBJECTS)
	
debug: $(OBJECTS)
	make objects CFLAGS="$(CFLAGSD)"

release: $(OBJECTS)
	make objects CFLAGS="$(CFLAGSR)"

cv21: $(OBJECTS)
	make objects OPENCVFLAGS="$(OPENCVFLAGS21)" OPENCVLIBRARIES="$(OPENCVLIBRARIES21)"

