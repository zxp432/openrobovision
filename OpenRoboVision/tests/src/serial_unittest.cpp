//
// OpenRoboVision
//
// test serial
//
// robocraft.ru
//

#include "gtest/gtest.h"

#include "orv/system/serial.h"
#include "orv/system/types.h"
#include "orv/system/times.h"

#if 1
TEST(Serial, WriteCOM) {

	Serial serial;

	serial.open("COM7", 9600);

	if(!serial.connected()){
		FAIL()<<"cant open serial port!";
	}

	char buf[1024];
	int res = 0;
	while(1){

		char wbuf[] = {0xff, 0xff, 0x00, 0x00, 0x00, 0x00 ,0xff};

		serial.write(wbuf, sizeof(wbuf));

		if( serial.waitInput(1000) > 0){
			if( (res = serial.available()) > 0 ){
				if( res = serial.read(buf, res) ){
					printf("[i] read data(%d): \n", res);
					for(int i=0; i<res; i++){
						printf("%02X ", (unsigned char)buf[i]);
						if(i>0 && (i+1)%16 == 0) {
							printf("\t");
							for(int j=i-15; j<=i; j++){
								printf("%c", buf[j]);
							}
							printf("\n");
						}
					}
					printf("\n");
					res = 0;
				}
			}
		}
		Sleep(1000);
	}
}
#endif
