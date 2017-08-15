//
//  hierarchical_pathfind.cpp
//  take2
//
//  Created by asdfuiop on 7/22/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//
//i already wrote it, but now i think structs would've been better
//for future reference, all tuple_ints are x,y, queue_tuple is dist, cost, which, from, and cost_to_move
//yo so I don't want to deal with hash_combine, so I won't be implementing a struct for coordinates.
//CHANGE TERRAIN COSTS TO STD::VECTOR, INDEX=MAP NUM, NUMBER THERE WILL BE COST
//Cost of unmoveable tiles=0; if 0, then no movement
#include "typedef.hpp"
#include "a_star.hpp"
#include "pathfinding.hpp"
#include <iostream>
//parameterize the mobility vector, find a way to only have to store one copy instead of two for forwards and back
/////reference only
std::vector<int> terrain_costs{1,2,3,4,0};
////
int MAX_DEPTH;

enum where{
    TOP, RIGHT, BOTTOM, LEFT
};

struct node_queue
{
    pf_node next_stop;
    int index_edge;
    float cost_to_here;
    float estimated_cost_to_there;
};
class distcomp
{
public:
    bool operator()(node_queue& lhs,node_queue& rhs)
    {
        //first float is heuristic, second float is dist travelled to that point, tuple int is the point compared
        float cost_left=lhs.estimated_cost_to_there+lhs.cost_to_here;
        float cost_right=rhs.estimated_cost_to_there+rhs.cost_to_here;
        if (cost_left == cost_right) {return (lhs.cost_to_here>rhs.cost_to_here);}
        else {return (cost_left>cost_right);}
    }
};
//std::unordered_map<pf_node_key, pf_node,pf_node_key::hash> all_nodes;
//std::unordered_map<local_node_key,std::vector<pf_node>,local_node_key::hash> all_local_nodes;

//average cost between entrance+exit
//perhaps add diagonal traversal?
//maybe return all 4 at once
std::vector<int> get_edge(vectormap& current, where which_one, bool& mine)
{
    std::vector<int> to_return;
    if (mine==true)
    {
        switch (which_one)
        {
            case TOP:
            {
                return current[0];
            }
            case LEFT:
            {
                to_return.reserve(current[0].size());
                for (int i=0; i<current.size(); ++i){
                    to_return.push_back(current[i][0]);
                }
                return to_return;
            }
            case RIGHT:
            {
                to_return.reserve(current[0].size());
                int rightmost=(current[0].size()-1);
                for (int i=0; i<current.size(); ++i){
                    to_return.push_back(current[i][rightmost]);
                }
                return to_return;
            }
            case BOTTOM:
            {
                return current[(current.size()-1)];
            }
        }
    }
    else{
        switch (which_one)
        {
            case BOTTOM:
            {
                return current[0];
            }
            case RIGHT:
            {
                to_return.reserve(current[0].size());
                for (int i=0; i<current.size(); ++i){
                    to_return.push_back(current[i][0]);
                }
                return to_return;
            }
            case LEFT:
            {
                to_return.reserve(current[0].size());
                int rightmost=(current[0].size()-1);
                for (int i=0; i<current.size(); ++i){
                    to_return.push_back(current[i][rightmost]);
                }
                return to_return;
            }
            case TOP:
            {
                return current[(current.size()-1)];
            }
        }
    }
}
//first and second must be same length. but why wouldn't they be
//the first one and second one should give the same value, but it'd be a pain to pass it on. therefore each one does it themselves.
//returns place in vector to cross.
//interedges
std::vector<tuple_int> cheapest(std::vector<int>& vec_terrain_costs, std::vector<int>& mine, std::vector<int>& other)
{
    int bestcost=90; //this is a massive move cost that would never be seen. probably should've used -1, but w/e
    int currentcost;
    std::vector<tuple_int> best;
    std::vector<tuple_int> to_return;
    for (int j=0; j<mine.size(); ++j)
    {
        for (int checks=-1; checks<2; ++checks)
        {
            if (checks+j>0 && (checks+j)<other.size())
            {
                if (checks==0)
                {
                    currentcost=(vec_terrain_costs[mine[j]]+vec_terrain_costs[other[checks+j]])/2;
                    if (currentcost<bestcost)
                    {
                        bestcost=currentcost;
                        best.clear();
                        best.push_back(tuple_int(j,checks+j));
                    }
                    else if (currentcost==bestcost)
                    {
                        best.push_back(tuple_int(j,checks+j));
                    }
                }
                else
                {
                    currentcost=1.5*(vec_terrain_costs[mine[j]]+vec_terrain_costs[other[checks+j]])/2;
                    if (currentcost<bestcost)
                    {
                        bestcost=currentcost;
                        best.clear();
                        best.push_back(tuple_int(j,checks+j));
                    }
                    else if (currentcost==bestcost)
                    {
                        best.push_back(tuple_int(j,checks+j));
                    }
                }
            }
        }
    }
    if (best.size()>1)
    {
        int index;
        if (best.size()>4)
        {
            index=best.size()/3;
            to_return.push_back(best[index]);
            index=best.size()*2/3;
            to_return.push_back(best[index]);
            return to_return;
        }
        else
        {
            index=best.size()/2;
            to_return.push_back(best[index]);
        }
    }
    return best;
};
//assumes a square map divisible by 8 or 16, preferably twice
std::unordered_map<tuple_int,vectormap,boost::hash<tuple_int>> cut(vectormap& to_cut, int x_begin,
                                                                   int y_begin, int cut_size)
