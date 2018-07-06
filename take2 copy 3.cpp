//forest,mountain,water,marsh?,plain/grass
//
//  main.cpp
//  take2
//
//  Created by asdfuiop on 6/10/17.
//  Copyright © 2017 asdfuiop
//makefile tutorial that helped me immensely with emscripten:
//http://blog.scottlogic.com/2014/03/12/native-code-emscripten-webgl-simmer-gently.html
#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <assert.h>
#include "mapgen.hpp"
#include "goodfunctions.hpp"
#include "hierarchical_pathfind.hpp"
#include "FastNoise.h"
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <SDL_image.h>
#include "test_init.hpp"
#include "country_controller.hpp"
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
//0=grass, 1=forest, 2=marsh, 3=mountain, 4=water,
//globals can go back into main if I really need it
int map_width = 512;
int map_height = 512;
float camera_x = 0;
float camera_y = 0;
int VIEW_HEIGHT = 512;
int VIEW_WIDTH = 512;
std::vector<std::vector<int>> possible_move_costs =
{
	{ 1,2,3,4,0 }, //basic move cost-used by fire and metal
	{ 1,1,2,4,0 }, //wood
	{ 1,2,2,4,2 }, //water
	{ 1,2,2,4,0 } //earthmap
};
tuple_set all_node_locs;
std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>> big_map_storage;
std::unordered_map<tuple_int, std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>>, boost::hash<tuple_int>> map_set_storage;
SDL_Window* gwindow = NULL;
SDL_Renderer* grenderer = NULL;
class back_text
{
public:
	back_text(int width, int height);
	back_text(void);
	~back_text();
	void free();
	void surface_processing(std::string path, tuple_set& to_add, std::string features = "", int seed = 0, float hurdle = .001);
	void simple_load(std::string path);
	void load_background(tuple_int& retrieve, tuple_triple_map& maps);
	void make_text(void);
	void render(int x, int y);
	int get_width();
	int get_height();
private:
    int width;
    int height;
    SDL_Texture* f_texture=NULL;
    SDL_Surface* t_surf=IMG_Load("assets/empty.png");
};
typedef std::unordered_map<tuple_int, back_text, boost::hash<tuple_int>> texture_storage;
#ifdef EMSCRIPTEN
tuple_triple_map maps;
texture_storage draw_maps_storage;
tuple_set processed;
tuple_set created;
bool mousedown=false;
int curx, cury;
int mousestartx, mousestarty;
bool mousemovedwhiledown=false;
#endif
back_text::back_text()
{
	f_texture = NULL;
	width = map_width;
	height = map_height;
}
back_text::back_text(int set_width, int set_height)
{
	f_texture = NULL;
	width = set_width;
	height = set_height;
}
back_text::~back_text()
{
	back_text::free();
}
void back_text::render(int x, int y)
{
	SDL_Rect render_rect = { x,y,width,height };
	SDL_RenderCopy(grenderer, f_texture, NULL, &render_rect);
}
void back_text::free()
{
	if (f_texture != NULL)
	{
		SDL_DestroyTexture(f_texture);
		f_texture = NULL;
		width = 0;
		height = 0;
	}
}
void back_text::simple_load(std::string path)
{
    SDL_FreeSurface(t_surf);
    t_surf=IMG_Load(path.c_str());
}
void back_text::load_background(tuple_int& retrieve, tuple_triple_map& maps)
{
    tuple_set forest, mount, water, marsh;
    forest = maps[std::make_tuple(std::get<0>(retrieve), std::get<1>(retrieve), Fst)];
    mount = maps[std::make_tuple(std::get<0>(retrieve), std::get<1>(retrieve), Mtn)];
    water = maps[std::make_tuple(std::get<0>(retrieve), std::get<1>(retrieve), Wtr)];
    marsh = maps[std::make_tuple(std::get<0>(retrieve), std::get<1>(retrieve), Msh)];
    simple_load("assets/grass1t.png");
    //maybe surface_procesing should return the surface so I can then thread this, then sequentially blit
	surface_processing("assets/water1t.png", water);
    surface_processing("assets/marsh1t.png", marsh);
    surface_processing("assets/forest1t.png", forest);
    surface_processing("assets/mount1t.png", mount);
    make_text();
}
void back_text::make_text()
{
	if (f_texture == NULL)
	{
		f_texture = SDL_CreateTextureFromSurface(grenderer, t_surf);
		SDL_FreeSurface(t_surf);
		t_surf = NULL;
	}
}
int back_text::get_width()
{
	return width;
}

