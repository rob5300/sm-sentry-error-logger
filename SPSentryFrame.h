#ifndef SPSENTRYFRAME
#define SPSENTRYFRAME
#include <cstdint>

/// <summary>
/// Holds information about a single frame, or function call in an error stack.
/// </summary>
class SPSentryFrame
{
public:
	std::string filename;
	int32_t lineno;
	std::string function;
};
#endif