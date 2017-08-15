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

void gen_terrain(const int& xStart, const int& xEnd,
                 const int& yStart, const int& yEnd,
                 const int& howmany_min, const int& howmany_max,
                 const int& directions_min, const int& directions_max,
                 const int& length_min,  const int& length_max,
                 const int& size_min,    const int& size_max,
                 const int& branch_min, const int& branch_max,
                 tuple_set& to_return,
                 unsigned seed = 0);


#endif /* mapgen_h */
