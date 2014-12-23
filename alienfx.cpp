#include <iostream>

#include "libalienfx/include/alienfx_device.h"
#include "libalienfx/include/alienfx_commands.h"

void zoneScan(alienFx::cAlienfx_device& alienfx, int maxZone=32) {
	for (int zone=0; zone<maxZone; zone++) {
		std::cout << "*********************************" << std::endl;
		std::cout << "Scanning zone " << zone << std::endl;
		alienfx.SetColour(alienFx::alienFx_commands::SET_COLOUR_BLINK, 0,
		                  (1<<zone),
		                  255, 0, 0);
		sleep(3);
		alienfx.Reset(alienFx::alienFx_resetTypes::ALL_LIGHTS_OFF);
		std::cout << "*********************************" << std::endl;
	}
}

int main() {
	alienFx::cAlienfx_device alienfx(3);
	bool chipFound = alienfx.Init();
	if (!chipFound) {
		std::cerr << "Did not find a supported alienfx chip!" << std::endl;
		return 1;
	}
	std::cout << "Turning lights off..." << std::endl;
	alienfx.Reset(alienFx::alienFx_resetTypes::ALL_LIGHTS_OFF);
	//alienfx.RebootChip();
	sleep(1);
	std::cout << "Turning lights on..." << std::endl;
	alienfx.Reset(alienFx::alienFx_resetTypes::ALL_LIGHTS_ON);
	zoneScan(alienfx);
	/*
	alienfx.SetColour(alienFx::alienFx_commands::SET_COLOUR_FIXED, 0,
	                  (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4),
	                  0, 0, 255);
	*/
	/*
	alienfx.SetColour(alienFx::alienFx_commands::SET_COLOUR_FIXED, 0,
	                  0xFFFFFFFF,
	                  0, 255, 0);
	sleep(2);
	alienfx.Reset(alienFx::alienFx_resetTypes::ALL_LIGHTS_OFF);
	alienfx.SetColour(alienFx::alienFx_commands::SET_COLOUR_FIXED, 0,
	                  0xFFFFFFFF,
	                  255, 0, 0);
	*/
	sleep(2);
	alienfx.RebootChip();
	return 0;
}

// bit 1 keyboard left
// bit 2 keyboard center
// bit 3 keyboard right
// bit 4 numpad

// bit 6 left outer light
// bit 7 right outer light
// bit 8 alien + strips on lid
// bit 9 alienware logo under screen
// bit 10 touchpad

// bit 12 status-leds

// bit 14 Power-LED ?
