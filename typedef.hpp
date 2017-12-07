//
//  typedef.hpp
//  why
//
//  Created by asdfuiop on 7/12/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#ifndef typedef_hpp
#define typedef_hpp

#include <vector>
#include <unordered_set>
#include <tuple>
#include <boost/functional/hash.hpp>
#include <unordered_map>

typedef std::tuple<int, int> tuple_int;
typedef std::unordered_set<tuple_int, boost::hash<tuple_int>> tuple_set;
typedef std::tuple<tuple_set,int> map_tuple;
typedef std::vector<std::vector<int>> vectormap;

#endif /* typedef_hpp */
