#pragma once

#include "graphics/BitmapFont.hpp"
#include "graphics/Sprite.hpp"
#include "graphics/TextureAtlas.hpp"
#include "utils/Types.hpp"

namespace pacman {

class Renderer;

// Attract mode states
enum class AttractState {
    CharacterIntro,  // Show ghost names and nicknames one at a time
    PelletPoints,    // Show pellet point values
    ChaseSequence,   // Blinky chases Pac-Man, then Super Pac-Man chases frightened Blinky
    PlayerSelect     // Show player selection
};

// Main menu screen for Pac-Man (US version attract mode)
// Cycles through: Character intro -> Pellet points -> Chase sequence -> Player select
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
    void updateCharacterIntro(f32 deltaTime);
    void updateChaseSequence(f32 deltaTime);

    void renderHeader(Renderer& renderer);
    void renderCharacterIntro(Renderer& renderer);
    void renderPelletPoints(Renderer& renderer);
    void renderChaseSequence(Renderer& renderer);
    void renderPlayerSelect(Renderer& renderer);
    void renderCredits(Renderer& renderer);

    void advanceToNextState();
    void startChaseSequence();

    TextureAtlas& m_atlas;
    BitmapFont& m_font;

    // Sprites for ghost showcase
    Sprite m_blinkySprite;
    Sprite m_pinkySprite;
    Sprite m_inkySprite;
    Sprite m_clydeSprite;
    Sprite m_pacmanSprite;
    Sprite m_superPacmanSprite;  // Large Pac-Man for chase sequence
    Sprite m_frightenedGhostSprite;

    // Attract mode state
    AttractState m_state{AttractState::CharacterIntro};
    f32 m_stateTimer{0.0f};
    i32 m_introStep{0};  // Current step in character intro (0-11: 3 steps per ghost x 4 ghosts)

    // Chase sequence state
    enum class ChasePhase { BlinkyChasesPacman, SuperPacmanChasesBlinky, Done };
    ChasePhase m_chasePhase{ChasePhase::BlinkyChasesPacman};
    f32 m_pacmanChaseX{0.0f};  // Pac-Man X in chase
    f32 m_ghostChaseX{0.0f};   // Ghost X in chase

    // Menu state
    i32 m_selectedPlayers{1};
    f32 m_blinkTimer{0.0f};
    bool m_showCursor{true};
    f32 m_powerPelletTimer{0.0f};
    bool m_showPowerPellet{true};

    // Timing constants
    static constexpr f32 INTRO_STEP_TIME = 0.75f;      // Time between each intro step
    static constexpr f32 INTRO_HOLD_TIME = 2.0f;       // Time to hold after all ghosts shown
    static constexpr f32 PELLET_DISPLAY_TIME = 3.0f;   // Time to show pellet points
    static constexpr f32 CHASE_SPEED = 80.0f;          // Pixels per second
    static constexpr f32 SUPER_PACMAN_SPEED = 120.0f;  // Super Pac-Man is faster
    static constexpr f32 PLAYER_SELECT_TIME = 8.0f;    // Time on player select before looping
    static constexpr f32 BLINK_INTERVAL = 0.3f;
    static constexpr f32 POWER_PELLET_BLINK = 0.2f;
    static constexpr f32 GHOST_ANIM_SPEED = 6.0f;
};

}  // namespace pacman
