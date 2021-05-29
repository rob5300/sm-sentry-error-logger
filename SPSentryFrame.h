#ifndef SPSENTRYFRAME
#define SPSENTRYFRAME
#include <cstdint>

class SPSentryFrame
{
public:
	std::string filename;
	int32_t lineno;
	std::string function;
};
#endif