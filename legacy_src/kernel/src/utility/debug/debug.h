#pragma once
#include "utility/print/print.h"

#ifdef DEBUG

#define IF_DEBUG(code) code
#define DEBUG_LOG(str) printf(str)

#else 

#define IF_DEBUG(code) 
#define DEBUG_LOG(str)

#endif