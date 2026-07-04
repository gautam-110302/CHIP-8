#pragma once

#include <SDL2/SDL.h>

class Platform{
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
public:
    Platform(const char* title, int WindowWidth, int WindowHeight, int TextureWidth, int TextureHeight);
    ~Platform();
    void Update(void const* buffer, int pitch);
    bool ProcessInput(uint8_t* keys);
};