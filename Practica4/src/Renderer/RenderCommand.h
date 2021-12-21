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

struct DrawRectInfo {
    Color color;
    int x1;
    int x2;
    int y1;
    int y2;
};

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

struct RayLinesInfo {
    RayLines* rl;
    int wRays;
};

struct RenderCommand {
    RenderCommandType tipo;
    union {
        DrawRectInfo drawRectInfo;
        DrawTextureLineInfo drawTextureLineInfo;
        RayLinesInfo rayLinesInfo;
    };
};

