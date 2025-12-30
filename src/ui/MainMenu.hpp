#pragma once

#include "graphics/BitmapFont.hpp"
#include "graphics/Sprite.hpp"
#include "graphics/TextureAtlas.hpp"
#include "utils/Types.hpp"

namespace pacman {

class Renderer;

// Main menu screen for Pac-Man
// Displays title, character introductions, and player selection
class MainMenu {
public:
    MainMenu(TextureAtlas& atlas, BitmapFont& font);
    ~MainMenu() = default;

    // Non-copyable, non-movable
    MainMenu(const MainMenu&) = delete;
    MainMenu& operator=(const MainMenu&) = delete;
    MainMenu(MainMenu&&) = delete;
    MainMenu& operator=(MainMenu&&) = delete;

    // Process input events - returns true if start was pressed
    bool processInput(SDL_Event& event);

    // Update menu animations
    void update(f32 deltaTime);

    // Render the menu
    void render(Renderer& renderer);

    // Get selected number of players (1 or 2)
    [[nodiscard]] i32 getSelectedPlayers() const { return m_selectedPlayers; }

    // Reset menu to initial state
    void reset();

private:
    void renderTitle(Renderer& renderer);
    void renderCharacterShowcase(Renderer& renderer);
    void renderPlayerSelection(Renderer& renderer);
    void renderCredits(Renderer& renderer);

    TextureAtlas& m_atlas;
    BitmapFont& m_font;

    // Sprites for ghost showcase
    Sprite m_blinkySprite;
    Sprite m_pinkySprite;
    Sprite m_inkySprite;
    Sprite m_clydeSprite;
    Sprite m_pacmanSprite;

    // Menu state
    i32 m_selectedPlayers{1};  // 1 or 2 players
    f32 m_animTimer{0.0f};
    f32 m_blinkTimer{0.0f};
    bool m_showCursor{true};

    // Animation constants
    static constexpr f32 BLINK_INTERVAL = 0.3f;
    static constexpr f32 GHOST_ANIM_SPEED = 6.0f;
};

}  // namespace pacman
