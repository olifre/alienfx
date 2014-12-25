#include <iostream>
#include <iomanip>
#include <getopt.h>

#include "alienfxdev/include/alienfx_device.h"
#include "alienfxdev/include/alienfx_commands.h"

#include "alienfxdev/include/alienfx_utils.h"

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

void printHelp(std::string& execName) {
	std::cout << std::setw(20) << " " << " alienfx commandline utility" << std::setw(20) << " " << std::endl;
	std::cout << " " << execName.c_str() << " [options]" << std::endl;
	std::cout << std::setw(20) << "--reboot   " << "  " << "Reboot light-chip." << std::endl;
	std::cout << std::setw(20) << "--lightson " << "  " << "Reset chip, execute lights-on." << std::endl;
	std::cout << std::setw(20) << "--lightsoff" << "  " << "Reset chip, execute lights-off." << std::endl;
	std::cout << std::setw(20) << "-c <string>" << "  " << "Execute given commandstring." << std::endl;
	std::cout << std::setw(20) << "-v         " << "  " << "Increase verbosity (may be specified several times)." << std::endl;
	std::cout << std::setw(20) << "-q         " << "  " << "Decrease verbosity (may be specified several times)." << std::endl;
	std::cout << std::setw(20) << "-h, --help " << "  " << "Show usage information." << std::endl;
}

namespace longParameters {
	enum _longParameters {
		NONE,
		HELP
	};
}

int main(int argc, char **argv) {
	int rebootChip = false;
	int lightsOn  = false;
	int lightsOff = false;

	std::string commandString("");
	bool execCommandString = false;
	
	unsigned int verbosity = 1;
	{
		// *********************
		// * PARAMETER PARSING *
		// *********************

		// We assume we are running on an OS which is useful:
		std::string execName(argv[0]);

		char *endptr;

		const struct option long_options[] = {
			{"reboot",        no_argument,         &rebootChip, true},
			{"lightson",      no_argument,         &lightsOn,   true},
			{"lightsoff",     no_argument,         &lightsOff,  true},
			{"help",          no_argument,         NULL,        longParameters::HELP},
			{0,               0,                   0,           longParameters::NONE}
		};
		if (argc > 1) {
			int opt = 0;
			int option_index = 0;
			while ((opt = getopt_long(argc, argv, "hc:qv", long_options, &option_index)) != -1) {
				switch (opt) {
				case 0:
					// long option which sets a var, getopt_long does this by itself.
					break;
				case 'h':
				case longParameters::HELP:
					printHelp(execName);
					exit(0);
				case 'q':
					if (verbosity != 0) {
						verbosity--;
					}
					break;
				case 'v':
					verbosity++;
					break;
				case 'c':
					//FIXME: Validate.
					commandString = optarg;
					execCommandString = true;
					break;
				default:
					printHelp(execName);
					exit(1);
				}
			}
		} else {
			printHelp(execName);
			exit(0);			
		}
	}
	alienFx::cAlienfx_device alienfx(verbosity);
	bool chipFound = alienfx.Init();
	if (!chipFound) {
		std::cerr << "Did not find a supported alienfx chip!" << std::endl;
		return 1;
	}

	if (rebootChip == true) {
		alienfx.RebootChip();
	}

	if (lightsOn == true) {
		if (verbosity > 1) {
			std::cout << "Turning lights on..." << std::endl;
		}
		alienfx.Reset(alienFx::alienFx_resetTypes::ALL_LIGHTS_ON);
	}

	if (lightsOff == true) {
		if (verbosity > 1) {
			std::cout << "Turning lights off..." << std::endl;
		}
		alienfx.Reset(alienFx::alienFx_resetTypes::ALL_LIGHTS_OFF);
	}

	if (execCommandString) {
		alienFx::cAlienfx_utils::ExecuteCommandString(&alienfx, commandString, verbosity);
	}
	//std::cout << "Turning lights off..." << std::endl;
	//alienfx.Reset(alienFx::alienFx_resetTypes::ALL_LIGHTS_OFF);
	//alienfx.RebootChip();
	//sleep(3);
	//std::cout << "Turning lights on..." << std::endl;
	//alienfx.Reset(alienFx::alienFx_resetTypes::ALL_LIGHTS_ON);

	//zoneScan(alienfx);

	//alienFx::cAlienfx_cmdparser alienfx_parser(&alienfx, verbosity);
	//alienfx.SetSpeed(3000);
	//alienfx_parser.ExecuteCommandString("Z06Z07Z09MF00FF0MFF00F0M0F00FFM0FF00FM00FF0FMF0FF00");
	//alienfx_parser.ExecuteCommandString("Z00MF00FF0MFF00F0M0F00FFM0FF00FM00FF0FMF0FF00");
	//alienfx_parser.ExecuteCommandString("Z06Z07Z09F222F666");
	//alienfx_parser.ExecuteCommandString("Z04Z06Z07Z08Z09Z10Z12Z14F111Z01Z02Z03Ffff");
	//alienfx_parser.ExecuteCommandString("Z12B0F0");
	//alienfx_parser.ExecuteCommandString("Z00BFFF");	
	/*
	alienfx.SetSpeed(3);
	sleep(3);
	alienfx.SetSpeed(30);
	sleep(3);
	alienfx.SetSpeed(300);
	sleep(3);
	*/
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
	//sleep(2);
	//alienfx.RebootChip();
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