//to_return.
{
    std::vector<int> row(to_cut[0].size()/cut_size);
    std::vector<int> temprow(to_cut[0].size()/cut_size);
    vectormap tempmap((to_cut.size()/cut_size),row);
    std::unordered_map<tuple_int,vectormap,boost::hash<tuple_int>> to_return;
    for (int ystart=y_begin; ystart<(y_begin+to_cut.size()); ystart+=cut_size){
        for (int xstart=x_begin; xstart<(to_cut[0].size()+x_begin); xstart+=cut_size)
        {
            for (int j=ystart; j<(ystart+cut_size);++j)
            {
                for (int i=xstart; i<(xstart+cut_size);++i)
                {
                    int index=(i-xstart);
                    temprow[index];
                }
                int index=(j-ystart);
                tempmap[index]=temprow;
            }
            //ystart/cut_size=y emplacement, xstart/cut_size=x emplacement
            to_return[tuple_int(xstart/cut_size,ystart/cut_size)]=tempmap;
        }
    }
    return to_return;
}
//if space is an issue, just provide the nodes when asked
path_with_cost pf_node_pathfind(pf_node& start, tuple_int& to,
                                std::unordered_map<tuple_int,vectormap,boost::hash<tuple_int>>& base_map_set,
                                std::unordered_map<pf_node_key, pf_node,pf_node_key::hash>& all_nodes,
                                int& depth)
{
    std::priority_queue<node_queue,std::vector<node_queue>,distcomp> open_list;
    std::unordered_set<tuple_int,boost::hash<tuple_int>> open_dupls;
    std::unordered_set<tuple_int,boost::hash<tuple_int>> closed_list;
    std::unordered_map<pf_node,pf_node,pf_node::hash> visits;
    path_with_cost to_return;
    path_with_cost other_stuff;
    node_queue temp;
    node_queue next;
    pf_node current=start;
    pf_node previous;
    pf_node path;
    bool success=false;
    while (success==false)
    {
        std::cout<<depth<<"shit"<<"mierda\n";
        if (depth>0)
        {
            if (current.location==to)
            {
                success=true;
                path=previous;
            }
            else if(which_map(current.location,depth)==which_map(to,depth))
            {
                int new_depth=depth-1;
                pf_node_key new_start_key;
                new_start_key.depth=(new_depth);
                new_start_key.location=current.location;
                new_start_key.t_mobility=current.t_mobility;
                pf_node new_start=all_nodes[new_start_key];
                to_return=pf_node_pathfind(new_start,to,base_map_set,all_nodes,(new_depth));
            }
            else if(open_list.size()==0){
                for (int edge_i=0; edge_i<current.associated_edges.size(); ++edge_i)
                {
                    pf_edge waypoint=current.associated_edges[edge_i];
                    temp.next_stop=all_nodes[waypoint.key_to];
                    if (closed_list.count(temp.next_stop.location)!=0 && open_dupls.count(temp.next_stop.location) != 0)
                    {
                        temp.cost_to_here=waypoint.cost;
                        temp.index_edge=edge_i;
                        temp.estimated_cost_to_there=calc_cost(temp.next_stop.location,to);
                        open_dupls.emplace(temp.next_stop.location);
                        open_list.push(temp);
                    }
                }
                closed_list.emplace(current.location);
                previous=current;
                if (open_list.size()==0)
                {
                    std::cout<<"sucks";
                    return to_return;
                }
            }
            else
            {
                next=open_list.top();
                open_list.pop();
                current=next.next_stop;
                visits[current]=previous;
                float cost_so_far=next.cost_to_here;
                for (int edge_i=0; edge_i<current.associated_edges.size(); ++edge_i)
                {
                    pf_edge waypoint=current.associated_edges[edge_i];
                    temp.next_stop=all_nodes[waypoint.key_to];
                    if (closed_list.count(temp.next_stop.location)!=0 && open_dupls.count(temp.next_stop.location) != 0)
                    {
                        temp.cost_to_here=waypoint.cost+cost_so_far;
                        temp.index_edge=edge_i;
                        temp.estimated_cost_to_there=calc_cost(temp.next_stop.location,to);
                        open_dupls.emplace(temp.next_stop.location);
                        open_list.push(temp);
                    }
                }
                closed_list.emplace(current.location);
                previous=current;
            }
        }
        else
        {
            to_return=a_pathfind_controller(base_map_set[which_map(to,0)], current.location, to, start.t_mobility);
            success=true;
            //builds off of this
        }
    }
    path=previous;
    pf_node path_next;
    path_next=visits[path];
    for (pf_edge edge:path.associated_edges){
        if (all_nodes[edge.key_to]==previous)
        {
            to_return.insert(to_return.end(),edge.path.begin(), edge.path.end());
        }
    }
    while (!(path == start)) //checkmate.
    {
        path=path_next;
        path_next=visits[path];
        for (pf_edge edge:path.associated_edges){
            if (all_nodes[edge.key_to]==previous)
            {
                to_return.insert(to_return.end(),edge.path.begin(), edge.path.end());
                break;
            }
        }
    }
    return to_return;
}

