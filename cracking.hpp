#ifndef _CRACKING_HPP_
#define _CRACKING_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

void cracking_hook_function(int from, int to);
void cracking_hook_call(int from, int to);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class cHook
{
public:
	int from;
	int to;
	unsigned char oldCode[5];
	cHook(int from, int to);
	void hook();
	void unhook();
};
#endif

#endif