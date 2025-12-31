#pragma once

#include "entities/Ghost.hpp"

namespace pacman {

// Blinky - the red ghost
// Chase behavior: directly targets Pacman's current tile
// Most aggressive ghost, always following Pacman
class Blinky : public Ghost {
public:
    Blinky(TextureAtlas& atlas, const Maze& maze);
    ~Blinky() override = default;

protected:
    // Blinky targets Pacman's exact position
    [[nodiscard]] Vec2i getChaseTarget(Vec2i pacmanTile, Direction pacmanDir) const override;

    // Scatter target: top-right corner
    [[nodiscard]] Vec2i getScatterTarget() const override;

    // Sprite name prefix
    [[nodiscard]] std::string getSpriteName() const override;

    // Starting position (above ghost house)
    [[nodiscard]] Vec2f getStartPosition() const override;
};

}  // namespace pacman
