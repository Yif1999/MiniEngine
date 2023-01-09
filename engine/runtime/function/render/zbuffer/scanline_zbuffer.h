#pragma once

#include "runtime/core/color/color.h"

namespace MiniEngine
{
    struct polygonItem
    {
        float a,b,c,d;
        int id;
        int dy;
        Color color;
        polygonItem *next;
    };

    struct edgeItem
    {
        float x;
        float dx;
        int dy;
        int id;
        edgeItem *next;
    };


    void zbuffer();
}