#pragma once

#include "graphics/TextureAtlas.hpp"
#include "utils/Types.hpp"

#include <SDL3/SDL.h>

#include <string>
#include <string_view>
#include <vector>

namespace pacman {

class Renderer;

// Flip options for sprite rendering
enum class SpriteFlip { None, Horizontal, Vertical, Both };

// A sprite that renders a region from a texture atlas
// Supports position, scale, rotation, flipping, and frame-based animation
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
    [[nodiscard]] i32 getFrameCount() const { return static_cast<i32>(m_frameNames.size()); }

    // Position (top-left corner in pixels)
    void setPosition(f32 x, f32 y);
    void setPosition(Vec2f pos);
    [[nodiscard]] Vec2f getPosition() const { return m_position; }

    // Size override (if not set, uses region size)
    void setSize(f32 w, f32 h);
    void setSize(Vec2f size);
    void clearSize();  // Revert to using region size
    [[nodiscard]] Vec2f getSize() const;

    // Rotation (in degrees, around center)
    void setRotation(f32 degrees);
    [[nodiscard]] f32 getRotation() const { return m_rotation; }

    // Flip
    void setFlip(SpriteFlip flip);
    [[nodiscard]] SpriteFlip getFlip() const { return m_flip; }

    // Alpha (0-255)
    void setAlpha(u8 alpha);
    [[nodiscard]] u8 getAlpha() const { return m_alpha; }

    // Visibility
    void setVisible(bool visible);
    [[nodiscard]] bool isVisible() const { return m_visible; }

    // Render the sprite
    void render(Renderer& renderer) const;

    // Get the current region
    [[nodiscard]] const SpriteRegion& getCurrentRegion() const;

private:
    const TextureAtlas& m_atlas;
    std::string m_currentRegion;
    std::vector<std::string> m_frameNames;

    Vec2f m_position{0.0f, 0.0f};
    Vec2f m_size{0.0f, 0.0f};
    bool m_useCustomSize{false};

    f32 m_rotation{0.0f};
    SpriteFlip m_flip{SpriteFlip::None};
    u8 m_alpha{255};
    bool m_visible{true};

    // Animation state
    i32 m_currentFrame{0};
    f32 m_frameTime{0.0f};
    f32 m_frameDuration{0.0f};  // Seconds per frame
};

}  // namespace pacman
