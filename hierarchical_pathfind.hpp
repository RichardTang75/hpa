//  hierarchical_pathfind.hpp
//  take2
//
//  Created by asdfuiop on 7/22/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#ifndef hierarchical_pathfind_hpp
#define hierarchical_pathfind_hpp

#include "pathfinding.hpp"
std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>> cut(vectormap& to_cut, int map_width,
	int map_height, int cut_size);
path_with_cost hierarchical_pathfind(tuple_int& to, tuple_int& from, std::vector<int>& movecosts,
									std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>>& mapset,
									int max_depth, int cut_size, node_retrieval& every_node_map);
node_retrieval entrances(std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>>& map_set,
						vectormap& big_map, int& cut_size,
						bool wrap, int min_x, int min_y, int max_x, int max_y,
						std::vector<std::vector<int>>& vec_of_terrain_costs);
#endif /* hierarchical_pathfind_hpp */
