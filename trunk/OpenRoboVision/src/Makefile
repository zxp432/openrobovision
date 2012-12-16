#
# Makefile
#

.PHONY: all clean

all: makeall

makeall:
	make -C ./system
	make -C ./robo
	make -C ./vision
	
debug:
	make -C ./system debug
	make -C ./robo debug
	make -C ./vision debug

release:
	make -C ./system release
	make -C ./robo release
	make -C ./vision release

ocv21:
	make -C ./system ocv21
	make -C ./robo ocv21
	make -C ./vision ocv21
	
	
clean:
	make -C ./system clean
	make -C ./robo clean
	make -C ./vision clean
