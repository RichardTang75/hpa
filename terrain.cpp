//
//  terrain.cpp
//  take2
//
//  Created by asdfuiop on 8/13/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#include <iostream>
#include "typedef.hpp"
#include "lodepng.h"

void tuple_set_expand(tuple_set& in, tuple_int& coord)
{
    int cur_x=std::get<0>(coord);
    int cur_y=std::get<1>(coord);
    //    in.emplace(tuple_int(cur_x+1,cur_y+1));
    //    in.emplace(tuple_int(cur_x-1,cur_y-1));
    //    in.emplace(tuple_int(cur_x-1,cur_y+1));
    //    in.emplace(tuple_int(cur_x+1,cur_y-1));
    in.emplace(tuple_int(cur_x+1,cur_y));
    in.emplace(tuple_int(cur_x-1,cur_y));
    in.emplace(tuple_int(cur_x,cur_y+1));
    in.emplace(tuple_int(cur_x,cur_y-1));
}
void tuple_set_remove(tuple_set& in, tuple_int& coord)
{
	int cur_x = std::get<0>(coord);
	int cur_y = std::get<1>(coord);
	in.erase(coord);
	std::vector<tuple_int> directions =
	{
		tuple_int(-1,0),tuple_int(0,-1),tuple_int(0,1),tuple_int(1,0),
		/*tuple_int(-1,1),tuple_int(-1,-1),tuple_int(1,1),tuple_int(1,-1),
		tuple_int(-2,0),tuple_int(2,0),tuple_int(0,-2),tuple_int(0,2),
		tuple_int(-2,1),tuple_int(-2,-1),tuple_int(2,1),tuple_int(2,-1),
		tuple_int(1,-2),tuple_int(1,2),tuple_int(-1,-2),tuple_int(-1,2)*/
	};
	for (tuple_int dir : directions)
	{
		int dx = std::get<0>(dir);
		int dy = std::get<1>(dir);
		tuple_int to_remove = tuple_int(cur_x + dx, cur_y + dy);
		if ((in.count(to_remove)) == 1)
		{
			in.erase(to_remove);
		}
	}
}
void snake_expand(tuple_set& in, tuple_int& coord, tuple_int& dir)
{
    int dx=std::get<0>(dir);
    int dy=std::get<1>(dir);
    int x=std::get<0>(coord);
    int y=std::get<1>(coord);
    for (int i=0; i<5; ++i)
    {
        tuple_int to_add=tuple_int((x+dx*i),(y+dy*i));
        tuple_set_expand(in, to_add);
    }
}
void bresenham(tuple_set& in, int startx, int endx, int starty, int endy)
{
	int dx = endx - startx;
	int dy = endy - starty;
	int dist = 2 * dy - dx;
	int y = starty;
	for (int x = startx; x < (endx + 1); ++x)
	{
		in.emplace(x, y);
		while (dist > 0 && dx != 0)
		{
			y = y + 1;
			dist = dist - 2 * dx;
            in.emplace(x, y);
		}
		dist = dist + 2 * dy;
	}
}
void bresenham_expand(tuple_set& in, tuple_int& coord, float& angle)
{
	int passed_x = std::get<0>(coord);
	int passed_y = std::get<1>(coord);
	int radius = 5; //small but noticeable protrusions. Many of them.
	int circle_x = radius*cos(angle) + passed_x;
	int circle_y = radius*sin(angle) + passed_y;
	int startx = std::min(passed_x, circle_x);
	int endx = std::max(passed_x, circle_x);
	int starty = std::min(passed_y, circle_y);
	int endy = std::max(passed_y, circle_y);
	int dx = endx - startx;
	int dy = endy - starty;
	int dist = 2 * dy - dx;
	int y = starty;
	for (int x = startx; x < (endx + 1); ++x)
	{
		tuple_int next = tuple_int(x, y);
		tuple_set_expand(in, next);
		while (dist > 0 && dx != 0)
		{
			y = y + 1;
			dist = dist - 2 * dx;
            tuple_int next = tuple_int(x, y);
            tuple_set_expand(in, next);
		}
		dist = dist + 2 * dy;
	}
}
void array_img(std::vector<std::vector<int>>& array,std::vector<unsigned char>& img,
               const int& rows, const int& cols, int terrain=0)
{
    std::vector<unsigned char> color0;
    std::vector<unsigned char> color1;
    std::vector<unsigned char> color2;
    std::vector<unsigned char> color3;
    std::vector<unsigned char> color4;
    std::vector<std::vector<unsigned char>> colors;
    //0=grass, 1=forest, 2=marsh, 3=mount, 4=water
    if (terrain==0)
    {
        color0={179,235,75,255};
        color1={42,97,24,255};
        color2={85,107,47,255};
        color3={122,106,61,255};
        color4={26,130,172,255};
    }
    colors={color0,color1,color2,color3,color4};
    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            img[4*row*cols+4*col]=colors[array[row][col]][0];
            img[4*row*cols+4*col+1]=colors[array[row][col]][1];
            img[4*row*cols+4*col+2]=colors[array[row][col]][2];
            img[4*row*cols+4*col+3]=colors[array[row][col]][3];
        }
    }
}
std::tuple<int,int,int,int> det_bounds(const int& x_size,
                                       const int& y_size,
                                       const int& side_size,
                                       tuple_set& N, tuple_set& E,
                                       tuple_set& S, tuple_set& W)
{
    int x_start=0;
    int x_end=x_size;
    int y_start=0;
    int y_end=y_size;
    if (N!=tuple_set({}))
    {
        y_start+=side_size;
    }
    if (S!=tuple_set({}))
    {
        y_end-=side_size;
    }
    if (E!=tuple_set({}))
    {
        x_end-=side_size;
    }
    if (W!=tuple_set({}))
    {
        x_start+=side_size;
    }
    return std::make_tuple(x_start,x_end,y_start,y_end);
    
}

void print_tuple(tuple_set toprint)
{
    for (tuple_int tup : toprint)
    {
        std::cout << std::get<0>(tup) <<","<< std::get<1>(tup) << "\n";
    }
    std::cout << toprint.size();
}
//in1 should be larger than in2, but i don't really care
void tuple_set_union(tuple_set& in1, tuple_set& in2)
{
    for (tuple_int ele: in2)
    {
        in1.emplace(ele);
    }
}
