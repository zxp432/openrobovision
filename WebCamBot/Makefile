#
# OpenRoboVision - webCamBot
#
#
# Author: 	Vladimir  aka noonv
# email: 	noonv13@gmail.com
# site:	robocraft.ru
#

CC		:= g++
TARGET	:= webCamBot

# объекты
OBJECTS := ./src/bot.o ./src/gimbal.o

# флаги компилятора
CFLAGS		:= -I. -I./include -DLINUX=1
# flags for debug
CFLAGSD		:= -I. -I./include -DLINUX=1 -O0 -g
# flags for release
CFLAGSR		:= -I. -I./include -DLINUX=1 -O3 -msse -msse2

LIBRARIES	:= -lpthread -lrt

# путь до библиотеки OpenRoboVision
ORVPATH := ../OpenRoboVision
ORVFLAGS 	:= -I$(ORVPATH)/include -L$(ORVPATH) 
#-Wl,-rpath,$(ORVPATH)
ORVLIBRARY 	:= -lorv

# for use OpenCV
OPENCVFLAGS		:= -DUSE_OPENCV=1 -I/usr/local/include/opencv -L/usr/local/lib -Wl,-rpath,/usr/local/lib
OPENCVLIBRARIES := -lopencv_core -lopencv_imgproc -lopencv_highgui
# -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_flann -lopencv_features2d

OPENCVFLAGS21	:= -DUSE_OPENCV=1 -I/usr/include/opencv -L/usr/lib -Wl,-rpath,/usr/lib
OPENCVLIBRARIES21 := -lcv -lcxcore -lhighgui
# -lcv -lcxcore -lhighgui -lml

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	make -C $(ORVPATH)
	$(CC) $(CFLAGS) $(ORVFLAGS) $(OPENCVFLAGS) -o $(TARGET) $(OBJECTS) ./src/main.cpp $(LIBRARIES) $(ORVLIBRARY) $(OPENCVLIBRARIES)

%.o: %.cpp
	$(CC) $(CFLAGS) $(ORVFLAGS) $(OPENCVFLAGS) -c -o $@ $< $(LIBRARIES) $(ORVLIBRARY) $(OPENCVLIBRARIES)

clean:
	rm -f *.o ./src/*.o $(TARGET)
	
rebuild:
	make clean
	make

cv21:
	make OPENCVFLAGS="$(OPENCVFLAGS21)" OPENCVLIBRARIES="$(OPENCVLIBRARIES21)"
