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
#include <thread>
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
std::vector<std::tuple<int,int,int>> cheapest(std::vector<int>& vec_terrain_costs, std::vector<int>& mine, std::vector<int>& other)
{
    float bestcost=90; //this is a massive move cost that would never be seen. probably should've used -1, but w/e
    float currentcost;
    std::vector<std::tuple<int, int, int>> best;
    std::vector<std::tuple<int, int, int>> to_return;
    for (int j=0; j<mine.size(); ++j)
    {
        for (int checks=-1; checks<2; ++checks)
        {
			if (vec_terrain_costs[mine[j]] == 0 or ((checks+j)<other.size() and vec_terrain_costs[other[checks + j]] == 0))
			{
				continue;
			}
            if (checks+j>0 && (checks+j)<other.size())
            {
                if (checks==0)
                {
                    currentcost=(vec_terrain_costs[mine[j]]+vec_terrain_costs[other[checks+j]])/2;
                    if (currentcost<bestcost)
                    {
                        bestcost=currentcost;
                        best.clear();
                        best.push_back(std::tuple<int, int, int>(j,checks+j,bestcost));
                    }
                    else if (currentcost==bestcost)
                    {
                        best.push_back(std::tuple<int, int, int>(j, checks + j, bestcost));
                    }
                }
                else
                {
                    currentcost=1.5*(vec_terrain_costs[mine[j]]+vec_terrain_costs[other[checks+j]])/2;
                    if (currentcost<bestcost)
                    {
                        bestcost=currentcost;
                        best.clear();
                        best.push_back(std::tuple<int, int, int>(j, checks + j, bestcost));
                    }
                    else if (currentcost==bestcost)
                    {
                        best.push_back(std::tuple<int, int, int>(j, checks + j, bestcost));
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
			return to_return;
        }
    }
    return best;
};
//assumes a square map divisible by 8 or 16, preferably twice
std::unordered_map<tuple_int,vectormap,boost::hash<tuple_int>> cut(vectormap& to_cut, int x_begin,
                                                                   int y_begin, int cut_size)
//to_return.
{
    std::vector<int> row(cut_size);
    std::vector<int> temprow(cut_size);
    vectormap tempmap(cut_size,row);
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
                                int cut_size, int& depth)
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
        if (current.location==to)
        {
            success=true;
            path=previous;
        }
        else if(which_map(current.location,depth,cut_size)==which_map(to,depth,cut_size))
        {
            int new_depth=depth-1;
			if (new_depth == 0)
			{
				tuple_int a_star_map = which_map(to, new_depth + 1, cut_size);
				to_return = a_pathfind_controller(base_map_set[a_star_map], current.location, to, start.t_mobility, std::get<0>(a_star_map)*cut_size, std::get<1>(a_star_map)*cut_size);
				success = true;
			}
			else
			{
				pf_node_key new_start_key;
				new_start_key.depth = (new_depth);
				new_start_key.location = current.location;
				new_start_key.t_mobility = current.t_mobility;
				pf_node new_start = all_nodes[new_start_key];
				to_return = pf_node_pathfind(new_start, to, base_map_set, all_nodes, cut_size, (new_depth));
				success = true;
			}
        }
        else if(open_list.size()==0){
            for (int edge_i=0; edge_i<current.associated_edges.size(); ++edge_i)
            {
                pf_edge waypoint=current.associated_edges[edge_i];
                temp.next_stop=all_nodes[waypoint.key_to];
                if (closed_list.count(temp.next_stop.location)==0 && open_dupls.count(temp.next_stop.location) == 0)
                {
					temp.cost_to_here = waypoint.cost;
                    temp.estimated_cost_to_there=calc_cost(temp.next_stop.location,to);
                    open_dupls.emplace(temp.next_stop.location);
                    open_list.push(temp);
                }
            }
            closed_list.emplace(current.location);
            previous=current;
            if (open_list.size()==0)
            {
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
				//std::cout << "\nyo this is" << std::get<0>(current.location) << "," << std::get<1>(current.location);
				//std::cout << "||" << waypoint.path.size()<<"||";
				//std::cout <<std::get<0>(temp.next_stop.location) << "," << std::get<1>(temp.next_stop.location);
                if (closed_list.count(temp.next_stop.location)==0 && open_dupls.count(temp.next_stop.location) == 0)
                {
                    temp.cost_to_here=waypoint.cost+cost_so_far;
                    temp.estimated_cost_to_there=calc_cost(temp.next_stop.location,to);
                    open_dupls.emplace(temp.next_stop.location);
                    open_list.push(temp);
                }
            }
            closed_list.emplace(current.location);
            previous=current;
        }
    }
    path=previous;
    pf_node path_next;
    path_next=visits[path];
    for (pf_edge edge:path.associated_edges){
        if (all_nodes[edge.key_to]==path_next)
        {
            to_return.insert(to_return.end(),edge.path.rbegin(), edge.path.rend());
			break;
        }
    }
    while (!(path == start)) //checkmate.
    {
        path=path_next;
        path_next=visits[path];
        for (pf_edge edge:path.associated_edges){
            if (all_nodes[edge.key_to]==path_next)
            {
				//std::cout << "\nYo watup" << std::get<0>(path.location) << "," << std::get<1>(path.location) << ":" << edge.path.size() << "||" << std::get<0>(path_next.location) << "," << std::get<1>(path_next.location);
                to_return.insert(to_return.end(),edge.path.rbegin(), edge.path.rend());
                break;
            }
        }
    }
    return to_return;
}

/*node_retrieval node_entrances(int cluster_size, node_retrieval& sublevel_nodes, int coord_x_start, int coord_x_end, int coord_y_start, int coord_y_end, int original_cut_size)
//coord x_start should be in base coords, not the first level coords
{
	std::unordered_map<pf_node_key, pf_node, pf_node_key::hash> cluster_graph_nodes;
	std::unordered_map<local_node_key, std::vector<pf_node_key>, local_node_key::hash> cluster_local_nodes;
	std::unordered_map<pf_node_key, pf_node, pf_node_key::hash> sublevel_graph_nodes = sublevel_nodes.all_nodes;
	std::unordered_map<local_node_key, std::vector<pf_node_key>, local_node_key::hash> sublevel_local_nodes=sublevel_nodes.local_nodes;
}
 */
void subset_entrances(
	tuple_int& which, vectormap& current_map, int cut_size, std::vector<int> costs, int from_right_size, int from_bottom_size,
	std::vector<std::vector<int>>& vec_from_container, std::vector<std::vector<int>>& vec_my_container,
	node_retrieval& subset_retrieval
) {
	std::unordered_map<pf_node_key,pf_node,pf_node_key::hash> subset_graph_nodes;
	std::unordered_map<local_node_key, std::vector<pf_node_key>, local_node_key::hash> subset_local_nodes;
	std::vector<pf_node_key> subset_nodes_in_this_section;
	int depth = 1;
	std::vector<tuple_int> mod_pos_entrances;
	std::vector<tuple_int> inter_edges;
	std::vector<int> travel_cost;
	for (int i = 0; i < vec_from_container.size(); ++i)
	{
		std::vector<int> myvec_comp = vec_my_container[i];
		std::vector<int> fromvec_comp = vec_from_container[i];
		if (fromvec_comp.size() == 0)
		{
			continue;
		}
		std::vector<std::tuple<int, int, int>> possible_entrances = cheapest(costs, myvec_comp, fromvec_comp);
		//1st-mine, 2nd-theirs, 3rd-cost, didn't feel like a struct.
		int half_coord;
		int inter_edge_half;
		if (i == 0 or i == 1)
		{
			if (i == 0) //from_left because it didn't work the other way
			{
				half_coord = std::get<0>(which)*cut_size;
				inter_edge_half = (std::get<0>(which)*cut_size) - 1;
			}
			else if (i == 1) //from_right
			{
				half_coord = (from_right_size - 1) + (std::get<0>(which)*cut_size);
				inter_edge_half = from_right_size + (std::get<0>(which)*cut_size);
			}
			for (std::tuple<int, int, int> half : possible_entrances)
			{
				int my_side = std::get<0>(half) + (std::get<1>(which)*cut_size);
				int inter_edge_side = std::get<1>(half) + (std::get<1>(which)*cut_size);
				mod_pos_entrances.push_back(tuple_int(half_coord, my_side));
				inter_edges.push_back(tuple_int(inter_edge_half, inter_edge_side));
				travel_cost.push_back(std::get<2>(half));
			}
		}
		else
		{
			if (i == 2) //from_top
			{
				half_coord = std::get<1>(which)*cut_size;
				inter_edge_half = (std::get<1>(which)*cut_size) - 1;
			}
			else if (i == 3) //from_bottom
			{
				half_coord = (from_bottom_size - 1) + (std::get<1>(which)*cut_size);
				inter_edge_half = from_bottom_size + (std::get<1>(which)*cut_size);
			}
			for (std::tuple<int, int, int> half : possible_entrances)
			{
				int my_side = std::get<0>(half) + (std::get<0>(which)*cut_size);
				int inter_edge_side = std::get<1>(half) + (std::get<0>(which)*cut_size);
				mod_pos_entrances.push_back(tuple_int(my_side, half_coord));
				inter_edges.push_back(tuple_int(inter_edge_side, inter_edge_half));
				travel_cost.push_back(std::get<2>(half));
			}
		}
	}
	for (int i_from = 0; i_from<mod_pos_entrances.size(); ++i_from)
	{
		tuple_int coord_from = mod_pos_entrances[i_from];
		std::unordered_set<tuple_int, boost::hash<tuple_int>> alreadyhit;
		//form a node first
		//then build the edges
		pf_node temp_node;
		pf_node_key temp_node_key;
		//shit need to convert back to global
		temp_node.location = mod_pos_entrances[i_from];
		temp_node_key.location = temp_node.location;
		temp_node.depth = depth;
		temp_node_key.depth = depth;
		temp_node.t_mobility = costs;
		temp_node_key.t_mobility = costs;
		temp_node.associated_edges.clear(); //gotta be safe.
		pf_edge inter_edge;
		inter_edge.key_to.depth = depth;
		inter_edge.key_to.location = inter_edges[i_from];
		inter_edge.key_to.t_mobility = costs;
		inter_edge.cost = travel_cost[i_from];
		inter_edge.path = { { std::tuple<tuple_int, int>(inter_edges[i_from], 0) },
		{ std::tuple<tuple_int, int>(temp_node.location, inter_edge.cost) } };
		temp_node.associated_edges.push_back(inter_edge);
		path_with_cost empty_path;
		for (int i_to = 0; i_to<mod_pos_entrances.size(); ++i_to)
		{
			tuple_int coord_to = mod_pos_entrances[i_to];
			if (coord_from != coord_to)
			{
				pf_edge temp_edge;
				float total_cost = 0;
				temp_edge.key_to.depth = depth;
				temp_edge.key_to.location = coord_to;
				temp_edge.key_to.t_mobility = costs;
				//std::cout << "\n" << std::get<0>(coord_from) << "," << std::get<1>(coord_from) << "     " << std::get<0>(coord_to) << "," << std::get<1>(coord_to);
				temp_edge.path = a_pathfind_controller(current_map, coord_from, coord_to, costs, std::get<0>(which)*cut_size, std::get<1>(which)*cut_size);
				if (temp_edge.path != empty_path)
				{
					for (std::tuple<tuple_int, float> tile : temp_edge.path)
					{
						total_cost += std::get<1>(tile);
					}
					temp_edge.cost = total_cost;
					temp_node.associated_edges.push_back(temp_edge);
				}
				else
				{
					continue;
				}
			}
		}
		subset_graph_nodes[temp_node_key] = temp_node;
		subset_nodes_in_this_section.push_back(temp_node_key);
	}
	local_node_key temp_local_key;
	temp_local_key.map_coord = which;
	temp_local_key.depth = depth;
	temp_local_key.t_mobility = costs;
	subset_local_nodes[temp_local_key] = subset_nodes_in_this_section;
	subset_retrieval.local_nodes = subset_local_nodes;
	subset_retrieval.all_nodes = subset_graph_nodes;
    if (which == tuple_int(0, 0) && costs==terrain_costs)
    {
        pf_node_key temp_node_key;
        temp_node_key.location=tuple_int(63,42);
        temp_node_key.depth=1;
        temp_node_key.t_mobility={1,2,3,4,0};
        pf_node better_have=subset_retrieval.all_nodes[temp_node_key];
        //std::cout << "\nyo better have" << std::get<0>(better_have.associated_edges[0].key_to.location) << "," << std::get<1>(better_have.associated_edges[0].key_to.location);
    }
}

node_retrieval entrances(std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>>& map_set,
						 vectormap& big_map, int& cut_size,
                         bool wrap, int min_x, int min_y, int max_x, int max_y,
                         std::vector<std::vector<int>>& vec_of_terrain_costs)
{
    //if wrap, try with max if no to the left, etc.
    //perhaps add
    //definitely need to have been cut already. assumes equal sizes n stuff.
    tuple_int which;
    vectormap current_map;
    node_retrieval to_return;
    int depth=1; //this should be obvious since it's the first pass
    std::vector<tuple_int> directions=
    {
        tuple_int(-1,0),tuple_int(0,-1),tuple_int(0,1),tuple_int(1,0)
    };
    bool mine=true;
    bool notmine=false;
    std::unordered_map<pf_node_key,pf_node,pf_node_key::hash> graph_nodes;
    std::unordered_map<local_node_key,std::vector<pf_node_key>,local_node_key::hash> nodes_in_map;
    std::vector<pf_node_key> nodes_in_this_section;
    for (auto which_map:map_set)
    {
        which=which_map.first;
		std::cout << "\n"<<std::get<0>(which) <<","<< std::get<1>(which);
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
		/*void subset_entrances(
		tuple_int& which, vectormap& current_map, int cut_size, std::vector<int> costs, int from_right_size, int from_bottom_size,
			std::vector<std::vector<int>>& vec_from_container, std::vector<std::vector<int>>& vec_my_container,
			node_retrieval& subset_retrieval
			)*/
		//think about using a pool next time.
        std::vector<std::vector<int>> vec_from_container={from_left,from_right,from_top,from_bottom};
        std::vector<std::vector<int>> vec_my_container={my_left,my_right,my_top,my_bottom};
		int threads_supported = std::thread::hardware_concurrency();
		if (threads_supported == 0)
		{
			threads_supported = 2;
		}
		std::vector<std::thread> threads;
		node_retrieval empty;
		int from_right_size = from_right.size();
		int from_bottom_size = from_bottom.size();
		std::vector<node_retrieval> subset_retrievals (vec_of_terrain_costs.size(), empty);
		for (int i = 0; i < vec_of_terrain_costs.size(); i += threads_supported)
		{
			for (int t = 0; t < threads_supported; ++t)
			{
                if (((i*threads_supported) + t)<vec_of_terrain_costs.size())
                {
                    std::vector<int> costs = vec_of_terrain_costs[(i*threads_supported) + t];
                    threads.push_back(std::thread(subset_entrances,
                                                  std::ref(which), std::ref(current_map), cut_size, costs, from_right.size(), from_bottom.size(),
                                                  std::ref(vec_from_container), std::ref(vec_my_container), std::ref(subset_retrievals[(i*threads_supported) + t])));
                }
			}
            for (auto& current_thread:threads)
            {
                current_thread.join();
            }
		}
		threads.clear();
		for (node_retrieval nr : subset_retrievals)
		{
			std::unordered_map<pf_node_key, pf_node, pf_node_key::hash> subset_nodes=nr.all_nodes;
			std::unordered_map<local_node_key, std::vector<pf_node_key>, local_node_key::hash> subset_local=nr.local_nodes;
			for (auto mapstuff : subset_nodes)
			{
				graph_nodes[(mapstuff.first)] = mapstuff.second;
			}
			for (auto mapstuff : subset_local)
			{
				nodes_in_map[(mapstuff.first)] = mapstuff.second;
			}
		}
    }
    to_return.local_nodes=nodes_in_map;
    to_return.all_nodes=graph_nodes;
    MAX_DEPTH=1;
    return to_return;
}
pf_node closest_pf_node(tuple_int& where_to, tuple_int& where_from, vectormap& where_at, int& depth, int cut_size,
                               std::vector<int>& movecosts,
                               node_retrieval& every_node_map)
{
	std::unordered_map<local_node_key, std::vector<pf_node_key>, local_node_key::hash> local_nodes=every_node_map.local_nodes;
	std::unordered_map<pf_node_key, pf_node, pf_node_key::hash> all_nodes = every_node_map.all_nodes;
    local_node_key access_key;
    access_key.depth=depth;
    access_key.map_coord=which_map(where_from,depth,cut_size);
    access_key.t_mobility=movecosts;
    std::vector<pf_node_key> keys_nearby_nodes=local_nodes[access_key];
	std::vector<pf_node> nearby_nodes;
	for (pf_node_key key : keys_nearby_nodes)
	{
		nearby_nodes.push_back(all_nodes[key]);
	}
    float cost;
    float best_cost=-1;
    pf_node best;
    for (pf_node node:nearby_nodes)
    {
        cost=calc_cost(where_to,node.location);
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
                                     int max_depth, int cut_size, node_retrieval& every_node_map)
{
    std::unordered_map<pf_node_key, pf_node,pf_node_key::hash> all_nodes=every_node_map.all_nodes;
    std::unordered_map<local_node_key,std::vector<pf_node_key>,local_node_key::hash> all_local_nodes=every_node_map.local_nodes;
    path_with_cost to_return;
    path_with_cost to_add;
	path_with_cost beginning;
    pf_node closest;
    pf_node previous;
    int deepest_depth=0;
    for (int i=1; i<=max_depth; ++i)
    {
        closest=closest_pf_node(to,from,mapset[which_map(to,i,cut_size)],i, cut_size, movecosts,every_node_map);
        if (which_map(closest.location,i,cut_size)==which_map(to,i,cut_size))
        {
            break;
        }
        if (i==1)
        {
            to_add=a_pathfind_controller(mapset[which_map(from,1,cut_size)], from, closest.location, movecosts);
        }
        else
        {
            to_add=pf_node_pathfind(previous, closest.location, mapset, all_nodes, cut_size, i);
        }
        previous=closest;
        to_return.insert(to_return.end(),to_add.rbegin(),to_add.rend());
        deepest_depth=i;
    }
    to_add=pf_node_pathfind(previous,to,mapset,all_nodes,cut_size,deepest_depth);
    to_return.insert(to_return.end(),to_add.rbegin(),to_add.rend());
	std::reverse(to_return.begin(), to_return.end());
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
