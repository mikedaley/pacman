#pragma once

#include "graphics/TextureAtlas.hpp"
#include "utils/Types.hpp"

#include <string>

namespace pacman {

class Renderer;

// Score values that can be displayed as popup sprites
enum class ScoreValue {
    // Ghost eating scores
    Points200 = 200,
    Points400 = 400,
    Points800 = 800,
    Points1600 = 1600,
    // Fruit scores
    Points100 = 100,
    Points300 = 300,
    Points500 = 500,
    Points700 = 700,
    Points1000 = 1000,
    Points2000 = 2000,
    Points3000 = 3000,
    Points5000 = 5000
};

// A popup sprite that displays score values (used when eating ghosts, fruit, etc.)
// This class provides a reusable way to render score sprites at any position.
class ScorePopup {
public:
    explicit ScorePopup(const TextureAtlas& atlas);
    ~ScorePopup() = default;

    // Non-copyable, non-movable (references atlas)
    ScorePopup(const ScorePopup&) = delete;
    ScorePopup& operator=(const ScorePopup&) = delete;
    ScorePopup(ScorePopup&&) = delete;
    ScorePopup& operator=(ScorePopup&&) = delete;

    // Set the score value to display
    void setScore(ScoreValue score);
    void setScore(i32 score);  // Convenience overload that maps int to ScoreValue

    // Position (top-left corner in pixels)
    void setPosition(f32 x, f32 y);
    void setPosition(Vec2f pos);
    [[nodiscard]] Vec2f getPosition() const { return m_position; }

    // Visibility
    void setVisible(bool visible) { m_visible = visible; }
    [[nodiscard]] bool isVisible() const { return m_visible; }

    // Render the score popup
    void render(Renderer& renderer) const;

    // Get the sprite region name for a given score value
    [[nodiscard]] static std::string getRegionName(ScoreValue score);
    [[nodiscard]] static std::string getRegionName(i32 score);

private:
    const TextureAtlas& m_atlas;
    std::string m_regionName;
    Vec2f m_position{0.0f, 0.0f};
    bool m_visible{true};
};

}  // namespace pacman
