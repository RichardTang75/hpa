//
//  goodfunctions.hpp
//  why
//
//  Created by asdfuiop on 7/12/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#ifndef goodfunctions_hpp
#define goodfunctions_hpp

#include "typedef.hpp"
float dist_squared(tuple_int& to, tuple_int& from);
bool in_bounds(const int& x_start, const int& x_end,
               const int& y_start, const int& y_end,
               tuple_int& coord);
bool in_bounds(const int& x_start, const int& x_end,
                      const int& y_start, const int& y_end,
                      const int& tempX, const int&tempY);
#endif /* goodfunctions_hpp */
