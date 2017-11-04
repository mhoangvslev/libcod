#ifndef _CRACKING_HPP_
#define _CRACKING_HPP_

/* gsc functions */
#include "gsc.hpp"

void cracking_hook_function(int from, int to);
void cracking_hook_call(int from, int to);

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
