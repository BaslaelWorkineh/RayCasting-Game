#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#define MAP_WIDTH 10
#define MAP_HEIGHT 10

int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,1,0,1,0,1},
    {1,0,1,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1}
};

float playerX = 2.5f, playerY = 2.5f;
float playerAngle = 0.0f;

int main(){
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event;

    int running = 1;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "Doom Style Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    while (running) {
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 70, 120, 200, 255);
        SDL_Rect sky = {0, 0, 800, 300};
        SDL_RenderFillRect(renderer, &sky);

        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_Rect floor = {0, 300, 800, 300};
        SDL_RenderFillRect(renderer, &floor);

        int tileSize = 20;
        for(int y = 0; y < MAP_HEIGHT; y++){
            for(int x = 0; x < MAP_WIDTH; x++){
                if(map[y][x] == 1) {
                    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
                }
                SDL_Rect tile = {780 - (x * tileSize), 580 - (y * tileSize), tileSize, tileSize};
                SDL_RenderFillRect(renderer, &tile);
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect playerRect = {
            780 - ((int)(playerX * tileSize) - tileSize/4),
            580 - ((int)(playerY * tileSize) - tileSize/4),
            tileSize/2,
            tileSize/2
        };
        SDL_RenderFillRect(renderer, &playerRect);

        int lineLength = tileSize;
        int lineX = (int)(playerX * tileSize + cosf(playerAngle) * lineLength);
        int lineY = (int)(playerY * tileSize + sinf(playerAngle) * lineLength);
        SDL_RenderDrawLine(renderer,
            (int)(playerX * tileSize),
            (int)(playerY * tileSize),
            lineX, lineY
        );

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit;

    return 0;
}