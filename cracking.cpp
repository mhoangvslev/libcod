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

int cracking_call_function(int func_address, char *args, unsigned char *data)
{
	int data_pos = 0;
	unsigned char stack[128];
	int stack_pos = 0;

	int mode_varargs = 0;
	int i;
	for (i=0; args[i]; i++)
	{
		if (args[i] == '.')
		{
			mode_varargs = 1;
			continue;
		}
		*(int *)(stack + stack_pos) = *(int*)(data + data_pos);

		if (args[i] == 'f' && mode_varargs)
		{
			double tmp_double = (double)*(float *)(data + data_pos);
			memcpy(stack + stack_pos, &tmp_double, 8);
		}
		if (args[i] == 'd')
		{
			double tmp_double = *(double *)(data + data_pos);
			memcpy(stack + stack_pos, &tmp_double, 8);
		}

		stack_pos += 4; // even 1-byte-chars will be 4-byte-aligned on stack
		if (args[i] == 'f' && mode_varargs)
			stack_pos += 4; // use 8 bytes for varargs/float, aka double
		if (args[i] == 'd')
			stack_pos += 4; // use 8 bytes for normal double

		switch (args[i])
		{
		case 'i':
		case 's':
		case 'f':
			data_pos += 4;
			break;
		case 'c':
			data_pos += 1;
			break;
		case 'd':
			data_pos += 8;
			break;
		}
	}

	// http://wiki.osdev.org/Inline_Assembly
	unsigned char *sp;
	asm("movl %%esp, %0" : "=g" (sp));

	memcpy(sp, stack, stack_pos);

	asm("movl %0, %%eax" : : "g" (func_address));
	asm("call *%eax");

	int ret;
	asm("movl %%eax, %0" : "=g" (ret));
	return ret;
}