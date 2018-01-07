//
//  pathfinding.cpp
//  take2
//
//  Created by asdfuiop on 8/13/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#include "typedef.hpp"
#include <unordered_map>

tuple_int which_map (tuple_int& location, int depth, int cut_size=8)
{
    int map_size=512;
    int globalx=std::get<0>(location);
    int globaly=std::get<1>(location);
    //truncation towards zero. -10,0 should be in -1,0, -10,-10 should be in -1,-1 etc.
	//i feel like I should just have std::trunc'd this
    if (globalx<0){globalx-=map_size;}
    if (globaly<0){globaly-=map_size;}
    int which_x=globalx/(pow(cut_size,depth));
    int which_y=globaly/(pow(cut_size,depth));
    return tuple_int(which_x,which_y);
}
float calc_cost(tuple_int& current, tuple_int& to)
{
    float x0=std::get<0>(current);
    float x1=std::get<0>(to);
    float y0=std::get<1>(current);
    float y1=std::get<1>(to);
    float dx=fabsf(x1-x0);
    float dy=fabsf(y1-y0);
    return dx+dy+(1.5-2)*(std::min(dx,dy));
}

