//
//  goodfunctions.cpp
//  why
//
//  Created by asdfuiop on 7/12/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#include "typedef.hpp"

////////////BROADLY UTILIZABLE FUNCTIONS
float dist_squared(tuple_int& to, tuple_int& from)
{
    float x0=std::get<0>(to);
    float x1=std::get<0>(from);
    float y0=std::get<1>(to);
    float y1=std::get<1>(from);
    return powf((x1-x0),2)+powf((y1-y0),2);
}
bool in_bounds(const int& x_start, const int& x_end,
               const int& y_start, const int& y_end,
               tuple_int& coord)
{
    int tempX=std::get<0>(coord);
    int tempY=std::get<1>(coord);
    return (tempX>=x_start and tempX<x_end and tempY>=y_start and tempY<y_end);
}
               
bool in_bounds(const int& x_start, const int& x_end,
                const int& y_start, const int& y_end,
                const int& tempX, const int&tempY)
{
    return (tempX>=x_start and tempX<x_end and tempY>=y_start and tempY<y_end);
}
