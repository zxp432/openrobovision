#
# Makefile
#

CC		:= g++
TARGET	:= 

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

# путь до библиотеки
LIBPATH = ../../include

# флаги компилятора
CFLAGS		:= -I. -I$(LIBPATH) -I$(LIBPATH)/vision -DLINUX=1 -DUSE_V4L2_LIB=1
# -DUSE_V4L2_LIB=1
# flags for debug
CFLAGSD		:= -I. -I$(LIBPATH) -I$(LIBPATH)/vision -DLINUX=1 -DUSE_V4L2_LIB=1 -O0 -g
# flags for release
CFLAGSR		:= -I. -I$(LIBPATH) -I$(LIBPATH)/vision -DLINUX=1 -DUSE_V4L2_LIB=1 -O3 -msse -msse2

LIBRARIES	:= 

# for use OpenCV
OPENCVFLAGS		:= -DUSE_OPENCV=1 -I/usr/local/include/opencv -L/usr/local/lib -Wl,-rpath,/usr/local/lib
OPENCVLIBRARIES := -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_video
# -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_flann -lopencv_features2d

OPENCVFLAGS21	:= -DUSE_OPENCV=1 -I/usr/include/opencv -L/usr/lib -Wl,-rpath,/usr/lib
OPENCVLIBRARIES21 := -lcv -lcxcore -lhighgui
# -lcv -lcxcore -lhighgui -lml

.PHONY: all clean

all: $(OBJECTS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(OPENCVFLAGS) -c -o $@ $< $(LIBRARIES) $(OPENCVLIBRARIES)

clean:
	rm -f *.o $(TARGET)

cv21:
	make OPENCVFLAGS="$(OPENCVFLAGS21)" OPENCVLIBRARIES="$(OPENCVLIBRARIES21)"
