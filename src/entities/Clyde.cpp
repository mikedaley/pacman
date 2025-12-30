#include "entities/Clyde.hpp"

#include <cmath>

namespace pacman {

Clyde::Clyde(TextureAtlas& atlas, const Maze& maze) : Ghost(atlas, maze, "Clyde") {
    reset();
}

Vec2i Clyde::getChaseTarget(Vec2i pacmanTile) const {
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
    // Clyde starts inside the ghost house on the right side
    // Ghost house interior is around tiles (11-16, 14-15)
    // Clyde is on the right: tile (15, 14)
    // Tile center: (15 * 8 + 4, 14 * 8 + 4) = (124, 116) in maze coords
    // Top-left of sprite: (124 - 8, 116 - 8) = (116, 108) in maze coords
    // Add MAZE_OFFSET_Y for screen coords
    return Vec2f{116.0f, 108.0f + constants::MAZE_OFFSET_Y};
}

}  // namespace pacman
