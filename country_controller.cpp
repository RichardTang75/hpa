//
//  country_controller.cpp
//  take2
//
//  Created by asdfuiop on 8/16/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#include "goodfunctions.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include "hierarchical_pathfind.hpp"
#include "boost/functional.hpp"
//#include <SDL2/SDL.h>
//#include <SDL2_image/SDL_image.h>
class unit_modifier //perhaps experience n shit
{

};
class unit
{
public:
    void move(void)
    {
		float cost = 0;
		tuple_int end;
		if (new_move_count > 0) { new_move_count -= 1; }
		while (cost <= effective_speed)
		{
			end=std::get<0>(current_path.back());
			cost += std::get<1>(current_path.back());
			current_path.pop_back();
		}
    }
	void set_move_target(tuple_int& to, node_retrieval& nodes, std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>>& mapset, int max_depth, int cut_size)
	{
		tuple_int current_move_target = std::get<0>(current_path[0]);
		if (dist_squared(to, current_move_target) > 100 or (new_move_count == 0))
		{
			tuple_int from = tuple_int(x, y);
			current_path = hierarchical_pathfind(to, from, t_mobility, mapset, max_depth, cut_size, nodes);
			new_move_count = 5;
		}
	}
    void attack(unit& other)
    {
        tuple_int other_loc=tuple_int(other.x,other.y);
        tuple_int loc=tuple_int(x,y);
        static int count=5;
        if (dist_squared(other_loc,loc)<powf(range,2))
        {
            float damage=effective_power-(other.armor)/(1/(other.effective_finesse-effective_finesse));
            float mor_damage=mor_per*effective_power/(1+(other.discipline-discipline));
            float end_damage=(other.armor/10+end_per)*effective_power;
            other.health-=damage;
            other.morale-=mor_damage;
            other.endurance-=end_damage;
        }
        else
        {
			//check if it's not close enough 
			//set move target
			move();
        }
    }
    void routing(int xstart, int ystart, int xend, int yend, node_retrieval& nodes, std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>>& mapset, int max_depth, int cut_size)
    {
        int try_size=(xstart-xend)/2;
        int dx=std::get<0>(dir_deployed_from);
        int dy=std::get<1>(dir_deployed_from);
		tuple_int run_to;
        for (;;)
        {
            int new_x=x+dx*try_size;
            int new_y=y+dy*try_size;
            if (try_size==1){
                if (in_bounds(xstart, xend, ystart, yend, new_x,new_y))
                {
					run_to = tuple_int(new_x, new_y);
                    set_move_target(run_to, nodes, mapset, max_depth, cut_size);
                }
                else
                {
                    new_x-=dx;
                    new_y-=dy;
					run_to = tuple_int(new_x, new_y);
                    set_move_target(run_to, nodes, mapset,max_depth, cut_size);
                }
            }
            else if (in_bounds(xstart, xend, ystart, yend, new_x,new_y))
            {
                continue;
            }
            else
            {
                new_x-=dx*try_size;
                new_y-=dy*try_size;
                try_size/=2;
            }
        }
    }
    void routed(void)
    {
        //call destructor, deal with this l8er.
    }
    void exhausted(void)
    {
        float exhaust=endurance/mendurance;
        effective_speed=(base_speed+base_speed*(exhaust))/2;
        effective_finesse=(base_finesse+base_finesse*(exhaust))/2;
        effective_power=(base_power+base_power+base_power*exhaust)/3;
    }
    void render(void)
    {
        SDL_Point* points[5];
        SDL_Point* outline[16];
        int count1=0;
        int count2=0;
        tuple_int my_loc=tuple_int(x,y);
        percent_x=powf((x-std::get<0>(to)),2)/dist_squared(to,my_loc);
        percent_y=powf((y-std::get<1>(to)),2)/dist_squared(to,my_loc);
        for (int width=-3;width<4;++width)
        {
            for (int height=-1; height<2; ++height)
            {
                if(width==-3 or height==-1 or height==1 or width==3)
                {
                    outline[count1]->x=(x+width)*(-1)*(percent_y);//the negative one is because up is negative
					outline[count1]->y = (y + height)*percent_x;
                    ++count1;
                }
                else
                {
					points[count2]->x = (x + width)*(-1)*(percent_y);
					points[count2]->y = (y + height)*percent_x;
                    ++count2;
                }
            }
        }
    }
private:
    int x,y,base_speed,base_finesse,base_power,armor,range,discipline,end_per,mor_per;
    float effective_finesse,effective_speed, effective_power;
    int rotation, mode;
    float health,endurance,morale;
    float mhealth, mendurance, mmorale;
    tuple_int dir_deployed_from; //for retreating and etc.
    float percent_x,percent_y;
    tuple_int to;
    path_with_cost current_path;
    int new_move_count=5;
    int reform_count;
	std::vector<int> t_mobility;
};
class country_controller{
private:
    int ranking;
    bool in_decline;
    int defend, expand, conquer;
    int mil_buildup, econ_buildup, dipl_buildup;
    int information;  //economic and military information. are they expanding, are they known, how much do they have, are they building up militarily or economically, are they trustworthy
    int trustworthiness;
    
};
class army_controller{
    //more units under attack, longer it takes to give orders, induces penalty if all suddenly get attacked
public:
    void atk_advance(){
        for (unit brig:associated_units)
        {
            
        }
        //move all units forward. if enemy unit spotted, move some to attack, others form a line
        //if any units attacked, nearby units retaliate.
        //must be able to play rock-paper-scissors? but maybe badly, making units rush in too slow
    }
    void def_advance(){
        
    }
    //take the hq if possible, otherwise put pressure while putting pressure elsewhere
private:
    int orders; //defend, attack, scout, raid, harass
    int disposition; //attack, defend, maneuver, harass? maybe make it part of maneuver
    int competence;
    int speed; //lowest of its constituent units
    int morale, supplies;
    int location;
    int skirm_cap, skirm_rad;
    int effect_strength;
    tuple_int headquarters;
    tuple_int suspected_enemy;
    path_with_cost supply_train;
    std::vector<unit> associated_units;
};