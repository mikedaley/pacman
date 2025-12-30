#include "graphics/ScorePopup.hpp"

#include "core/Renderer.hpp"

#include <SDL3/SDL.h>

namespace pacman {

ScorePopup::ScorePopup(const TextureAtlas& atlas) : m_atlas(atlas) {
    // Default to 200 points
    m_regionName = "points_200";
}

void ScorePopup::setScore(ScoreValue score) {
    m_regionName = getRegionName(score);
}

void ScorePopup::setScore(i32 score) {
    m_regionName = getRegionName(score);
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

    // Source rectangle (region in atlas)
    SDL_FRect srcRect{static_cast<f32>(region.x), static_cast<f32>(region.y),
                      static_cast<f32>(region.w), static_cast<f32>(region.h)};

    // Destination rectangle
    SDL_FRect dstRect{m_position.x, m_position.y, static_cast<f32>(region.w),
                      static_cast<f32>(region.h)};

    SDL_RenderTexture(renderer.get(), m_atlas.getTexture(), &srcRect, &dstRect);
}

std::string ScorePopup::getRegionName(ScoreValue score) {
    switch (score) {
        case ScoreValue::Points100:
            return "points_100";
        case ScoreValue::Points200:
            return "points_200";
        case ScoreValue::Points300:
            return "points_300";
        case ScoreValue::Points400:
            return "points_400";
        case ScoreValue::Points500:
            return "points_500";
        case ScoreValue::Points700:
            return "points_700";
        case ScoreValue::Points800:
            return "points_800";
        case ScoreValue::Points1000:
            return "points_1000";
        case ScoreValue::Points1600:
            return "points_1600";
        case ScoreValue::Points2000:
            return "points_2000";
        case ScoreValue::Points3000:
            return "points_3000";
        case ScoreValue::Points5000:
            return "points_5000";
        default:
            return "points_200";
    }
}

std::string ScorePopup::getRegionName(i32 score) {
    switch (score) {
        case 100:
            return "points_100";
        case 200:
            return "points_200";
        case 300:
            return "points_300";
        case 400:
            return "points_400";
        case 500:
            return "points_500";
        case 700:
            return "points_700";
        case 800:
            return "points_800";
        case 1000:
            return "points_1000";
        case 1600:
            return "points_1600";
        case 2000:
            return "points_2000";
        case 3000:
            return "points_3000";
        case 5000:
            return "points_5000";
        default:
            return "points_200";
    }
}

}  // namespace pacman
