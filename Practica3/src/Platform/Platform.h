#pragma once
#ifdef PLATFORM_PC
#include "PC\PlatformPC.h"
#elif PLATFORM_PS4
#include "PS4\PlatformPS4.h"
#else 
#error "Plataforma no encontrada"
#endif
