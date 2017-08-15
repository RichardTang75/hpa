//
//  pathfinding.hpp
//  take2
//
//  Created by asdfuiop on 8/13/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#ifndef pathfinding_hpp
#define pathfinding_hpp

#include <queue>
#include "typedef.hpp"

typedef std::vector<std::tuple<tuple_int,float>> path_with_cost;

tuple_int which_map (tuple_int& location, int depth, int cut_size=8);
float calc_cost(tuple_int& current, tuple_int& to);
struct pf_node_key
{
    tuple_int location;
    std::vector<int> t_mobility;
    int depth;
    bool operator==(const pf_node_key& other) const
    {
        return (location==other.location && t_mobility==other.t_mobility && depth==other.depth);
    }
    struct hash
    {
        size_t operator() (const pf_node_key& pf) const
        {
            std::size_t seed=0;
            boost::hash_combine(seed, pf.location);
            boost::hash_combine(seed, pf.t_mobility);
            boost::hash_combine(seed, pf.depth);
            return seed;
        }
    };
};
struct local_node_key
{
    tuple_int map_coord;
    int depth;
    std::vector<int> t_mobility;
    bool operator==(const local_node_key& other) const
    {
        return (map_coord==other.map_coord && depth==other.depth && t_mobility==other.t_mobility);
    }
    struct hash
    {
        size_t operator() (const local_node_key& key) const
        {
            std::size_t seed=0;
            boost::hash_combine(seed,key.map_coord);
            boost::hash_combine(seed,key.depth);
            boost::hash_combine(seed,key.t_mobility);
            return seed;
        }
    };
};
struct pf_edge
{
    pf_node_key key_to;
    float cost;
    path_with_cost path;
};
//you can make pf_nodes out of pf_nodes. combine their edges
struct pf_node
{
    //if it reaches the vectormap in which the target is in, then it starts it again with the next lowest level;
    tuple_int location; //based on global
    int depth;
    tuple_int which_vectormap;
    std::vector<int> t_mobility;
    std::vector<pf_edge> associated_edges;
    bool operator==(const pf_node& other) const
    {
        return (location==other.location && depth==other.depth && t_mobility==other.t_mobility);
    }
    struct hash
    {
        size_t operator() (const pf_node& pf) const
        {
            std::size_t seed=0;
            boost::hash_combine(seed, pf.location);
            boost::hash_combine(seed, pf.depth);
            return seed;
        }
    };
};
struct node_retrieval{
    std::unordered_map<pf_node_key,pf_node,pf_node_key::hash> all_nodes;
    std::unordered_map<local_node_key,std::vector<pf_node>,local_node_key::hash> local_nodes;
};
#endif /* pathfinding_hpp */
