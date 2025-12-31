#include "entities/Player.hpp"

#include "core/Renderer.hpp"

#include <cmath>

namespace pacman {

Player::Player(TextureAtlas& atlas, const Maze& maze) : m_sprite(atlas), m_maze(maze) {
    m_sprite.setAnimationFrames(
        {"pacman_closed", "pacman_left_half", "pacman_left_open", "pacman_left_half"});
    m_sprite.setAnimationSpeed(constants::PACMAN_ANIM_SPEED);
}

void Player::update(f32 deltaTime) {
    if (m_playingDeathAnim) {
        return;  // Death animation is handled separately
    }

    // Try to turn in the desired direction (implements cornering)
    tryTurn();

    // If we still don't have a direction, try to start moving
    if (m_direction == Direction::None && m_desiredDirection != Direction::None) {
        if (canMoveInDirection(m_desiredDirection)) {
            m_direction = m_desiredDirection;
            updateAnimation();
        }
    }

    // Only move if we have a direction
    if (m_direction == Direction::None) {
        m_sprite.update(deltaTime);
        return;
    }

    // Movement
    constexpr f32 HALF_SPRITE = 8.0f;

    // Calculate new position based on current direction
    Vec2f velocity = Vec2f{static_cast<f32>(directionToVec(m_direction).x),
                           static_cast<f32>(directionToVec(m_direction).y)};
    Vec2f newPos = m_position + velocity * constants::PACMAN_SPEED * deltaTime;

    // Get center at the new position
    f32 centerX = newPos.x + HALF_SPRITE;
    f32 centerY = newPos.y + HALF_SPRITE;
    f32 mazeCenterY = centerY - constants::MAZE_OFFSET_Y;

    // Get the current tile based on center position (relative to maze)
    Vec2i currentTile = Maze::pixelToTile(centerX, mazeCenterY);
    Vec2f tileCenter = getTileCenter(currentTile.x, currentTile.y);

    // Check if we can continue moving in the current direction
    Vec2i dirVec = directionToVec(m_direction);
    Vec2i nextTile = Vec2i{currentTile.x + dirVec.x, currentTile.y + dirVec.y};

    // Check if moving would take us into a wall tile
    if (!m_maze.isWalkable(nextTile.x, nextTile.y)) {
        // There's a wall ahead - stop at the tile center
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

    m_sprite.setPosition(m_position);
    m_sprite.update(deltaTime);
}

void Player::render(Renderer& renderer) {
    m_sprite.render(renderer);
}

Vec2i Player::getTile() const {
    constexpr f32 HALF_SPRITE = 8.0f;
    return Maze::pixelToTile(m_position.x + HALF_SPRITE,
                             m_position.y + HALF_SPRITE - constants::MAZE_OFFSET_Y);
}

void Player::setDesiredDirection(Direction dir) {
    m_desiredDirection = dir;
}

void Player::reset() {
    m_position.x = 104.0f;
    m_position.y = 180.0f + constants::MAZE_OFFSET_Y;
    m_direction = Direction::None;
    m_desiredDirection = Direction::None;
    m_playingDeathAnim = false;
    m_deathAnimTimer = 0.0f;
    m_sprite.setPosition(m_position);
    updateAnimation(true);
}

void Player::updateAnimation(bool forceUpdate) {
    if (m_direction == m_lastAnimDirection && !forceUpdate) {
        return;
    }
    m_lastAnimDirection = m_direction;

    switch (m_direction) {
        case Direction::None:
            // Default to left-facing when idle (matches starting direction)
            m_sprite.setAnimationFrames(
                {"pacman_closed", "pacman_left_half", "pacman_left_open", "pacman_left_half"});
            m_sprite.setFrame(0);  // Start on closed frame
            break;
        case Direction::Up:
            m_sprite.setAnimationFrames(
                {"pacman_closed", "pacman_up_half", "pacman_up_open", "pacman_up_half"});
            break;
        case Direction::Down:
            m_sprite.setAnimationFrames(
                {"pacman_closed", "pacman_down_half", "pacman_down_open", "pacman_down_half"});
            break;
        case Direction::Left:
            m_sprite.setAnimationFrames(
                {"pacman_closed", "pacman_left_half", "pacman_left_open", "pacman_left_half"});
            break;
        case Direction::Right:
            m_sprite.setAnimationFrames(
                {"pacman_closed", "pacman_right_half", "pacman_right_open", "pacman_right_half"});
            break;
        default:
            break;
    }
    m_sprite.setAnimationSpeed(constants::PACMAN_ANIM_SPEED);
}

void Player::playDeathAnimation() {
    m_playingDeathAnim = true;
    m_deathAnimTimer = 0.0f;

    // Set up death animation frames
    std::vector<std::string> deathFrames;
    for (i32 i = 0; i < DEATH_ANIM_FRAMES; ++i) {
        deathFrames.push_back("pacman_death_" + std::to_string(i));
    }
    m_sprite.setAnimationFrames(deathFrames);
    m_sprite.setAnimationSpeed(10.0f);
    m_sprite.setFrame(0);
}

void Player::updateDeathAnimation(f32 deltaTime) {
    if (!m_playingDeathAnim) {
        return;
    }

    m_deathAnimTimer += deltaTime;

    // Only update if not on last frame
    if (m_sprite.getCurrentFrame() < DEATH_ANIM_FRAMES - 1) {
        m_sprite.update(deltaTime);
    }
}

bool Player::isDeathAnimationComplete() const {
    return m_playingDeathAnim && m_deathAnimTimer >= DEATH_ANIM_FRAME_TIME * DEATH_ANIM_FRAMES;
}

bool Player::canMoveInDirection(Direction dir) const {
    constexpr f32 HALF_SPRITE = 8.0f;
    f32 centerX = m_position.x + HALF_SPRITE;
    f32 centerY = m_position.y + HALF_SPRITE - constants::MAZE_OFFSET_Y;

    Vec2i currentTile = Maze::pixelToTile(centerX, centerY);
    Vec2i dirVec = directionToVec(dir);
    Vec2i targetTile = Vec2i{currentTile.x + dirVec.x, currentTile.y + dirVec.y};

    return m_maze.isWalkable(targetTile.x, targetTile.y);
}

Vec2f Player::getTileCenter(i32 tileX, i32 tileY) const {
    return Vec2f{static_cast<f32>(tileX * constants::TILE_SIZE + constants::TILE_SIZE / 2),
                 static_cast<f32>(tileY * constants::TILE_SIZE + constants::TILE_SIZE / 2 +
                                  constants::MAZE_OFFSET_Y)};
}

void Player::tryTurn() {
    if (m_desiredDirection == Direction::None) {
        return;
    }

    // Ignore if already moving in the desired direction
    if (m_desiredDirection == m_direction) {
        return;
    }

    // Reversing direction is always allowed immediately
    if (m_desiredDirection == oppositeDirection(m_direction)) {
        m_direction = m_desiredDirection;
        updateAnimation();
        return;
    }

    // For turns (not reversals), we need to be close enough to the tile center
    constexpr f32 HALF_SPRITE = 8.0f;
    constexpr f32 CORNER_TOLERANCE = 4.0f;

    f32 centerX = m_position.x + HALF_SPRITE;
    f32 centerY = m_position.y + HALF_SPRITE;
    f32 mazeCenterY = centerY - constants::MAZE_OFFSET_Y;

    Vec2i currentTile = Maze::pixelToTile(centerX, mazeCenterY);
    Vec2f tileCenter = getTileCenter(currentTile.x, currentTile.y);

    // Check if we can turn in the desired direction
    if (!canMoveInDirection(m_desiredDirection)) {
        return;
    }

    bool isHorizontalTurn =
        (m_desiredDirection == Direction::Left || m_desiredDirection == Direction::Right);
    bool isVerticalTurn =
        (m_desiredDirection == Direction::Up || m_desiredDirection == Direction::Down);

    bool canTurn = false;

    if (isVerticalTurn) {
        f32 xDiff = std::abs(centerX - tileCenter.x);
        if (xDiff <= CORNER_TOLERANCE) {
            canTurn = true;
            m_position.x = tileCenter.x - HALF_SPRITE;
        }
    } else if (isHorizontalTurn) {
        f32 yDiff = std::abs(centerY - tileCenter.y);
        if (yDiff <= CORNER_TOLERANCE) {
            canTurn = true;
            m_position.y = tileCenter.y - HALF_SPRITE;
        }
    }

    if (canTurn) {
        m_direction = m_desiredDirection;
        updateAnimation();
    }
}

}  // namespace pacman
