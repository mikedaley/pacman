#include "entities/Ghost.hpp"

#include "core/Renderer.hpp"

#include <cmath>
#include <limits>

namespace pacman {

Ghost::Ghost(TextureAtlas& atlas, const Maze& maze, std::string_view name)
    : m_sprite(atlas), m_maze(maze), m_name(name) {
    m_sprite.setAnimationSpeed(10.0f);
}

void Ghost::update(f32 deltaTime, Vec2i pacmanTile) {
    // Update state timer
    m_stateTimer += deltaTime;

    // Handle InHouse state - ghost bounces up and down waiting to exit
    if (m_state == GhostState::InHouse) {

        constexpr f32 BOUNCE_SPEED = 30.0f;  // pixels per second
        f32 houseCenterY = HOUSE_CENTER_Y * constants::TILE_SIZE + constants::TILE_SIZE / 2 +
                           constants::MAZE_OFFSET_Y;
        f32 bounceOffset = HOUSE_BOUNCE_RANGE * constants::TILE_SIZE;

        if (m_bouncingUp) {
            m_position.y -= BOUNCE_SPEED * deltaTime;
            if (m_position.y + 8.0f < houseCenterY - bounceOffset) {
                m_bouncingUp = false;
                m_direction = Direction::Down;
                updateAnimation();
            }
        } else {
            m_position.y += BOUNCE_SPEED * deltaTime;
            if (m_position.y + 8.0f > houseCenterY + bounceOffset) {
                m_bouncingUp = true;
                m_direction = Direction::Up;
                updateAnimation();
            }
        }

        m_sprite.setPosition(m_position);
        m_sprite.update(deltaTime);
        return;
    }

    // Handle LeavingHouse state - ghost moves to center then up through door
    if (m_state == GhostState::LeavingHouse) {
        constexpr f32 EXIT_SPEED = 60.0f;  // pixels per second
        f32 targetX = HOUSE_EXIT_X * constants::TILE_SIZE;
        f32 targetY = HOUSE_EXIT_Y * constants::TILE_SIZE + constants::MAZE_OFFSET_Y;
        f32 centerX = m_position.x + 8.0f;
        f32 centerY = m_position.y + 8.0f;

        // First move to center X
        if (std::abs(centerX - targetX) > 1.0f) {
            if (centerX < targetX) {
                m_position.x += EXIT_SPEED * deltaTime;
                m_direction = Direction::Right;
            } else {
                m_position.x -= EXIT_SPEED * deltaTime;
                m_direction = Direction::Left;
            }
        }
        // Then move up to exit
        else if (centerY > targetY) {
            m_position.x = targetX - 8.0f;  // Snap to center
            m_position.y -= EXIT_SPEED * deltaTime;
            m_direction = Direction::Up;
        }
        // Reached exit position
        else {
            m_position.y = targetY - 8.0f;  // Snap to exit position
            m_state = GhostState::Scatter;
            m_stateTimer = 0.0f;
            m_direction = Direction::Left;  // Exit moving left
            // Set to invalid tile so ghost makes a decision immediately
            m_lastDecisionTile = Vec2i{-1, -1};
        }

        updateAnimation();
        m_sprite.setPosition(m_position);
        m_sprite.update(deltaTime);
        return;
    }

    // Handle frightened timer
    if (m_state == GhostState::Frightened) {
        f32 previousTimer = m_frightenedTimer;
        m_frightenedTimer -= deltaTime;
        if (m_frightenedTimer <= 0.0f) {
            m_state = GhostState::Chase;
            updateAnimation();
        } else if (previousTimer > constants::FRIGHTENED_FLASH_TIME &&
                   m_frightenedTimer <= constants::FRIGHTENED_FLASH_TIME) {
            // Start flashing when crossing the threshold
            updateAnimation();
        }
    } else if (m_state == GhostState::Scatter || m_state == GhostState::Chase) {
        // Scatter/Chase mode switching (Level 1 timing)
        // Scatter 7s, Chase 20s, Scatter 7s, Chase 20s, Scatter 5s, Chase 20s, Scatter 5s, Chase
        // forever
        constexpr f32 MODE_TIMES[] = {7.0f, 20.0f, 7.0f, 20.0f, 5.0f, 20.0f, 5.0f};
        constexpr i32 NUM_MODES = 7;

        f32 elapsed = 0.0f;
        bool isScatter = true;
        for (i32 i = 0; i < NUM_MODES; ++i) {
            elapsed += MODE_TIMES[i];
            if (m_stateTimer < elapsed) {
                isScatter = (i % 2 == 0);  // Even indices are scatter
                break;
            }
            isScatter = false;  // After all modes, stay in chase
        }

        GhostState targetState = isScatter ? GhostState::Scatter : GhostState::Chase;
        if (m_state != targetState) {
            m_state = targetState;
            // Ghosts reverse direction when mode changes
            m_direction = oppositeDirection(m_direction);
            updateAnimation();
        }
    }

    // Get target tile based on current state
    Vec2i targetTile;
    switch (m_state) {
        case GhostState::InHouse:
        case GhostState::LeavingHouse:
            // These states are handled above, should not reach here
            return;
        case GhostState::Scatter:
            targetTile = getScatterTarget();
            break;
        case GhostState::Chase:
            targetTile = getChaseTarget(pacmanTile);
            break;
        case GhostState::Frightened:
            // In frightened mode, make random turns at intersections
            targetTile = pacmanTile;  // Will be overridden by random choice
            break;
        case GhostState::Eaten:
            // Ghost just eaten, game is paused - don't move
            return;
        case GhostState::Eyes:
            // Return to ghost house
            targetTile = Vec2i{13, 14};  // Ghost house entrance
            break;
    }

    // Movement constants
    constexpr f32 HALF_SPRITE = 8.0f;
    constexpr f32 CENTER_TOLERANCE = 1.0f;  // How close to center to make decisions
    f32 speed = getSpeed();

    // Get current position center
    f32 centerX = m_position.x + HALF_SPRITE;
    f32 centerY = m_position.y + HALF_SPRITE - constants::MAZE_OFFSET_Y;
    Vec2i currentTile = Maze::pixelToTile(centerX, centerY);
    Vec2f tileCenter = getTileCenter(currentTile.x, currentTile.y);

    // Calculate movement
    Vec2i dirVec = directionToVec(m_direction);
    Vec2i nextTile{currentTile.x + dirVec.x, currentTile.y + dirVec.y};
    // Check if ghost can continue in current direction
    // Eyes can enter through door, all ghosts can exit (move up) through door
    bool canContinue = m_maze.isWalkable(nextTile.x, nextTile.y) ||
                       (m_maze.isGhostHouseDoor(nextTile.x, nextTile.y) &&
                        (m_state == GhostState::Eyes || m_direction == Direction::Up));

    // Check if we're at/near the center of a tile
    f32 distToCenterX = std::abs(centerX - tileCenter.x);
    f32 distToCenterY = std::abs((m_position.y + HALF_SPRITE) - tileCenter.y);
    bool atTileCenter = (distToCenterX <= CENTER_TOLERANCE && distToCenterY <= CENTER_TOLERANCE);

    // Check if this is a new tile we haven't made a decision at
    bool atNewTile =
        (currentTile.x != m_lastDecisionTile.x || currentTile.y != m_lastDecisionTile.y);

    // Make direction decision when entering a new tile at its center, or when blocked
    // When blocked (!canContinue), we MUST make a decision regardless of position
    if ((atNewTile && atTileCenter) || !canContinue) {
        // Snap to tile center for clean movement
        m_position.x = tileCenter.x - HALF_SPRITE;
        m_position.y = tileCenter.y - HALF_SPRITE;

        // Choose direction for this intersection
        Direction newDir = chooseDirection(targetTile);
        if (newDir != Direction::None) {
            m_direction = newDir;
            updateAnimation();
            // Recalculate after direction change
            dirVec = directionToVec(m_direction);
            nextTile = Vec2i{currentTile.x + dirVec.x, currentTile.y + dirVec.y};
            canContinue = m_maze.isWalkable(nextTile.x, nextTile.y) ||
                          (m_maze.isGhostHouseDoor(nextTile.x, nextTile.y) &&
                           (m_state == GhostState::Eyes || m_direction == Direction::Up));
        }
        m_lastDecisionTile = currentTile;
    }

    // Move in current direction
    Vec2f velocity{static_cast<f32>(dirVec.x), static_cast<f32>(dirVec.y)};
    Vec2f newPos = m_position + velocity * speed * deltaTime;

    // Recalculate tile center after potential snap
    tileCenter = getTileCenter(currentTile.x, currentTile.y);

    // Keep ghost aligned to corridor center
    // When moving horizontally, lock Y to tile center
    // When moving vertically, lock X to tile center
    if (m_direction == Direction::Left || m_direction == Direction::Right) {
        newPos.y = tileCenter.y - HALF_SPRITE;
    } else if (m_direction == Direction::Up || m_direction == Direction::Down) {
        newPos.x = tileCenter.x - HALF_SPRITE;
    }

    // Check for wall collision - stop at tile center if next tile is a wall
    if (!canContinue) {
        // Clamp to tile center
        if (m_direction == Direction::Left) {
            if (newPos.x + HALF_SPRITE < tileCenter.x) {
                newPos.x = tileCenter.x - HALF_SPRITE;
            }
        } else if (m_direction == Direction::Right) {
            if (newPos.x + HALF_SPRITE > tileCenter.x) {
                newPos.x = tileCenter.x - HALF_SPRITE;
            }
        } else if (m_direction == Direction::Up) {
            if (newPos.y + HALF_SPRITE < tileCenter.y) {
                newPos.y = tileCenter.y - HALF_SPRITE;
            }
        } else if (m_direction == Direction::Down) {
            if (newPos.y + HALF_SPRITE > tileCenter.y) {
                newPos.y = tileCenter.y - HALF_SPRITE;
            }
        }
    }

    m_position = newPos;

    // Handle tunnel wrap-around
    if (m_position.x < -16.0f) {
        m_position.x = static_cast<f32>(constants::GAME_WIDTH);
    } else if (m_position.x > static_cast<f32>(constants::GAME_WIDTH)) {
        m_position.x = -16.0f;
    }

    // Check if eyes have reached the ghost house (after movement)
    if (m_state == GhostState::Eyes) {
        Vec2i eyesTile = getTile();
        // Ghost house door is at row 12, tiles 13-14
        // Eyes need to enter through the door and reach inside
        if (eyesTile.y >= 13 && eyesTile.y <= 15 && eyesTile.x >= 12 && eyesTile.x <= 15) {
            // Respawn as normal ghost - transition to leaving house
            m_state = GhostState::LeavingHouse;
            m_stateTimer = 0.0f;
            updateAnimation();
        }
    }

    // Update sprite position and animation
    m_sprite.setPosition(m_position);
    m_sprite.update(deltaTime);
}

void Ghost::render(Renderer& renderer) {
    m_sprite.render(renderer);
}

Vec2i Ghost::getTile() const {
    constexpr f32 HALF_SPRITE = 8.0f;
    return Maze::pixelToTile(m_position.x + HALF_SPRITE,
                             m_position.y + HALF_SPRITE - constants::MAZE_OFFSET_Y);
}

void Ghost::setFrightened() {
    // Don't frighten ghosts that are:
    // - Already eyes (returning to house)
    // - Just eaten (showing score)
    // - Inside the ghost house (InHouse or LeavingHouse)
    if (m_state != GhostState::Eyes && m_state != GhostState::Eaten &&
        m_state != GhostState::InHouse && m_state != GhostState::LeavingHouse) {
        m_state = GhostState::Frightened;
        m_frightenedTimer = constants::FRIGHTENED_DURATION;
        // Reverse direction when frightened
        m_direction = oppositeDirection(m_direction);
        updateAnimation();
    }
}

void Ghost::setEaten() {
    // Transition to Eaten state - ghost is invisible, score sprite shown
    m_state = GhostState::Eaten;
    m_stateTimer = 0.0f;
}

void Ghost::transitionToEyes() {
    // Called after the eaten pause ends, ghost becomes eyes returning home
    m_state = GhostState::Eyes;
    m_stateTimer = 0.0f;
    updateAnimation();
}

void Ghost::releaseFromHouse() {
    if (m_state == GhostState::InHouse) {
        m_state = GhostState::LeavingHouse;
        m_stateTimer = 0.0f;
    }
}

void Ghost::reset() {
    m_position = getStartPosition();
    m_stateTimer = 0.0f;
    m_frightenedTimer = 0.0f;
    m_bouncingUp = true;

    // Check if this ghost starts inside the house
    if (startsInHouse()) {
        m_state = GhostState::InHouse;
        m_direction = Direction::Up;  // Start bouncing up
    } else {
        m_state = GhostState::Scatter;  // Start in scatter mode
        m_direction = Direction::Left;  // Initial direction
    }

    // Snap to tile center and record as decision tile
    constexpr f32 HALF_SPRITE = 8.0f;
    f32 centerX = m_position.x + HALF_SPRITE;
    f32 centerY = m_position.y + HALF_SPRITE - constants::MAZE_OFFSET_Y;
    Vec2i currentTile = Maze::pixelToTile(centerX, centerY);
    Vec2f tileCenter = getTileCenter(currentTile.x, currentTile.y);

    // Snap position to exact tile center
    m_position.x = tileCenter.x - HALF_SPRITE;
    m_position.y = tileCenter.y - HALF_SPRITE;

    // Mark this tile as already decided so we move immediately
    m_lastDecisionTile = currentTile;

    m_sprite.setPosition(m_position);
    updateAnimation();
}

void Ghost::updateAnimation() {
    std::string spriteName = getSpriteName();
    std::string dirStr;

    switch (m_direction) {
        case Direction::Up:
            dirStr = "up";
            break;
        case Direction::Down:
            dirStr = "down";
            break;
        case Direction::Left:
            dirStr = "left";
            break;
        case Direction::Right:
            dirStr = "right";
            break;
        default:
            dirStr = "left";
            break;
    }

    if (m_state == GhostState::Frightened) {
        // Use frightened animation
        if (m_frightenedTimer <= constants::FRIGHTENED_FLASH_TIME) {
            // Flash between blue and white - only set if not already flashing
            if (!m_isFlashing) {
                m_sprite.setAnimationFrames(
                    {"ghost_frightened_0", "ghost_frightened_1", "ghost_flash_0", "ghost_flash_1"});
                m_sprite.setAnimationSpeed(constants::GHOST_FLASH_SPEED);
                m_isFlashing = true;
            }
        } else {
            m_sprite.setAnimationFrames({"ghost_frightened_0", "ghost_frightened_1"});
            m_sprite.setAnimationSpeed(10.0f);  // Normal animation speed
            m_isFlashing = false;
        }
        return;  // Don't update animation based on direction in frightened mode
    } else if (m_state == GhostState::Eyes) {
        m_isFlashing = false;
        // Use eyes animation (single frame per direction)
        m_sprite.setRegion("ghost_eyes_" + dirStr);
    } else {
        // Normal animation
        m_isFlashing = false;
        std::string frame0 = spriteName + "_" + dirStr + "_0";
        std::string frame1 = spriteName + "_" + dirStr + "_1";
        m_sprite.setAnimationFrames({frame0, frame1});
    }

    m_lastAnimDirection = m_direction;
}

bool Ghost::canMoveInDirection(Direction dir) const {
    Vec2i currentTile = getTile();
    Vec2i dirVec = directionToVec(dir);
    Vec2i targetTile{currentTile.x + dirVec.x, currentTile.y + dirVec.y};

    // Check if target is ghost house door
    if (m_maze.isGhostHouseDoor(targetTile.x, targetTile.y)) {
        // Eyes can enter the house (moving down through door)
        // All ghosts can exit the house (moving up through door)
        return m_state == GhostState::Eyes || dir == Direction::Up;
    }

    // Check no-upward-turn zones (only applies in Scatter/Chase modes, not Frightened)
    // In the original Pac-Man, ghosts cannot turn upward in certain zones near tunnels
    if (dir == Direction::Up && m_state != GhostState::Frightened && m_state != GhostState::Eyes) {
        if (m_maze.isNoUpwardTurnZone(currentTile.x, currentTile.y)) {
            return false;
        }
    }

    return m_maze.isWalkable(targetTile.x, targetTile.y);
}

Direction Ghost::chooseDirection(Vec2i targetTile) {
    Vec2i currentTile = getTile();
    Direction reverseDir = oppositeDirection(m_direction);

    // Possible directions in priority order: Up, Left, Down, Right
    Direction priorities[] = {Direction::Up, Direction::Left, Direction::Down, Direction::Right};

    Direction bestDir = Direction::None;
    f32 bestDistance = std::numeric_limits<f32>::max();

    for (Direction dir : priorities) {
        // Ghosts cannot reverse direction (except when frightened, handled elsewhere)
        if (dir == reverseDir) {
            continue;
        }

        if (canMoveInDirection(dir)) {
            Vec2i dirVec = directionToVec(dir);
            Vec2i nextTile{currentTile.x + dirVec.x, currentTile.y + dirVec.y};

            // Calculate distance to target
            f32 dx = static_cast<f32>(nextTile.x - targetTile.x);
            f32 dy = static_cast<f32>(nextTile.y - targetTile.y);
            f32 distance = dx * dx + dy * dy;  // Squared distance is fine for comparison

            if (distance < bestDistance) {
                bestDistance = distance;
                bestDir = dir;
            }
        }
    }

    // If no valid direction found (shouldn't happen), allow reverse
    if (bestDir == Direction::None && canMoveInDirection(reverseDir)) {
        bestDir = reverseDir;
    }

    return bestDir;
}

f32 Ghost::getSpeed() const {
    switch (m_state) {
        case GhostState::Frightened:
            return constants::GHOST_FRIGHTENED_SPEED;
        case GhostState::Eyes:
            return constants::GHOST_SPEED * 2.0f;  // Eyes move faster
        default:
            // Check if in tunnel
            Vec2i tile = getTile();
            if (m_maze.isTunnel(tile.x, tile.y)) {
                return constants::GHOST_TUNNEL_SPEED;
            }
            return constants::GHOST_SPEED;
    }
}

Vec2f Ghost::getTileCenter(i32 tileX, i32 tileY) const {
    return Vec2f{static_cast<f32>(tileX * constants::TILE_SIZE + constants::TILE_SIZE / 2),
                 static_cast<f32>(tileY * constants::TILE_SIZE + constants::TILE_SIZE / 2 +
                                  constants::MAZE_OFFSET_Y)};
}

}  // namespace pacman
