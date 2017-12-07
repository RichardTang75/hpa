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
<<<<<<< HEAD
std::vector<tuple_int> directions=
{
    tuple_int(-1,-1),tuple_int(-1,0),tuple_int(-1,1),
    tuple_int(0,-1),                tuple_int(0,1),
    tuple_int(1,-1),tuple_int(1,0),tuple_int(1,1)
};

enum Cardinal {Northwest, West, Southwest,
                North,          South,
                Northeast, East, Southeast};
=======
>>>>>>> 4918a06dd6a7386429b997948ad1c36a30d0fd7f
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
inline tuple_set bresenham(int startx, int endx, int starty, int endy)
{
	int dx = endx - startx;
	int dy = endy - starty;
	int dist = 2 * dy - dx;
	int y = starty;
	tuple_set line_parts;
<<<<<<< HEAD
	for (int x = startx; x < (endx + 1); ++x)
=======
	for (int x = 0; x < (endx + 1); ++x)
>>>>>>> 4918a06dd6a7386429b997948ad1c36a30d0fd7f
	{
		line_parts.emplace(x, y);
		while (dist > 0)
		{
			y = y + 1;
			dist = dist - 2 * dx;
		}
		dist = dist + 2 * dy;
	}
<<<<<<< HEAD
    return line_parts;
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
=======
>>>>>>> 4918a06dd6a7386429b997948ad1c36a30d0fd7f
}
void step1(const int& xStart, const int& xEnd,
	const int& yStart, const int& yEnd,
	const int& howmany_min, const int& howmany_max,
	const int& directions_min, const int& directions_max,
	const int& length_min, const int& length_max,
<<<<<<< HEAD
    tuple_set& to_return, std::vector<tuple_set>& neighbors,
=======
	tuple_set& to_return,
>>>>>>> 4918a06dd6a7386429b997948ad1c36a30d0fd7f
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
<<<<<<< HEAD
	std::uniform_real_distribution<float> angle(0, (2*acos(-1)));
=======
	std::uniform_real_distribution<float> angle(0, 2*acos(-1));
>>>>>>> 4918a06dd6a7386429b997948ad1c36a30d0fd7f
	std::vector<tuple_int> additions = { tuple_int(-1,0),tuple_int(1,0),tuple_int(0,-1),tuple_int(1,0) };
	int how_many = rand_many(eng);
	int temp_many_directions;
	float angle_chosen;
	int tempX, tempY;
<<<<<<< HEAD
    int end_x, end_y;
	int cur_x, cur_y;
    while (to_return.size() < how_many)
=======
	int d_x, d_y;
	int end_x, end_y;
	int cur_x, cur_y;
	int size_many;
	while (to_return.size() < how_many)
>>>>>>> 4918a06dd6a7386429b997948ad1c36a30d0fd7f
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
<<<<<<< HEAD
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
            tuple_set bresen=bresenham(std::min(cur_x, end_x), std::max(cur_x, end_x),
                                       std::min(cur_y, end_y), std::max(cur_y, end_y));
            //final step takes care of over the bounds from the tuple set
			tuple_set_union(to_union, bresen);
=======
			end_x = cos(angle_chosen)*radius;
			end_y = sin(angle_chosen)*radius;
			cur_x = std::get<0>(coord);
			cur_y = std::get<1>(coord);
			if (end_x > xEnd)
			{

			}
			else if(end_x < xStart)
			{

			}
			if (end_y > yEnd)
			{

			}
			else if (end_y < yStart)
			{

			}
			tuple_set_union(to_union,bresenham(std::min(cur_x, end_x), std::max(cur_x, end_x),
												std::min(cur_y, end_y), std::max(cur_y, end_y)));
>>>>>>> 4918a06dd6a7386429b997948ad1c36a30d0fd7f
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
<<<<<<< HEAD
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
                tuple_set bresen=bresenham(std::min(cur_x, end_x), std::max(cur_x, end_x),
                                           std::min(cur_y, end_y), std::max(cur_y, end_y));
                //final step takes care of over the bounds from the tuple set
                tuple_set_union(to_union, bresen);
=======
				int temp_length = length_many(eng);
				direction_chosen = directions[direction_chooser(eng)];
				d_x = std::get<0>(direction_chosen);
				d_y = std::get<1>(direction_chosen);
				for (int i = 0; i<temp_length; ++i)
				{
					cur_x = std::get<0>(coord);
					cur_y = std::get<1>(coord);
					to_union.emplace(tuple_int(cur_x + (d_x*i), cur_y + (d_y*i)));
				}
>>>>>>> 4918a06dd6a7386429b997948ad1c36a30d0fd7f
			}
		}
	}
	tuple_set_union(to_return, to_union);
	to_union.clear();
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
<<<<<<< HEAD
=======
    std::vector<tuple_int> directions=
    {
        tuple_int(-1,-1),tuple_int(-1,0),tuple_int(-1,1),
        tuple_int(0,-1),                tuple_int(0,1),
        tuple_int(1,-1),tuple_int(1,0),tuple_int(1,1)
    };
