#pragma once

#include "graphics/Sprite.hpp"
#include "graphics/TextureAtlas.hpp"
#include "map/Maze.hpp"
#include "utils/Constants.hpp"
#include "utils/Types.hpp"

#include <string>

namespace pacman {

class Renderer;

// Ghost behavior states
enum class GhostState {
    InHouse,       // Inside ghost house, bouncing and waiting to exit
    LeavingHouse,  // Exiting the ghost house through the door
    Scatter,       // Move to corner target
    Chase,         // Chase Pacman (AI varies per ghost)
    Frightened,    // Blue, can be eaten by Pacman
    Eaten,         // Just eaten, showing score sprite (game paused)
    Eyes           // Returning to ghost house after being eaten
};

// Base class for all ghosts
class Ghost {
public:
    Ghost(TextureAtlas& atlas, const Maze& maze, std::string_view name);
    virtual ~Ghost() = default;

    // Non-copyable, non-movable
    Ghost(const Ghost&) = delete;
    Ghost& operator=(const Ghost&) = delete;
    Ghost(Ghost&&) = delete;
    Ghost& operator=(Ghost&&) = delete;

    // Update ghost position and animation
    void update(f32 deltaTime, Vec2i pacmanTile);

    // Render the ghost
    void render(Renderer& renderer);

    // Position management
    void setPosition(Vec2f pos);
    void setPosition(f32 x, f32 y);
    [[nodiscard]] Vec2f getPosition() const { return m_position; }
    [[nodiscard]] Vec2i getTile() const;

    // Direction
    void setDirection(Direction dir);
    [[nodiscard]] Direction getDirection() const { return m_direction; }

    // State management
    void setState(GhostState state);
    [[nodiscard]] GhostState getState() const { return m_state; }
    void setFrightened();
    void setEaten();          // Called when Pacman eats this ghost
    void transitionToEyes();  // Called after eaten pause to start returning home
    [[nodiscard]] bool isFrightened() const { return m_state == GhostState::Frightened; }
    [[nodiscard]] bool isEaten() const { return m_state == GhostState::Eaten; }
    [[nodiscard]] bool isEyes() const { return m_state == GhostState::Eyes; }

    // Reset ghost to starting position
    virtual void reset();

    // Check if ghost starts inside the house
    [[nodiscard]] virtual bool startsInHouse() const { return false; }

    // Get the dot limit for this ghost to leave the house (0 = leave immediately)
    [[nodiscard]] virtual i32 getDotLimit() const { return 0; }

    // Release ghost from house (called when dot limit reached)
    void releaseFromHouse();

protected:
    // Override in derived classes to provide AI target
    [[nodiscard]] virtual Vec2i getChaseTarget(Vec2i pacmanTile) const = 0;
    [[nodiscard]] virtual Vec2i getScatterTarget() const = 0;

    // Get the sprite name prefix for this ghost (e.g., "blinky", "pinky")
    [[nodiscard]] virtual std::string getSpriteName() const = 0;

    // Starting position for this ghost
    [[nodiscard]] virtual Vec2f getStartPosition() const = 0;

    // Update animation frames based on direction and state
    void updateAnimation();

    // Movement helpers
    [[nodiscard]] bool canMoveInDirection(Direction dir) const;
    [[nodiscard]] Direction chooseDirection(Vec2i targetTile);
    [[nodiscard]] f32 getSpeed() const;
    [[nodiscard]] Vec2f getTileCenter(i32 tileX, i32 tileY) const;

    // Check if at an intersection (multiple paths available)
    [[nodiscard]] bool isAtIntersection() const;

    Sprite m_sprite;
    const Maze& m_maze;
    std::string m_name;

    Vec2f m_position{0.0f, 0.0f};
    Direction m_direction{Direction::Left};
    Direction m_nextDirection{Direction::None};

    GhostState m_state{GhostState::Scatter};
    f32 m_stateTimer{0.0f};
    f32 m_frightenedTimer{0.0f};

    // Track if we've made a decision at current tile
    Vec2i m_lastDecisionTile{-1, -1};

    // Animation tracking
    Direction m_lastAnimDirection{Direction::None};
    bool m_isFlashing{false};

    // Ghost house behavior
    bool m_bouncingUp{false};                        // Direction of bounce in house
    static constexpr f32 HOUSE_CENTER_Y = 14.0f;     // Center row of ghost house
    static constexpr f32 HOUSE_BOUNCE_RANGE = 0.5f;  // Tiles to bounce up/down
    static constexpr f32 HOUSE_EXIT_X = 13.5f;       // X position to exit (between tiles 13-14)
    static constexpr f32 HOUSE_EXIT_Y = 11.0f;       // Y position above door
};

}  // namespace pacman
