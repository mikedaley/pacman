#pragma once

#include "utils/Types.hpp"

namespace pacman::constants {

// Game resolution (classic Pacman: 224x288)
inline constexpr i32 TILE_SIZE = 8;
inline constexpr i32 MAZE_WIDTH = 28;                      // tiles
inline constexpr i32 MAZE_HEIGHT = 31;                     // tiles (actual maze)
inline constexpr i32 HEADER_HEIGHT = 3 * TILE_SIZE;        // 24 pixels for score area
inline constexpr i32 FOOTER_HEIGHT = 2 * TILE_SIZE;        // 16 pixels for lives/fruit
inline constexpr i32 GAME_WIDTH = MAZE_WIDTH * TILE_SIZE;  // 224
inline constexpr i32 GAME_HEIGHT =
    HEADER_HEIGHT + (MAZE_HEIGHT * TILE_SIZE) + FOOTER_HEIGHT;  // 288
inline constexpr i32 MAZE_OFFSET_Y = HEADER_HEIGHT;             // Maze starts after header

// Window settings (scaled up for modern displays)
inline constexpr i32 WINDOW_SCALE = 3;
inline constexpr i32 WINDOW_WIDTH = GAME_WIDTH * WINDOW_SCALE;    // 672
inline constexpr i32 WINDOW_HEIGHT = GAME_HEIGHT * WINDOW_SCALE;  // 864
inline constexpr const char* WINDOW_TITLE = "Pacman";

// Game settings
inline constexpr f32 FIXED_TIMESTEP = 1.0f / 60.0f;  // 60 FPS fixed update
inline constexpr i32 MAX_UPDATES_PER_FRAME = 5;      // Prevent spiral of death

// Base speed (100% speed in original arcade = 75.75757625 pixels/sec)
inline constexpr f32 BASE_SPEED = 75.75757625f;

// Pacman settings (Level 1: 80% speed)
inline constexpr f32 PACMAN_SPEED = BASE_SPEED * 0.80f;  // ~60.6 pixels per second
inline constexpr i32 PACMAN_START_LIVES = 3;
inline constexpr f32 PACMAN_ANIM_SPEED = 15.0f;  // frames per second (chomps ~7.5 times/sec)

// Ghost settings (Level 1 speeds from original game)
inline constexpr f32 GHOST_SPEED = BASE_SPEED * 0.75f;             // ~56.8 pixels/sec (75%)
inline constexpr f32 GHOST_FRIGHTENED_SPEED = BASE_SPEED * 0.50f;  // ~37.9 pixels/sec (50%)
inline constexpr f32 GHOST_TUNNEL_SPEED = BASE_SPEED * 0.40f;      // ~30.3 pixels/sec (40%)

// Timing
inline constexpr f32 FRIGHTENED_DURATION = 6.0f;    // seconds
inline constexpr f32 FRIGHTENED_FLASH_TIME = 2.0f;  // seconds before end to start flashing
inline constexpr f32 GHOST_FLASH_SPEED = 20.0f;     // frames per second for flashing animation

// Scoring
inline constexpr i32 SCORE_PELLET = 10;
inline constexpr i32 SCORE_POWER_PELLET = 50;

}  // namespace pacman::constants