// code for the SDL per-pixel stuff came from http://sol.gfxile.net/gp/ch02.html and
// https://www.gamedev.net/forums/topic/358269-copying-pixels-to-an-sdl-surface/.
void back_text::surface_processing(std::string path, tuple_set& to_add, std::string features,int seed,float hurdle)
{
    SDL_Surface* initial=IMG_Load(path.c_str());
    SDL_Surface* temp=IMG_Load("assets/empty.png");
    Uint32* srcpixels=(Uint32*)initial->pixels;
    if(SDL_MUSTLOCK(temp))
    {
        SDL_LockSurface(temp);
    }
    if(SDL_MUSTLOCK(initial))
    {
        SDL_LockSurface(initial);
    }
    int scanline=(temp->pitch)/4;
    Uint8 red,green,blue;
	Uint8 nothing, alpha;
    Uint32* dstpixels=(Uint32*)temp->pixels;
    for (tuple_int coord: to_add)
    {
        if (in_bounds(0, map_width,0 , map_height, coord))
        {
            int x=std::get<0>(coord);
            int y=std::get<1>(coord);
            SDL_GetRGB(srcpixels[(scanline*y)+x],initial->format,&red,&green,&blue);
            dstpixels[(scanline*y)+x]=SDL_MapRGBA(temp->format,red,green,blue,0xFF);
        }
    }
    if (SDL_MUSTLOCK(temp))
    {
        SDL_UnlockSurface(temp);
    }
    if(SDL_MUSTLOCK(initial))
    {
        SDL_LockSurface(initial);
    }
    SDL_BlitSurface(temp, NULL, t_surf, NULL);
    SDL_FreeSurface(initial);
    SDL_FreeSurface(temp);
    initial=NULL;
    temp=NULL;
    dstpixels=NULL;
    srcpixels=NULL;
}

//terrain type should be 0 for grass and I'll later expand this with either choice or if statements

