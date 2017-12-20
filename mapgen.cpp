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
inline int cardinal_overflow(const int& xStart, const int& xEnd,
                             const int& yStart, const int& yEnd,
                             const int& end_x, const int& end_y)
{
    int xCard, yCard;
    std::tie(xCard, yCard)=tuple_int(-1,-1);
    if (end_x > xEnd)
    {xCard=East;}
    else if(end_x < xStart)
    {xCard=West;}
    
    if (end_y > yEnd)
    {yCard=South;}
    else if (end_y < yStart)
    {yCard=North;}

    if (xCard==-1 && yCard == -1)
    {return -1;}
    else if (xCard==-1)
    {return yCard;}
    else if (yCard==-1)
    {return xCard;}
    else
    {
        if (xCard==East)
        {
            if (yCard==North)
            {return Northeast;}
            else
            {return Southeast;}
        }
        else
        {
            if (yCard==North)
            {return Northwest;}
            else
            {return Southwest;}
        }
    }
}
inline void cardinal_emplacer(const int& xStart, const int& xEnd,
                             const int& yStart, const int& yEnd,
                             const int& end_x, const int& end_y,
                             const int& start_x, const int& start_y,
                             const float& angle,
                              const Cardinal& direction, std::vector<tuple_set>& neighbors)
                             //neighbors
{
    switch (direction)
    {
            //use angle and to_edge and y_overflow to determine the size
        case North:
        {
            int yoverflow=yStart-end_y;
            int to_edge=start_y-yStart;
            float dx=cos(angle);
            float dy=sin(angle);
            float slope=dx/dy;
            int startx=round(to_edge*slope);
            for (int i=0; i<yoverflow; ++i)
            {
                neighbors[North].emplace(tuple_int(round(startx+slope*i),512-i));
            }
            break;
        }
        case South:
        {
            int yoverflow=end_y-yEnd;
            int to_edge=yEnd-start_y;
            float dx=cos(angle);
            float dy=sin(angle);
            float slope=dx/dy;
            int startx=round(to_edge*slope);
            for (int i=0; i<yoverflow; ++i)
            {
                neighbors[South].emplace(tuple_int(round(startx+slope*i),i));
            }
            break;
        }
        case East:
        {
            int xoverflow=end_x-xEnd;
            int to_edge=xEnd-start_x;
            float dx=cos(angle);
            float dy=sin(angle);
            float slope=dy/dx;
            int starty=round(to_edge*slope);
            for (int i=0; i<xoverflow; ++i)
            {
                neighbors[East].emplace(tuple_int(512-i,round(starty+slope*i)));
            }
            break;
        }
        case West:
        {
            int xoverflow=xStart-end_x;
            int to_edge=start_x-xStart;
            float dx=cos(angle);
            float dy=sin(angle);
            float slope=dy/dx;
            int starty=round(to_edge*slope);
            for (int i=0; i<xoverflow; ++i)
            {
                neighbors[West].emplace(tuple_int(512-i,round(starty+slope*i)));
            }
            break;
        }
        case Northeast:
        {
            cardinal_emplacer(xStart,xEnd, yStart,  yEnd,
                              end_x, end_y, start_x,  start_y,angle, North, neighbors);
            cardinal_emplacer(xStart,xEnd, yStart,  yEnd,
                              end_x, end_y, start_x,  start_y,angle, East, neighbors);
        }
        case Northwest:
        {
            cardinal_emplacer(xStart,xEnd, yStart,  yEnd,
                              end_x, end_y, start_x,  start_y,angle, North, neighbors);
            cardinal_emplacer(xStart,xEnd, yStart,  yEnd,
                              end_x, end_y, start_x,  start_y,angle, West, neighbors);
        }
        case Southeast:
        {
            cardinal_emplacer(xStart,xEnd, yStart,  yEnd,
                              end_x, end_y, start_x,  start_y,angle, South, neighbors);
            cardinal_emplacer(xStart,xEnd, yStart,  yEnd,
                              end_x, end_y, start_x,  start_y,angle, East, neighbors);
        }
        case Southwest:
        {
            cardinal_emplacer(xStart,xEnd, yStart,  yEnd,
                              end_x, end_y, start_x,  start_y,angle, South, neighbors);
            cardinal_emplacer(xStart,xEnd, yStart,  yEnd,
                              end_x, end_y, start_x,  start_y,angle, West, neighbors);
        }
    }
}
void step1(const int& xStart, const int& xEnd,
	const int& yStart, const int& yEnd,
	const int& howmany_min, const int& howmany_max,
	const int& directions_min, const int& directions_max,
	const int& length_min, const int& length_max,
    tuple_set& to_return, std::vector<tuple_set>& neighbors,
	unsigned seed = 0)
{
	static thread_local std::mt19937 eng(seed);
	tuple_set to_union;
	tuple_set step;
	std::unordered_set<int> unacceptableX;
	std::unordered_set<int> unacceptableY;
	std::uniform_int_distribution<int> randX(xStart, xEnd);
	std::uniform_int_distribution<int> randY(yStart, yEnd);
	std::uniform_int_distribution<int> rand_many(howmany_min, howmany_max);
	std::uniform_int_distribution<int> length_many(length_min, length_max);
	std::uniform_int_distribution<int> directions_many(directions_min, directions_max);
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
            int fin_card=cardinal_overflow(xStart, xEnd, yStart, yEnd, end_x, end_y);
            if (fin_card != -1)
            {
                Cardinal actual_card= Cardinal(fin_card);
                cardinal_emplacer(xStart, xEnd, yStart, yEnd, end_x, end_y, cur_x, cur_y, angle_chosen, actual_card, neighbors);
            }                        //use angle and to_edge and y_overflow to determine the size
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
                int fin_card=cardinal_overflow(xStart, xEnd, yStart, yEnd, end_x, end_y);
                if (fin_card != -1)
                {
                    Cardinal actual_card= Cardinal(fin_card);
                    cardinal_emplacer(xStart, xEnd, yStart, yEnd, end_x, end_y, cur_x, cur_y, angle_chosen, actual_card, neighbors);
                }                        //use angle and to_edge and y_overflow to determine the size
				bresenham(to_union, std::min(cur_x, end_x), std::max(cur_x, end_x),
							std::min(cur_y, end_y), std::max(cur_y, end_y));
                //final step takes care of over the bounds from the tuple set
			}
		}
	}
	tuple_set_union(to_return, to_union);
	to_union.clear();
}
void step2 (const int& xStart, const int& xEnd,
			const int& yStart, const int& yEnd, 
			const int& size_min, const int& size_max,
			const std::vector<tuple_set>& pert_neighbors,
			tuple_set& to_return,
			unsigned seed = 0)
{
	tuple_set step;
	for (tuple_set neighbor : pert_neighbors)
	{
		tuple_set_union(neighbor, to_return);
	}
	std::uniform_int_distribution<int> size_chooser(size_min, size_max);
	static thread_local std::mt19937 eng(seed);
	int size_many = size_chooser(eng);
	//size_many is the floor for how small the clumps are
	//STEP IS TO AVOID HAVING TO HIT ALREADY HIT COORDS
	for (int i = 0; i < size_many; ++i)
	{
		for (tuple_int coord : to_return)
		{
			tuple_set_expand(step, coord);
		}
		tuple_set_union(to_return, step);
	}
	std::cout << to_return.size() << "\n";
	tuple_set_union(to_return, step);
	step.clear();
	////first make the lines
	////then fill
	////when ligating, take into consideration climes
	////threads can start new threads if enough cores
	////eventually preprocess it to fewer nodes using visibility graph
	//for (int i = 0; i < (size_many); ++i)
	//{
	//	for (tuple_int coord : to_return)
	//	{
	//		float temp_thresh = threshold(eng);
	//		tuple_int temp_coord;
	//		if (temp_thresh<.05)
	//		{
	//			direction_chosen = directions[direction_chooser(eng)];
	//			snake_expand(to_return, coord, direction_chosen);
	//		}
	//	}
	//	tuple_set_union(to_return, step);
	//	step.clear();
	//}
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
    tuple_set step;
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
    int how_many=rand_many(eng);
    tuple_int direction_chosen;
    int temp_many_directions;
    int tempX, tempY;
    int d_x, d_y;
    int cur_x, cur_y;
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
	//adding extra flair to prevent it from looking like it all radiates from one point
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
    int size_many=size_chooser(eng);
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
   /* first make the lines
    then fill
    when ligating, take into consideration climes
    threads can start new threads if enough cores
    eventually preprocess it to fewer nodes using visibility graph*/
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

/*
to do: fix seed generation for this part. introduce a way to generate multiple maps, add back keyboard commands for movin round.
*/
void retrieve_maps(const int& map_x, const int& map_y, const int& terrain,
	const int& howmany_min, const int& howmany_max,
	const int& directions_min, const int& directions_max,
	const int& length_min, const int& length_max,
	const int& size_min, const int& size_max,
	overflow_map& neighbors,
	tuple_triple_map& created,
	unsigned long long init_seed = 0)
{
	std::vector<tuple_set> pert_neighbors;
	tuple_set empty;
	for (int i = 0; i<directions.size(); ++i)
	{
		tuple_int dir = directions[i];
		int nearby_x = std::get<0>(dir) + map_x;
		int nearby_y = std::get<1>(dir) + map_y;
		if (neighbors.count(std::make_tuple(nearby_x, nearby_y, terrain)) == 1)
		{
			//start from NE, get the bottom-right diagonal overflow, put that in the NE position of the relevant neighbors
			pert_neighbors.push_back(neighbors[std::make_tuple(nearby_x, nearby_y, terrain)][8 - i]);
		}
		else
		{
			//neighbors[Northwest], neighbors[North], etc from the enum cardinal
			std::vector<tuple_set> neighbors_to_pass_in = { empty, empty, empty,
				empty,		  empty,
				empty, empty, empty };
			//need no neigh, step1, receptacle to be return'd
			step1(0, 512, 0, 512, howmany_min, howmany_max, directions_min, directions_max,
				length_min, length_max, created[std::make_tuple(nearby_x, nearby_y, terrain)], neighbors_to_pass_in, init_seed);
			neighbors[std::make_tuple(nearby_x, nearby_y, terrain)] = neighbors_to_pass_in;
			pert_neighbors.push_back(neighbors[std::make_tuple(nearby_x, nearby_y, terrain)][7 - i]);
		}
	}
	//remember to step 1 the initial one too
	std::vector<tuple_set> neighbors_to_pass_in = { empty, empty, empty,
		empty,		  empty,
		empty, empty, empty };
	if (created.count(std::make_tuple(map_x, map_y, terrain)) == 0)
	{
		step1(0, 512, 0, 512, howmany_min, howmany_max,
			directions_min, directions_max,
			length_min, length_max, created[std::make_tuple(map_x, map_y, terrain)], neighbors_to_pass_in, init_seed);
	}
	neighbors[std::make_tuple(map_x, map_y, terrain)] = neighbors_to_pass_in;
	step2(0, 512, 0, 512, size_min, size_max, pert_neighbors, created[std::make_tuple(map_x, map_y, terrain)], init_seed);
}

//needed for threadin.
tuple_triple_map get_valid_created(const int& map_x, const int& map_y, const int& terrain, tuple_triple_map& total_created)
{
	tuple_triple_map to_return;
	if (total_created.count(std::make_tuple(map_x, map_y, terrain)) == 1)
	{
		to_return[std::make_tuple(map_x, map_y, terrain)] = total_created[std::make_tuple(map_x, map_y, terrain)];
	}
	for (tuple_int dir : directions)
	{
		int get_x = map_x + std::get<0>(dir);
		int get_y = map_y + std::get<1>(dir);
		if (total_created.count(std::make_tuple(get_x, get_y, terrain)) == 1)
		{
			to_return[std::make_tuple(get_x, get_y, terrain)] = total_created[std::make_tuple(get_x, get_y, terrain)];
		}
	}
	return to_return;
}

overflow_map get_valid_neighbors(const int& map_x, const int& map_y, const int& terrain, overflow_map& all_neighbors)
{
	overflow_map to_return;
	for (tuple_int dir : directions)
	{
		int get_x = map_x + std::get<0>(dir);
		int get_y = map_y + std::get<1>(dir);
		if (all_neighbors.count(std::make_tuple(get_x, get_y, terrain)))
		{
			to_return[std::make_tuple(get_x, get_y, terrain)] = all_neighbors[std::make_tuple(get_x, get_y, terrain)];
		}
	}
	return to_return;
}

/*
unordered_map of every square-> neighbors
unordered map of every square->base map
to get one map, you get retrieve base maps and each of neighbor's maps. thus for every one, you need 9 made.
*/

//return tuple set forests, mountains, water, marsh, pass in by parameter+change neighbors
//be sure not to call this for finished maps
std::tuple<std::vector<tuple_set>, vectormap> map_controller
															(	const int& map_x, const int& map_y,
																const int& x_size, const int& y_size,
																tuple_triple_map& created,
																overflow_map& neighbors,
																unsigned long long init_seed = 0,
																int terrain_type = 0)
{
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
	//tuple_set summed;
	int cols = x_size;
	int rows = y_size;
	int x_start, x_end, y_start, y_end;
	std::tie(x_start, x_end, y_start, y_end) = std::make_tuple(0, 512, 0, 512);
	int init = 0; //maybe make this terrain type?
	std::vector<unsigned char> image(4 * rows*cols);
	std::vector<int> row(cols, init);
	vectormap map(rows, row);
	tuple_triple_map fst_triple = get_valid_created(map_x, map_y, Fst, created);
	tuple_triple_map mtn_triple = get_valid_created(map_x, map_y, Mtn, created);
	tuple_triple_map wtr_triple = get_valid_created(map_x, map_y, Wtr, created);
	tuple_triple_map msh_triple = get_valid_created(map_x, map_y, Msh, created);
	overflow_map fst_neighbors = get_valid_neighbors(map_x, map_y, Fst, neighbors);
	overflow_map mtn_neighbors = get_valid_neighbors(map_x, map_y, Mtn, neighbors);
	overflow_map wtr_neighbors = get_valid_neighbors(map_x, map_y, Wtr, neighbors);
	overflow_map msh_neighbors = get_valid_neighbors(map_x, map_y, Msh, neighbors);
	std::vector<tuple_triple_map> triples = { fst_triple, mtn_triple, wtr_triple, msh_triple };
	std::vector<overflow_map> overflows = { fst_neighbors, mtn_neighbors, wtr_neighbors, msh_neighbors };
	retrieve_maps( map_x, map_y, Fst, 6, 12, 2, 4, 5, 10, 14, 18, std::ref(fst_neighbors), std::ref(fst_triple), seeds[0]);
	retrieve_maps( map_x, map_y, Mtn, 6, 12, 2, 4, 5, 10, 14, 18, std::ref(mtn_neighbors), std::ref(mtn_triple), seeds[1]);
	retrieve_maps( map_x, map_y, Wtr, 6, 12, 2, 4, 5, 10, 14, 18, std::ref(wtr_neighbors), std::ref(wtr_triple), seeds[2]);
	retrieve_maps( map_x, map_y, Msh, 6, 12, 2, 4, 5, 10, 14, 18, std::ref(msh_neighbors), std::ref(msh_triple), seeds[3]);
/*	gen_fst.join();
	gen_mtn.join();
	gen_wtr.join();
	gen_msh.join();
 */
	for (tuple_triple_map triple : triples)
	{
		for (auto triple_pair : triple)
		{
			std::tuple<int, int, int> key = triple_pair.first;
			created[key] = triple[key];
		}
	}
	for (overflow_map neigh : overflows)
	{
		for (auto triple_pair : neigh)
		{
			std::tuple<int, int, int> key = triple_pair.first;
			neighbors[key] = neigh[key];
		}
	}
	tuple_set forests = fst_triple[std::make_tuple(map_x, map_y, Fst)];
	tuple_set mountains = mtn_triple[std::make_tuple(map_x, map_y, Mtn)];
	tuple_set water = wtr_triple[std::make_tuple(map_x, map_y, Wtr)];
	tuple_set marsh = msh_triple[std::make_tuple(map_x, map_y, Msh)];
	std::cout << "JOINED";
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
	std::vector<tuple_set> to_return_sets = { forests,mountains,water,marsh };
	return std::make_tuple(to_return_sets, map);
}


