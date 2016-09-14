#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

// that makes debugging easier after some months
#define DEBUG_MYSQL 0
#define DEBUG_GSC 0
#define DEBUG_MEMORY 0

// GSC MODULES
#define COMPILE_MYSQL 1
#define COMPILE_PLAYER 1
#define COMPILE_MEMORY 1
#define COMPILE_UTILS 1

// RATE LIMITER
#define COMPILE_RATELIMITER 1

// GLOBAL MACROS
#define STR(s) #s // stringify used for __FUNCTION__
#define INT(address) (*(int*)(address))

#endif
