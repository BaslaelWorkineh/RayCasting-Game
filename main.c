#include <stdio.h>
#include <SDL2/SDL.h>

int main(){
    SDL_Window *window = NULL;
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "Doom Style Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    SDL_Delay(3000);
    SDL_DestroyWindow(window);
    SDL_Quit;

    return 0;
}