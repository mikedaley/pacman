#include "entities/Blinky.hpp"

namespace pacman {

Blinky::Blinky(TextureAtlas& atlas, const Maze& maze) : Ghost(atlas, maze, "Blinky") {
    reset();
}

Vec2i Blinky::getChaseTarget(Vec2i pacmanTile) const {
    // Blinky directly targets Pacman's current tile
    return pacmanTile;
}

Vec2i Blinky::getScatterTarget() const {
    // Blinky's scatter target is the top-right corner
    return Vec2i{25, -3};
}

std::string Blinky::getSpriteName() const {
    return "blinky";
}

Vec2f Blinky::getStartPosition() const {
    // Blinky starts just above the ghost house, centered horizontally
    // The ghost house door is at row 12, so Blinky starts at row 11
    // Tile (13, 11) in maze coordinates
    // Sprite is 16x16, position is top-left corner
    // Tile center: (13 * 8 + 4, 11 * 8 + 4) = (108, 92) in maze coords
    // Top-left of sprite: (108 - 8, 92 - 8) = (100, 84) in maze coords
    // Add MAZE_OFFSET_Y for screen coords: (100, 84 + 24) = (100, 108)
    return Vec2f{100.0f, 84.0f + constants::MAZE_OFFSET_Y};
}

}  // namespace pacman
