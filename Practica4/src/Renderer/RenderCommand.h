#pragma once
#include <cstdint>

//TODO probar a quitar el include y poner class Image
#include "../Renderer/Renderer.h"
#include "../Utils.h"


enum class RenderCommandType {
    CLEAR_RECT,
    DRAW_TEXTURE,
    DRAW_IMAGE,
    PRESENT_FRAME,
    RAY_LINES
};

/// <summary>
/// Dibuja dos rectangulos, uno de la mitad de arriba y otro de la mitad de abajo
/// El primer color es la parte de arriba y el segundo de la parte de abajo
/// </summary>
struct ClearRectInfo {
    Color color1;
    Color color2;
};

struct DrawTextureLineInfo {
    Image* image;
    int texX;
    int x1;
    int x2;
    int y1;
    int y2;
};

struct DrawImageInfo {
    Image* im;
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

struct RayLinesInfo {
    RayLines* rl;
};

struct RenderCommand {

    RenderCommandType tipo;//fuera de la union
    union {
        ClearRectInfo clearRectInfo;
        DrawTextureLineInfo drawTextureLineInfo;
        DrawImageInfo drawImageInfo;
        RayLinesInfo rayLinesInfo;
       
        /*No se puede hacer asi, hay que poner punteros, lo que puede generar problemas al destruirlo, hay que tener cuidado.
        string str;                 string* str;
        vector<int> v;              vector<int>* v;
        */
    };

};

