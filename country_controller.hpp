//
//  country_controller.hpp
//  take2
//
//  Created by asdfuiop on 8/16/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#ifndef country_controller_hpp
#define country_controller_hpp

#include <stdio.h>
#include "typedef.hpp"
#include "hierarchical_pathfind.hpp"
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
enum class unit_type
{
	infantry, cavalry, ranged, skirmisher, shock
};
//maybe tie this to unit type?
enum class attack_preference
{
	opportunistic, normal, decapitation, pursuit
};

class unit_modifier //perhaps experience n shit
{
};
class unit
	//automatic unit-level when no active orders?
{
public:
	unit(tuple_int start, int speed, int finesse, int power, int armr, int rng, int disc, int end_per_dmg, int mor_per_dmg,
		float shealth, float sendurance, float smorale, tuple_int dir_start, std::vector<int> mobility);
	void move(void);
	//~unit();
	void set_move_target(tuple_int& to, node_retrieval& nodes, std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>>& mapset, int max_depth, int cut_size);
	void attack(unit& other);
	void routing(int xstart, int ystart, int xend, int yend, node_retrieval& nodes, std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>>& mapset, int max_depth, int cut_size);
	void routed(void);
	void exhausted(void);
	void render(SDL_Renderer*, int camera_x, int camera_y);
	tuple_int get_pos(void);
	void update(void);
	void set_attack_target(void);
	void maintain_distance(void);
private:
	int x, y, base_speed, base_finesse, base_power, armor, range, discipline, end_per, mor_per;
	float effective_finesse, effective_speed, effective_power;
	int rotation, mode;
	int length, height;
	int atk_inclination, def_inclination, move_inclination, retreat_inclination;
	float health, endurance, morale;
	float mhealth, mendurance, mmorale;
	tuple_int dir_deployed_from; //for retreating and etc.
	float percent_x, percent_y;
	tuple_int to;
	tuple_int self_chosen_to;
	path_with_cost current_path;
	unit* attack_target;
	unit* self_chosen_attack_target;
	//int new_move_count=5; do i need this with HPA?
	int reform_count;
	std::vector<int> t_mobility;
	bool selected;
	bool commanded;
	attack_preference attitude;
};
#endif /* country_controller_hpp */
