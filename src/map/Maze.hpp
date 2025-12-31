#pragma once

#include "graphics/Sprite.hpp"
#include "graphics/TextureAtlas.hpp"
#include "utils/Types.hpp"

#include <array>
#include <vector>

namespace pacman {

// Tile types in the maze
enum class TileType : u8 {
    Empty = 0,       // Empty space (black)
    Wall,            // Blue wall
    Pellet,          // Small dot
    PowerPellet,     // Large flashing pellet
    GhostHouseDoor,  // Pink door to ghost house
};

class Maze {
public:
    // Classic Pacman maze is 28x31 tiles (we use 28x36 for status area)
    static constexpr i32 WIDTH = 28;
    static constexpr i32 HEIGHT = 31;
    static constexpr i32 TILE_SIZE = 8;

    explicit Maze(TextureAtlas& atlas);
    ~Maze() = default;

    // Non-copyable, non-movable
    Maze(const Maze&) = delete;
    Maze& operator=(const Maze&) = delete;
    Maze(Maze&&) = delete;
    Maze& operator=(Maze&&) = delete;

    // Load the classic Pacman maze layout
    void loadClassicMaze();

    // Get tile at position
    [[nodiscard]] TileType getTile(i32 x, i32 y) const;

    // Set tile at position
    void setTile(i32 x, i32 y, TileType type);

    // Check if a position is walkable (not a wall)
    [[nodiscard]] bool isWalkable(i32 x, i32 y) const;

    // Check if a position is the ghost house door
    [[nodiscard]] bool isGhostHouseDoor(i32 x, i32 y) const;

    // Check if position is in the tunnel (wrap-around area)
    [[nodiscard]] bool isTunnel(i32 x, i32 y) const;

    // Check if position is in a no-upward-turn zone for ghosts
    // These are special zones near tunnel entrances where ghosts cannot turn up
    [[nodiscard]] bool isNoUpwardTurnZone(i32 x, i32 y) const;

    // Eat pellet at position, returns points earned (0 if no pellet)
    i32 eatPellet(i32 x, i32 y);

    // Update animations (power pellet flashing)
    void update(f32 deltaTime);

    // Render the maze
    void render(Renderer& renderer, i32 offsetY = 0);

    // Convert pixel position to tile position
    [[nodiscard]] static Vec2i pixelToTile(f32 x, f32 y);

private:
    void defineMazeTileRegions();

    TextureAtlas& m_atlas;
    std::array<TileType, WIDTH * HEIGHT> m_tiles{};
    i32 m_pelletCount{0};

    // Power pellet animation
    f32 m_powerPelletTimer{0.0f};
    bool m_powerPelletVisible{true};
};

}  // namespace pacman
