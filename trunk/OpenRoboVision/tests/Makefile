#
# Makefile
#
# http://www.gnu.org/software/make/manual/make.html
#

CC		:= g++
TARGET	:= tests

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard ./src/*_unittest.cpp))

# флаги компилятора
CFLAGS		:= -I. -DLINUX=1 

LIBRARIES	:= -lpthread -lrt

# путь до библиотеки OpenRoboVision
ORVPATH := ..
ORVFLAGS 	:= -I$(ORVPATH)/include -L$(ORVPATH) 
#-Wl,-rpath,$(ORVPATH)
ORVLIBRARY 	:= -lorv

# GTEST
GTESTPATH := $(ORVPATH)/3rdparty
GTESTFLAGS := -I$(GTESTPATH)
GTESTOBJ := gtest-all.o

# for use OpenCV
OPENCVFLAGS		:= -DUSE_OPENCV=1 -I/usr/local/include/opencv -L/usr/local/lib -Wl,-rpath,/usr/local/lib
OPENCVLIBRARIES := -lopencv_core -lopencv_imgproc -lopencv_highgui
# -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_flann -lopencv_features2d

OPENCVFLAGS21	:= -DUSE_OPENCV=1 -I/usr/include/opencv -L/usr/lib -Wl,-rpath,/usr/lib
OPENCVLIBRARIES21 := -lcv -lcxcore -lhighgui
# -lcv -lcxcore -lhighgui -lml

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) $(GTESTOBJ)
	make -C $(ORVPATH)
	$(CC) $(CFLAGS) $(ORVFLAGS) $(OPENCVFLAGS) $(GTESTFLAGS) -o $(TARGET) $(OBJECTS) $(GTESTOBJ) ./src/main.cpp $(LIBRARIES) $(ORVLIBRARY) $(OPENCVLIBRARIES)

%.o: %.cpp
	$(CC) $(CFLAGS) $(ORVFLAGS) $(OPENCVFLAGS) $(GTESTFLAGS) -c -o $@ $< $(LIBRARIES) $(ORVLIBRARY) $(OPENCVLIBRARIES)

gtest-all.o:
	$(CC) $(CFLAGS) $(GTESTFLAGS) -c -o gtest-all.o $(GTESTPATH)/gtest/gtest-all.cc $(LIBRARIES)
	
clean:
	rm -f ./*.o ./src/*.o $(TARGET)
	
rebuild:
	make clean
	make

cv21:
	make OPENCVFLAGS="$(OPENCVFLAGS21)" OPENCVLIBRARIES="$(OPENCVLIBRARIES21)"
