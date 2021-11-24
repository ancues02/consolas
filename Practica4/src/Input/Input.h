#pragma once
#ifdef PLATFORM_PC
#include "PC\InputPC.h"
#elif PLATFORM_PS4
#include "PS4\InputPS4.h"
#else 
#error "Plataforma no encontrada"
#endif
