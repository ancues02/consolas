#pragma once
#include <cstdint>

#define FLIPENDIAN_32(n) (((n << 24) & 0xff000000) | ((n << 8) & 0x00ff0000) | ((n >> 8) & 0x0000ff00) | ((n >> 24) & 0x000000ff))
#define FLIPENDIAN_16(n) (((n << 8) & 0xff00) | ((n >> 8) & 0x00ff))
#define ABGR_ARGB(n) ((n & 0xff000000) | ((n << 16) & 0x00ff0000) | (n & 0x0000ff00) | ((n >> 16) & 0x000000ff))
#define DEG_RAD(n) (n*3.1415 /180)

// Estructura para color
struct Color {
	uint8_t a, r, g, b;
 };

// Metodo para pasar a un color que se usa en PS4
static uint32_t bitshiftToUint32(const Color& col) {
	return (col.a << 24) | (col.r << 16) | (col.g << 8) | (col.b << 0);
}


