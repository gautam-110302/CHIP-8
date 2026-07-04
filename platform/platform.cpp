#include "platform.h"

#include <iostream>

Platform::Platform(const char* title, int WindowWidth, int WindowHeight, int TextureWidth, int TextureHeight){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout<<"SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    }
    else{
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WindowWidth, WindowHeight, SDL_WINDOW_SHOWN);
        if(window == NULL){
            std::cout<<"Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        }
        else{
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, TextureWidth, TextureHeight);
        }
    }
}

Platform::~Platform(){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::Update(const void* buffer, int pitch){
    SDL_UpdateTexture(texture, NULL, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t* keys){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            return true;
        }
        else if(event.type == SDL_KEYDOWN){
            switch(event.key.keysym.sym){
                case SDLK_1: keys[0x1] = 1; break;
                case SDLK_2: keys[0x2] = 1; break;
                case SDLK_3: keys[0x3] = 1; break;
                case SDLK_4: keys[0xC] = 1; break;
                case SDLK_q: keys[0x4] = 1; break;
                case SDLK_w: keys[0x5] = 1; break;
                case SDLK_e: keys[0x6] = 1; break;
                case SDLK_r: keys[0xD] = 1; break;
                case SDLK_a: keys[0x7] = 1; break;
                case SDLK_s: keys[0x8] = 1; break;
                case SDLK_d: keys[0x9] = 1; break;
                case SDLK_f: keys[0xE] = 1; break;
                case SDLK_z: keys[0xA] = 1; break;
                case SDLK_x: keys[0x0] = 1; break;
                case SDLK_c: keys[0xB] = 1; break;
                case SDLK_v: keys[0xF] = 1; break;
            }
        }
        else if(event.type == SDL_KEYUP){
            switch(event.key.keysym.sym){
                case SDLK_1: keys[0x1] = 0; break;
                case SDLK_2: keys[0x2] = 0; break;
                case SDLK_3: keys[0x3] = 0; break;
                case SDLK_4: keys[0xC] = 0; break;
                case SDLK_q: keys[0x4] = 0; break;
                case SDLK_w: keys[0x5] = 0; break;
                case SDLK_e: keys[0x6] = 0; break;
                case SDLK_r: keys[0xD] = 0; break;
                case SDLK_a: keys[0x7] = 0; break;
                case SDLK_s: keys[0x8] = 0; break;
                case SDLK_d: keys[0x9] = 0; break;
                case SDLK_f: keys[0xE] = 0; break;
                case SDLK_z: keys[0xA] = 0; break;
                case SDLK_x: keys[0x0] = 0; break;
                case SDLK_c: keys[0xB] = 0; break;
                case SDLK_v: keys[0xF] = 0; break;
            }
        }
    }
    return false;
}