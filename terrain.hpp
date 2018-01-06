//
//  terrain.hpp
//  take2
//
//  Created by asdfuiop on 8/13/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#ifndef terrain_hpp
#define terrain_hpp

void tuple_set_expand(tuple_set& in, tuple_int& coord);
void tuple_set_remove(tuple_set& in, tuple_int& coord);
void snake_expand(tuple_set& in, tuple_int& coord, tuple_int& dir);
std::tuple<int,int,int,int> det_bounds(const int& x_size,
                                       const int& y_size,
                                       const int& side_size,
                                       tuple_set& N, tuple_set& E,
                                       tuple_set& S, tuple_set& W);
void print_tuple(tuple_set toprint);
void tuple_set_union(tuple_set& in1, tuple_set& in2);
void bresenham(tuple_set& in, int startx, int endx, int starty, int endy);
void bresenham_expand(tuple_set& in, tuple_int& coord, float& angle);

#endif /* terrain_hpp */
