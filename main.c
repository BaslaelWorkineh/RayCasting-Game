#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define FOV (M_PI / 3.0)
#define PLAYER_HEIGHT 32.0F
#define TILE_SIZE 64
#define PROJ_PLANE_WIDTH 320
#define PROJ_PLANE_HEIGHT 200
#define WALL_STRIP_WIDTH 4                // Wider strips = fewer rays = better performance
#define NUM_RAYS (800 / WALL_STRIP_WIDTH) // Now only 200 rays instead of 400

int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 2, 0, 3, 3, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 2, 0, 1},
    {1, 0, 4, 0, 4, 0, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 1},
    {1, 0, 3, 3, 3, 0, 3, 3, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

float playerX = 1.5f * TILE_SIZE, playerY = 1.5f * TILE_SIZE;
float playerAngle = M_PI / 4.0f;
float playerZ = 0.0f;
float playerVelZ = 0.0f;
int isJumping = 0;

SDL_Texture* wall1Texture = NULL;
SDL_Texture* wall2Texture = NULL;
SDL_Texture* wall3Texture = NULL;
SDL_Texture* skyTexture = NULL;

void getWallColor(int wallType, int shade, int *r, int *g, int *b)
{
    switch (wallType)
    {
    case 1:
        *r = shade;
        *g = shade / 4;
        *b = shade / 4;
        break;
    case 2:
        *r = shade / 4;
        *g = shade;
        *b = shade / 4;
        break;
    case 3:
        *r = shade / 4;
        *g = shade / 4;
        *b = shade;
        break;
    case 4:
        *r = shade;
        *g = shade;
        *b = shade / 4;
        break;
    default:
        *r = shade;
        *g = shade;
        *b = shade;
        break;
    }
}

int main()
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    window = SDL_CreateWindow(
        "Doom Style Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface* wall1Surface = IMG_Load("wall1.png");
    if (wall1Surface) {
        wall1Texture = SDL_CreateTextureFromSurface(renderer, wall1Surface);
        SDL_FreeSurface(wall1Surface);
    }

    SDL_Surface* wall2Surface = IMG_Load("wall2.png");
    if (wall2Surface) {
        wall2Texture = SDL_CreateTextureFromSurface(renderer, wall2Surface);
        SDL_FreeSurface(wall2Surface);
    }

    SDL_Surface* wall3Surface = IMG_Load("wall3.png");
    if (wall3Surface) {
        wall3Texture = SDL_CreateTextureFromSurface(renderer, wall3Surface);
        SDL_FreeSurface(wall3Surface);
    }

    SDL_Surface* skySurface = IMG_Load("sky.jpeg");
    if (skySurface) {
        skyTexture = SDL_CreateTextureFromSurface(renderer, skySurface);
        SDL_FreeSurface(skySurface);
    }

    int running = 1;

    printf("=== DOOM-STYLE RAYCASTING DEMO ===\n");
    printf("Controls:\n");
    printf("  WASD or Arrow Keys - Move and turn\n");
    printf("  SPACE - Jump (jump over yellow walls!)\n");
    printf("  ESC - Quit\n\n");
    printf("MINIMAP (top-right):\n");
    printf("  Yellow dot = Player position\n");
    printf("  Colored walls = Red, Green, Blue, Yellow (jumpable)\n\n");

    float projPlaneDist = (PROJ_PLANE_WIDTH / 2.0f) / tanf(FOV / 2.0f);

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    running = 0;
                    break;
                case SDLK_UP:
                    printf("Up arrow pressed!\n");
                    break;
                case SDLK_DOWN:
                    printf("Down arrow pressed!\n");
                    break;
                case SDLK_LEFT:
                    printf("Left arrow pressed!\n");
                    break;
                case SDLK_RIGHT:
                    printf("Right arrow pressed!\n");
                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        int horizon = SCREEN_HEIGHT / 2 + (int)playerZ;

        if (skyTexture) {
            SDL_RenderCopy(renderer, skyTexture, NULL, NULL);
        } else {
            SDL_SetRenderDrawColor(renderer, 70, 120, 200, 255);
            SDL_Rect sky = {0, 0, SCREEN_WIDTH, horizon};
            SDL_RenderFillRect(renderer, &sky);
        }

        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_Rect floor = {0, horizon, SCREEN_WIDTH, SCREEN_HEIGHT - horizon};
        SDL_RenderFillRect(renderer, &floor);


        for (int col = 0; col < NUM_RAYS; col++)
        {
            float rayAngle = playerAngle - (FOV / 2.0f) + (col * FOV / NUM_RAYS);
            float rayX = playerX;
            float rayY = playerY;
            float rayDirX = cosf(rayAngle);
            float rayDirY = sinf(rayAngle);

            int mapX = (int)(rayX / TILE_SIZE);
            int mapY = (int)(rayY / TILE_SIZE);

            float deltaDistX = fabsf(1.0f / rayDirX);
            float deltaDistY = fabsf(1.0f / rayDirY);

            float sideDistX, sideDistY;
            int stepX, stepY;
            int hit = 0;
            int side;

            if (rayDirX < 0)
            {
                stepX = -1;
                sideDistX = (rayX / TILE_SIZE - mapX) * deltaDistX;
            }
            else
            {
                stepX = 1;
                sideDistX = (mapX + 1.0f - rayX / TILE_SIZE) * deltaDistX;
            }

            if (rayDirY < 0)
            {
                stepY = -1;
                sideDistY = (rayY / TILE_SIZE - mapY) * deltaDistY;
            }
            else
            {
                stepY = 1;
                sideDistY = (mapY + 1.0f - rayY / TILE_SIZE) * deltaDistY;
            }

            while (hit == 0)
            {
                if (sideDistX < sideDistY)
                {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                }
                else
                {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }

                if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT ||
                    map[mapY][mapX] > 0)
                {
                    hit = 1;
                }
            }

            float distance;
            if (side == 0)
            {
                distance = (mapX - rayX / TILE_SIZE + (1 - stepX) / 2) / rayDirX * TILE_SIZE;
            }
            else
            {
                distance = (mapY - rayY / TILE_SIZE + (1 - stepY) / 2) / rayDirY * TILE_SIZE;
            }

            // REMOVED: distance *= cosf(rayAngle - playerAngle);
            // This was causing distortion. The distance from the DDA algorithm is already corrected.
            int wallHeight = (int)(PROJ_PLANE_HEIGHT * TILE_SIZE / (distance + 0.0001f));

            int horizon = 300 + (int)playerZ;
            int wallTop = horizon - wallHeight / 2;
            int wallBottom = horizon + wallHeight / 2;

            if (wallTop < 0)
                wallTop = 0;
            if (wallBottom >= 600)
                wallBottom = 599;

            int screenX = col * WALL_STRIP_WIDTH;
            int shade = 255 - (int)(distance * 255 / 400.0f);
            if (shade < 50)
                shade = 50;
            if (shade > 255)
                shade = 255;

            int wallType = (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT) ? map[mapY][mapX] : 1;

            // --- STABLE TEXTURE COORDINATE CALCULATION ---
            // This prevents the textures from flickering or "swimming" during movement.
            float wall_hit_coord;
            if (side == 0) { // Hit a vertical wall line
                wall_hit_coord = playerY + distance * rayDirY;
            } else { // Hit a horizontal wall line
                wall_hit_coord = playerX + distance * rayDirX;
            }
            
            // Normalize the hit coordinate to a 0.0-1.0 value within the tile.
            float wallX_normalized = fmodf(wall_hit_coord / (float)TILE_SIZE, 1.0f);

            int texX = (int)(wallX_normalized * 32.0f);
            if (side == 0 && rayDirX > 0) texX = 32 - texX - 1;
            if (side == 1 && rayDirY < 0) texX = 32 - texX - 1;

            SDL_Rect srcRect = {texX * 2, 0, 2, 64};
            SDL_Rect wallSlice = {screenX, wallTop, WALL_STRIP_WIDTH, wallBottom - wallTop};

            if (wallType == 1) {
                if (wall1Texture) {
                    SDL_SetTextureColorMod(wall1Texture, shade, shade, shade);
                    SDL_RenderCopy(renderer, wall1Texture, &srcRect, &wallSlice);
                } else {
                    SDL_SetRenderDrawColor(renderer, shade, shade/4, shade/4, 255);
                    SDL_RenderFillRect(renderer, &wallSlice);
                }
            } else if (wallType == 2) {
                if (wall2Texture) {
                    SDL_SetTextureColorMod(wall2Texture, shade, shade, shade);
                    SDL_RenderCopy(renderer, wall2Texture, &srcRect, &wallSlice);
                } else {
                    SDL_SetRenderDrawColor(renderer, shade/4, shade, shade/4, 255);
                    SDL_RenderFillRect(renderer, &wallSlice);
                }
            } else if (wallType == 3) {
                if (wall3Texture) {
                    SDL_SetTextureColorMod(wall3Texture, shade, shade, shade);
                    SDL_RenderCopy(renderer, wall3Texture, &srcRect, &wallSlice);
                } else {
                    SDL_SetRenderDrawColor(renderer, shade/4, shade/4, shade, 255);
                    SDL_RenderFillRect(renderer, &wallSlice);
                }
            } else if (wallType == 4) {
                if (wall1Texture) {
                    SDL_SetTextureColorMod(wall1Texture, shade, shade, shade);
                    SDL_RenderCopy(renderer, wall1Texture, &srcRect, &wallSlice);
                } else {
                    SDL_SetRenderDrawColor(renderer, shade, shade, shade/4, 255);
                    SDL_RenderFillRect(renderer, &wallSlice);
                }
            } else {
                SDL_SetRenderDrawColor(renderer, shade, shade, shade, 255);
                SDL_RenderFillRect(renderer, &wallSlice);
            }
        }

        int minimapSize = 15;
        int minimapOffsetX = 600;
        int minimapOffsetY = 20;

        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                switch (map[y][x])
                {
                case 0:
                    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
                    break;
                case 1:
                    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
                    break;
                case 2:
                    SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
                    break;
                case 3:
                    SDL_SetRenderDrawColor(renderer, 50, 50, 200, 255);
                    break;
                case 4:
                    SDL_SetRenderDrawColor(renderer, 200, 200, 50, 255);
                    break;
                default:
                    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                    break;
                }
                SDL_Rect tile = {minimapOffsetX + (x * minimapSize), minimapOffsetY + (y * minimapSize), minimapSize, minimapSize};
                SDL_RenderFillRect(renderer, &tile);
            }
        }

        int playerMapX = minimapOffsetX + (int)(playerX / TILE_SIZE) * minimapSize + minimapSize / 2;
        int playerMapY = minimapOffsetY + (int)(playerY / TILE_SIZE) * minimapSize + minimapSize / 2;
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_Rect playerDot = {playerMapX - 2, playerMapY - 2, 4, 4};
        SDL_RenderFillRect(renderer, &playerDot);

        const float gravity = 0.8f;
        const float jumpPower = 12.0f;

        if (isJumping)
        {
            playerZ += playerVelZ;
            playerVelZ -= gravity;

            if (playerZ <= 0.0f)
            {
                playerZ = 0.0f;
                playerVelZ = 0.0f;
                isJumping = 0;
            }
        }

        const Uint8 *keystate = SDL_GetKeyboardState(NULL);
        float moveSpeed = 1.0f;
        float rotSpeed = 0.01f;

        if (keystate[SDL_SCANCODE_SPACE] && !isJumping)
        {
            isJumping = 1;
            playerVelZ = jumpPower;
        }

        if (keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A])
        {
            playerAngle -= rotSpeed;
        }
        if (keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D])
        {
            playerAngle += rotSpeed;
        }
        if (keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W])
        {
            float newX = playerX + cosf(playerAngle) * moveSpeed;
            float newY = playerY + sinf(playerAngle) * moveSpeed;
            int mapX = (int)(newX / TILE_SIZE);
            int mapY = (int)(newY / TILE_SIZE);

            int canMove = 0;
            if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT)
            {
                if (map[mapY][mapX] == 0)
                {
                    canMove = 1;
                }
                else if (map[mapY][mapX] == 4 && playerZ > 20.0f)
                {
                    canMove = 1;
                }
            }

            if (canMove)
            {
                playerX = newX;
                playerY = newY;
            }
        }
        if (keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S])
        {
            float newX = playerX - cosf(playerAngle) * moveSpeed;
            float newY = playerY - sinf(playerAngle) * moveSpeed;
            int mapX = (int)(newX / TILE_SIZE);
            int mapY = (int)(newY / TILE_SIZE);

            int canMove = 0;
            if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT)
            {
                if (map[mapY][mapX] == 0)
                {
                    canMove = 1;
                }
                else if (map[mapY][mapX] == 4 && playerZ > 20.0f)
                {
                    canMove = 1;
                }
            }

            if (canMove)
            {
                playerX = newX;
                playerY = newY;
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(8);
    }

    if (wall1Texture) SDL_DestroyTexture(wall1Texture);
    if (wall2Texture) SDL_DestroyTexture(wall2Texture);
    if (wall3Texture) SDL_DestroyTexture(wall3Texture);
    if (skyTexture) SDL_DestroyTexture(skyTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}