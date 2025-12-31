#pragma once

#include "graphics/TextureAtlas.hpp"
#include "utils/Types.hpp"

#include <SDL3/SDL.h>

#include <string>
#include <string_view>
#include <vector>

namespace pacman {

class Renderer;

// A sprite that renders a region from a texture atlas
// Supports position and frame-based animation
class Sprite {
public:
    explicit Sprite(const TextureAtlas& atlas);
    ~Sprite() = default;

    // Non-copyable, non-movable (references atlas)
    Sprite(const Sprite&) = delete;
    Sprite& operator=(const Sprite&) = delete;
    Sprite(Sprite&&) = delete;
    Sprite& operator=(Sprite&&) = delete;

    // Set the current region to render
    void setRegion(std::string_view regionName);

    // Animation: set multiple frames and cycle through them
    void setAnimationFrames(const std::vector<std::string>& frameNames);
    void setAnimationSpeed(f32 framesPerSecond);
    void update(f32 deltaTime);
    void setFrame(i32 frameIndex);
    [[nodiscard]] i32 getCurrentFrame() const { return m_currentFrame; }

    // Position (top-left corner in pixels)
    void setPosition(f32 x, f32 y);
    void setPosition(Vec2f pos);

    // Render the sprite
    void render(Renderer& renderer) const;

    // Get the current region
    [[nodiscard]] const SpriteRegion& getCurrentRegion() const;

private:
    const TextureAtlas& m_atlas;
    std::string m_currentRegion;
    std::vector<std::string> m_frameNames;

    Vec2f m_position{0.0f, 0.0f};

    // Animation state
    i32 m_currentFrame{0};
    f32 m_frameTime{0.0f};
    f32 m_frameDuration{0.0f};  // Seconds per frame
};

}  // namespace pacman
