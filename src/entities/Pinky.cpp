#include "entities/Pinky.hpp"

namespace pacman {

Pinky::Pinky(TextureAtlas& atlas, const Maze& maze) : Ghost(atlas, maze, "Pinky") {
    reset();
}

Vec2i Pinky::getChaseTarget(Vec2i pacmanTile, Direction pacmanDir) const {
    // Pinky targets 4 tiles ahead of Pacman's current direction
    // This recreates the original arcade behavior, including the overflow bug
    // where facing Up also shifts 4 tiles Left
    constexpr i32 LOOK_AHEAD = 4;

    Vec2i dirVec = directionToVec(pacmanDir);
    Vec2i target = pacmanTile;

    target.x += dirVec.x * LOOK_AHEAD;
    target.y += dirVec.y * LOOK_AHEAD;

    // Replicate the original overflow bug: when Pac-Man faces up,
    // the target is also shifted 4 tiles to the left
    if (pacmanDir == Direction::Up) {
        target.x -= LOOK_AHEAD;
    }

    return target;
}

Vec2i Pinky::getScatterTarget() const {
    // Pinky's scatter target is the top-left corner
    return Vec2i{2, -3};
}

std::string Pinky::getSpriteName() const {
    return "pinky";
}

Vec2f Pinky::getStartPosition() const {
    // Pinky starts in the center of ghost house
    // Ghost house interior: columns 11-16 (pixels 88-135), center at 112
    // Top-left of 16x16 sprite: (112 - 8, 116 - 8) = (104, 108)
    return Vec2f{104.0f, 108.0f + constants::MAZE_OFFSET_Y};
}

}  // namespace pacman
