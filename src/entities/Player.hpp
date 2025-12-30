#pragma once

#include "graphics/Sprite.hpp"
#include "graphics/TextureAtlas.hpp"
#include "map/Maze.hpp"
#include "utils/Constants.hpp"
#include "utils/Types.hpp"

namespace pacman {

class Renderer;

class Player {
public:
    Player(TextureAtlas& atlas, const Maze& maze);
    ~Player() = default;

    // Non-copyable, non-movable
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;
    Player(Player&&) = delete;
    Player& operator=(Player&&) = delete;

    // Update player position and animation
    void update(f32 deltaTime);

    // Render the player
    void render(Renderer& renderer);

    // Position management
    void setPosition(Vec2f pos);
    void setPosition(f32 x, f32 y);
    [[nodiscard]] Vec2f getPosition() const { return m_position; }
    [[nodiscard]] Vec2i getTile() const;

    // Direction control
    void setDesiredDirection(Direction dir);
    [[nodiscard]] Direction getDirection() const { return m_direction; }
    [[nodiscard]] Direction getDesiredDirection() const { return m_desiredDirection; }

    // Reset player to starting position
    void reset();

    // Animation control
    void updateAnimation(bool forceUpdate = false);
    void playDeathAnimation();
    void updateDeathAnimation(f32 deltaTime);
    [[nodiscard]] bool isDeathAnimationComplete() const;
    [[nodiscard]] i32 getDeathAnimFrame() const;

    // Visibility
    void setVisible(bool visible) { m_visible = visible; }
    [[nodiscard]] bool isVisible() const { return m_visible; }

    // Death animation constants
    static constexpr f32 DEATH_ANIM_FRAME_TIME = 0.1f;
    static constexpr i32 DEATH_ANIM_FRAMES = 11;

private:
    // Movement helpers
    [[nodiscard]] bool canMoveInDirection(Direction dir) const;
    [[nodiscard]] Vec2f getTileCenter(i32 tileX, i32 tileY) const;
    void tryTurn();

    Sprite m_sprite;
    const Maze& m_maze;

    Vec2f m_position{0.0f, 0.0f};
    Direction m_direction{Direction::None};
    Direction m_desiredDirection{Direction::None};
    Direction m_lastAnimDirection{Direction::None};

    bool m_visible{true};
    bool m_playingDeathAnim{false};
    f32 m_deathAnimTimer{0.0f};
};

}  // namespace pacman
