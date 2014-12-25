#ifndef __alienfx_utils_h__
#define __alienfx_utils_h__

#include <string>

namespace alienFx {
	class cAlienfx_device;

	class cAlienfx_utils {
	protected:
		cAlienfx_utils();
		virtual ~cAlienfx_utils();
		
	public:
		static bool ExecuteCommandString(cAlienfx_device* aDevice, const std::string& commandString, int aVerbosity=1);
	};
};

#endif /* __alienfx_utils_h__ */
