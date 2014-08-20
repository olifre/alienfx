#include <iostream>

#include "libalienfx/include/alienfx_device.h"

int main() {
	alienFx::cAlienfx_device alienfx(3);
	bool chipFound = alienfx.Init();
	if (!chipFound) {
		std::cerr << "Did not find a supported alienfx chip!" << std::endl;
		return 1;
	}
	return 0;
}
