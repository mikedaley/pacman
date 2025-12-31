#pragma once

#include "graphics/TextureAtlas.hpp"
#include "utils/Types.hpp"

#include <string>

namespace pacman {

class Renderer;

// A popup sprite that displays score values (used when eating ghosts)
class ScorePopup {
public:
    explicit ScorePopup(const TextureAtlas& atlas);
    ~ScorePopup() = default;

    // Non-copyable, non-movable (references atlas)
    ScorePopup(const ScorePopup&) = delete;
    ScorePopup& operator=(const ScorePopup&) = delete;
    ScorePopup(ScorePopup&&) = delete;
    ScorePopup& operator=(ScorePopup&&) = delete;

    // Set the score value to display (200, 400, 800, 1600)
    void setScore(i32 score);

    // Position (top-left corner in pixels)
    void setPosition(f32 x, f32 y);
    void setPosition(Vec2f pos);

    // Visibility
    void setVisible(bool visible) { m_visible = visible; }

    // Render the score popup
    void render(Renderer& renderer) const;

private:
    const TextureAtlas& m_atlas;
    std::string m_regionName;
    Vec2f m_position{0.0f, 0.0f};
    bool m_visible{true};
};

}  // namespace pacman