//node_retrieval node_entrances()
//{
//    //get the nodes on the edges
//    //node pathfind from node to node
//    //store nodes
//}

node_retrieval entrances(std::unordered_map<tuple_int,vectormap,boost::hash<tuple_int>>& map_set,
                         bool wrap, int min_x, int min_y, int max_x, int max_y,
                         std::vector<std::vector<int>>& vec_of_terrain_costs)
{
    //if wrap, try with max if no to the left, etc.
    //perhaps add
    //definitely need to have been cut already. assumes equal sizes n stuff.
    tuple_int which;
    vectormap current_map;
    node_retrieval to_return;
    int cut_size=8;
    int depth=1; //this should be obvious since it's the first pass
    std::vector<tuple_int> directions=
    {
        tuple_int(-1,0),tuple_int(0,-1),tuple_int(0,1),tuple_int(1,0)
    };
    bool mine=true;
    bool notmine=false;
    std::unordered_map<pf_node_key,pf_node,pf_node_key::hash> graph_nodes;
    std::unordered_map<local_node_key,std::vector<pf_node>,local_node_key::hash> nodes_in_map;
    std::vector<pf_node> nodes_in_this_section;
    for (auto which_map:map_set)
    {
        which=which_map.first;
        current_map=which_map.second;
        std::vector<int> from_left,from_right,from_top,from_bottom;
        std::vector<int> my_left,my_right,my_top,my_bottom;
        my_left=get_edge(current_map,where::LEFT,mine);
        my_right=get_edge(current_map,where::RIGHT,mine);
        my_top=get_edge(current_map,where::TOP,mine);
        my_bottom=get_edge(current_map,where::BOTTOM,mine);
        for (tuple_int dir:directions)
        {
            tuple_int nearby=tuple_int(std::get<0>(dir)+std::get<0>(which),std::get<1>(dir)+std::get<1>(which));
            if (wrap==true)
            {
                int wrap_x=std::get<0>(nearby);
                int wrap_y=std::get<1>(nearby);
                if (std::get<0>(nearby)<min_x)
                {
                    wrap_x=max_x;
                }
                else if (std::get<0>(nearby)>max_x)
                {
                    wrap_x=min_x;
                }
                if (std::get<1>(nearby)<min_y)
                {
                    wrap_y=max_y;
                }
                else if (std::get<1>(nearby)>max_y)
                {
                    wrap_y=min_y;
                }
                nearby=tuple_int(wrap_x,wrap_y);
            }
            //aka to the left of current
            if (dir==tuple_int(-1,0)){
                if (map_set.count(nearby)==1)
                {
                    from_left=get_edge(map_set[nearby],where::LEFT,notmine);
                }
            }
            else if (dir==tuple_int(1,0))
            {
                if (map_set.count(nearby)==1)
                {
                    from_right=get_edge(map_set[nearby],where::RIGHT,notmine);
                }
            }
            else if (dir==tuple_int(0,-1))
            {
                if (map_set.count(nearby)==1)
                {
                    from_top=get_edge(map_set[nearby],where::TOP,notmine);
                }
            }
            else
            {
                if (map_set.count(nearby)==1)
                {
                    from_bottom=get_edge(map_set[nearby],where::RIGHT,notmine);
                }
            }
        }
        std::vector<std::vector<int>> vec_from_container={from_left,from_right,from_top,from_bottom};
        std::vector<std::vector<int>> vec_my_container={my_left,my_right,my_top,my_bottom};
        for (std::vector<int> costs: vec_of_terrain_costs)
        {
            std::vector<tuple_int> mod_pos_entrances;
            mod_pos_entrances.clear();//just in case
            std::vector<tuple_int> inter_edges;
            inter_edges.clear();//see above
            for (int i=0; i<vec_from_container.size(); ++i)
            {
                std::vector<int> myvec_comp=vec_my_container[i];
                std::vector<int> fromvec_comp=vec_from_container[i];
                if (fromvec_comp.size()==0)
                {
                    std::cout<<"\nmierda";
                    continue;
                }
                std::vector<tuple_int> possible_entrances=cheapest(costs,myvec_comp,fromvec_comp);
                int half_coord;
                int inter_edge_half;
                if (myvec_comp==from_left or myvec_comp==from_right)
                {
                    if (myvec_comp==from_left)
                    {
                        half_coord=std::get<0>(which)*cut_size;
                        inter_edge_half=(std::get<0>(which)*cut_size)-1;
                    }
                    else if(myvec_comp==from_right)
                    {
                        half_coord=(from_right.size()-1)+(std::get<0>(which)*cut_size);
                        inter_edge_half=from_right.size()+(std::get<0>(which)*cut_size);
                    }
                    for (tuple_int half:possible_entrances)
                    {
                        int my_side=std::get<0>(half)+(std::get<1>(which)*cut_size);
                        int inter_edge_side=std::get<1>(half)+(std::get<1>(which)*cut_size);
                        mod_pos_entrances.push_back(tuple_int(half_coord,my_side));
                        inter_edges.push_back(tuple_int(inter_edge_half,inter_edge_side));
                    }
                }
                else
                {
                    if (myvec_comp==from_top)
                    {
                        half_coord=std::get<1>(which)*cut_size;
                        inter_edge_half=(std::get<1>(which)*cut_size)-1;
                    }
                    else if(myvec_comp==from_bottom)
                    {
                        half_coord=(from_bottom.size()-1)+(std::get<1>(which)*cut_size);
                        inter_edge_half=from_bottom.size()+(std::get<1>(which)*cut_size);                        }
                    for (tuple_int half:possible_entrances)
                    {
                        int my_side=std::get<0>(half)+(std::get<0>(which)*cut_size);
                        int inter_edge_side=std::get<1>(half)+(std::get<0>(which)*cut_size);
                        mod_pos_entrances.push_back(tuple_int(half_coord,my_side));
                        inter_edges.push_back(tuple_int(inter_edge_half,inter_edge_side));
                    }
                }
                for (int i_from=0; i_from<mod_pos_entrances.size(); ++i_from)
                {
                    tuple_int coord_from=mod_pos_entrances[i_from];
                    //form a node first
                    //then build the edges
                    pf_node temp_node;
                    pf_node_key temp_node_key;
                    //shit need to convert back to global
                    temp_node.location=mod_pos_entrances[i_from];
                    temp_node_key.location=temp_node.location;
                    temp_node.depth=depth;
                    temp_node_key.depth=depth;
                    temp_node.t_mobility=costs;
                    temp_node_key.t_mobility=costs;
                    pf_edge inter_edge;
                    inter_edge.key_to.depth=depth;
                    inter_edge.key_to.location=inter_edges[i_from];
                    inter_edge.key_to.t_mobility=costs;
                    inter_edge.path=a_pathfind_controller(current_map,coord_from,inter_edges[i_from],costs);
                    path_with_cost empty_path;
                    float total_cost=0;
                    if (inter_edge.path!=empty_path)
                    {
                        for (std::tuple<tuple_int,float> tile:inter_edge.path)
                        {
                            total_cost+=std::get<1>(tile);
                        }
                    }
                    inter_edge.cost=total_cost;
                    temp_node.associated_edges.clear(); //gotta be safe.
                    temp_node.associated_edges.push_back(inter_edge);
                    for(int i_to=0; i_to<mod_pos_entrances.size();++i_to)
                    {
                        tuple_int coord_to=mod_pos_entrances[i_to];
                        if (coord_from != coord_to)
                        {
                            pf_edge temp_edge;
                            float total_cost=0;
                            temp_edge.key_to.depth=depth;
                            temp_edge.key_to.location=coord_to;
                            temp_edge.key_to.t_mobility=costs;
                            temp_edge.path=a_pathfind_controller(current_map,coord_from,coord_to,costs);
                            if (temp_edge.path!=empty_path)
                            {
                                for (std::tuple<tuple_int,float> tile:temp_edge.path)
                                {
                                    total_cost+=std::get<1>(tile);
                                }
                                temp_edge.cost=total_cost;
                                temp_node.associated_edges.push_back(temp_edge);
                                //                                pf_edge_key temp_edge_key;
                                //                                temp_edge_key.from=coord_from;
                                //                                temp_edge_key.to=coord_to;
                                //                                temp_edge_key.t_mobility=costs;
                                //                                graph_edges[temp_edge_key]=temp_edge;
                            }
                        }
                    }
                    graph_nodes[temp_node_key]=temp_node;
                    nodes_in_this_section.push_back(temp_node);
                }
                local_node_key temp_local_key;
                temp_local_key.map_coord=which;
                temp_local_key.depth=depth;
                temp_local_key.t_mobility=costs;
                nodes_in_map[temp_local_key]=nodes_in_this_section;
                nodes_in_this_section.clear();
            }
        }
    }
    to_return.local_nodes=nodes_in_map;
    to_return.all_nodes=graph_nodes;
    MAX_DEPTH=1;
    return to_return;
}
pf_node closest_pf_node(tuple_int& where_to, tuple_int& where_from, vectormap& where_at, int& depth,
                               std::vector<int>& movecosts,
                               std::unordered_map<local_node_key,std::vector<pf_node>,local_node_key::hash>& local_nodes)
{
    local_node_key access_key;
    access_key.depth=depth;
    access_key.map_coord=which_map(where_to,depth);
    access_key.t_mobility=movecosts;
    std::vector<pf_node> nearby_nodes=local_nodes[access_key];
    float cost;
    float best_cost=-1;
    pf_node best;
    for (pf_node node:nearby_nodes)
    {
        cost=calc_cost(where_from,node.location);
        if (cost<best_cost or best_cost==-1)
        {
            best_cost=cost;
            best=node;
        }
    }
    return best;
}
path_with_cost hierarchical_pathfind(tuple_int& to, tuple_int& from,std::vector<int>& movecosts,
                                    std::unordered_map<tuple_int,vectormap,boost::hash<tuple_int>>& mapset,
                                     int max_depth, node_retrieval& every_node_map)
{
    std::unordered_map<pf_node_key, pf_node,pf_node_key::hash> all_nodes=every_node_map.all_nodes;
    std::unordered_map<local_node_key,std::vector<pf_node>,local_node_key::hash> all_local_nodes=every_node_map.local_nodes;
    path_with_cost to_return;
    path_with_cost to_add;
    pf_node closest;
    pf_node previous;
    int deepest_depth=0;
    for (int i=1; i<=max_depth; ++i)
    {
        closest=closest_pf_node(to,from,mapset[which_map(to,i)],i,movecosts,all_local_nodes);
        if (which_map(closest.location,i)==which_map(to,i))
        {
            break;
        }
        if (i==1)
        {
            to_add=a_pathfind_controller(mapset[which_map(from,1)], from, to, movecosts);
        }
        else
        {
            to_add=pf_node_pathfind(previous, closest.location, mapset, all_nodes, i);
        }
        previous=closest;
        to_return.insert(to_return.end(),to_add.begin(),to_add.end());
        deepest_depth=i;
    }
    to_add=pf_node_pathfind(previous,to,mapset,all_nodes,deepest_depth);
    to_return.insert(to_return.end(),to_add.begin(),to_add.end());
    //go to closest then go up, then go down if same sector.
    //find closest pf_node up
    //closest_pf_node(<#tuple_int where_to#>, <#tuple_int where_from#>, <#vectormap where_at#>)
    //hierarchical_pathfind(closest_pf_node, from, mapset, depth-1)
    //to_return+=hierarchical_pathfind
    //go to map with the to
    //closest_pf_node then go down
    return to_return;
}
//std::unordered_map<path_int,std::vector<tuple_int>,boost::hash<path_int>> paths(vectormap& to_find,
//                                                          vectormap& N, vectormap& E,
//                                                          vectormap& S, vectormap& W,
//                                                          int depth=0){
//    std::unordered_map<path_int,std::vector<tuple_int>,boost::hash<path_int>> to_return;
//    return to_return;
//}
