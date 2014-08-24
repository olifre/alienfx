#ifndef __alienfx_device_h__
#define __alienfx_device_h__

#include <stdio.h>
#include <cstdlib>
#include <string>
#include <string.h>
#include <iostream>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

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

		void afx_set(unsigned char cmd, unsigned char idx, unsigned char zone, unsigned char r1, unsigned char g1,
		             unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2, bool chk);
		void afx_cmd(unsigned char cmd, unsigned char block, unsigned char data1, unsigned char data2, unsigned char data3,
		             unsigned char data4, unsigned char data5, unsigned char data6);
		int afx_get();
		void afx_reset();
		void afx_reboot();
		void afx_spd(int speed);
		//bool afx_raw(unsigned char cmd[DATA_LENGTH]);
		bool afx_process(char *cmd);

		void UnInit();

	public:
		cAlienfx_device(int verbosity);
		bool Init();
		virtual ~cAlienfx_device();
	
	};
};

#endif /* __alienfx_device_h__ */
