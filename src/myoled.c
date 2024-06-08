#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "myoled.h"
#include "oled.h"
#include "font.h"

static struct display_info disp;

int oled_demo(unsigned char *buf) {
	
	unsigned char *buffer = buf;	
#if 0
	oled_putstrto(&disp, 0, 9+1, "this garbage sort is :");
	disp.font = font1;
	switch(buffer[2]){
		case 0x42:
		oled_putstrto(&disp, 20, 20, "dry waste");
		break;
		case 0x41:	
		oled_putstrto(&disp, 20, 20, "wet waste");
		break;
		case 0x40:
		oled_putstrto(&disp, 20, 18+2, "recyclable waste");
		break;
		case 0x3f:
		oled_putstrto(&disp, 20, 18+2, "hazardous waste");
		break;
		case 0x3e:
		oled_putstrto(&disp, 20, 18+2, "recognizable failed");
		break;
		default:
		break;
	}
#endif	
	disp.font = font3;
	oled_send_buffer(&disp);
	return 0;
}

void show_error(int err, int add) {
	//const gchar* errmsg;
	//errmsg = g_strerror(errno);
	printf("\nERROR: %i, %i\n\n", err, add);
	//printf("\nERROR\n");
}

void myoled_deinit() {
	oled_close(&disp);
//	free(disp);
}

void myoled_init() {
	int e;
//	disp = (struct display_info *)malloc(sizeof(struct display_info));
	memset(&disp, 0, sizeof(disp));
	disp.address = OLED_I2C_ADDR;

	e = oled_open(&disp, "/dev/i2c-3");
	disp.font = font2;

	if (e < 0) {
		show_error(1, e);
	} else {
		e = oled_init(&disp);
	}
}