>>>>>>> 4918a06dd6a7386429b997948ad1c36a30d0fd7f
    int how_many=rand_many(eng);
    tuple_int direction_chosen;
    int temp_many_directions;
    int tempX, tempY;
    int d_x, d_y;
    int cur_x, cur_y;
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
<<<<<<< HEAD
//	for (tuple_int point : summed)
//	{
//		int cur_x = std::get<0>(point);
//		int cur_y = std::get<1>(point);
//		for (tuple_int dirs : additions)
//		{
//			int dx = std::get<0>(dirs);
//			int dy = std::get<1>(dirs);
//			tuple_int temp_coord = tuple_int(cur_x + dx, cur_y + dy);
//			if (summed.count(temp_coord) == 0)
//			{
//				final_step.emplace(temp_coord);
//			}
//		}
//	}
=======
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
>>>>>>> 4918a06dd6a7386429b997948ad1c36a30d0fd7f
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
<<<<<<< HEAD
    //debugging processes and leftover only
//	for (tuple_int coord : final_step)
//	{
//		int tempX = std::get<0>(coord);
//		int tempY = std::get<1>(coord);
//		if (in_bounds(x_start, x_end, y_start, y_end, tempX, tempY) == true)
//		{
//			walls[tempY][tempX] = 1;
//		}
//	}
//	array_img(map, image, rows, cols, terrain_type);
//	array_img(walls, wallimage, rows, cols, terrain_type);
//	std::vector<unsigned char> png;
//	std::vector<unsigned char> png2;
//	lodepng::State state;
//	lodepng::State state2;
//	unsigned error = lodepng::encode(png, image, rows, cols, state);
//	if (!error)
//	{
//		lodepng::save_file(png, "cplusplus.png");
//	}
//	else
//	{
//		std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << "\n";
//	}
//	unsigned nerror = lodepng::encode(png2, wallimage, rows, cols, state2);
//	if (!nerror)
//	{
//		lodepng::save_file(png2, "cplusplus2.png");
//	}
//	else
//	{
//		std::cout << "encoder error " << nerror << ": " << lodepng_error_text(nerror) << "\n";
//	}
	std::vector<tuple_set> to_return_sets = { forests,mountains,water,marsh,ret_north,ret_east,ret_south,ret_west };
	return std::make_tuple(to_return_sets, map);
}

void retrieve_maps(int map_x, int map_y, const int& x_size, const int& y_size,
                   std::unordered_map<tuple_int,tuple_set, boost::hash<tuple_int>>& neighbors,
                   std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>>& maps,
                   tuple_set& created,
                   int terrain_type = 0, unsigned long long init_seed = 0)
{
    std::vector<tuple_set> pert_neighbors;
    for (tuple_int dir:directions)
    {
        tuple_int nearby=(std::get<0>(dir)+map_x,std::get<1>(dir)+map_y);
        if (created.count(nearby)==1)
        {
            pert_neighbors.push_back(neighbors[nearby]);
        }
//        else
//        {
//            //neighbors[Northwest], neighbors[North], etc from the enum cardinal
//            std::vector<tuple_set> neighbors_to_pass_in={neighbors(std::get<0>(nearby)-1, std:)];
//            step1(0, 512, 0, 512, <#const int &howmany_min#>, <#const int &howmany_max#>, <#const int &directions_min#>, <#const int &directions_max#>, <#const int &length_min#>, <#const int &length_max#>, <#tuple_set &to_return#>, <#std::vector<tuple_set> &neighbors#>)
//        }
    }
}

/*
unordered_map of every square-> neighbors
unordered map of every square->base map
to get one map, you get retrieve base maps and each of neighbor's maps. thus for every one, you need 9 made.
*/
=======

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
>>>>>>> 4918a06dd6a7386429b997948ad1c36a30d0fd7f
