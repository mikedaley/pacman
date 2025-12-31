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

const SpriteRegion& Sprite::getCurrentRegion() const {
    if (m_currentRegion.empty()) {
        throw std::runtime_error("No sprite region set");
    }
    return m_atlas.getRegion(m_currentRegion);
}

void Sprite::render(Renderer& renderer) const {
    if (m_currentRegion.empty()) {
        return;
    }

    const auto& region = m_atlas.getRegion(m_currentRegion);

    // Source rectangle (region in atlas)
    SDL_FRect srcRect{static_cast<f32>(region.x), static_cast<f32>(region.y),
                      static_cast<f32>(region.w), static_cast<f32>(region.h)};

    // Destination rectangle (use region size)
    SDL_FRect dstRect{m_position.x, m_position.y, static_cast<f32>(region.w),
                      static_cast<f32>(region.h)};

    SDL_RenderTexture(renderer.get(), m_atlas.getTexture(), &srcRect, &dstRect);
}

}  // namespace pacman
