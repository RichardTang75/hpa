//
//  a_star.hpp
//  take2
//
//  Created by asdfuiop on 8/8/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#ifndef a_star_hpp
#define a_star_hpp

#include <stdio.h>
std::vector<std::tuple<tuple_int, float>> a_pathfind_controller(vectormap& map, tuple_int& from, tuple_int& to,
																std::vector<int>& terrain_costs, int offsetX = 0, int offsetY = 0,
																int wrap = 0, int seamless = 0);

#endif /* a_star_hpp */
