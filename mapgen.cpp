//
//  genterrain.cpp
//  why
//
//  Created by asdfuiop on 7/12/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//
#include <iostream>
#include <random>
#include <chrono>
#include "typedef.hpp"
#include "goodfunctions.hpp"
#include "terrain.hpp"
void gen_terrain(const int& xStart, const int& xEnd,
                 const int& yStart, const int& yEnd,
                 const int& howmany_min, const int& howmany_max,
                 const int& directions_min, const int& directions_max,
                 const int& length_min,  const int& length_max,
                 const int& size_min,    const int& size_max,
                 const int& branch_min, const int& branch_max,
                 tuple_set& to_return,
                 unsigned seed = 0)
{
    static thread_local std::mt19937 eng(seed);
    tuple_set to_union;
    tuple_set visited;
    tuple_set step;
    tuple_set step2;
    std::unordered_set<int> unacceptableX;
    std::unordered_set<int> unacceptableY;
    std::uniform_int_distribution<int> randX(xStart, xEnd);
    std::uniform_int_distribution<int> randY(yStart, yEnd);
    std::uniform_int_distribution<int> rand_many(howmany_min,howmany_max);
    std::uniform_int_distribution<int> length_many(length_min,length_max);
    std::uniform_int_distribution<int> directions_many(directions_min,directions_max);
    std::uniform_int_distribution<int> direction_chooser(0,7);
    std::uniform_int_distribution<int> size_chooser(size_min,size_max);
    std::uniform_real_distribution<float> threshold(0,1);
    std::vector<tuple_int> additions={tuple_int(-1,0),tuple_int(1,0),tuple_int(0,-1),tuple_int(1,0)};
    std::vector<tuple_int> directions=
    {
        tuple_int(-1,-1),tuple_int(-1,0),tuple_int(-1,1),
        tuple_int(0,-1),                tuple_int(0,1),
        tuple_int(1,-1),tuple_int(1,0),tuple_int(1,1)
    };
    int how_many=rand_many(eng);
    tuple_int direction_chosen;
    int temp_many_directions;
    int tempX;
    int tempY;
    int d_x;
    int d_y;
    int cur_x;
    int cur_y;
    int size_many;
    while (to_return.size() < how_many)
    {
        tempX = randX(eng);
        tempY = randY(eng);
        if (unacceptableX.count(tempX) == 0 && unacceptableY.count(tempY) == 0)
        {
            to_return.emplace(std::tuple<int,int>(tempX, tempY));
            for (int n = -5; n == 5; ++n)
            {
                unacceptableX.emplace(int(tempX + n));
                unacceptableY.emplace(int(tempY + n));
            }
        }
    }
    for (tuple_int coord: to_return)
    {
        temp_many_directions=directions_many(eng);
        for (int n=0;n<temp_many_directions;++n)
        {
            int temp_length=length_many(eng);
            direction_chosen = directions[direction_chooser(eng)];
            d_x=std::get<0>(direction_chosen);
            d_y=std::get<1>(direction_chosen);
            for (int i=0;i<temp_length;++i)
            {
                cur_x=std::get<0>(coord);
                cur_y=std::get<1>(coord);
                to_union.emplace(tuple_int(cur_x+(d_x*i),cur_y+(d_y*i)));
            }
        }
    }
    tuple_set_union(to_return,to_union);
    to_union.clear();
    for (tuple_int coord: to_return)
    {
        temp_many_directions=directions_many(eng);
        for (int n=0;n<temp_many_directions;++n)
        {
            float temp_thresh=threshold(eng);
            if (temp_thresh<.01)
            {
                int temp_length=length_many(eng);
                direction_chosen = directions[direction_chooser(eng)];
                d_x=std::get<0>(direction_chosen);
                d_y=std::get<1>(direction_chosen);
                for (int i=0;i<temp_length;++i)
                {
                    cur_x=std::get<0>(coord);
                    cur_y=std::get<1>(coord);
                    to_union.emplace(tuple_int(cur_x+(d_x*i),cur_y+(d_y*i)));
                }
            }
        }
    }
    tuple_set_union(to_return,to_union);
    to_union.clear();
    //initialize stuff for step outside loop because otherwise it would be hard.
    size_many=size_chooser(eng);
    //size_many is the floor for how small the clumps are
    //STEP IS TO AVOID HAVING TO HIT ALREADY HIT COORDS
    for (int i=0;i < size_many; ++i)
    {
        for (tuple_int coord: to_return)
        {
            tuple_set_expand(step,coord);
        }
    }
    std::cout<<to_return.size()<<"\n";
    tuple_set_union(to_return,step);
    step.clear();
    //first make the lines
    //then fill
    //when ligating, take into consideration climes
    //threads can start new threads if enough cores
    //eventually preprocess it to fewer nodes using visibility graph
    for (int i=0;i < (size_many*3/2); ++i)
    {
        for (tuple_int coord: to_return)
        {
            float temp_thresh=threshold(eng);
            tuple_int temp_coord;
            if (temp_thresh<.05)
            {
                direction_chosen = directions[direction_chooser(eng)];
                snake_expand(to_return,coord,direction_chosen);
            }
        }
        tuple_set_union(to_return,step);
        step.clear();
    }
}
