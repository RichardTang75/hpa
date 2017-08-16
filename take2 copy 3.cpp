//forest,mountain,water,marsh?,plain/grass
//
//  main.cpp
//  take2
//
//  Created by asdfuiop on 6/10/17.
//  Copyright © 2017 asdfuiop

#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include "lodepng.h"
#include "mapgen.hpp"
#include "goodfunctions.hpp"
#include "hierarchical_pathfind.hpp"
#include "terrain.hpp"
#include <SDL.h>
#include <SDL_image.h>
//#include <SDL2/SDL.h>
//#include <SDL2_image/SDL_image.h>
//0=grass, 1=forest, 2=marsh, 3=mountain, 4=water,
std::vector<std::vector<int>> possible_move_costs=
{
    {1,2,3,4,0}, //basic move cost-used by fire and metal
    {1,1,2,4,0}, //wood
    {1,2,2,4,2}, //water
    {1,2,2,4,0} //earth
};
class unit{
public:
    void move(tuple_int to, tuple_int from)
    {
        //stuff
    }
private:
    int x,y,speed,finesse,power,armor,range,discipline;
    float health,endurance,morale;
    tuple_int dir_deployed_from; //for retreating and etc.
};
SDL_Window* gwindow=NULL;
SDL_Renderer* grenderer=NULL;
class back_text
{
public:
    back_text(int width, int height);
    ~back_text();
    void free();
    void surface_processing(std::string path, tuple_set& to_add, std::string features="",int seed=0, float hurdle=.001);
    void simple_load(std::string path);
    void make_text(void);
    void render(int x,int y);
    int get_width();
    int get_height();
private:
    int width;
    int height;
    SDL_Texture* f_texture=NULL;
    SDL_Surface* t_surf=IMG_Load("empty.png");
};
back_text::back_text(int set_width, int set_height)
{
    f_texture=NULL;
    width=set_width;
    height=set_height;
}
back_text::~back_text()
{
    back_text::free();
}
void back_text::render(int x, int y)
{
    SDL_Rect render_rect={x,y,width,height};
    SDL_RenderCopy(grenderer,f_texture,NULL, &render_rect);
}
void back_text::free()
{
    if (f_texture!=NULL)
    {
        SDL_DestroyTexture(f_texture);
        f_texture=NULL;
        width=0;
        height=0;
    }
}
void back_text::simple_load(std::string path)
{
    SDL_FreeSurface(t_surf);
    t_surf=NULL;
    t_surf=IMG_Load(path.c_str());
}
void back_text::make_text()
{
    if (f_texture==NULL)
    {
        f_texture=SDL_CreateTextureFromSurface(grenderer, t_surf);
        SDL_FreeSurface(t_surf);
        t_surf=NULL;
    }
}
int back_text::get_width()
{
    return width;
}

void back_text::surface_processing(std::string path, tuple_set& to_add, std::string features,int seed,float hurdle)
{
    SDL_Surface* initial=IMG_Load(path.c_str());
    SDL_Surface* temp=IMG_Load("empty.png");
    Uint32* srcpixels=(Uint32*)initial->pixels;
    if(SDL_MUSTLOCK(temp))
    {
        SDL_LockSurface(temp);
    }
    int scanline=(temp->pitch)/4;
    Uint8 red,green,blue;
    int width=temp->w;
    int height=temp->h;
    Uint32* pixels=(Uint32*)temp->pixels;
    for (int y=0; y<(height);++y)
    {
        for (int x=0; x<(width);++x)
        {
            if (to_add.count(std::make_tuple(x,y))>0)
            {
                SDL_GetRGB(srcpixels[(scanline*y)+x],temp->format,&red,&green,&blue);
                pixels[(scanline*y)+x]=SDL_MapRGBA(temp->format,red,green,blue,0xFF);
            }
        }
    }
    if (SDL_MUSTLOCK(temp))
    {
        SDL_UnlockSurface(temp);
    }
    SDL_BlitSurface(temp, NULL, t_surf, NULL);
    if (!(features.empty()))
    {
        SDL_FreeSurface(temp);
        temp=NULL;
        temp=IMG_Load(features.c_str());
        int temp_width=temp->w;
        int temp_height=temp->h;
        std::mt19937 eng(seed);
        std::uniform_real_distribution<float> thresh(0,1);
        for (tuple_int coord:to_add)
        {
            float temp_thresh=thresh(eng);
            if (temp_thresh<hurdle)
            {
                SDL_Rect dstrect{(std::get<0>(coord)-temp_width/2),(std::get<1>(coord)-temp_height/2),
                                    temp_width,temp_height};
                SDL_BlitSurface(temp,NULL,t_surf,&dstrect);
            }
        }
    }
}

