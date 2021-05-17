#ifndef SPSENTRYFRAME
#define SPSENTRYFRAME
#include <cstdint>

class SPSentryFrame
{
public:
	const char* filename;
	int32_t lineno;
	const char* function;
};
#endif