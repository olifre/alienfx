#ifndef __alienfx_cmdparser_h__
#define __alienfx_cmdparser_h__

#include <string>

namespace alienFx {
	class cAlienfx_device;

	class cAlienfx_cmdparser {
	protected:
		cAlienfx_device* lDevice;
		int lVerbosity;
	
	public:
		cAlienfx_cmdparser(cAlienfx_device* aDevice, int verbosity);
		virtual ~cAlienfx_cmdparser();

		bool ExecuteCommandString(const std::string& commandString);
	};
};

#endif /* __alienfx_cmdparser_h__ */
