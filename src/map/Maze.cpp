#include "map/Maze.hpp"

#include "core/Renderer.hpp"
#include "utils/Constants.hpp"

namespace pacman {

// Classic Pacman maze layout
// # = wall, . = pellet, o = power pellet, - = ghost house door, ' ' = empty
// clang-format off
static constexpr const char* CLASSIC_MAZE[] = {
    "############################",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#o####.#####.##.#####.####o#",
    "#.####.#####.##.#####.####.#",
    "#..........................#",
    "#.####.##.########.##.####.#",
    "#.####.##.########.##.####.#",
    "#......##....##....##......#",
    "######.##### ## #####.######",
    "     #.##### ## #####.#     ",
    "     #.##          ##.#     ",
    "     #.## ###--### ##.#     ",
    "######.## #      # ##.######",
    "      .   #      #   .      ",
    "######.## #      # ##.######",
    "     #.## ######## ##.#     ",
    "     #.##          ##.#     ",
    "     #.## ######## ##.#     ",
    "######.## ######## ##.######",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#.####.#####.##.#####.####.#",
    "#o..##.......  .......##..o#",
    "###.##.##.########.##.##.###",
    "###.##.##.########.##.##.###",
    "#......##....##....##......#",
    "#.##########.##.##########.#",
    "#.##########.##.##########.#",
    "#..........................#",
    "############################",
};
// clang-format on

Maze::Maze(TextureAtlas& atlas) : m_atlas(atlas) {
    defineMazeTileRegions();
    m_tiles.fill(TileType::Empty);
}

void Maze::defineMazeTileRegions() {
    // The maze in the sprite sheet is pre-rendered, but we also need individual elements
    // Pellet and power pellet for rendering on top of the maze
    // These should already be defined in SpriteSheet.hpp, but let's ensure they exist

    // Small pellet (2x2 pixels, centered in 8x8 tile)
    if (!m_atlas.hasRegion("pellet")) {
        m_atlas.defineRegion("pellet", 8, 8, 8, 8);
    }

    // Power pellet (larger, 8x8)
    if (!m_atlas.hasRegion("power_pellet")) {
        m_atlas.defineRegion("power_pellet", 8, 24, 8, 8);
    }

    // The full maze image from the sprite sheet (left portion)
    // Maze with pellets starts at (0, 0) and is 224x248 pixels
    if (!m_atlas.hasRegion("maze_full")) {
        m_atlas.defineRegion("maze_full", 0, 0, 224, 248);
    }

    // Empty maze (without pellets) starts at (224, 0)
    if (!m_atlas.hasRegion("maze_empty")) {
        m_atlas.defineRegion("maze_empty", 228, 0, 224, 248);
    }
}

void Maze::loadClassicMaze() {
    m_pelletCount = 0;

    for (i32 y = 0; y < HEIGHT; ++y) {
        const char* row = CLASSIC_MAZE[y];
        for (i32 x = 0; x < WIDTH; ++x) {
            char c = row[x];
            TileType type = TileType::Empty;

            switch (c) {
                case '#':
                    type = TileType::Wall;
                    break;
                case '.':
                    type = TileType::Pellet;
                    ++m_pelletCount;
                    break;
                case 'o':
                    type = TileType::PowerPellet;
                    ++m_pelletCount;
                    break;
                case '-':
                    type = TileType::GhostHouseDoor;
                    break;
                default:
                    type = TileType::Empty;
                    break;
            }

            m_tiles[y * WIDTH + x] = type;
        }
    }
}

TileType Maze::getTile(i32 x, i32 y) const {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return TileType::Wall;  // Out of bounds is treated as wall
    }
    return m_tiles[y * WIDTH + x];
}

void Maze::setTile(i32 x, i32 y, TileType type) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        m_tiles[y * WIDTH + x] = type;
    }
}

bool Maze::isWalkable(i32 x, i32 y) const {
    // Special case: tunnel wrap-around at row 14
    // Allow movement to/from out-of-bounds tiles on the tunnel row
    if (y == 14 && (x < 0 || x >= WIDTH)) {
        return true;
    }

    TileType tile = getTile(x, y);
    return tile != TileType::Wall && tile != TileType::GhostHouseDoor;
}

bool Maze::isGhostHouseDoor(i32 x, i32 y) const {
    return getTile(x, y) == TileType::GhostHouseDoor;
}

bool Maze::isTunnel(i32 x, i32 y) const {
    // Tunnel is at row 14 (0-indexed), columns 0-5 and 22-27
    return y == 14 && (x < 6 || x >= 22);
}

bool Maze::isNoUpwardTurnZone(i32 x, i32 y) const {
    // In the original Pac-Man, there are two "red zones" where ghosts cannot turn upward.
    // These zones are located near the tunnel entrances to give Pac-Man an advantage.
    // The zones are at specific intersections above and below the ghost house.
    //
    // Zone 1: Above ghost house, left side - tiles (12, 11) and (12, 23)
    // Zone 2: Above ghost house, right side - tiles (15, 11) and (15, 23)
    //
    // Note: These restrictions only apply during Scatter and Chase modes,
    // not during Frightened mode (handled in Ghost::canMoveInDirection)

    // Left zone tiles
    if (x == 12 && (y == 11 || y == 23)) {
        return true;
    }
    // Right zone tiles
    if (x == 15 && (y == 11 || y == 23)) {
        return true;
    }

    return false;
}

i32 Maze::eatPellet(i32 x, i32 y) {
    TileType tile = getTile(x, y);

    if (tile == TileType::Pellet) {
        setTile(x, y, TileType::Empty);
        --m_pelletCount;
        return constants::SCORE_PELLET;
    } else if (tile == TileType::PowerPellet) {
        setTile(x, y, TileType::Empty);
        --m_pelletCount;
        return constants::SCORE_POWER_PELLET;
    }

    return 0;
}

void Maze::update(f32 deltaTime) {
    // Flash power pellets (toggle visibility every 0.2 seconds)
    m_powerPelletTimer += deltaTime;
    if (m_powerPelletTimer >= 0.2f) {
        m_powerPelletTimer -= 0.2f;
        m_powerPelletVisible = !m_powerPelletVisible;
    }
}

void Maze::render(Renderer& renderer, i32 offsetY) {
    // Render the empty maze background (no pellets)
    const auto& mazeRegion = m_atlas.getRegion("maze_empty");
    renderer.drawTextureRegion(m_atlas.getTexture(), mazeRegion, 0, offsetY);

    // Draw pellets and power pellets on top of the empty maze
    for (i32 y = 0; y < HEIGHT; ++y) {
        for (i32 x = 0; x < WIDTH; ++x) {
            TileType tile = m_tiles[y * WIDTH + x];
            i32 px = x * TILE_SIZE;
            i32 py = y * TILE_SIZE + offsetY;

            if (tile == TileType::Pellet) {
                const auto& region = m_atlas.getRegion("pellet");
                renderer.drawTextureRegion(m_atlas.getTexture(), region, px, py);
            } else if (tile == TileType::PowerPellet && m_powerPelletVisible) {
                const auto& region = m_atlas.getRegion("power_pellet");
                renderer.drawTextureRegion(m_atlas.getTexture(), region, px, py);
            }
        }
    }
}

Vec2i Maze::pixelToTile(f32 x, f32 y) {
    return {static_cast<i32>(x) / TILE_SIZE, static_cast<i32>(y) / TILE_SIZE};
}

}  // namespace pacman
