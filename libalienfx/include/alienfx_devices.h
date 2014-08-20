#ifndef __alienfx_devices_h__
#define __alienfx_devices_h__

#include <vector>

namespace alienFx {
	struct tAlienfx_dev {
		const char* devName;
		const int VID;
		const int PID;
		const int START_BYTE;
	};

	static const std::vector<tAlienfx_dev> gAlienfx_devices =
		{
			{"M17RX5",              0x187c, 0x524, 0x02},
			{"AURORA_NONALX",       0x187c, 0x513, 0x02},
			{"M14XR2",              0x187c, 0x521, 0x02},
			{"ALLPOWERFULL_M11XR3", 0x187c, 0x522, 0x02},
			{"ALLPOWERFULL_M11X",   0x187c, 0x514, 0x02},
			{"ALLPOWERFULL",        0x187c, 0x512, 0x02},
			{"AREA51",              0x187c, 0x511, 0x00}
		};
};

#endif /* __alienfx_devices_h__ */






