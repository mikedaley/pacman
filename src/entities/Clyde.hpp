#pragma once

#include "entities/Ghost.hpp"

namespace pacman {

// Clyde - the orange ghost
// Chase behavior: targets Pacman directly when 8+ tiles away,
//                 switches to scatter target when closer than 8 tiles
// This creates his "shy" or "pokey" personality - he backs off when close
class Clyde : public Ghost {
public:
    Clyde(TextureAtlas& atlas, const Maze& maze);
    ~Clyde() override = default;

    // Clyde starts inside the ghost house
    [[nodiscard]] bool startsInHouse() const override { return true; }

    // Clyde's dot limit to leave house (60 on level 1)
    [[nodiscard]] i32 getDotLimit() const override { return 1; }  // TODO: restore to 60

protected:
    // Clyde targets Pacman when far, scatter corner when close
    [[nodiscard]] Vec2i getChaseTarget(Vec2i pacmanTile, Direction pacmanDir) const override;

    // Scatter target: bottom-left corner
    [[nodiscard]] Vec2i getScatterTarget() const override;

    // Sprite name prefix
    [[nodiscard]] std::string getSpriteName() const override;

    // Starting position (inside ghost house, right side)
    [[nodiscard]] Vec2f getStartPosition() const override;

private:
    // Distance threshold for switching between chase and scatter behavior
    static constexpr i32 CHASE_DISTANCE_THRESHOLD = 8;  // tiles
};

}  // namespace pacman
