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

std::tuple<std::vector<tuple_set>, vectormap> map_controller
															(const int& map_x, const int& map_y,
																const int& x_size, const int& y_size,
																std::unordered_map< std::tuple<int, int, int>, tuple_set, boost::hash<std::tuple<int, int, int>>>& created,
																std::unordered_map< std::tuple<int, int, int>, std::vector<tuple_set>, boost::hash<std::tuple<int, int, int>>>& neighbors,
																unsigned long long init_seed = 0,
																int terrain_type = 0);

#endif /* mapgen_h */
