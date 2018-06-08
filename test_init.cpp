//
//  test_init.cpp
//  take2
//
//  Created by asdfuiop on 8/20/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#include "typedef.hpp"
#include "pathfinding.hpp"
#include "hierarchical_pathfind.hpp"
#include <iostream>
#include "country_controller.hpp"
//warning incredibly cpu intensive :l
void map_stuff(int width, int height, vectormap& bigmap, tuple_set& all_node_locs)
{
	std::vector<std::vector<int>> possible_move_costs =
	{
		{1,2,3,4,0}, //basic move cost-used by fire and metal
		{1,1,2,4,0}, //wood
		{1,2,2,4,2}, //water
		{1,2,2,4,0} //earth
	};
	std::vector<int> basic = { 1,2,3,4,0 };
	int cut_size = 64;
	std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>> map_set = cut(bigmap, width, height, cut_size);
	node_retrieval nodes = entrances(map_set, bigmap, cut_size, false, 0, 0, width / cut_size, height / cut_size, possible_move_costs);
    // purely for visualization purposes
	/*
    for (auto node_key: nodes.all_nodes)
    {
        if ((node_key.first).t_mobility==basic)
        {
            all_node_locs.emplace((node_key.first).location);
        }
    }
    // purely for visualization purposes
	tuple_int start = tuple_int(10, 10);
	tuple_int end = tuple_int(510, 510);
	path_with_cost path_and_cost = hierarchical_pathfind(end, start, basic, map_set, 1, cut_size, nodes);
	std::cout << "Done";
	std::cout << "\n" << path_and_cost.size();
	int count = 0;
    for (int i=0; i<path_and_cost.size();i++)
    {
        if (count>1000)
        {
            break;
        }
        ++count;
        int x=std::get<0>(std::get<0>(path_and_cost[i]));
        int y=std::get<1>(std::get<0>(path_and_cost[i]));
        all_node_locs.emplace(x,y);
		std::cout << "\n"  << x << "," << y;
    }
	*/
}
