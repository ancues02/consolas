#pragma once
#include <cstdint>

//TODO probar a quitar el include y poner class Image
#include "../Renderer/Renderer.h"

enum class RenderCommandType {
    CLEAR,
    DRAW_IMAGE,
    DRAW_LINE,
    END_FRAME,
};


struct ClearInfo {
    uint32_t color;
};
struct DrawLineInfo {

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


struct RenderCommand {

    RenderCommandType tipo;//fuera de la union
    union {
        ClearInfo clearInfo;
        DrawLineInfo drawLineInfo;
        DrawImageInfo drawImageInfo;
        /*No se puede hacer asi, hay que poner punteros, lo que puede generar problemas al destruirlo, hay que tener cuidado.
        string str;                 string* str;
        vector<int> v;              vector<int>* v;
        */
    };

};

