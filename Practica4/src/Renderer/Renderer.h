#pragma once
#ifdef PLATFORM_PC
#include "PC\RendererPC.h"
#elif PLATFORM_PS4
#include "PS4\RendererPS4.h"
#else 
#error "Plataforma no encontrada"
#endif