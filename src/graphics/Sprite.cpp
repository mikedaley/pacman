#include "graphics/Sprite.hpp"

#include "core/Renderer.hpp"

#include <stdexcept>

namespace pacman {

Sprite::Sprite(const TextureAtlas& atlas) : m_atlas(atlas) {}

void Sprite::setRegion(std::string_view regionName) {
    m_currentRegion = std::string(regionName);
    m_frameNames.clear();
    m_currentFrame = 0;
}

void Sprite::setAnimationFrames(const std::vector<std::string>& frameNames) {
    if (frameNames.empty()) {
        return;
    }

    m_frameNames = frameNames;
    m_currentFrame = 0;
    m_currentRegion = m_frameNames[0];
    m_frameTime = 0.0f;
}

void Sprite::setAnimationSpeed(f32 framesPerSecond) {
    if (framesPerSecond > 0.0f) {
        m_frameDuration = 1.0f / framesPerSecond;
    } else {
        m_frameDuration = 0.0f;
    }
}

void Sprite::update(f32 deltaTime) {
    if (m_frameNames.empty() || m_frameDuration <= 0.0f) {
        return;
    }

    m_frameTime += deltaTime;

    while (m_frameTime >= m_frameDuration) {
        m_frameTime -= m_frameDuration;
        m_currentFrame = (m_currentFrame + 1) % static_cast<i32>(m_frameNames.size());
        m_currentRegion = m_frameNames[m_currentFrame];
    }
}

void Sprite::setFrame(i32 frameIndex) {
    if (m_frameNames.empty()) {
        return;
    }

    m_currentFrame = frameIndex % static_cast<i32>(m_frameNames.size());
    if (m_currentFrame < 0) {
        m_currentFrame += static_cast<i32>(m_frameNames.size());
    }
    m_currentRegion = m_frameNames[m_currentFrame];
    m_frameTime = 0.0f;
}

void Sprite::setPosition(f32 x, f32 y) {
    m_position = {x, y};
}

void Sprite::setPosition(Vec2f pos) {
    m_position = pos;
}

void Sprite::setSize(f32 w, f32 h) {
    m_size = {w, h};
    m_useCustomSize = true;
}

void Sprite::setSize(Vec2f size) {
    m_size = size;
    m_useCustomSize = true;
}

void Sprite::clearSize() {
    m_useCustomSize = false;
}

Vec2f Sprite::getSize() const {
    if (m_useCustomSize) {
        return m_size;
    }

    if (m_currentRegion.empty()) {
        return {0.0f, 0.0f};
    }

    const auto& region = m_atlas.getRegion(m_currentRegion);
    return {static_cast<f32>(region.w), static_cast<f32>(region.h)};
}

void Sprite::setRotation(f32 degrees) {
    m_rotation = degrees;
}

void Sprite::setFlip(SpriteFlip flip) {
    m_flip = flip;
}

void Sprite::setAlpha(u8 alpha) {
    m_alpha = alpha;
}

void Sprite::setVisible(bool visible) {
    m_visible = visible;
}

const SpriteRegion& Sprite::getCurrentRegion() const {
    if (m_currentRegion.empty()) {
        throw std::runtime_error("No sprite region set");
    }
    return m_atlas.getRegion(m_currentRegion);
}

void Sprite::render(Renderer& renderer) const {
    if (!m_visible || m_currentRegion.empty()) {
        return;
    }

    const auto& region = m_atlas.getRegion(m_currentRegion);

    // Source rectangle (region in atlas)
    SDL_FRect srcRect{static_cast<f32>(region.x), static_cast<f32>(region.y),
                      static_cast<f32>(region.w), static_cast<f32>(region.h)};

    // Destination rectangle
    Vec2f size = getSize();
    SDL_FRect dstRect{m_position.x, m_position.y, size.x, size.y};

    // Convert flip enum to SDL flags
    SDL_FlipMode flipMode = SDL_FLIP_NONE;
    switch (m_flip) {
        case SpriteFlip::Horizontal:
            flipMode = SDL_FLIP_HORIZONTAL;
            break;
        case SpriteFlip::Vertical:
            flipMode = SDL_FLIP_VERTICAL;
            break;
        case SpriteFlip::Both:
            flipMode = static_cast<SDL_FlipMode>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
            break;
        default:
            break;
    }

    // Set alpha modulation
    SDL_SetTextureAlphaMod(m_atlas.getTexture(), m_alpha);

    // Render with rotation and flip
    SDL_RenderTextureRotated(renderer.get(), m_atlas.getTexture(), &srcRect, &dstRect, m_rotation,
                             nullptr,  // Rotate around center
                             flipMode);

    // Reset alpha
    SDL_SetTextureAlphaMod(m_atlas.getTexture(), 255);
}

}  // namespace pacman
