#include <stdio.h>
#include <cstdlib>
#include <string>
#include <string.h>
#include <iostream>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

#include "alienfx_device.h"

#include "alienfx_devices.h"

#include "alienfx_constants.h"
#include "alienfx_states.h"
#include "alienfx_commands.h"

//Include the zone presets & useful consts
//#include "presets.h"
//#include "consts.h"

alienFx::cAlienfx_device::cAlienfx_device(int verbosity) :
	lDevice(NULL),
	lVerbosity(verbosity),
	lUsbContext(NULL),
	lUsbSleep(137500) //sdk recommends 250 msec
{
	
}

alienFx::cAlienfx_device::~cAlienfx_device() {
	UnInit();
}

void alienFx::cAlienfx_device::UnInit() {
	if (lAlienFx != NULL) {
		libusb_release_interface(lAlienFx, 0);
		libusb_attach_kernel_driver(lAlienFx, 0);
	  libusb_close(lAlienFx);
	}
	if (lUsbContext != NULL) {
		libusb_exit(lUsbContext);
	}
}

bool alienFx::cAlienfx_device::Init() {
	if (lUsbContext != NULL) {
		std::cerr << "Init() called more than once, fix this!" << std::endl;
		exit(1);
	}
	libusb_init(&lUsbContext);
	for (auto fxDevice : gAlienfx_devices) {
		lDevice = &fxDevice;
		if (lVerbosity > 1) {
			std::cout << "Looking for " << lDevice->devName << "... ";
		}
		lAlienFx = libusb_open_device_with_vid_pid(lUsbContext, lDevice->VID, lDevice->PID);
		if (lAlienFx == NULL && (lVerbosity > 1)) {
			std::cout << "not found." << std::endl;
		}
		if (lAlienFx != NULL) {
			if (lVerbosity > 0) {
				std::cout << " found!" << std::endl;
			}
			// Steal device: 
			int res = libusb_kernel_driver_active(lAlienFx, 0);
			if (res == 1) {
				if (lVerbosity > 1) {
					std::cout << "Another kernel driver is active, detaching it... ";
				}
				res = libusb_detach_kernel_driver(lAlienFx, 0);
				if (res == 0) {
					if (lVerbosity > 1) {
						std::cout << "success!" << std::endl;
						std::cout << "Claiming interface... " << std::endl;
					}
					res = libusb_claim_interface(lAlienFx, 0);
					if (res == 0) {
						if (lVerbosity > 1) {
							std::cout << "success!" << std::endl;
						}
						if (lVerbosity > 0) {
							std::cout << "Found " << lDevice->devName << ", using it." << std::endl;
						}
						break;
					} else {
						std::cerr << "Could not claim interface!" << std::endl;	
						UnInit();
						exit(1);
					}
				}
				if (res == LIBUSB_ERROR_NOT_FOUND) {
					std::cerr << "Failed to get USB device! Are you running two instances of this tool?" << std::endl;
					UnInit();
					exit(1);
				}
			}
		}
	}
	if (lAlienFx == NULL)	 {
		return false;
	} else {
		return true;
	}
}

int alienFx::cAlienfx_device::ReadDevice(unsigned char* pData, int pDataLength) {
	if (lVerbosity > 1) {
		std::cout << "Debug: USB Read" << std::endl;
		for (int i=0; i<pDataLength; i++) {
			printf("[%i]", pData[i]);
		}
		std::cout << std::endl;
	}
	int v = libusb_control_transfer(lAlienFx,
	                                LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
	                                0x0001,
	                                0x0101,
	                                0x0000,
	                                pData, pDataLength, 0);
	return v;
}

int alienFx::cAlienfx_device::WriteDevice(unsigned char* pData, int pDataLength) {
	if (lVerbosity > 1) {
		std::cout << "Debug: USB Write" << std::endl;
		for (int i=0; i<pDataLength; i++) {
			printf("[%i]", pData[i]);
		}
		std::cout << std::endl;
	}
	int v = libusb_control_transfer(lAlienFx,
	                                LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
	                                0x0009,
	                                0x0202,
	                                0x0000,
	                                pData, pDataLength, 0);
	
	usleep(lUsbSleep);
	
	memset(pData,0,pDataLength);
	ReadDevice(pData,pDataLength);
	return v;
}

bool alienFx::cAlienfx_device::CheckReady() {
	bool ready = false;
	unsigned int crashcheck = 0;
	unsigned char thedata[alienFx::DATA_LENGTH];
	unsigned char check[alienFx::DATA_LENGTH];
	while (!ready) {
		memset(thedata, alienFx::FILL_BYTE, alienFx::DATA_LENGTH);
		thedata[0] = lDevice->START_BYTE;
		thedata[1] = alienFx_commands::GET_STATUS;
		memcpy(check, thedata, alienFx::DATA_LENGTH);
		WriteDevice(thedata,  alienFx::DATA_LENGTH);
		ready = memcmp(check, thedata, DATA_LENGTH);
		if (lVerbosity > 1) {
			if (ready) {
				std::cout << "Debug: Lightchip now ready." << std::endl;
			}
			if (thedata[0] == alienFx_states::BUSY) {
				std::cout << "Debug: Lightchip is busy." << std::endl;
			}
			if (thedata[0] == alienFx_states::UNKNOWN_COMMAND) {
				std::cout << "Debug: Lightchip claims 'unknown command'." << std::endl;
			}
		}
		if (crashcheck == 4) {
			std::cerr << "Warning: The Lightchip is not responding!" << std::endl;
		}
		if (crashcheck == 12) {
			std::cerr << "Error: Lightchip failed to respond." << std::endl;
			UnInit();
			exit(1);
		}
		if (!ready) {
			usleep(lUsbSleep);
		}
		crashcheck++;
	}
	return ready;
}
