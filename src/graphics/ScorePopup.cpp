#include "graphics/ScorePopup.hpp"

#include "core/Renderer.hpp"

#include <SDL3/SDL.h>

namespace pacman {

ScorePopup::ScorePopup(const TextureAtlas& atlas) : m_atlas(atlas) {
    m_regionName = "points_200";
}

void ScorePopup::setScore(i32 score) {
    switch (score) {
        case 200:
            m_regionName = "points_200";
            break;
        case 400:
            m_regionName = "points_400";
            break;
        case 800:
            m_regionName = "points_800";
            break;
        case 1600:
            m_regionName = "points_1600";
            break;
        default:
            m_regionName = "points_200";
            break;
    }
}

void ScorePopup::setPosition(f32 x, f32 y) {
    m_position = {x, y};
}

void ScorePopup::setPosition(Vec2f pos) {
    m_position = pos;
}

void ScorePopup::render(Renderer& renderer) const {
    if (!m_visible || m_regionName.empty()) {
        return;
    }

    const auto& region = m_atlas.getRegion(m_regionName);

    SDL_FRect srcRect{static_cast<f32>(region.x), static_cast<f32>(region.y),
                      static_cast<f32>(region.w), static_cast<f32>(region.h)};

    SDL_FRect dstRect{m_position.x, m_position.y, static_cast<f32>(region.w),
                      static_cast<f32>(region.h)};

    SDL_RenderTexture(renderer.get(), m_atlas.getTexture(), &srcRect, &dstRect);
}

}  // namespace pacman
