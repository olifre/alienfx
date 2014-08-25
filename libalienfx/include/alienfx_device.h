#ifndef __alienfx_device_h__
#define __alienfx_device_h__

#include <stdio.h>
#include <cstdlib>
#include <string>
#include <string.h>
#include <iostream>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

#include "alienfx_public_constants.h"

//Include the zone presets & useful consts
//#include "presets.h"
//#include "consts.h"

namespace alienFx {

	struct tAlienfx_dev;

	class cAlienfx_device {
	protected:
		tAlienfx_dev* lDevice;

		int lVerbosity;
	
		libusb_context* lUsbContext;
		libusb_device_handle* lAlienFx;

		unsigned int lUsbSleep;

		bool CheckReady();

		int ReadDevice(unsigned char* pData, int pDataLength);
		int WriteDevice(unsigned char* pData, int pDataLength);

		void SendCommand(unsigned char cmd, unsigned char block=0x00, unsigned char data1=0x00, unsigned char data2=0x00, unsigned char data3=0x00,
		                 unsigned char data4=0x00, unsigned char data5=0x00, unsigned char data6=0x00);
		unsigned char GetStatus();
		void SetSpeed(unsigned int speed);
		void SetColour(unsigned char cmd, unsigned char idx, unsigned char zoneMask,
		               unsigned char r1=0x00, unsigned char g1=0x00, unsigned char b1=0x00,
		               unsigned char r2=0x00, unsigned char g2=0x00, unsigned char b2=0x00, bool checkReady=true);
		void RebootChip();
		void Reset(alienFx::alienFx_resetTypes resetType = alienFx::alienFx_resetTypes::ALL_LIGHTS_OFF);
		
		void UnInit();

	public:
		cAlienfx_device(int verbosity);
		bool Init();
		virtual ~cAlienfx_device();
	
	};
};

#endif /* __alienfx_device_h__ */
