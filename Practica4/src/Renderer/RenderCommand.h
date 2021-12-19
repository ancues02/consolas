#pragma once
#include <cstdint>

//TODO probar a quitar el include y poner class Image
#include "../Renderer/Renderer.h"

enum class RenderCommandType {
    CLEAR_RECT,
    DRAW_TEXTURE,
    DRAW_IMAGE,
    PRESENT_FRAME,
};

/// <summary>
/// Dibuja dos rectangulos, uno de la mitad de arriba y otro de la mitad de abajo
/// El primer color es la parte de arriba y el segundo de la parte de abajo
/// </summary>
struct ClearRectInfo {
    Color color;
    int x1;
    int x2;
    int y1;
    int y2;
};
struct DrawTextureLineInfo {
    uint32_t color;
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

struct PresentInfo {

};

struct RenderCommand {

    RenderCommandType tipo;//fuera de la union
    union {
        ClearRectInfo clearRectInfo;
        DrawTextureLineInfo drawTextureLineInfo;
        DrawImageInfo drawImageInfo;
        PresentInfo presentInfo;
        /*No se puede hacer asi, hay que poner punteros, lo que puede generar problemas al destruirlo, hay que tener cuidado.
        string str;                 string* str;
        vector<int> v;              vector<int>* v;
        */
    };

};

