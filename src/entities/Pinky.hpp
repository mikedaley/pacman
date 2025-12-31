#pragma once

#include "entities/Ghost.hpp"

namespace pacman {

// Pinky - the pink ghost
// Chase behavior: targets 4 tiles ahead of Pacman's current direction
// This makes Pinky try to ambush Pacman from the front
class Pinky : public Ghost {
public:
    Pinky(TextureAtlas& atlas, const Maze& maze);
    ~Pinky() override = default;

    // Pinky starts inside the ghost house
    [[nodiscard]] bool startsInHouse() const override { return true; }

    // Pinky's dot limit to leave house (0 = leaves immediately when game starts)
    [[nodiscard]] i32 getDotLimit() const override { return 0; }

protected:
    // Pinky targets 4 tiles ahead of Pacman
    [[nodiscard]] Vec2i getChaseTarget(Vec2i pacmanTile, Direction pacmanDir) const override;

    // Scatter target: top-left corner
    [[nodiscard]] Vec2i getScatterTarget() const override;

    // Sprite name prefix
    [[nodiscard]] std::string getSpriteName() const override;

    // Starting position (inside ghost house, center)
    [[nodiscard]] Vec2f getStartPosition() const override;
};

}  // namespace pacman
