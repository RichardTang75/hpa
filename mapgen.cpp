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
#include "visualtestdebug.hpp"
#include "libraries\FastNoiseSIMD\FastNoiseSIMD\FastNoiseSIMD.h"

std::vector<tuple_int> directions=
{
    tuple_int(-1,-1),tuple_int(-1,0),tuple_int(-1,1),
    tuple_int(0,-1),                tuple_int(0,1),
    tuple_int(1,-1),tuple_int(1,0),tuple_int(1,1)
};
enum Cardinal {Northwest, West, Southwest,
                North,          South,
                Northeast, East, Southeast};
enum Terrain {Grs, Fst, Msh, Mtn, Wtr};
//to do: find if doing it a more expensive way would be too expensive. i.e. store centers, calculate ratios,
//step by step singular removal. if too close, then offset both by a random amount? 5-10, 10-20?
void terrain_overlap(tuple_set& bottom, tuple_set& top, float threshold=.05 ,int seed = 0)
{
	std::mt19937 eng;
    std::random_device rd;
	if (seed == 0)
	{
		std::mt19937 eng(rd());
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
vectormap noise_map_gen(int map_x, int map_y, int map_width, int map_height, int terrain_type = 0, int seed = 0)
{
	std::vector<double> starter (map_width*map_height);
}
tuple_triple_map get_pertinent_triple(tuple_triple_map& full_triple, int map_x, int map_y, Terrain terrain_type)
{
	tuple_triple_map to_return;
	for (tuple_int dir: directions)
	{
		auto getting = std::make_tuple(map_x + std::get<0>(dir), map_y + std::get<1>(dir), terrain_type);
		to_return[getting] = full_triple[getting];
	}
	return to_return;
}
void step1(const int& map_width, const int& map_height,
	const int& howmany_min, const int& howmany_max,
	const int& directions_min, const int& directions_max,
	const int& length_min, const int& length_max,
    const int& size_min, const int& size_max,
    tuple_set& to_return,
	unsigned seed = 0)
{
	static thread_local std::mt19937 eng(seed);
	tuple_set to_union;
	std::unordered_set<int> unacceptableX;
	std::unordered_set<int> unacceptableY;
	std::uniform_int_distribution<int> randX(0, map_width);
	std::uniform_int_distribution<int> randY(0, map_height);
	std::uniform_int_distribution<int> rand_many(howmany_min, howmany_max);
	std::uniform_int_distribution<int> length_many(length_min, length_max);
	std::uniform_int_distribution<int> directions_many(directions_min, directions_max);
    std::uniform_int_distribution<int> size_chooser(size_min, size_max);
	std::uniform_real_distribution<float> threshold(0, 1);
	std::uniform_real_distribution<float> angle(0, (2*acos(-1)));
	int how_many = rand_many(eng);
	int temp_many_directions;
	float angle_chosen;
	int tempX, tempY;
    int end_x, end_y;
	int cur_x, cur_y;
    while (to_return.size() < how_many)
	{
		tempX = randX(eng);
		tempY = randY(eng);
		if (unacceptableX.count(tempX) == 0 && unacceptableY.count(tempY) == 0)
		{
			to_return.emplace(std::tuple<int, int>(tempX, tempY));
			for (int n = -5; n == 5; ++n)
			{
				unacceptableX.emplace(int(tempX + n));
				unacceptableY.emplace(int(tempY + n));
			}
		}
	}
	for (tuple_int coord : to_return)
	{
		temp_many_directions = directions_many(eng);
		for (int n = 0; n<temp_many_directions; ++n)
		{
			int radius = length_many(eng);
			angle_chosen = angle(eng);
            cur_x = std::get<0>(coord);
            cur_y = std::get<1>(coord);
			end_x = cur_x+cos(angle_chosen)*radius;
			end_y = cur_y+sin(angle_chosen)*radius;
            bresenham(to_union, std::min(cur_x, end_x), std::max(cur_x, end_x),
						std::min(cur_y, end_y), std::max(cur_y, end_y));
            //final step takes care of over the bounds from the tuple set
		}
	}
	tuple_set_union(to_return, to_union);
	to_union.clear();
	for (tuple_int coord : to_return)
	{
		temp_many_directions = directions_many(eng);
		for (int n = 0; n<temp_many_directions; ++n)
		{
			float temp_thresh = threshold(eng);
			if (temp_thresh<.01)
			{
                int radius = length_many(eng);
                angle_chosen = angle(eng);
                cur_x = std::get<0>(coord);
                cur_y = std::get<1>(coord);
                end_x = cur_x+cos(angle_chosen)*radius;
                end_y = cur_y+sin(angle_chosen)*radius;
                bresenham(to_union, std::min(cur_x, end_x), std::max(cur_x, end_x),
							std::min(cur_y, end_y), std::max(cur_y, end_y));
			}
		}
	}
	tuple_set_union(to_return, to_union);
	to_union.clear();
    int size_many = size_chooser(eng);
    for (int i = 0; i < size_many; ++i)
    {
        for (tuple_int coord : to_return)
        {
            tuple_set_expand(to_union, coord);
        }
        tuple_set_union(to_return, to_union);
    }
    //std::cout << to_return.size() << "\n";
    tuple_set_union(to_return, to_union);
    to_union.clear();
    ////first make the lines
    ////then fill
    ////when ligating, take into consideration climes
    ////threads can start new threads if enough cores
    ////eventually preprocess it to fewer nodes using visibility graph
	
    for (int i = 0; i < (size_many); ++i)
    {
        for (tuple_int coord : to_return)
        {
            float temp_thresh = threshold(eng);
            if (temp_thresh<.05)
            {
                float angle_chosen = angle(eng);
                bresenham_expand(to_union, coord, angle_chosen);
            }
        }
        tuple_set_union(to_return, to_union);
        to_union.clear();
    }
}
void step2(const int& map_x, const int& map_y, const int& terrain,
           const int& map_width, const int& map_height,
           tuple_triple_map& maps, tuple_set& place_here)
{
    for (tuple_int dir: directions)
    {
        int dx=std::get<0>(dir);
        int dy=std::get<1>(dir);
        std::tuple<int, int, int> lookin = std::make_tuple(map_x+dx, map_y+dy, terrain);
        tuple_set selected = maps[lookin];
        for (tuple_int coord: selected)
        {
			if (in_bounds(0, map_width, 0, map_height, coord) == false)
			{
				int x_new = -1;
				int y_new = -1;
				if (dx == 1)
				{
					if (std::get<0>(coord)<0)
					{
						x_new = 513 + std::get<0>(coord);
					}
				}
				else if (dx == -1)
				{
					if (std::get<0>(coord)>512)
					{
						x_new = std::get<0>(coord) - 513;
					}
				}
				if (dy == 1)
				{
					if (std::get<1>(coord)<0)
					{
						y_new = 513 + std::get<1>(coord);
					}
				}
				else if (dy == -1)
				{
					if (std::get<1>(coord)>512)
					{
						y_new = std::get<1>(coord) - 513;
					}
				}
				if (x_new != -1 && y_new != -1)
				{
					tuple_int mod_coord = tuple_int(x_new, y_new);
					if (in_bounds(0, map_width, 0, map_height, mod_coord))
					{
						(place_here).emplace(mod_coord);
					}
				}
				else if (abs(dx+dy) == 1) //no diagonals
				{
					if (x_new != -1 && y_new == -1)
					{
						tuple_int mod_coord = tuple_int(x_new, std::get<1>(coord));
						if (in_bounds(0, map_width, 0, map_height, mod_coord))
						{
							(place_here).emplace(mod_coord);
						}
					}
					else if (x_new == -1 && y_new != -1)
					{
						tuple_int mod_coord = tuple_int(std::get<0>(coord), y_new);
						if (in_bounds(0, map_width, 0, map_height, mod_coord))
						{
							(place_here).emplace(mod_coord);
						}
					}
				}
				
			}
        }
    }
}

void get_full_map(const int& map_x, const int& map_y,
                  const int& map_width, const int& map_height,
                   tuple_triple_map& maps,
                   tuple_set& processed,
                  tuple_set& created,
                  unsigned long long init_seed)
{
    std::random_device rd;
    if (init_seed == 0)
    {
        init_seed =
        (static_cast<unsigned long long>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) +
         static_cast<unsigned long long> (rd()));
    }
    std::vector<unsigned> seeds(36);
    std::seed_seq seq
    {
        init_seed
    };
    seq.generate(seeds.begin(), seeds.end());
    int seed_start=0;
    if (processed.count(tuple_int(map_x, map_y))==0)
    {
#ifdef EMSCRIPTEN
        if (created.count(tuple_int(map_x, map_y))==0)
        {
            
            step1 (map_width, map_height, 6, 12, 2, 4, 5, 10, 14, 18,
                   std::ref(maps[std::make_tuple(map_x, map_y, Fst)]), seeds[seed_start+0]);
            step1 (map_width, map_height, 1, 2, 3, 5, 68, 86, 3, 5,
                   std::ref(maps[std::make_tuple(map_x, map_y, Mtn)]), seeds[seed_start+1]);
            step1 (map_width, map_height, 2, 3, 2, 4, 8, 16, 16, 24,
                   std::ref(maps[std::make_tuple(map_x, map_y, Wtr)]), seeds[seed_start+2]);
            step1 (map_width, map_height, 3, 5, 2, 4, 5, 10, 15, 18,
                   std::ref(maps[std::make_tuple(map_x, map_y, Msh)]), seeds[seed_start+3]);
            created.emplace(tuple_int(map_x, map_y));
            seed_start=seed_start+4;
        }
        for (tuple_int dir: directions)
        {
            int desired_x=std::get<0>(dir)+map_x;
            int desired_y=std::get<1>(dir)+map_y;
            if (created.count(tuple_int(desired_x, desired_y))==0)
            {
                step1 (map_width, map_height, 6, 12, 2, 4, 5, 10, 14, 18,
                       std::ref(maps[std::make_tuple(desired_x, desired_y, Fst)]), seeds[seed_start+0]);
                step1 (map_width, map_height, 1, 2, 3, 5, 68, 86, 3, 5,
                       std::ref(maps[std::make_tuple(desired_x, desired_y, Mtn)]), seeds[seed_start+1]);
                step1 (map_width, map_height, 2, 3, 2, 4, 8, 16, 16, 24,
                       std::ref(maps[std::make_tuple(desired_x, desired_y, Wtr)]), seeds[seed_start+2]);
                step1 (map_width, map_height, 3, 5, 2, 4, 5, 10, 15, 18,
                       std::ref(maps[std::make_tuple(desired_x, desired_y, Msh)]), seeds[seed_start+3]);
                created.emplace(tuple_int (desired_x, desired_y));
                seed_start=seed_start+4;
                
            }
        }//thread here too
        step2( map_x, map_y, Fst, map_width, map_height, std::ref(maps));
        step2( map_x, map_y, Mtn, map_width, map_height, std::ref(maps));
        step2( map_x, map_y, Wtr, map_width, map_height, std::ref(maps));
        step2( map_x, map_y, Msh, map_width, map_height, std::ref(maps));
#else
        if (created.count(tuple_int(map_x, map_y))==0)
        {

            std::thread gen_fst (step1, map_width, map_height, 6, 12, 2, 4, 5, 10, 14, 18,
                                 std::ref(maps[std::make_tuple(map_x, map_y, Fst)]), seeds[seed_start+0]);
            std::thread gen_mtn (step1, map_width, map_height, 1, 2, 3, 5, 68, 86, 3, 5,
                                 std::ref(maps[std::make_tuple(map_x, map_y, Mtn)]), seeds[seed_start+1]);
            std::thread gen_wtr (step1, map_width, map_height, 2, 3, 2, 4, 8, 16, 16, 24,
                                 std::ref(maps[std::make_tuple(map_x, map_y, Wtr)]), seeds[seed_start+2]);
            std::thread gen_msh (step1, map_width, map_height, 3, 5, 2, 4, 5, 10, 15, 18,
                                 std::ref(maps[std::make_tuple(map_x, map_y, Msh)]), seeds[seed_start+3]);
            gen_fst.join();
            gen_mtn.join();
            gen_wtr.join();
            gen_msh.join();
			if (tuple_int(map_x, map_y) == tuple_int(0, 0))
			{
				checkthis(maps[std::make_tuple(map_x, map_y, Mtn)], map_x, map_y, 512, 512);
			}
            created.emplace(tuple_int(map_x, map_y));
            seed_start=seed_start+4;
        }
        for (tuple_int dir: directions)
        {
            int desired_x=std::get<0>(dir)+map_x;
            int desired_y=std::get<1>(dir)+map_y;
            if (created.count(tuple_int(desired_x, desired_y))==0)
            {
                std::thread gen_fst (step1, map_width, map_height, 6, 12, 2, 4, 5, 10, 14, 18,
                                     std::ref(maps[std::make_tuple(desired_x, desired_y, Fst)]), seeds[seed_start+0]);
                std::thread gen_mtn (step1, map_width, map_height, 1, 2, 3, 5, 68, 86, 3, 5,
                                     std::ref(maps[std::make_tuple(desired_x, desired_y, Mtn)]), seeds[seed_start+1]);
                std::thread gen_wtr (step1, map_width, map_height, 2, 3, 2, 4, 8, 16, 16, 24,
                                     std::ref(maps[std::make_tuple(desired_x, desired_y, Wtr)]), seeds[seed_start+2]);
                std::thread gen_msh (step1, map_width, map_height, 3, 5, 2, 4, 5, 10, 15, 18,
                                     std::ref(maps[std::make_tuple(desired_x, desired_y, Msh)]), seeds[seed_start+3]);
                gen_fst.join();
                gen_mtn.join();
                gen_wtr.join();
                gen_msh.join();
				if (tuple_int(map_x, map_y) == tuple_int(0, 0))
				{
					checkthis(maps[std::make_tuple(desired_x, desired_y, Mtn)], desired_x, desired_y, 512, 512);
				}
                created.emplace(tuple_int (desired_x, desired_y));
                seed_start=seed_start+4;
            }
        }//thread here too
		tuple_set place_fst, place_mtn, place_wtr, place_msh;
		//constant rewrites?
		tuple_triple_map fst_maps = get_pertinent_triple(maps, map_x, map_y, Fst);
		tuple_triple_map mtn_maps = get_pertinent_triple(maps, map_x, map_y, Mtn);
		tuple_triple_map wtr_maps = get_pertinent_triple(maps, map_x, map_y, Wtr);
		tuple_triple_map msh_maps = get_pertinent_triple(maps, map_x, map_y, Msh);
		//assert(-1 == 1);
        std::thread finish_fst (step2, map_x, map_y, Fst, map_width, map_height, std::ref(fst_maps), std::ref(place_fst));
        std::thread finish_mtn (step2, map_x, map_y, Mtn, map_width, map_height, std::ref(mtn_maps), std::ref(place_mtn));
        std::thread finish_wtr (step2, map_x, map_y, Wtr, map_width, map_height, std::ref(wtr_maps), std::ref(place_wtr));
		std::thread finish_msh (step2, map_x, map_y, Msh, map_width, map_height, std::ref(msh_maps), std::ref(place_msh));
        finish_fst.join();
        finish_mtn.join();
        finish_wtr.join();
        finish_msh.join();
		tuple_set_union(std::ref(maps[std::make_tuple(map_x, map_y, Fst)]), place_fst);
		tuple_set_union(std::ref(maps[std::make_tuple(map_x, map_y, Mtn)]), place_mtn);
		tuple_set_union(std::ref(maps[std::make_tuple(map_x, map_y, Wtr)]), place_wtr);
		tuple_set_union(std::ref(maps[std::make_tuple(map_x, map_y, Msh)]), place_msh);
#endif
		//std::cout<<"Joined";
        processed.emplace(map_x, map_y);
    }
}

//return tuple set forests, mountains, water, marsh, pass in by parameter+change neighbors
//be sure not to call this for finished maps
//be sure to redo seed generation. i can't remember what now, but it's important
std::tuple<std::vector<tuple_set>, vectormap> map_controller
(	const int& map_x, const int& map_y,
    const int& map_width, const int& map_height,
    tuple_triple_map& maps,
    tuple_set& created,
    tuple_set& processed,
    unsigned long long init_seed = 0,
    int terrain_type = 0)
{
    get_full_map(map_x, map_y, map_width, map_height, maps, processed, created, init_seed);
	//tuple_set summed;
	int cols = map_width;
	int rows = map_height;
	int init = 0; //maybe make this terrain type?
   	std::vector<int> row(cols, init);
	vectormap map(rows, row);
	tuple_set forests = maps[std::make_tuple(map_x, map_y, Fst)];
	tuple_set mountains = maps[std::make_tuple(map_x, map_y, Mtn)];
	tuple_set water = maps[std::make_tuple(map_x, map_y, Wtr)];
	tuple_set marsh = maps[std::make_tuple(map_x, map_y, Msh)];
	//ALWAYS DELETE ONE ON TOP;
	std::vector<tuple_set> terrain_overlap_decider = { marsh, forests, mountains, water };
	for (int it = 0; it < terrain_overlap_decider.size();++it)
	{
		for (int it2 = (it+1); it2 < terrain_overlap_decider.size(); ++it2)
		{
			if (it2 == 3)
			{
				continue;
			}
			else
			{
				terrain_overlap(terrain_overlap_decider[it], terrain_overlap_decider[it2], .375);
			}
		}
	}
	std::tie(marsh, forests, mountains, water) = std::make_tuple(terrain_overlap_decider[0], terrain_overlap_decider[1],
		terrain_overlap_decider[2], terrain_overlap_decider[3]);
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
			if (in_bounds(0, map_width, 0, map_height, tempX, tempY) == true)
			{
				map[tempY][tempX] = array_num;
			}
		}
	}
	std::vector<tuple_set> to_return_sets = { forests,mountains,water,marsh };
	return std::make_tuple(to_return_sets, map);
}


