#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define FOV (M_PI / 3.0)
#define PLAYER_HEIGHT 32.0F
#define TILE_SIZE 64
#define PROJ_PLANE_WIDTH 320
#define PROJ_PLANE_HEIGHT 200

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


float playerX = 1.5f * TILE_SIZE, playerY = 1.5f * TILE_SIZE;
float playerAngle = M_PI / 4.0f;

int main(){
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event;

    int running = 1;

    SDL_Init(SDL_INIT_VIDEO);

    float projPlaneDist = (PROJ_PLANE_WIDTH / 2.0f) / tanf(FOV / 2.0f);

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

        // int tileSize = 20;
        // for(int y = 0; y < MAP_HEIGHT; y++){
        //     for(int x = 0; x < MAP_WIDTH; x++){
        //         if(map[y][x] == 1) {
        //             SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        //         } else {
        //             SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        //         }
        //         SDL_Rect tile = {780 - (x * tileSize), 580 - (y * tileSize), tileSize, tileSize};
        //         SDL_RenderFillRect(renderer, &tile);
        //     }
        // }

       
         // --- Ray casting loop ---
         for (int col = 0; col < PROJ_PLANE_WIDTH; col++) {
            // Calculate the angle of the ray for this column
            float rayAngle = playerAngle - (FOV / 2.0f) + (col * FOV / PROJ_PLANE_WIDTH);

            // Ray position and direction
            float rayX = playerX;
            float rayY = playerY;
            float rayDirX = cosf(rayAngle);
            float rayDirY = sinf(rayAngle);

            // Step size for ray
            float stepSize = 1.0f;
            float distance = 0.0f;
            int hit = 0;

            // Step along the ray until we hit a wall or go out of bounds
            while (!hit && distance < 1000.0f) {
                rayX += rayDirX * stepSize;
                rayY += rayDirY * stepSize;
                distance += stepSize;

                int mapX = (int)(rayX / TILE_SIZE);
                int mapY = (int)(rayY / TILE_SIZE);

                if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) {
                    hit = 1; // Out of bounds
                } else if (map[mapY][mapX] == 1) {
                    hit = 1; // Hit a wall
                }
            }

            // Draw a vertical line for the wall slice (for now, just a simple effect)
            int wallHeight = (int)(PROJ_PLANE_HEIGHT * TILE_SIZE / (distance + 0.0001f));
            int wallTop = (300 - wallHeight / 2);
            int wallBottom = (300 + wallHeight / 2);

            SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
            SDL_RenderDrawLine(renderer, col * 2.5 , wallTop, col * 2.5 , wallBottom);
        }

        //player movement
         const Uint8 *keystate = SDL_GetKeyboardState(NULL);
         float moveSpeed = 2.0f;
         float rotSpeed = 0.05f;
 
         if (keystate[SDL_SCANCODE_LEFT]) {
             playerAngle -= rotSpeed;
         }
         if (keystate[SDL_SCANCODE_RIGHT]) {
             playerAngle += rotSpeed;
         }
         if (keystate[SDL_SCANCODE_UP]) {
             playerX += cosf(playerAngle) * moveSpeed;
             playerY += sinf(playerAngle) * moveSpeed;
         }
         if (keystate[SDL_SCANCODE_DOWN]) {
             playerX -= cosf(playerAngle) * moveSpeed;
             playerY -= sinf(playerAngle) * moveSpeed;
         }
        
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit;

    return 0;
}