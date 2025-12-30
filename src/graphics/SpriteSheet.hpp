#pragma once

#include "graphics/TextureAtlas.hpp"

namespace pacman {

// Defines all sprite regions for the Pacman sprite sheet
// Call this after creating a TextureAtlas to set up all named regions
inline void defineSpriteRegions(TextureAtlas& atlas) {
    // ============================================
    // PACMAN SPRITES (16x16)
    // ============================================

    // Pacman facing right (row 0, starting at x=456)
    atlas.defineRegion("pacman_right_open", 456, 0, 16, 16);
    atlas.defineRegion("pacman_right_half", 472, 0, 16, 16);
    atlas.defineRegion("pacman_closed", 488, 0, 16, 16);  // Closed is shared for all directions

    // Pacman facing left (row 1, starting at x=456)
    atlas.defineRegion("pacman_left_open", 456, 16, 16, 16);
    atlas.defineRegion("pacman_left_half", 472, 16, 16, 16);

    // Pacman facing up (row 2, starting at x=456)
    atlas.defineRegion("pacman_up_open", 456, 32, 16, 16);
    atlas.defineRegion("pacman_up_half", 472, 32, 16, 16);

    // Pacman facing down (row 3, starting at x=456)
    atlas.defineRegion("pacman_down_open", 456, 48, 16, 16);
    atlas.defineRegion("pacman_down_half", 472, 48, 16, 16);

    // Pacman death animation (sprites 3-13 on row 0, starting at x=504)
    atlas.defineGrid("pacman_death", 504, 0, 16, 16, 11, 1);

    // Large Pacman (32x32) for title/cutscenes
    atlas.defineRegion("pacman_large_open", 488, 16, 32, 32);
    atlas.defineRegion("pacman_large_half", 520, 16, 32, 32);
    atlas.defineRegion("pacman_large_closed", 552, 16, 32, 32);

    // ============================================
    // GHOST SPRITES (16x16)
    // ============================================

    // Ghost body animations (2 frames each for wiggle)
    // Row starts at y=64

    // Blinky (Red) - facing right, left, up, down (2 frames each)
    atlas.defineRegion("blinky_right_0", 456, 64, 16, 16);
    atlas.defineRegion("blinky_right_1", 472, 64, 16, 16);
    atlas.defineRegion("blinky_left_0", 488, 64, 16, 16);
    atlas.defineRegion("blinky_left_1", 504, 64, 16, 16);
    atlas.defineRegion("blinky_up_0", 520, 64, 16, 16);
    atlas.defineRegion("blinky_up_1", 536, 64, 16, 16);
    atlas.defineRegion("blinky_down_0", 552, 64, 16, 16);
    atlas.defineRegion("blinky_down_1", 568, 64, 16, 16);

    // Pinky (Pink)
    atlas.defineRegion("pinky_right_0", 456, 80, 16, 16);
    atlas.defineRegion("pinky_right_1", 472, 80, 16, 16);
    atlas.defineRegion("pinky_left_0", 488, 80, 16, 16);
    atlas.defineRegion("pinky_left_1", 504, 80, 16, 16);
    atlas.defineRegion("pinky_up_0", 520, 80, 16, 16);
    atlas.defineRegion("pinky_up_1", 536, 80, 16, 16);
    atlas.defineRegion("pinky_down_0", 552, 80, 16, 16);
    atlas.defineRegion("pinky_down_1", 568, 80, 16, 16);

    // Inky (Cyan)
    atlas.defineRegion("inky_right_0", 456, 96, 16, 16);
    atlas.defineRegion("inky_right_1", 472, 96, 16, 16);
    atlas.defineRegion("inky_left_0", 488, 96, 16, 16);
    atlas.defineRegion("inky_left_1", 504, 96, 16, 16);
    atlas.defineRegion("inky_up_0", 520, 96, 16, 16);
    atlas.defineRegion("inky_up_1", 536, 96, 16, 16);
    atlas.defineRegion("inky_down_0", 552, 96, 16, 16);
    atlas.defineRegion("inky_down_1", 568, 96, 16, 16);

    // Clyde (Orange)
    atlas.defineRegion("clyde_right_0", 456, 112, 16, 16);
    atlas.defineRegion("clyde_right_1", 472, 112, 16, 16);
    atlas.defineRegion("clyde_left_0", 488, 112, 16, 16);
    atlas.defineRegion("clyde_left_1", 504, 112, 16, 16);
    atlas.defineRegion("clyde_up_0", 520, 112, 16, 16);
    atlas.defineRegion("clyde_up_1", 536, 112, 16, 16);
    atlas.defineRegion("clyde_down_0", 552, 112, 16, 16);
    atlas.defineRegion("clyde_down_1", 568, 112, 16, 16);

    // Frightened ghost (blue)
    atlas.defineRegion("ghost_frightened_0", 584, 64, 16, 16);
    atlas.defineRegion("ghost_frightened_1", 600, 64, 16, 16);

    // Frightened ghost flashing (white) - next to blue frightened on same row
    atlas.defineRegion("ghost_flash_0", 616, 64, 16, 16);
    atlas.defineRegion("ghost_flash_1", 632, 64, 16, 16);

    // Ghost eyes (when eaten) - right, left, up, down
    // Eyes are on third page, row 5 (0-indexed), starting column 8 (0-indexed)
    atlas.defineRegion("ghost_eyes_right", 584, 80, 16, 16);
    atlas.defineRegion("ghost_eyes_left", 600, 80, 16, 16);
    atlas.defineRegion("ghost_eyes_up", 616, 80, 16, 16);
    atlas.defineRegion("ghost_eyes_down", 632, 80, 16, 16);

    // ============================================
    // FRUIT SPRITES (16x16)
    // ============================================
    atlas.defineRegion("fruit_cherry", 488, 48, 16, 16);
    atlas.defineRegion("fruit_strawberry", 504, 48, 16, 16);
    atlas.defineRegion("fruit_orange", 520, 48, 16, 16);
    atlas.defineRegion("fruit_apple", 536, 48, 16, 16);
    atlas.defineRegion("fruit_melon", 552, 48, 16, 16);
    atlas.defineRegion("fruit_galaxian", 568, 48, 16, 16);
    atlas.defineRegion("fruit_bell", 584, 48, 16, 16);
    atlas.defineRegion("fruit_key", 600, 48, 16, 16);

    // ============================================
    // MAZE TILES (8x8)
    // ============================================

    // Pellets
    atlas.defineRegion("pellet", 8, 8, 8, 8);         // Small dot
    atlas.defineRegion("power_pellet", 8, 24, 8, 8);  // Large flashing pellet

    // ============================================
    // SCORE POINT SPRITES
    // ============================================
    atlas.defineRegion("points_200", 456, 128, 16, 16);
    atlas.defineRegion("points_400", 472, 128, 16, 16);
    atlas.defineRegion("points_800", 488, 128, 16, 16);
    atlas.defineRegion("points_1600", 504, 128, 16, 16);

    // Fruit scores
    atlas.defineRegion("points_100", 456, 144, 16, 16);
    atlas.defineRegion("points_300", 472, 144, 16, 16);
    atlas.defineRegion("points_500", 488, 144, 16, 16);
    atlas.defineRegion("points_700", 504, 144, 16, 16);
    atlas.defineRegion("points_1000", 520, 144, 24, 16);
    atlas.defineRegion("points_2000", 456, 160, 24, 16);
    atlas.defineRegion("points_3000", 480, 160, 24, 16);
    atlas.defineRegion("points_5000", 504, 160, 24, 16);
}

}  // namespace pacman
