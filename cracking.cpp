#include "cracking.hpp"

void cracking_hook_function(int from, int to)
{
	int relative = to - (from+5); // +5 is the position of next opcode
	memset((void *)from, 0xE9, 1); // JMP-OPCODE
	memcpy((void *)(from+1), &relative, 4); // set relative address with endian
}

void cracking_hook_call(int from, int to)
{
	int relative = to - (from+5); // +5 is the position of next opcode
	memcpy((void *)(from+1), &relative, 4); // set relative address with endian
}

cHook::cHook(int from, int to)
{
	this->from = from;
	this->to = to;
}

void cHook::hook()
{
	memcpy((void *)oldCode, (void *)from, 5);
	cracking_hook_function(from, to);
}

void cHook::unhook()
{
	memcpy((void *)from, (void *)oldCode, 5);
}
