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
#include <thread>
#include "typedef.hpp"
#include "goodfunctions.hpp"
#include "terrain.hpp"
#include "lodepng.h"
void terrain_overlap(tuple_set& bottom, tuple_set& top, float threshold=.05 ,int seed = 0)
{
	std::mt19937 eng;
	if (seed == 0)
	{
		std::mt19937 eng(std::random_device());
	}
	else
	{
		std::mt19937 eng(seed);
	}
	std::uniform_real_distribution<float> thresh(0, 1);
	for (tuple_int coord : bottom)
	{
		if (top.count(coord) == 1 && thresh(eng) < threshold)
		{
			tuple_set_remove(top, coord);
		}
	}
}
void gen_terrain(const int& xStart, const int& xEnd,
                 const int& yStart, const int& yEnd,
                 const int& howmany_min, const int& howmany_max,
                 const int& directions_min, const int& directions_max,
                 const int& length_min,  const int& length_max,
                 const int& size_min,    const int& size_max,
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
		tuple_set_union(to_return, step);
    }
    std::cout<<to_return.size()<<"\n";
    tuple_set_union(to_return,step);
    step.clear();
    //first make the lines
    //then fill
    //when ligating, take into consideration climes
    //threads can start new threads if enough cores
    //eventually preprocess it to fewer nodes using visibility graph
    for (int i=0;i < (size_many); ++i)
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
std::tuple<std::vector<tuple_set>, vectormap> map_controller
															(const int& x_size, const int& y_size,
																const int& side_size,
																tuple_set& N, tuple_set& E,
																tuple_set& S, tuple_set& W,
																unsigned long long init_seed = 0,
																int map_x = 0, int map_y = 0,
																int terrain_type = 0)
{
	tuple_set ret_north;
	tuple_set ret_south;
	tuple_set ret_east;
	tuple_set ret_west;
	std::vector<tuple_int> additions = { tuple_int(-1,0),tuple_int(1,0),tuple_int(0,-1),tuple_int(1,0),
		tuple_int(-1,-1),tuple_int(-1,1),tuple_int(1,1),tuple_int(1,-1) };
	std::vector<tuple_int> overlap_additions = { tuple_int(-1,0),tuple_int(1,0),tuple_int(0,-1),tuple_int(1,0) };
	std::random_device rd;
	if (init_seed == 0)
	{
		init_seed =
			(static_cast<unsigned long long>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) +
				static_cast<unsigned long long> (rd()));
	}
	std::vector<unsigned> seeds(4);
	std::seed_seq seq
	{
		init_seed
	};
	seq.generate(seeds.begin(), seeds.end());
	tuple_set forests;
	tuple_set mountains;
	tuple_set water;
	tuple_set marsh;
	tuple_set summed;
	tuple_set final_step;
	int cols = x_size;
	int rows = y_size;
	std::tuple<int, int, int, int> bounds = det_bounds(x_size, y_size, side_size, N, E, S, W);
	int x_start = std::get<0>(bounds);
	int x_end = std::get<1>(bounds);
	int y_start = std::get<2>(bounds);
	int y_end = std::get<3>(bounds);
	int init = 0; //maybe make this terrain type?
	std::vector<unsigned char> image(4 * rows*cols);
	std::vector<int> row(cols, init);
	vectormap map(rows, row);
	//final step visualized
	vectormap walls(rows, row);
	std::vector<unsigned char> wallimage(4 * rows*cols);
	std::thread gen_for(gen_terrain, x_start, x_end, y_start, y_end, 6, 12, 2, 4, 5, 10, 14, 18, std::ref(forests), seeds[0]);
	std::thread gen_mtn(gen_terrain, x_start, x_end, y_start, y_end, 1, 2, 3, 5, 58, 86, 7, 10, std::ref(mountains), seeds[1]);
	std::thread gen_wtr(gen_terrain, x_start, x_end, y_start, y_end, 2, 3, 2, 4, 8, 16, 16, 24, std::ref(water), seeds[2]);
	std::thread gen_msh(gen_terrain, x_start, x_end, y_start, y_end, 3, 5, 2, 4, 5, 10, 15, 18, std::ref(marsh), seeds[3]);
	gen_for.join();
	gen_mtn.join();
	gen_wtr.join();
	gen_msh.join();
	std::cout << "JOINED";
	//when(and if) i get threading working, also thread in the unioning? maybe?
	tuple_set_union(summed, forests);
	tuple_set_union(summed, mountains);
	tuple_set_union(summed, water);
	tuple_set_union(summed, marsh);
	//ALWAYS DELETE ONE ON TOP;
	std::vector<tuple_set> terrain_overlap_decider = { marsh, forests, mountains, water };
	for (int it = 0; it < terrain_overlap_decider.size();++it)
	{
		for (int it2 = (it+1); it2 < terrain_overlap_decider.size(); ++it2)
		{
			if (it2 == 3)
			{
				terrain_overlap(terrain_overlap_decider[it], terrain_overlap_decider[it2], .15);
			}
			else
			{
				terrain_overlap(terrain_overlap_decider[it], terrain_overlap_decider[it2], .075);
			}
		}
	}
	std::tie(marsh, forests, mountains, water) = std::make_tuple(terrain_overlap_decider[0], terrain_overlap_decider[1],
		terrain_overlap_decider[2], terrain_overlap_decider[3]);
	//KEEP EXPANDING UNTIL THEY OVERLAP
	//THOSE ARE NODES
	//REMOVE NODES IF PREVIOUS COST IS EQUAL TO NEW COST FOR MOVEMENT
	for (tuple_int point : summed)
	{
		int cur_x = std::get<0>(point);
		int cur_y = std::get<1>(point);
		for (tuple_int dirs : additions)
		{
			int dx = std::get<0>(dirs);
			int dy = std::get<1>(dirs);
			tuple_int temp_coord = tuple_int(cur_x + dx, cur_y + dy);
			if (summed.count(temp_coord) == 0)
			{
				final_step.emplace(temp_coord);
			}
		}
	}
	std::vector<map_tuple> total =
	{ map_tuple(forests,1),map_tuple(marsh,2),
		map_tuple(mountains,3),map_tuple(water,4),
	};
	for (map_tuple proc : total)
	{
		tuple_set terrain = std::get<0>(proc);
		int array_num = std::get<1>(proc);
		for (tuple_int point : terrain)
		{
			int tempX = std::get<0>(point);
			int tempY = std::get<1>(point);
			if (in_bounds(x_start, x_end, y_start, y_end, tempX, tempY) == true)
			{
				map[tempY][tempX] = array_num;
			}
		}
	}

	for (tuple_int coord : final_step)
	{
		int tempX = std::get<0>(coord);
		int tempY = std::get<1>(coord);
		if (in_bounds(x_start, x_end, y_start, y_end, tempX, tempY) == true)
		{
			walls[tempY][tempX] = 1;
		}
	}
	array_img(map, image, rows, cols, terrain_type);
	array_img(walls, wallimage, rows, cols, terrain_type);
	std::vector<unsigned char> png;
	std::vector<unsigned char> png2;
	lodepng::State state;
	lodepng::State state2;
	unsigned error = lodepng::encode(png, image, rows, cols, state);
	if (!error)
	{
		lodepng::save_file(png, "cplusplus.png");
	}
	else
	{
		std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << "\n";
	}
	unsigned nerror = lodepng::encode(png2, wallimage, rows, cols, state2);
	if (!nerror)
	{
		lodepng::save_file(png2, "cplusplus2.png");
	}
	else
	{
		std::cout << "encoder error " << nerror << ": " << lodepng_error_text(nerror) << "\n";
	}
	std::vector<tuple_set> to_return_sets = { forests,mountains,water,marsh,ret_north,ret_east,ret_south,ret_west };
	return std::make_tuple(to_return_sets, map);
}
