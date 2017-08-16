//
//  a_star.cpp
//  take2
//
//  Created by asdfuiop on 8/8/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#include "pathfinding.hpp"
#include "goodfunctions.hpp"
#include <iostream>

struct queue_item
{
    float heuristic;
    float cost_so_far;
    float cost_to_move;
    tuple_int which_one;
    tuple_int where_from;
};
class distcomp
{
public:
    bool operator()(queue_item& lhs,queue_item& rhs)
    {
        //first float is heuristic, second float is dist travelled to that point, tuple int is the point compared
        float cost_left=lhs.heuristic+lhs.cost_so_far;
        float cost_right=rhs.heuristic+rhs.cost_so_far;
        if (cost_left == cost_right) {return (lhs.cost_so_far>rhs.cost_so_far);}
        else {return (cost_left>cost_right);}
    }
};
inline queue_item determine_queue (vectormap& map, tuple_int& current,tuple_int& consider, tuple_int& to,
                                   bool& diag, float& cost_to_here, std::vector<int> costs)
{
    queue_item to_return;
    to_return.heuristic=calc_cost(consider,to);
    to_return.which_one=consider;
    to_return.where_from=current;
    if (diag==false)
    {
        to_return.cost_to_move=(costs[  (map[std::get<1>(consider)][std::get<0>(consider)]) ]+
                                costs[  (map[std::get<1>(current)][std::get<0>(current)])   ])/float(2);
        to_return.cost_so_far=cost_to_here+to_return.cost_to_move;
    }
    else
    {
        to_return.cost_to_move=1.5*(costs[  (map[std::get<1>(consider)][std::get<0>(consider)]) ]+
                                    costs[  (map[std::get<1>(current)][std::get<0>(current)])    ])/float(2);
        to_return.cost_so_far=cost_to_here+to_return.cost_to_move;
    }
    return to_return;
}

