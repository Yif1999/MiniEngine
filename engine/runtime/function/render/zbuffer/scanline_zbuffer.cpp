#include "runtime/function/render/zbuffer/scanline_zbuffer.h"

#include <string>
#include <iostream>

#define window_size 512

namespace MiniEngine
{
    void zbuffer()
    {
        short int color[window_size][3]={0};
        float depth[window_size]={[0 ... (window_size-1)]=-1};
        // float depth[window_size]={0};
        // for (int i=0; i<window_size; i++)
        // {
        //     depth[i]=-1;
        // }
        // std::cout<<depth[234]<<std::endl;

        std::vector<polygonItem*> polygonTable(window_size,nullptr);
        std::vector<edgeItem*> edgeTable(window_size,nullptr);


    }
}