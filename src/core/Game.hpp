#pragma once

#include "core/Renderer.hpp"
#include "core/Window.hpp"
#include "entities/Blinky.hpp"
#include "entities/Clyde.hpp"
#include "entities/Ghost.hpp"
#include "entities/Player.hpp"
#include "graphics/BitmapFont.hpp"
#include "graphics/ScorePopup.hpp"
#include "graphics/Sprite.hpp"
#include "graphics/TextureAtlas.hpp"
#include "map/Maze.hpp"
#include "ui/MainMenu.hpp"
#include "utils/Constants.hpp"
#include "utils/Types.hpp"

#include <memory>

namespace pacman {

class Game {
public:
    Game();
    ~Game();

    // Non-copyable and non-movable (singleton-like main game instance)
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;

    void run();

private:
    void processEvents();
    void update(f32 deltaTime);
    void render();

    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<TextureAtlas> m_spriteAtlas;
    std::unique_ptr<BitmapFont> m_font;
    std::unique_ptr<Maze> m_maze;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<Sprite> m_lifeSprite;
    std::unique_ptr<Blinky> m_blinky;
    std::unique_ptr<Clyde> m_clyde;
    std::unique_ptr<ScorePopup> m_scorePopup;
    std::unique_ptr<MainMenu> m_mainMenu;

    // Application state (which screen is active)
    enum class AppState { MainMenu, Playing };
    AppState m_appState{AppState::MainMenu};

    // Two-player support
    i32 m_numPlayers{2};     // 1 or 2 players
    i32 m_currentPlayer{0};  // 0 = player 1, 1 = player 2

    // Per-player state (index 0 = player 1, index 1 = player 2)
    i32 m_scores[2]{0, 0};
    i32 m_lives[2]{constants::PACMAN_START_LIVES, constants::PACMAN_START_LIVES};

    i32 m_highScore{0};

    bool m_running{false};
    f32 m_accumulator{0.0f};

    // Game state machine
    enum class GameState {
        Ready,       // Showing "READY!" text before round starts
        Playing,     // Normal gameplay
        GhostEaten,  // Pause when ghost is eaten, showing score
        Dying,       // Death pause before animation
        DeathAnim,   // Playing death animation
        Respawning,  // Showing "READY!" after death before resuming
        GameOver     // No lives remaining
    };
    GameState m_gameState{GameState::Ready};
    f32 m_stateTimer{0.0f};

    static constexpr f32 READY_TIME = 2.0f;         // "READY!" display time
    static constexpr f32 DEATH_PAUSE_TIME = 1.0f;   // Pause before animation starts
    static constexpr f32 GHOST_EATEN_PAUSE = 1.0f;  // Pause when ghost is eaten

    // Ghost eating tracking (for consecutive ghost scoring)
    i32 m_ghostsEatenThisEnergizer{0};  // Resets when power pellet effect ends
    Ghost* m_eatenGhost{nullptr};       // Pointer to the ghost currently in Eaten state

    // Ghost house release tracking
    i32 m_dotsEatenThisLife{0};  // Dots eaten since start/respawn, used for ghost release

    // Collision detection
    void checkGhostCollision();
    void handlePacmanDeath();
    void resetPositions();

    // Game initialization
    void startNewGame();
};

}  // namespace pacman
