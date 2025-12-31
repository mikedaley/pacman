#include "entities/Blinky.hpp"

namespace pacman {

Blinky::Blinky(TextureAtlas& atlas, const Maze& maze) : Ghost(atlas, maze, "Blinky") {
    reset();
}

Vec2i Blinky::getChaseTarget(Vec2i pacmanTile, [[maybe_unused]] Direction pacmanDir) const {
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
    // Original Pac-Man: Blinky at tile (13.5, 11) - above ghost house door
    // Center: (13.5 * 8, 11 * 8 + 4) = (108, 92) in maze coords
    // Top-left of 16x16 sprite: (108 - 8, 92 - 8) = (100, 84)
    return Vec2f{100.0f, 84.0f + constants::MAZE_OFFSET_Y};
}

}  // namespace pacman
