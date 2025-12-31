#pragma once

#include "entities/Ghost.hpp"

namespace pacman {

class Blinky;  // Forward declaration

// Inky - the cyan ghost
// Chase behavior: Uses Blinky's position as a pivot. Draws a vector from Blinky
// to 2 tiles ahead of Pacman, then doubles it to get the target.
// This creates unpredictable behavior that depends on Blinky's position.
class Inky : public Ghost {
public:
    Inky(TextureAtlas& atlas, const Maze& maze, const Blinky& blinky);
    ~Inky() override = default;

    // Inky starts inside the ghost house
    [[nodiscard]] bool startsInHouse() const override { return true; }

    // Inky's dot limit to leave house (30 on level 1)
    [[nodiscard]] i32 getDotLimit() const override { return 30; }

protected:
    // Inky's target depends on both Pacman and Blinky
    [[nodiscard]] Vec2i getChaseTarget(Vec2i pacmanTile, Direction pacmanDir) const override;

    // Scatter target: bottom-right corner
    [[nodiscard]] Vec2i getScatterTarget() const override;

    // Sprite name prefix
    [[nodiscard]] std::string getSpriteName() const override;

    // Starting position (inside ghost house, left side)
    [[nodiscard]] Vec2f getStartPosition() const override;

private:
    const Blinky& m_blinky;  // Reference to Blinky for targeting calculation
};

}  // namespace pacman
