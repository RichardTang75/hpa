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
#include <SDL.h>
#include <SDL_image.h>
//#include <SDL2/SDL.h>
//#include <SDL2_image/SDL_image.h>
#include "test_init.hpp"
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
    SDL_Rect sizer={width/4,height/4,7,3};
    SDL_Rect sizer2={width/4+1,height/4+1,5,1};
    SDL_RenderDrawRect(grenderer, &sizer);
    SDL_SetRenderDrawColor(grenderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(grenderer, &sizer2);
    //
    map_stuff(width,height,map);
    SDL_RenderPresent(grenderer);
	std::vector<unit> selected;
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

