#include "entities/Clyde.hpp"

#include <cmath>

namespace pacman {

Clyde::Clyde(TextureAtlas& atlas, const Maze& maze) : Ghost(atlas, maze, "Clyde") {
    reset();
}

Vec2i Clyde::getChaseTarget(Vec2i pacmanTile, [[maybe_unused]] Direction pacmanDir) const {
    // Calculate Euclidean distance to Pacman
    Vec2i currentTile = getTile();
    i32 dx = currentTile.x - pacmanTile.x;
    i32 dy = currentTile.y - pacmanTile.y;
    f32 distance = std::sqrt(static_cast<f32>(dx * dx + dy * dy));

    // If distance is 8+ tiles, target Pacman directly (like Blinky)
    // If closer than 8 tiles, retreat to scatter corner
    if (distance >= static_cast<f32>(CHASE_DISTANCE_THRESHOLD)) {
        return pacmanTile;
    } else {
        return getScatterTarget();
    }
}

Vec2i Clyde::getScatterTarget() const {
    // Clyde's scatter target is the bottom-left corner
    return Vec2i{0, 35};
}

std::string Clyde::getSpriteName() const {
    return "clyde";
}

Vec2f Clyde::getStartPosition() const {
    // Clyde starts on the right side of ghost house
    // Ghost house interior ends at pixel 135, so right edge of sprite at 135
    // Sprite is 16 pixels wide, so left edge at 135 - 16 = 119
    // Center at 119 + 8 = 127
    return Vec2f{120.0f, 108.0f + constants::MAZE_OFFSET_Y};
}

}  // namespace pacman