bool init(int& width, int& height)
{
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO)<0)
	{
		success = false;
		std::cout << "Error Code:" << SDL_GetError() << "\n";
	}
	else {
		gwindow = SDL_CreateWindow("Temp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
		if (gwindow == NULL)
		{
			success = false;
			std::cout << "Error Code:" << SDL_GetError() << "\n";
		}
		else
		{
			grenderer = SDL_CreateRenderer(gwindow, -1, SDL_RENDERER_ACCELERATED);
			if (grenderer == NULL)
			{
				success = false;
				std::cout << "Error Code:" << SDL_GetError() << "\n";
			}
			else
			{
				SDL_SetRenderDrawColor(grenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				int img_flags = IMG_INIT_PNG;
				if (!(IMG_Init(img_flags) & img_flags))
				{
					success = false;
					std::cout << "Error Code(sdl_image):" << IMG_GetError() << "\n";
				}
			}
		}
	}
	return success;
}
void close()
{
	SDL_DestroyRenderer(grenderer);
	grenderer = NULL;
	SDL_DestroyWindow(gwindow);
	gwindow = NULL;
	IMG_Quit();
	SDL_Quit();
}
std::vector<unit> in_box(int startx, int starty, int endx, int endy, std::vector<unit>& all_units)
{
    std::vector<unit> selected;
    for (unit& possible : all_units)
    {
        int unit_x, unit_y;
        std::tie(unit_x, unit_y) = possible.get_pos();
        if (unit_x > startx && unit_x<endx && unit_y>starty && unit_x < endy)
        {
            selected.push_back(possible);
        }
    }
    return selected;
}
//max of 4 at once
void draw_everything(std::vector<unit>& all,
	tuple_int& primary,
	texture_storage& draw_maps_storage)
{
    int draw_x=std::get<0>(primary)*map_width-camera_x;
    int draw_y=std::get<1>(primary)*map_height-camera_y;
    draw_maps_storage[primary].render(draw_x, draw_y);
    SDL_SetRenderDrawColor(grenderer, 0x00, 0x00, 0xFF, 0xFF);
    for (unit& i : all)
    {
		int unit_x, unit_y;
		std::tie(unit_x, unit_y) = i.get_pos();
		if (in_bounds(camera_x, camera_x + VIEW_WIDTH, camera_y, camera_y + VIEW_HEIGHT, unit_x, unit_y))
		{
			i.render(grenderer, camera_x, camera_y);
		}
    }
    std::vector<tuple_int> to_draw;
    if (camera_x-std::get<0>(primary)*map_width>0)
    {
        to_draw.push_back(tuple_int(1,0));
    }
    else if (camera_x-std::get<0>(primary)*map_width==0)
    {
        //this is meant to be nothing.
    }
    else
    {
        to_draw.push_back(tuple_int(-1,0));
    }
    if (camera_y-std::get<1>(primary)*map_height>0)
    {
        to_draw.push_back(tuple_int(0,1));
    }
    else if (camera_y-std::get<1>(primary)*map_height==0)
    {
        //this is also meant to be nothing.
    }
    else
    {
        to_draw.push_back(tuple_int(0,-1));
    }
    if (to_draw.size()>1) //for diagonals
    {
        to_draw.push_back(tuple_int(std::get<0>(to_draw[0])+std::get<0>(to_draw[1]),
                                    std::get<1>(to_draw[0])+std::get<1>(to_draw[1])));
    }
    for (tuple_int next: to_draw)
    {
        tuple_int drawin = tuple_int((std::get<0>(next)+std::get<0>(primary)), (std::get<1>(next)+std::get<1>(primary)));
        draw_x=std::get<0>(drawin)*map_width-camera_x;
        draw_y=std::get<1>(drawin)*map_height-camera_y;
        draw_maps_storage[drawin].render(draw_x,draw_y);
    }
}
//void draw_everything(back_text& back, std::vector<unit>& all)
//{
//    back.render(0, 0);
//    SDL_SetRenderDrawColor(grenderer, 0x00, 0x00, 0xFF, 0xFF);
//    for (unit i : all)
//    {
//        i.render(grenderer);
//    }
//}
//too far, free, reset? write to disk the vectormap? why didn't i just allow direct creation of texture from vectormaps.
//threadpools?

void prepare_the_maps(tuple_int& primary,
	tuple_set& processed,
	tuple_set& created,
	tuple_triple_map& maps,
	texture_storage& draw_maps_storage, 
	std::unordered_map<tuple_int, std::vector<int>, boost::hash<tuple_int>>& bases,
	std::unordered_map<tuple_int, std::unordered_map<tuple_int, char, boost::hash<tuple_int>>, boost::hash<tuple_int>>& seamlessness,
	std::unordered_map<tuple_int, tuple_set, boost::hash<tuple_int>>& bound_coords)
{
	vectormap map;
	std::vector<tuple_int> first_dirs = {
		tuple_int(-1,0), tuple_int(1,0),
		tuple_int(0,-1), tuple_int(0,1) };
	std::vector<tuple_int> second_dirs = {
		tuple_int(-1,-1),tuple_int(-1,1),
		tuple_int(1,-1), tuple_int(1,1)
	};

	if (processed.count(primary) == 0)
	{
		back_text temp(512, 512);
		map = map_controller(std::get<0>(primary), std::get<1>(primary), map_width, map_height, maps, created, 
			processed, bases, seamlessness, bound_coords);
		big_map_storage[primary] = map;
		draw_maps_storage.insert(std::make_pair(primary, temp));
		draw_maps_storage[primary].load_background(primary, maps);
	}
	for (tuple_int dir : first_dirs)
	{
		tuple_int retrievin = tuple_int(std::get<0>(primary) + std::get<0>(dir),
			std::get<1>(primary) + std::get<1>(dir));
		if (processed.count(retrievin) == 0)
		{
			back_text temp(512, 512);
			map = map_controller(std::get<0>(retrievin), std::get<1>(retrievin), map_width, map_height, maps, created, 
				processed, bases, seamlessness, bound_coords);
			big_map_storage[retrievin] = map;
			draw_maps_storage.insert(std::make_pair(retrievin, temp));
			draw_maps_storage[retrievin].load_background(retrievin, maps);
		}
	}
	for (tuple_int dir : second_dirs)
	{
		tuple_int retrievin = tuple_int(std::get<0>(primary) + std::get<0>(dir),
			std::get<1>(primary) + std::get<1>(dir));
		if (processed.count(retrievin) == 0)
		{
			back_text temp(512, 512);
			map = map_controller(std::get<0>(retrievin), std::get<1>(retrievin), map_width, map_height, maps, created, 
				processed, bases, seamlessness, bound_coords);
			big_map_storage[retrievin] = map;
			draw_maps_storage.insert(std::make_pair(retrievin, temp));
			draw_maps_storage[retrievin].load_background(retrievin, maps);
		}
	}
}
#ifdef EMSCRIPTEN
void draw(void)
{
    std::vector<int> common = { 1,2,3,4,0 };
    unit temp(tuple_int(256, 256), 10, 10, 10, 10, 10, 10, 0, 0, 100, 100, 100, tuple_int(-1, -1), common);
    std::vector<unit> selected;
    std::vector<unit> all{ temp };
    tuple_int primary=std::make_tuple(std::round(camera_x/512),
                                      std::round(camera_y/512));
    prepare_the_maps(primary, processed, created, maps, draw_maps_storage);
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
        {
            close();
            break;
        }
        switch (e.type)
        {
            case SDL_MOUSEBUTTONDOWN:
                switch (e.button.button)
            {
                case SDL_BUTTON_LEFT:
                    SDL_GetMouseState(&mousestartx, &mousestarty);
                    std::cout << mousestartx << "," << mousestarty;
                    mousedown = true;
                    break;
            }
                break;
            case SDL_MOUSEBUTTONUP:
                if (mousemovedwhiledown == true)
                {
                    //select units (
                    mousemovedwhiledown = false;
                }
                mousedown=false;
                break;
            case SDL_MOUSEMOTION:
                if (mousedown == true)
                {
                    SDL_GetMouseState(&curx, &cury);
                }
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
            {
                case SDLK_UP:
                    std::cout << "UP";
                    camera_y=camera_y-8;
                    break;
                case SDLK_DOWN:
                    camera_y=camera_y+8;
                    break;
                case SDLK_LEFT:
                    camera_x=camera_x-8;
                    break;
                case SDLK_RIGHT:
                    camera_x=camera_x+8;
                    break;
            }
                break;
        }
        primary=std::make_tuple(std::round(camera_x/512),
                                std::round(camera_y/512));
        prepare_the_maps(primary, processed, created, maps, draw_maps_storage);
        SDL_RenderClear(grenderer);
        draw_everything(all, primary, draw_maps_storage);
        if (mousedown==true)
        {
            SDL_Rect selectbox = { mousestartx ,mousestarty,
                curx - mousestartx,cury - mousestarty };
            SDL_RenderDrawRect(grenderer, &selectbox);
            mousemovedwhiledown = true;
        }
        SDL_RenderPresent(grenderer);
    }
}
#endif
void update(void)
{

}
int main(int argc, char* argv[])
{
	//Updates per second-30 at max?
	const int FPS = 30;
	const int milliseconds_per_update = 1000 / FPS;
    if (!init(map_width,map_height))
    {
        std::cout<<"Failed to start \n";
    }
	auto t1 = std::chrono::high_resolution_clock::now();
#ifdef EMSCRIPTEN
    emscripten_set_main_loop(draw,0,1);
#else
    bool quit=false;
    std::vector<int> common = { 1,2,3,4,0 };
    unit temp(tuple_int(256, 256), 10, 10, 10, 10, 10, 10, 0, 0, 100, 100, 100, tuple_int(-1, -1), common);
    std::vector<unit> selected;
    std::vector<unit> all{ temp };
    tuple_int primary=std::make_tuple(std::round(camera_x/512),
                                      std::round(camera_y/512));
    tuple_triple_map maps;
    texture_storage draw_maps_storage;
	tuple_set to_draw =
	{
		tuple_int(-1,-1),tuple_int(-1,0),tuple_int(-1,1),
		tuple_int(0,-1),tuple_int(0,0),tuple_int(0,1),
		tuple_int(1,-1),tuple_int(1,0),tuple_int(1,1)
	};
	std::unordered_map<tuple_int, std::unordered_map<tuple_int, char, boost::hash<tuple_int>>, boost::hash<tuple_int>> seamlessness;
	std::unordered_map<tuple_int, std::vector<int>, boost::hash<tuple_int>> bases;
	std::unordered_map<tuple_int, std::vector<std::pair<tuple_int, tuple_int>>, boost::hash<tuple_int>> bound_lines;
	std::unordered_map<tuple_int, tuple_set, boost::hash<tuple_int>> bound_coords;
	// have to convert all numbers to enums at some point
	for (tuple_int dir : to_draw)
	{
		for (tuple_int initialize : to_draw)
		{
			if (initialize != tuple_int(0, 0))
			{
				seamlessness[dir][initialize] = 1;
			}
		}
		if (dir == tuple_int(0, 0))
		{

		}
		else
		{
			int start_x = 0;
			int end_x = map_width;
			int start_y = 0;
			int end_y = map_height;
			if (std::get<0>(dir) > 0)
			{
				start_x = 0;
				end_x = map_width / 2;
				seamlessness[dir][tuple_int(1, 0)] = 0;
				seamlessness[dir][tuple_int(1, 1)] = 0;
				seamlessness[dir][tuple_int(1, -1)] = 0;
			}
			else if (std::get<0>(dir) < 0)
			{
				start_x = map_width / 2;
				end_x = map_width;
				seamlessness[dir][tuple_int(-1, 0)] = 0;
				seamlessness[dir][tuple_int(-1, 1)] = 0;
				seamlessness[dir][tuple_int(-1, -1)] = 0;
			}
			if (std::get<1>(dir) > 0)
			{
				start_y = 0;
				end_y = map_height / 2;
				seamlessness[dir][tuple_int(0, 1)] = 0;
				seamlessness[dir][tuple_int(1, 1)] = 0;
				seamlessness[dir][tuple_int(-1, 1)] = 0;
			}
			else if (std::get<1>(dir)<0)
			{
				start_y = map_height / 2;
				end_y = map_height;
				seamlessness[dir][tuple_int(0, -1)] = 0;
				seamlessness[dir][tuple_int(1, -1)] = 0;
				seamlessness[dir][tuple_int(-1, -1)] = 0;
			}
			bases[dir] = {start_x, end_x, start_y, end_y, Wtr, Grs};
			bound_lines[dir] = { std::make_pair(tuple_int(start_x, start_y), tuple_int(end_x, start_y)), 
									std::make_pair(tuple_int(start_x, start_y), tuple_int(start_x, end_y)) };
			for (int i = start_x; i < end_x; i++)
			{
				if (std::get<1>(dir) > 0)
				{
					bound_coords[dir].emplace(tuple_int(i, end_y));
				}
				else if (std::get<1>(dir) < 0)
				{
					bound_coords[dir].emplace(tuple_int(i, start_y));
				}
			}
			for (int j = start_y; j < end_y; j++)
			{
				if (std::get<0>(dir) > 0)
				{
					bound_coords[dir].emplace(tuple_int(end_x, j));
				}
				else if (std::get<0>(dir) < 0)
				{
					bound_coords[dir].emplace(tuple_int(start_x, j));
				}
			}
		}
	}
    tuple_set processed;
    tuple_set created;
	node_retrieval nodes;
	tuple_set preprocessed_maps;

	//Look into whether I need processed and preprocessed maps
	if (primary == tuple_int(0,0))
	{
		prepare_the_maps(primary, processed, created, maps, draw_maps_storage, bases, seamlessness, bound_coords);
		//manually step2 for wraparound? bool? put it in seamlessness equivalent?
	}

	std::vector<int> basic = { 1,2,3,4,0 };


	if ((primary == tuple_int(0, 0)) && (preprocessed_maps.count(primary) == 0))
	{
		std::vector<std::vector<int>> possible_move_costs =
		{
			{ 1,2,3,4,0 }, //basic move cost-used by fire and metal
			{ 1,1,2,4,0 }, //wood
			{ 1,2,2,4,2 }, //water
			{ 1,2,2,4,0 } //earth
		};
		std::cout << std::get<0>(primary) << "," << std::get<1>(primary);
		int cut_size = 64;
		preprocessed_maps.emplace(primary);
		std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>> map_set = cut(big_map_storage[primary], map_width, map_height, cut_size);
		map_set_storage[primary] = map_set;
		//nodes = entrances(map_set, big_map_storage[primary], cut_size, false, 0, 0, map_width / cut_size, map_height / cut_size, possible_move_costs);
	}




	auto t2 = std::chrono::high_resolution_clock::now();
	std::cout << "\nMap generation took: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " milliseconds\n";
    draw_everything(all, primary, draw_maps_storage);
    SDL_SetRenderDrawColor(grenderer, 0x00, 0x00, 0x00, 0xFF);
    //    for (tuple_int coord : all_node_locs)
    //    {
    //        SDL_RenderDrawPoint(grenderer, std::get<0>(coord), std::get<1>(coord));
    //    }
    SDL_RenderPresent(grenderer);
    bool mousedown=false;
    int mousestartx, mousestarty;
	int mouseposx, mouseposy;
    bool mousemovedwhiledown=false;
    SDL_Event e;
	int done_time, start_time, time_elapsed;
    while (!quit)
    {
		start_time = SDL_GetTicks();
		while (SDL_PollEvent(&e) != 0)
        {
            SDL_RenderClear(grenderer);
            if (e.type == SDL_QUIT)
            {
                quit=true;
                close();
                break;
            }
            switch (e.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                    switch (e.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        SDL_GetMouseState(&mousestartx, &mousestarty);
                        std::cout << mousestartx << "," << mousestarty;
						mousedown = true;
                        break;
					case SDL_BUTTON_RIGHT:
						//will never fire before finding out primary
						SDL_GetMouseState(&mouseposx, &mouseposy);
						for (unit &acting : selected)
						{
							tuple_int destination = tuple_int(mouseposx + camera_x, mouseposy + camera_y);
							int destination_map_x = (mouseposx + camera_x) / 512;
							int destination_map_y = (mouseposy + camera_y) / 512;
							acting.set_move_target(destination, nodes,
								map_set_storage[tuple_int(destination_map_x, destination_map_y)], 1, 64);
						}
                }
                    draw_everything(all, primary, draw_maps_storage);
                    SDL_RenderPresent(grenderer);
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (mousemovedwhiledown == true)
                    {
                        //select units (
                        mousemovedwhiledown = false;
                    }
					SDL_GetMouseState(&mouseposx, &mouseposy);
                    mousedown=false;
					selected = in_box(mousestartx, mousestarty, mouseposx, mouseposy, all);
                    draw_everything(all, primary, draw_maps_storage);
                    SDL_RenderPresent(grenderer);
                    break;
                case SDL_MOUSEMOTION:
                    int curx, cury;
                    draw_everything(all, primary, draw_maps_storage);
                    if (mousedown == true)
                    {
                        SDL_GetMouseState(&curx, &cury);
                        SDL_Rect selectbox = { mousestartx ,mousestarty,
                            curx - mousestartx,cury - mousestarty };
                        SDL_RenderDrawRect(grenderer, &selectbox);
                        mousemovedwhiledown = true;
                    }
                    SDL_RenderPresent(grenderer);
                    break;
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym)
                {
                    case SDLK_UP:
                        camera_y=camera_y-8;
                        break;
                    case SDLK_DOWN:
                        camera_y=camera_y+8;
                        break;
                    case SDLK_LEFT:
                        camera_x=camera_x-8;
                        break;
                    case SDLK_RIGHT:
                        camera_x=camera_x+8;
                        break;
					case SDLK_w:
						std::cout << "\n" << camera_x << "," << camera_y;
						break;
                }
                    primary=std::make_tuple(std::round(camera_x/512),
                                            std::round(camera_y/512));

					//&& (preprocessed_maps.count(primary)==0)
					if ((primary == tuple_int(0, 0)) && (preprocessed_maps.count(primary) == 0))
					{
						preprocessed_maps.emplace(primary);
						std::vector<std::vector<int>> possible_move_costs =
						{
							{ 1,2,3,4,0 }, //basic move cost-used by fire and metal
							{ 1,1,2,4,0 }, //wood
							{ 1,2,2,4,2 }, //water
							{ 1,2,2,4,0 } //earth
						};
						int cut_size = 64;
						std::unordered_map<tuple_int, vectormap, boost::hash<tuple_int>> map_set = cut(big_map_storage[primary], map_width, map_height, cut_size);
						nodes = entrances(map_set, big_map_storage[primary], cut_size, false, 0, 0, map_width / cut_size, map_height / cut_size, possible_move_costs);
					}

					/*
					if (primary == tuple_int(0,0))
					{
						prepare_the_maps(primary, processed, created, maps, draw_maps_storage, );
					}
					*/
					draw_everything(all, primary, draw_maps_storage);
                    SDL_RenderPresent(grenderer);
                    break;
            }
        }
		update();
		done_time = SDL_GetTicks();
		time_elapsed = done_time - start_time;
		if (time_elapsed < milliseconds_per_update)
		{
			SDL_Delay(milliseconds_per_update - time_elapsed);
		}
    }
#endif
    return 0;
}
