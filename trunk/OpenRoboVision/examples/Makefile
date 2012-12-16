#
# Makefile
#

CC		:= g++

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

TARGET	:= $(patsubst %.o, %, $(OBJECTS)) 

# путь до библиотеки
LIBPATH := ..

CFLAGS		:= -I. -I$(LIBPATH) -DLINUX=1
LIBRARIES	:= -lpthread -lrt

# путь до библиотеки OpenRoboVision
ORVPATH := ..
ORVFLAGS 	:= -I$(ORVPATH)/include -L$(ORVPATH) 
#-Wl,-rpath,$(ORVPATH)
ORVLIBRARY 	:= -lorv

# for use OpenCV
OPENCVFLAGS		:= -DUSE_OPENCV=1 -I/usr/local/include/opencv -L/usr/local/lib -Wl,-rpath,/usr/local/lib
OPENCVLIBRARIES := -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_video -lopencv_calib3d
# -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_flann -lopencv_features2d

OPENCVFLAGS21	:= -DUSE_OPENCV=1 -I/usr/include/opencv -L/usr/lib -Wl,-rpath,/usr/lib
OPENCVLIBRARIES21 := -lcv -lcxcore -lhighgui
# -lcv -lcxcore -lhighgui -lml

.PHONY: all clean

all: ORV $(OBJECTS)

ORV:
	make -C $(ORVPATH)

%.o: %.cpp
	$(CC) $(CFLAGS) $(ORVFLAGS) $(OPENCVFLAGS) -o $(patsubst %.o, %, $@) $< $(ORVLIBRARY) $(LIBRARIES) $(OPENCVLIBRARIES)
	
clean:
	rm -f *.o *.exe $(TARGET)

rebuild:
	make clean
	make
