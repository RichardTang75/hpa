//
//  mapgen.h
//  why
//
//  Created by asdfuiop on 7/12/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#ifndef mapgen_h
#define mapgen_h

#include "typedef.hpp"

vectormap map_controller
(const int& map_x, const int& map_y,
	const int& map_width, const int& map_height,
	tuple_triple_map& maps,
	tuple_set& created,
	tuple_set& processed,
	std::unordered_map<tuple_int, std::vector<int>, boost::hash<tuple_int>>& bases,
	std::unordered_map<tuple_int, std::unordered_map<tuple_int, char, boost::hash<tuple_int>>, boost::hash<tuple_int>>& seamlessness,
	std::unordered_map<tuple_int, tuple_set, boost::hash<tuple_int>> bound_coords,
	unsigned long long init_seed = 0
);
#endif /* mapgen_h */
