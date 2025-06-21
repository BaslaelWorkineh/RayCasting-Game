# Doom-Style Game in Pure C

A simple Doom-style 3D game engine written in pure C using SDL2 for graphics. This project demonstrates the fundamentals of raycasting-based 3D rendering, similar to the original Doom engine.

## Features

- **Raycasting 3D Engine**: Renders a 3D world from a 2D map using raycasting
- **Player Movement**: WASD controls for movement and rotation
- **Simple Map System**: 2D grid-based world with walls and empty spaces
- **Distance-based Shading**: Walls get darker with distance for depth perception
- **Real-time Rendering**: 60 FPS game loop with smooth movement

## Prerequisites

You need SDL2 development libraries installed on your system.

### Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install libsdl2-dev
```

### macOS (with Homebrew):
```bash
brew install sdl2
```

### Windows:
Download SDL2 development libraries from [libsdl.org](https://www.libsdl.org/download-2.0.php)

## Building and Running

### Quick Start:
```bash
# Install dependencies (Ubuntu/Debian)
make install-deps

# Build and run
make run
```

### Manual Build:
```bash
# Compile
make

# Run
./doom_game
```

### Debug Build:
```bash
make debug
./doom_game
```

## Controls

- **W** - Move forward
- **S** - Move backward  
- **A** - Turn left
- **D** - Turn right
- **ESC** - Quit game

## How It Works

### Raycasting Algorithm
1. For each column of pixels on the screen, cast a ray from the player's position
2. The ray travels in the direction determined by the player's viewing angle plus the column offset
3. When the ray hits a wall, calculate the distance
4. Draw a vertical line representing the wall, with height inversely proportional to distance

### Key Components

- **Player Structure**: Stores position (x, y) and viewing angle
- **2D Map**: Simple grid where 1 = wall, 0 = empty space
- **Ray Casting**: Core algorithm that determines what the player sees
- **Pixel Buffer**: Direct pixel manipulation for fast rendering

## Code Structure

```
main.c          - Main game logic and rendering
Makefile        - Build configuration
README.md       - This file
```

### Main Functions:

- `init_graphics()` - Initialize SDL2 and create window/renderer
- `render_frame()` - Main rendering loop using raycasting
- `cast_ray()` - Cast a single ray and return distance to wall
- `handle_input()` - Process keyboard input for player movement
- `update_player()` - Update player position and handle collisions

## Learning Objectives

This project teaches:

1. **3D Graphics Fundamentals**: Understanding how 3D rendering works without modern GPU APIs
2. **Raycasting**: The core technique used in early 3D games like Doom and Wolfenstein 3D
3. **Game Loop**: Real-time rendering and input handling
4. **Memory Management**: Direct pixel buffer manipulation
5. **Mathematical Concepts**: Trigonometry, vector math, and coordinate systems

## Next Steps for Enhancement

- Add textures to walls
- Implement sprites (enemies, items)
- Add multiple wall heights
- Implement doors and interactive objects
- Add sound effects
- Optimize rendering performance
- Add minimap display

## Technical Notes

- Uses SDL2 for cross-platform window management and input
- Renders at 800x600 resolution by default
- Simple collision detection prevents walking through walls
- Fish-eye effect correction for realistic perspective
- Distance-based shading for depth perception

## Troubleshooting

**Black screen**: Make sure SDL2 is properly installed
**Compilation errors**: Check that all SDL2 development headers are available
**Slow performance**: Try reducing screen resolution or ray precision

## License

This is educational code - feel free to use and modify for learning purposes.
# RayCasting-Game
