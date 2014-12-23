#include <stdio.h>
#include <cstdlib>
#include <string>
#include <string.h>
#include <iostream>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

#include "alienfx_cmdparser.h"

#include "alienfx_device.h"
#include "alienfx_commands.h"

alienFx::cAlienfx_cmdparser::cAlienfx_cmdparser(cAlienfx_device* aDevice, int verbosity) :
	lDevice(aDevice),
	lVerbosity(verbosity)
{
	
}

alienFx::cAlienfx_cmdparser::~cAlienfx_cmdparser() {
	
}

bool alienFx::cAlienfx_cmdparser::ExecuteCommandString(const std::string& commandString) {
	uint16_t loopBlock = 0x00;
	bool settingZones = false;
	uint32_t zoneMask = 0x00000000;
	unsigned char r1 = 0x00;
	unsigned char g1 = 0x00;
	unsigned char b1 = 0x00;
	unsigned char r2 = 0x00;
	unsigned char g2 = 0x00;
	unsigned char b2 = 0x00;
	lDevice->Reset(alienFx::alienFx_resetTypes::ALL_LIGHTS_ON);
	for(std::string::size_type i = 0; i < commandString.size(); ++i) {
		switch (commandString[i]) {
		case 'z':
		case 'Z':
			{
				unsigned int zone = std::strtol(commandString.substr(i+1, 2).c_str(), NULL, 10);
				i += 2;
				if (!settingZones) {
					settingZones = true;
					zoneMask = 0x00000000;
					if (loopBlock > 0) {
						// Mark block as finished. 
						if (lVerbosity > 1) {
							std::cout << "Ending loop block " << loopBlock << ", reset zone-mask." << std::endl;
						}
						lDevice->EndLoopBlock();
					}
					loopBlock++;
				}
				if (zone > 0) {
					zoneMask |= (1 << (zone-1));
				} else {
					zoneMask = 0xFFFFFFFF;
				}
				if (lVerbosity > 1) {
					printf("Add zone: %2d Mask: 0x%08X\r\n", zone, zoneMask);
				}
			}
			break;
		case 'm':
		case 'M':
			{
				settingZones = false;
				// MORPHING
				i++;
				r1 = std::strtol(commandString.substr(i++, 1).c_str(), NULL, 16);
				g1 = std::strtol(commandString.substr(i++, 1).c_str(), NULL, 16);
				b1 = std::strtol(commandString.substr(i++, 1).c_str(), NULL, 16);
				r2 = std::strtol(commandString.substr(i++, 1).c_str(), NULL, 16);
				g2 = std::strtol(commandString.substr(i++, 1).c_str(), NULL, 16);
				b2 = std::strtol(commandString.substr(i++, 1).c_str(), NULL, 16);
				if (lVerbosity > 1) {
					printf("[%d] Morph: From 0x%X%X%X to 0x%X%X%X.\r\n", loopBlock, r1, g1, b1, r2, g2, b2);
				}
				lDevice->SetColour(alienFx::alienFx_commands::SET_COLOUR_MORPH, loopBlock,
				                   zoneMask,
				                   r1, g1, b1, r2, g2, b2, false);
				//sleep(2);
				i--;
			}
			break;
		case 'f':
		case 'F':
			{
				settingZones = false;
				// FIXED
				i++;
				r1 = std::strtol(commandString.substr(i++, 1).c_str(), NULL, 16);
				g1 = std::strtol(commandString.substr(i++, 1).c_str(), NULL, 16);
				b1 = std::strtol(commandString.substr(i++, 1).c_str(), NULL, 16);
				r2 = 0x00;
				g2 = 0x00;
				b2 = 0x00;
				if (lVerbosity > 1) {
					printf("[%d] Fixed: Colour 0x%X%X%X.\r\n", loopBlock, r1, g1, b1);
				}
				lDevice->SetColour(alienFx::alienFx_commands::SET_COLOUR_FIXED, loopBlock,
				                   zoneMask,
				                   r1, g1, b1, r2, g2, b2, false);
				//sleep(2);
				i--;
			}
			break;
		case 'b':
		case 'B':
			{
				settingZones = false;
				// BLINK
				i++;
				r1 = std::strtol(commandString.substr(i++, 1).c_str(), NULL, 16);
				g1 = std::strtol(commandString.substr(i++, 1).c_str(), NULL, 16);
				b1 = std::strtol(commandString.substr(i++, 1).c_str(), NULL, 16);
				r2 = 0x00;
				g2 = 0x00;
				b2 = 0x00;
				if (lVerbosity > 1) {
					printf("[%d] Blink: Colour 0x%X%X%X.\r\n", loopBlock, r1, g1, b1);
				}
				lDevice->SetColour(alienFx::alienFx_commands::SET_COLOUR_BLINK, loopBlock,
				                   zoneMask,
				                   r1, g1, b1, r2, g2, b2, false);
				//sleep(2);
				i--;
			}
			break;
		default:
			settingZones = false;
			std::cout << commandString[i] << std::endl;
		}
		//std::cout << commandString[i] << std::endl;
	}
	lDevice->EndLoopBlock();
	lDevice->TransmitExecute();
}
