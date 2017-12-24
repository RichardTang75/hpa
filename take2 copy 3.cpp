//forest,mountain,water,marsh?,plain/grass
//
//  main.cpp
//  take2
//
//  Created by asdfuiop on 6/10/17.
//  Copyright © 2017 asdfuiop

// code for the pixel came from http://sol.gfxile.net/gp/ch02.html and 
// https://www.gamedev.net/forums/topic/358269-copying-pixels-to-an-sdl-surface/.
#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include "mapgen.hpp"
#include "goodfunctions.hpp"
#include "hierarchical_pathfind.hpp"
//#include <SDL.h>
//#include <SDL_image.h>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "test_init.hpp"
#include "country_controller.hpp"
//0=grass, 1=forest, 2=marsh, 3=mountain, 4=water,
//globals can go back into main if I really need it
int map_width=512;
int map_height=512;
int camera_x=256;
int camera_y=256;
std::vector<std::vector<int>> possible_move_costs=
{
    {1,2,3,4,0}, //basic move cost-used by fire and metal
    {1,1,2,4,0}, //wood
    {1,2,2,4,2}, //water
    {1,2,2,4,0} //earth
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
    Uint32* dstpixels=(Uint32*)temp->pixels;
    for (int y=0; y<(height);++y)
    {
        for (int x=0; x<(width);++x)
        {
            if (to_add.count(std::make_tuple(x,y))>0)
            {
                SDL_GetRGB(srcpixels[(scanline*y)+x],initial->format,&red,&green,&blue);
                dstpixels[(scanline*y)+x]=SDL_MapRGBA(temp->format,red,green,blue,0xFF);
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
std::vector<unit> in_box(int startx, int starty, int endx, int endy, std::vector<unit>& all_units)
{
	std::vector<unit> selected;
	for (unit possible : all_units)
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
/*
const int& map_x, const int& map_y,
const int& x_size, const int& y_size,
const int& side_size,
tuple_set& created,
std::unordered_map< std::tuple<int, int, int>, std::vector<tuple_set>, boost::hash<std::tuple<int, int, int>>>& neighbors,
unsigned long long init_seed = 0,
int terrain_type = 0
*/
void load_background(back_text& back, tuple_int& retrieve, tuple_triple_map& created, overflow_map& neighbors)
{
	std::tuple<std::vector<tuple_set>, vectormap> sets_map;
    sets_map = map_controller(std::get<0>(retrieve), std::get<1>(retrieve), map_width, map_height, created, neighbors);
	std::vector<tuple_set> temp_map = std::get<0>(sets_map);
	vectormap map = std::get<1>(sets_map);
	tuple_set forest, mount, water, marsh;
	forest = temp_map[0];
	mount = temp_map[1];
	water = temp_map[2];
	marsh = temp_map[3];
	back.simple_load("grass1t.png");
	back.surface_processing("marsh1t.png", marsh);
	back.surface_processing("forest1t.png", forest);
	back.surface_processing("mount1t.png", mount);
	back.surface_processing("water1t.png", water);
	back.make_text();
}
//max of 4 at once
void draw_everything(back_text& back, std::vector<unit>& all,
                     tuple_int& primary)
{
	back.render(0, 0);
	SDL_SetRenderDrawColor(grenderer, 0x00, 0x00, 0xFF, 0xFF);
	for (unit i : all)
	{
		i.render(grenderer);
	}
    std::vector<tuple_int> to_draw;
    if (camera_x-std::get<0>(primary)*512>256)
    {
        to_draw.push_back(std::make_tuple(std::get<0>(primary)+1,
                                          std::get<1>(primary)));
    }
    else if (camera_x-std::get<0>(primary)*512==256)
    {
        //this is meant to be nothing.
    }
    else
    {
        to_draw.push_back(std::make_tuple(std::get<0>(primary)-1,
                                          std::get<1>(primary)));
    }
    if (camera_y-std::get<1>(primary)*512>256)
    {
        to_draw.push_back(std::make_tuple(std::get<0>(primary),
                                          std::get<1>(primary)+1));
    }
    else if (camera_y-std::get<1>(primary)*512==256)
    {
        //this is also meant to be nothing.
    }
    else
    {
        to_draw.push_back(std::make_tuple(std::get<0>(primary),
                                          std::get<1>(primary)-1));
    }
}
void draw_everything(back_text& back, std::vector<unit>& all)
{
    back.render(0, 0);
    SDL_SetRenderDrawColor(grenderer, 0x00, 0x00, 0xFF, 0xFF);
    for (unit i : all)
    {
        i.render(grenderer);
    }
}
void prepare_the_maps(tuple_int primary,
                      tuple_set& finished,
                      overflow_map& neighbors,
                      tuple_triple_map& created)
{
    std::vector<tuple_int> first_dirs={
        tuple_int(-1,0), tuple_int(1,0),
        tuple_int(0,-1), tuple_int(0,1)};
    std::vector<tuple_int> second_dirs={
        tuple_int(-1,-1),tuple_int(-1,1),
        tuple_int(1,-1), tuple_int(1,1)
    };
    for (tuple_int dir: first_dirs)
    {
        tuple_int retrievin = tuple_int(std::get<0>(primary)+std::get<0>(dir),
                                        std::get<1>(primary)+std::get<1>(dir));
        if (finished.count(retrievin)==0)
        {
            map_controller(std::get<0>(retrievin), std::get<1>(retrievin), map_width, map_height, created, neighbors);
        }
    }
}
int main(int argc, char* argv[])
{

    if (!init(map_width,map_height))
    {
        std::cout<<"Failed to start \n";
    }
    bool quit=false;
    SDL_RenderClear(grenderer);
	std::vector<int> common = { 1,2,3,4,0 };
	unit temp(tuple_int(256, 256), 10, 10, 10, 10, 10, 10, 0, 0, 100, 100, 100, tuple_int(-1, -1), common);
	std::vector<unit> selected;
	std::vector<unit> all{ temp };
	back_text back(512, 512);
    tuple_triple_map created;
    overflow_map neighbors;
    tuple_int init=tuple_int(0,0);
	load_background(back, init, created, neighbors);
	draw_everything(back, all);
    SDL_RenderPresent(grenderer);
	bool mousedown=false;
	int mousestartx, mousestarty;
	bool mousemovedwhiledown=false;
	SDL_Event e;
    std::vector<tuple_int> to_draw;
	while (!quit) {
		while (SDL_PollEvent(&e) != 0)
		{
            tuple_int primary=std::make_tuple(std::trunc(camera_x/512),
                                              std::trunc(camera_y/512));
            //prepare_the_maps(primary);
            //std::vector<tuple_int> maps_to_draw=what_to_draw(primary);
            SDL_RenderClear(grenderer);
			if (e.type == SDL_QUIT)
			{
				quit = true;
				close();
				break;
			}
			switch (e.type)
			{
			case SDL_MOUSEBUTTONDOWN:
				draw_everything(back, all);
				switch (e.button.button)
				{
				case SDL_BUTTON_LEFT:
					SDL_GetMouseState(&mousestartx, &mousestarty);
					std::cout << mousestartx << "," << mousestarty;
					mousedown = true;
					break;
				}
				break;
				SDL_RenderPresent(grenderer);
			case SDL_MOUSEBUTTONUP:
				mousedown = false;
				draw_everything(back, all);
				if (mousemovedwhiledown == true)
				{
					//select units (
					mousemovedwhiledown = false;
				}
				SDL_RenderPresent(grenderer);
				break;
			case SDL_MOUSEMOTION:
				draw_everything(back, all);
				if (mousedown == true)
				{
					int curx, cury;
					SDL_GetMouseState(&curx, &cury);
					SDL_Rect selectbox = { mousestartx ,mousestarty,
											curx - mousestartx,cury - mousestarty };
					SDL_RenderDrawRect(grenderer, &selectbox);
					mousemovedwhiledown = true;
				}
				SDL_RenderPresent(grenderer);
				break;
			case SDL_KEYDOWN:
				draw_everything(back, all);
				switch (e.key.keysym.sym)
				{
				case SDLK_UP:
					std::cout << "UP";
					break;
                case SDLK_DOWN:
                    break;
                case SDLK_LEFT:
                    break;
                case SDLK_RIGHT:
                    break;
                }
				break;
			}
		}
		SDL_Delay(10);
	}
	return 0;
}

