#pragma once
#include <cstdint>

//TODO probar a quitar el include y poner class Image
#include "../Renderer/Renderer.h"
#include "../Utils.h"


enum class RenderCommandType {
    DRAW_RECT,
    DRAW_TEXTURE,
    PRESENT_FRAME,
    RAY_LINES
};

// Comando para dibujar un rectangulo de un color
struct DrawRectInfo {
    Color color;
    int x1;
    int x2;
    int y1;
    int y2;
};

// Informacion para dibujar una columna de una textura,
// No lo usamos porque para mejorar el rendimiento pasamos
// directamente toda la informacion en la estructura RayLines
// que se usa en el comando RAY_LINES, que tendrá el mismo nº
// de RayLines como el ancho de la pantalla
struct DrawTextureLineInfo {
    Image* image;
    int texX;
    int x1;
    int x2;
    int y1;
    int y2;
};

struct RayLines {
    Image* im;
    int texX;
    int x1;
    int x2;
    int y1;
    int y2;
};

// Comando para procesar el raycast de toda la pantalla
struct RayLinesInfo {
    RayLines* rl;   // Array de rayos, se borrara tras ser porcesado
    int wRays;      // Tamanio del array de rayos (igual que el ancho de la pantalla)
};


struct RenderCommand {
    RenderCommandType tipo;
    union {
        DrawRectInfo drawRectInfo;
        DrawTextureLineInfo drawTextureLineInfo;
        RayLinesInfo rayLinesInfo;
    };
};

