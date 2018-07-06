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
typedef std::vector<std::vector<int>> vectormap;
typedef std::tuple<tuple_set,int> map_tuple;
typedef std::unordered_map< std::tuple<int, int, int>, std::vector<tuple_set>, boost::hash<std::tuple<int, int, int>>> overflow_map;
typedef std::unordered_map< std::tuple<int, int, int>, tuple_set, boost::hash<std::tuple<int, int, int>>> tuple_triple_map;

enum Terrain { Grs, Fst, Msh, Mtn, Wtr };

#endif /* typedef_hpp */