//terrain type should be 0 for grass and I'll later expand this with either choice or if statements
std::tuple<std::vector<tuple_set>,vectormap> map_controller
                                    (const int& x_size, const int& y_size,
                                      const int& side_size,
                                      tuple_set& N, tuple_set& E,
                                      tuple_set& S, tuple_set& W,
                                      unsigned long long init_seed=0,
                                      int map_x=0, int map_y=0,
                                      int terrain_type=0)
{
    tuple_set ret_north;
    tuple_set ret_south;
    tuple_set ret_east;
    tuple_set ret_west;
    std::vector<tuple_int> additions={tuple_int(-1,0),tuple_int(1,0),tuple_int(0,-1),tuple_int(1,0),
        tuple_int(-1,-1),tuple_int(-1,1),tuple_int(1,1),tuple_int(1,-1)};
    std::random_device rd;
    if (init_seed==0)
    {
        init_seed=
        (static_cast<unsigned long long>(std::chrono::high_resolution_clock::now().time_since_epoch().count())+
         static_cast<unsigned long long> (rd()));
    }
    std::vector<unsigned> seeds(4);
    std::seed_seq seq
    {
        init_seed
    };
    seq.generate(seeds.begin(),seeds.end());
    tuple_set forests;
    tuple_set mountains;
    tuple_set water;
    tuple_set marsh;
    tuple_set summed;
    tuple_set final_step;
    int cols = x_size;
    int rows = y_size;
    std::tuple<int,int,int,int> bounds=det_bounds(x_size,y_size,side_size,N,E,S,W);
    int x_start=std::get<0>(bounds);
    int x_end=std::get<1>(bounds);
    int y_start=std::get<2>(bounds);
    int y_end=std::get<3>(bounds);
    int init = 0; //maybe make this terrain type?
    std::vector<unsigned char> image(4*rows*cols);
    std::vector<int> row(cols, init);
    vectormap map(rows, row);
    //final step visualized
    vectormap walls(rows,row);
    std::vector<unsigned char> wallimage(4*rows*cols);
    std::thread gen_for(gen_terrain,x_start,x_end,y_start,y_end,6,12,2,4,5,10,8,16,5,10,std::ref(forests),seeds[0]);
    std::thread gen_mtn(gen_terrain,x_start,x_end,y_start,y_end,1,2,3,5,48,86,7,10,7,12,std::ref(mountains),seeds[1]);
    std::thread gen_wtr(gen_terrain,x_start,x_end,y_start,y_end,2,3,2,4,8,16,12,24,5,10,std::ref(water),seeds[2]);
    std::thread gen_msh(gen_terrain,x_start,x_end,y_start,y_end,3,5,2,4,5,10,15,30,5,10,std::ref(marsh),seeds[3]);
    gen_for.join();
    gen_mtn.join();
    gen_wtr.join();
    gen_msh.join();
    std::cout<<"JOINED";
    //when(and if) i get threading working, also thread in the unioning? maybe?
    tuple_set_union(summed,forests);
    tuple_set_union(summed,mountains);
    tuple_set_union(summed,water);
    tuple_set_union(summed,marsh);
    //KEEP EXPANDING UNTIL THEY OVERLAP
    //THOSE ARE NODES
    //REMOVE NODES IF PREVIOUS COST IS EQUAL TO NEW COST FOR MOVEMENT
    for(tuple_int point:summed)
    {
        int cur_x=std::get<0>(point);
        int cur_y=std::get<1>(point);
        for (tuple_int dirs:additions)
        {
            int dx=std::get<0>(dirs);
            int dy=std::get<1>(dirs);
            tuple_int temp_coord=tuple_int(cur_x+dx,cur_y+dy);
            if (summed.count(temp_coord)==0)
            {
                final_step.emplace(temp_coord);
            }
        }
    }
    std::vector<map_tuple> total=
    {   map_tuple(forests,1),map_tuple(marsh,2),
        map_tuple(mountains,3),map_tuple(water,4),
    };
    for (map_tuple proc:total)
    {
        tuple_set terrain=std::get<0>(proc);
        int array_num=std::get<1>(proc);
        for (tuple_int point:terrain)
        {
            int tempX=std::get<0>(point);
            int tempY=std::get<1>(point);
            if (in_bounds(x_start,x_end,y_start,y_end,tempX,tempY)==true)
            {
                map[tempY][tempX]=array_num;
            }
        }
    }

    for (tuple_int coord:final_step)
    {
        int tempX=std::get<0>(coord);
        int tempY=std::get<1>(coord);
        if (in_bounds(x_start,x_end,y_start,y_end,tempX,tempY)==true)
        {
            walls[tempY][tempX]=1;
        }
    }
    array_img(map,image,rows,cols,terrain_type);
    array_img(walls,wallimage,rows,cols,terrain_type);
    std::vector<unsigned char> png;
    std::vector<unsigned char> png2;
    lodepng::State state;
    lodepng::State state2;
    unsigned error = lodepng::encode(png,image,rows,cols,state);
    if (!error)
    {
        lodepng::save_file(png,"cplusplus.png");
    }
    else
    {
        std::cout<<"encoder error "<<error<<": "<<lodepng_error_text(error)<<"\n";
    }
    unsigned nerror = lodepng::encode(png2,wallimage,rows,cols,state2);
    if (!nerror)
    {
        lodepng::save_file(png2,"cplusplus2.png");
    }
    else
    {
        std::cout<<"encoder error "<<nerror<<": "<<lodepng_error_text(nerror)<<"\n";
    }
    std::vector<tuple_set> to_return_sets={forests,mountains,water,marsh,ret_north,ret_east,ret_south,ret_west};
    return std::make_tuple(to_return_sets,map);
}
bool init(int& width, int& height)
{
    bool success=true;
    if (SDL_Init(SDL_INIT_VIDEO)<0)
    {
        success=false;
        std::cout<<"Error Code:"<<SDL_GetError()<<"\n";
    }
    else{
        gwindow=SDL_CreateWindow("Temp", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
        if (gwindow==NULL)
        {
            success=false;
            std::cout<<"Error Code:"<<SDL_GetError()<<"\n";
        }
        else
        {
            grenderer=SDL_CreateRenderer(gwindow, -1, SDL_RENDERER_ACCELERATED);
            if (grenderer==NULL)
            {
                success=false;
                std::cout<<"Error Code:"<<SDL_GetError()<<"\n";
            }
            else
            {
                SDL_SetRenderDrawColor(grenderer,0xFF,0xFF,0xFF,0xFF);
                int img_flags=IMG_INIT_PNG;
                if (!(IMG_Init(img_flags) & img_flags))
                {
                    success=false;
                    std::cout<<"Error Code(sdl_image):"<<IMG_GetError()<<"\n";
                }
            }
        }
    }
    return success;
}
void close()
{
    SDL_DestroyRenderer(grenderer);
    grenderer=NULL;
    SDL_DestroyWindow(gwindow);
    gwindow=NULL;
    IMG_Quit();
    SDL_Quit();
}
int main(int argc, char* argv[])
{
    int width=512;
    int height=512;
    if (!init(width,height))
    {
        std::cout<<"Failed to start \n";
    }
    tuple_set empty;
    std::tuple<std::vector<tuple_set>,vectormap> sets_map;
    sets_map=map_controller(512,512,24,empty,empty,empty,empty);
    std::vector<tuple_set> temp_map=std::get<0>(sets_map);
    vectormap map=std::get<1>(sets_map);
    int cut_size=8;
    std::unordered_map<tuple_int,vectormap,boost::hash<tuple_int>> map_set=cut(map, 0, 0, cut_size);
	std::cout << "\n" << map_set.size() << "," << map_set[tuple_int(0, 0)].size() << "," << map_set[tuple_int(0, 0)][0].size();
    node_retrieval nodes=entrances(map_set, map, false, 0, 0, width/cut_size, height/cut_size, possible_move_costs);
    back_text back(512,512);
    back_text grass(512,512);
    tuple_set forest,mount,water,marsh,N,E,S,W;
    forest=temp_map[0];
    mount=temp_map[1];
    water=temp_map[2];
    marsh=temp_map[3];
    grass.simple_load("grass1t.png");
    grass.make_text();
    back.surface_processing("marsh1t.png",marsh);
    back.surface_processing("forest1t.png",forest);
    back.surface_processing("mount1t.png",mount);
    back.surface_processing("water1t.png",water);
    back.make_text();
    bool quit=false;
    SDL_Event e;
    SDL_SetRenderDrawColor(grenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(grenderer);
    grass.render(0,0);
    back.render(0,0);
    //
    SDL_SetRenderDrawColor(grenderer, 0x00, 0x00, 0xFF, 0xFF);
    SDL_Rect sizer={width/4,height/4,8,4};
    SDL_Rect sizer2={width/4+1,height/4+1,6,2};
    SDL_RenderDrawRect(grenderer, &sizer);
    SDL_SetRenderDrawColor(grenderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(grenderer, &sizer2);
    //
    tuple_int start=tuple_int(10,10);
    tuple_int end=tuple_int(510,510);
    std::vector<int> basic={1,2,3,4,0};
    path_with_cost path_and_cost=hierarchical_pathfind(end, start, basic, map_set, 1, nodes);
    std::cout<<map_set.size()<<"\n"<<nodes.local_nodes.size()<<"\n"<<nodes.all_nodes.size()<<"\n";
    std::cout<<path_and_cost.size();
    int count=0;
    for (auto pair : nodes.all_nodes)
    {
        pf_node nodey=pair.second;
        std::cout<<"\n"<<std::get<0>(nodey.location)<<","<<std::get<1>(nodey.location)<<","<<nodey.associated_edges.size();
    }
    for (int i=0; i<path_and_cost.size();i++)
    {
        if (count>1000)
        {
            break;
        }
        ++count;
        int x=std::get<0>(std::get<0>(path_and_cost[i]));
        int y=std::get<1>(std::get<0>(path_and_cost[i]));
        std::cout<<x<<","<<y<<","<<std::get<1>(path_and_cost[i])<<"\n";
    }
    SDL_RenderPresent(grenderer);
    while (!quit){
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type==SDL_QUIT)
            {
                quit=true;
            }
            else if (e.type==SDL_KEYDOWN)
            {
                SDL_RenderClear(grenderer);
                grass.render(0,0);
                back.render(0,0);
                SDL_SetRenderDrawColor(grenderer, 0x00, 0x00, 0xFF, 0xFF);
                SDL_RenderDrawRect(grenderer, &sizer);
                SDL_RenderPresent(grenderer);
                switch (e.key.keysym.sym)
                {
                    case SDLK_UP:
                        std::cout<<"UP";
                        break;
                }
            }
        }
        SDL_Delay(10);
    }
	return 0;
}

