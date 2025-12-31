#include "entities/Inky.hpp"

#include "entities/Blinky.hpp"

namespace pacman {

Inky::Inky(TextureAtlas& atlas, const Maze& maze, const Blinky& blinky)
    : Ghost(atlas, maze, "Inky"), m_blinky(blinky) {
    reset();
}

Vec2i Inky::getChaseTarget(Vec2i pacmanTile, Direction pacmanDir) const {
    // Inky's targeting is complex:
    // 1. Find a point 2 tiles ahead of Pacman
    // 2. Draw a vector from Blinky to that point
    // 3. Double the vector to get the final target
    constexpr i32 LOOK_AHEAD = 2;

    Vec2i dirVec = directionToVec(pacmanDir);
    Vec2i pivotPoint = pacmanTile;

    pivotPoint.x += dirVec.x * LOOK_AHEAD;
    pivotPoint.y += dirVec.y * LOOK_AHEAD;

    // Replicate the original overflow bug: when Pac-Man faces up,
    // the pivot point is also shifted 2 tiles to the left
    if (pacmanDir == Direction::Up) {
        pivotPoint.x -= LOOK_AHEAD;
    }

    // Get Blinky's current tile
    Vec2i blinkyTile = m_blinky.getTile();

    // Calculate vector from Blinky to pivot point, then double it
    i32 vectorX = pivotPoint.x - blinkyTile.x;
    i32 vectorY = pivotPoint.y - blinkyTile.y;

    Vec2i target;
    target.x = pivotPoint.x + vectorX;  // Effectively: blinkyTile + 2 * vector
    target.y = pivotPoint.y + vectorY;

    return target;
}

Vec2i Inky::getScatterTarget() const {
    // Inky's scatter target is the bottom-right corner
    return Vec2i{27, 35};
}

std::string Inky::getSpriteName() const {
    return "inky";
}

Vec2f Inky::getStartPosition() const {
    // Inky starts on the left side of ghost house
    // Ghost house interior: 48 pixels wide, center at pixel 112
    // Space 3 ghosts evenly: centers at 96, 112, 128 (16 pixels apart)
    // Inky center at 96, top-left = (96 - 8, 116 - 8) = (88, 108)
    return Vec2f{88.0f, 108.0f + constants::MAZE_OFFSET_Y};
}

}  // namespace pacman