std::vector<std::tuple<tuple_int, float>> a_pathfind_controller(vectormap& map, tuple_int& from, tuple_int& to,
																std::vector<int>& terrain_costs, int offsetX = 0, int offsetY=0, 
																int wrap=0, int seamless=0)
{
    std::priority_queue<queue_item,std::vector<queue_item>,distcomp> openlist;
    std::unordered_set<tuple_int,boost::hash<tuple_int>> open_dups;
    tuple_set closedlist;
    std::vector<std::tuple<tuple_int,float>> to_return;

    std::vector<tuple_int> diag_directions=
    {
        tuple_int(-1,-1),tuple_int(-1,1),tuple_int(1,-1),tuple_int(1,1),
    };
    std::vector<tuple_int> directions=
    {
        tuple_int(-1,0),tuple_int(0,-1),tuple_int(0,1),tuple_int(1,0)
    };
	int start_x = std::get<0>(from) - offsetX;
	int start_y = std::get<1>(from) - offsetY;
	int end_x = std::get<0>(to) - offsetX;
	int end_y = std::get<1>(to) - offsetY;
	tuple_int mod_start = tuple_int(start_x, start_y);
	tuple_int mod_end = tuple_int(end_x, end_y);
	tuple_int current = mod_start;
	if (terrain_costs[(map[std::get<1>(mod_start)][std::get<0>(mod_start)])] == 0 or 
		(terrain_costs[(map[std::get<1>(mod_end)][std::get<0>(mod_end)])] == 0))
	{
		return to_return;
	}
    queue_item temp;
    std::unordered_map<tuple_int,std::tuple<tuple_int,float>,boost::hash<tuple_int>> visits;
    bool success=false;
    float costs_to_here=0;
    bool diag;
    while (success==false)
    {
        if (current==mod_end)
        {
            success=true;
        }
        else if(openlist.size()==0)
        {
            //only for starting. preferably.
            diag=false;
            for (tuple_int dir:directions)
            {
                tuple_int consider=tuple_int(std::get<0>(dir)+std::get<0>(current),std::get<1>(dir)+std::get<1>(current));
                if (closedlist.count(consider)==0 && in_bounds(0, map[0].size(), 0, map.size(), consider)&&
                    open_dups.count(consider)==0 && terrain_costs[  (map[std::get<1>(consider)][std::get<0>(consider)]) ]!=0)
                {
                    temp=determine_queue(map, current, consider, to, diag, costs_to_here, terrain_costs);
                    open_dups.emplace(consider);
                    openlist.push(temp);
                }
            }
            diag=true;
            for (tuple_int dir:diag_directions)
            {
                tuple_int consider=tuple_int(std::get<0>(dir)+std::get<0>(current),std::get<1>(dir)+std::get<1>(current));
                if (closedlist.count(consider)==0 && in_bounds(0, map[0].size(), 0, map.size(), consider) &&
                    open_dups.count(consider)==0 && terrain_costs[  (map[std::get<1>(consider)][std::get<0>(consider)]) ]!=0)
                {
                    temp=determine_queue(map, current, consider, to, diag, costs_to_here, terrain_costs);
                    open_dups.emplace(consider);
                    openlist.push(temp);
                }
            }
            if (openlist.size()==0)
            {
                return to_return;
            }
        }
        else
        {
            queue_item top=openlist.top();
            openlist.pop();
            //the first one of the element was used for determining the top only
            costs_to_here=top.cost_so_far;
            visits[top.which_one]=std::make_tuple(top.where_from,top.cost_to_move);
            current=top.which_one;
            diag=false;
            closedlist.emplace(current);
            for (tuple_int dir:directions)
            {
                tuple_int consider=tuple_int(std::get<0>(dir)+std::get<0>(current),std::get<1>(dir)+std::get<1>(current));
                if (closedlist.count(consider)==0 && in_bounds(0, map[0].size(), 0, map.size(), consider) &&
                    open_dups.count(consider)==0 && terrain_costs[  (map[std::get<1>(consider)][std::get<0>(consider)]) ]!=0)
                {
                    temp=determine_queue(map, current, consider, to, diag, costs_to_here, terrain_costs);
                    open_dups.emplace(consider);
                    openlist.push(temp);
                }
            }
            diag=true;
            for (tuple_int dir:diag_directions)
            {
                tuple_int consider=tuple_int(std::get<0>(dir)+std::get<0>(current),std::get<1>(dir)+std::get<1>(current));
                if (closedlist.count(consider)==0 && in_bounds(0, map[0].size(), 0, map.size(), consider) &&
                    open_dups.count(consider)==0 && terrain_costs[  (map[std::get<1>(consider)][std::get<0>(consider)]) ]!=0)
                {
                    temp=determine_queue(map, current, consider, to, diag, costs_to_here, terrain_costs);
                    open_dups.emplace(consider);
                    openlist.push(temp);
                }
            }
        }
    }
    std::tuple<tuple_int,float> coord_came_and_cost=visits[mod_end];
    tuple_int path=std::get<0>(coord_came_and_cost);
    to_return.reserve(int(calc_cost(from,to)));
    //cost that is added is for going from one step to the next one. at the end there is no need for a cost.
    //further additions can be done by other functions. probably.
    to_return.push_back(std::make_tuple(to,0));
	int mod_path_x = std::get<0>(path) + offsetX;
	int mod_path_y = std::get<1>(path) + offsetY;
	tuple_int mod_path = tuple_int(mod_path_x, mod_path_y);
    to_return.push_back(std::make_tuple(path,std::get<1>(coord_came_and_cost)));
    while (path != mod_start)
    {
        coord_came_and_cost=visits[path];
        path=std::get<0>(coord_came_and_cost);
		int mod_path_x = std::get<0>(path)+offsetX;
		int mod_path_y = std::get<1>(path)+offsetY;
		tuple_int mod_path = tuple_int(mod_path_x, mod_path_y);
        to_return.push_back(std::make_tuple(mod_path,std::get<1>(coord_came_and_cost)));
    }
    return to_return;
}
