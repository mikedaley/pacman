#include "core/Game.hpp"

#include "graphics/SpriteSheet.hpp"
#include "utils/Constants.hpp"

#include <SDL3/SDL.h>

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

namespace pacman {

Game::Game() {
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        throw std::runtime_error(std::string("Failed to initialize SDL: ") + SDL_GetError());
    }

    // Create window and renderer
    m_window = std::make_unique<Window>(constants::WINDOW_TITLE, constants::WINDOW_WIDTH,
                                        constants::WINDOW_HEIGHT);

    m_renderer = std::make_unique<Renderer>(*m_window);

    // Load sprite atlas and define all sprite regions
    m_spriteAtlas = std::make_unique<TextureAtlas>(m_renderer->get(), "assets/sprites/sprites.png");
    defineSpriteRegions(*m_spriteAtlas);

    // Create bitmap font for text rendering
    m_font = std::make_unique<BitmapFont>(
        m_renderer->get(), "assets/fonts/Arcade - Pac-Man - Miscellaneous - Text.png");

    // Create and load the maze
    m_maze = std::make_unique<Maze>(*m_spriteAtlas);
    m_maze->loadClassicMaze();

    // Create player
    m_player = std::make_unique<Player>(*m_spriteAtlas, *m_maze);
    m_player->reset();

    // Create life indicator sprite
    m_lifeSprite = std::make_unique<Sprite>(*m_spriteAtlas);
    m_lifeSprite->setRegion("pacman_left_half");

    // Create ghosts
    m_blinky = std::make_unique<Blinky>(*m_spriteAtlas, *m_maze);
    m_pinky = std::make_unique<Pinky>(*m_spriteAtlas, *m_maze);
    m_inky = std::make_unique<Inky>(*m_spriteAtlas, *m_maze, *m_blinky);
    m_clyde = std::make_unique<Clyde>(*m_spriteAtlas, *m_maze);

    // Create score popup for displaying points when eating ghosts/fruit
    m_scorePopup = std::make_unique<ScorePopup>(*m_spriteAtlas);

    // Create main menu
    m_mainMenu = std::make_unique<MainMenu>(*m_spriteAtlas, *m_font);
}

Game::~Game() {
    // Clean up in reverse order
    m_mainMenu.reset();
    m_scorePopup.reset();
    m_clyde.reset();
    m_inky.reset();
    m_pinky.reset();
    m_blinky.reset();
    m_lifeSprite.reset();
    m_player.reset();
    m_maze.reset();
    m_font.reset();
    m_spriteAtlas.reset();
    m_renderer.reset();
    m_window.reset();
    SDL_Quit();
}

void Game::run() {
    m_running = true;
    u64 lastTime = SDL_GetTicksNS();

    while (m_running) {
        u64 currentTime = SDL_GetTicksNS();
        f32 frameTime = static_cast<f32>(currentTime - lastTime) / 1'000'000'000.0f;
        lastTime = currentTime;

        // Cap frame time to prevent spiral of death
        if (frameTime > constants::FIXED_TIMESTEP * constants::MAX_UPDATES_PER_FRAME) {
            frameTime = constants::FIXED_TIMESTEP * constants::MAX_UPDATES_PER_FRAME;
        }

        processEvents();

        // Fixed timestep update
        m_accumulator += frameTime;
        while (m_accumulator >= constants::FIXED_TIMESTEP) {
            update(constants::FIXED_TIMESTEP);
            m_accumulator -= constants::FIXED_TIMESTEP;
        }

        render();
    }
}

void Game::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                m_running = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE) {
                    if (m_appState == AppState::Playing) {
                        // Return to main menu
                        m_appState = AppState::MainMenu;
                        m_mainMenu->reset();
                    } else {
                        m_running = false;
                    }
                    break;
                }

                // Handle input based on app state
                if (m_appState == AppState::MainMenu) {
                    if (m_mainMenu->processInput(event)) {
                        // Start game
                        m_numPlayers = m_mainMenu->getSelectedPlayers();
                        m_appState = AppState::Playing;
                        startNewGame();
                    }
                } else if (m_appState == AppState::Playing) {
                    switch (event.key.key) {
                        case SDLK_UP:
                            m_player->setDesiredDirection(Direction::Up);
                            break;
                        case SDLK_DOWN:
                            m_player->setDesiredDirection(Direction::Down);
                            break;
                        case SDLK_LEFT:
                            m_player->setDesiredDirection(Direction::Left);
                            break;
                        case SDLK_RIGHT:
                            m_player->setDesiredDirection(Direction::Right);
                            break;
                        default:
                            break;
                    }
                }
                break;

            default:
                break;
        }
    }
}

void Game::update(f32 deltaTime) {
    // Handle app state
    if (m_appState == AppState::MainMenu) {
        m_mainMenu->update(deltaTime);
        return;
    }

    // Handle game state machine
    if (m_gameState != GameState::Playing) {
        m_stateTimer += deltaTime;

        switch (m_gameState) {
            case GameState::Ready:
                // Waiting to start - show "READY!" text
                if (m_stateTimer >= READY_TIME) {
                    m_gameState = GameState::Playing;
                    m_stateTimer = 0.0f;
                }
                return;

            case GameState::Dying:
                // Initial pause - everything frozen
                if (m_stateTimer >= DEATH_PAUSE_TIME) {
                    m_gameState = GameState::DeathAnim;
                    m_stateTimer = 0.0f;
                    m_player->playDeathAnimation();
                }
                return;

            case GameState::DeathAnim:
                // Playing death animation
                m_player->updateDeathAnimation(deltaTime);

                if (m_player->isDeathAnimationComplete()) {
                    // Check if game over
                    m_lives[m_currentPlayer]--;
                    if (m_lives[m_currentPlayer] <= 0) {
                        m_gameState = GameState::GameOver;
                        m_stateTimer = 0.0f;
                    } else {
                        // Reset positions and show READY
                        resetPositions();
                        m_gameState = GameState::Respawning;
                        m_stateTimer = 0.0f;
                    }
                }
                return;

            case GameState::Respawning:
                // Show "READY!" before resuming play
                if (m_stateTimer >= READY_TIME) {
                    m_gameState = GameState::Playing;
                    m_stateTimer = 0.0f;
                }
                return;

            case GameState::GhostEaten:
                // Pause showing score sprite where ghost was eaten
                if (m_stateTimer >= GHOST_EATEN_PAUSE) {
                    // Transition the eaten ghost from Eaten to Eyes state
                    if (m_eatenGhost != nullptr) {
                        m_eatenGhost->transitionToEyes();
                        m_eatenGhost = nullptr;
                    }
                    m_gameState = GameState::Playing;
                    m_stateTimer = 0.0f;
                }
                return;

            case GameState::GameOver:
                // Game over state - TODO: show game over screen
                return;

            case GameState::Playing:
                // Handled below
                break;
        }
    }

    // Update maze (power pellet flashing)
    m_maze->update(deltaTime);

    // Update player (handles movement, animation, pellet eating)
    m_player->update(deltaTime);

    // Check for pellet eating at player's tile
    Vec2i playerTile = m_player->getTile();
    i32 points = m_maze->eatPellet(playerTile.x, playerTile.y);
    if (points > 0) {
        m_scores[m_currentPlayer] += points;
        if (m_scores[m_currentPlayer] > m_highScore) {
            m_highScore = m_scores[m_currentPlayer];
        }

        // Track dots eaten for ghost release
        m_dotsEatenThisLife++;

        // Check if any ghosts should be released from the house
        // Release ghosts when their dot limit is reached
        if (m_pinky->getState() == GhostState::InHouse &&
            m_dotsEatenThisLife >= m_pinky->getDotLimit()) {
            m_pinky->releaseFromHouse();
        }
        if (m_inky->getState() == GhostState::InHouse &&
            m_dotsEatenThisLife >= m_inky->getDotLimit()) {
            m_inky->releaseFromHouse();
        }
        if (m_clyde->getState() == GhostState::InHouse &&
            m_dotsEatenThisLife >= m_clyde->getDotLimit()) {
            m_clyde->releaseFromHouse();
        }

        // Power pellet triggers frightened mode
        if (points == constants::SCORE_POWER_PELLET) {
            m_blinky->setFrightened();
            m_pinky->setFrightened();
            m_inky->setFrightened();
            m_clyde->setFrightened();
        }
    }

    // Update ghosts
    Vec2i pacmanTile = m_player->getTile();
    Direction pacmanDir = m_player->getDirection();
    m_blinky->update(deltaTime, pacmanTile, pacmanDir);
    m_pinky->update(deltaTime, pacmanTile, pacmanDir);
    m_inky->update(deltaTime, pacmanTile, pacmanDir);
    m_clyde->update(deltaTime, pacmanTile, pacmanDir);

    // Reset ghost eaten counter when no ghosts are frightened anymore
    // (Power pellet effect has worn off)
    if (m_ghostsEatenThisEnergizer > 0 && !m_blinky->isFrightened() && !m_pinky->isFrightened() &&
        !m_inky->isFrightened() && !m_clyde->isFrightened()) {
        m_ghostsEatenThisEnergizer = 0;
    }

    // Check for ghost-Pacman collision
    checkGhostCollision();
}

void Game::render() {
    // Clear with black background
    m_renderer->setDrawColor(0, 0, 0);
    m_renderer->clear();

    // Render based on app state
    if (m_appState == AppState::MainMenu) {
        m_mainMenu->render(*m_renderer);
        m_renderer->present();
        return;
    }

    // Draw header - classic Pacman layout:
    // Row 0: "    1UP   HIGH SCORE   2UP    "
    // Row 1: "     00        00        "  (scores)
    // The maze starts at row 3 (y=24)

    // "1UP" label - only show when player 1 is active
    if (m_currentPlayer == 0) {
        m_font->drawText(*m_renderer, "1UP", 24, 0);
    }

    // "HIGH SCORE" centered
    m_font->drawText(*m_renderer, "HIGH SCORE", 72, 0);

    // "2UP" label - only show when player 2 is active (and in 2-player mode)
    if (m_numPlayers == 2 && m_currentPlayer == 1) {
        m_font->drawText(*m_renderer, "2UP", 176, 0);
    }

    // Player 1 score (always shown, right-aligned under "1UP" position)
    std::string score1Str = std::to_string(m_scores[0]);
    if (score1Str == "0")
        score1Str = "00";
    i32 score1X = 56 - static_cast<i32>(score1Str.length()) * 8;
    m_font->drawText(*m_renderer, score1Str, score1X, 8);

    // High score (centered under "HIGH SCORE" - center at x=112)
    std::string highScoreStr = std::to_string(m_highScore);
    if (highScoreStr == "0")
        highScoreStr = "00";
    i32 highScoreX = 112 - static_cast<i32>(highScoreStr.length()) * 8 / 2;
    m_font->drawText(*m_renderer, highScoreStr, highScoreX, 8);

    // Player 2 score (always shown in 2-player mode, right-aligned under "2UP" position)
    if (m_numPlayers == 2) {
        std::string score2Str = std::to_string(m_scores[1]);
        if (score2Str == "0")
            score2Str = "00";
        i32 score2X = 208 - static_cast<i32>(score2Str.length()) * 8;
        m_font->drawText(*m_renderer, score2Str, score2X, 8);
    }

    // Draw maze (offset by header height)
    m_maze->render(*m_renderer, constants::MAZE_OFFSET_Y);

    // Draw player sprite (hide during game over and when ghost is eaten)
    if (m_gameState != GameState::GameOver && m_gameState != GameState::GhostEaten) {
        m_player->render(*m_renderer);
    }

    // Draw ghosts (hide during death animation, respawning, game over, and when eaten)
    if (m_gameState != GameState::DeathAnim && m_gameState != GameState::Respawning &&
        m_gameState != GameState::GameOver) {
        if (!m_blinky->isEaten()) {
            m_blinky->render(*m_renderer);
        }
        if (!m_pinky->isEaten()) {
            m_pinky->render(*m_renderer);
        }
        if (!m_inky->isEaten()) {
            m_inky->render(*m_renderer);
        }
        if (!m_clyde->isEaten()) {
            m_clyde->render(*m_renderer);
        }
    }

    // Draw score popup when ghost is eaten
    if (m_gameState == GameState::GhostEaten) {
        m_scorePopup->render(*m_renderer);
    }

    // Draw "READY!" text during ready states (yellow)
    if (m_gameState == GameState::Ready || m_gameState == GameState::Respawning) {
        // "READY!" appears below the ghost house, centered
        // Ghost house is around tile (11-16, 13-15), so READY goes at around y=17 in tiles
        i32 readyY = 17 * constants::TILE_SIZE + constants::MAZE_OFFSET_Y;
        i32 readyX = (constants::GAME_WIDTH - 6 * 8) / 2;  // "READY!" is 6 chars
        m_font->drawText(*m_renderer, "READY!", readyX, readyY, FontColor::Yellow);
    }

    // Draw "GAME OVER" text (red)
    if (m_gameState == GameState::GameOver) {
        i32 gameOverY = 17 * constants::TILE_SIZE + constants::MAZE_OFFSET_Y;
        i32 gameOverX = (constants::GAME_WIDTH - 10 * 8) / 2;  // "GAME OVER" is 9 chars
        m_font->drawText(*m_renderer, "GAME  OVER", gameOverX, gameOverY, FontColor::Red);
    }

    // Draw lives at bottom left (for current player)
    i32 livesY = constants::GAME_HEIGHT - constants::FOOTER_HEIGHT;
    i32 currentLives = m_lives[m_currentPlayer];
    for (i32 i = 0; i < currentLives - 1; ++i) {  // -1 because current life is in play
        m_lifeSprite->setPosition(static_cast<f32>(16 + i * 16), static_cast<f32>(livesY));
        m_lifeSprite->render(*m_renderer);
    }

    // Draw fruit indicator at bottom right (cherry for level 1)
    const auto& fruitRegion = m_spriteAtlas->getRegion("fruit_cherry");
    m_renderer->drawTextureRegion(m_spriteAtlas->getTexture(), fruitRegion,
                                  constants::GAME_WIDTH - 24, livesY);

    m_renderer->present();
}

void Game::checkGhostCollision() {
    Vec2i playerTile = m_player->getTile();

    // Check collision with each ghost
    Ghost* ghosts[] = {m_blinky.get(), m_pinky.get(), m_inky.get(), m_clyde.get()};

    for (Ghost* ghost : ghosts) {
        Vec2i ghostTile = ghost->getTile();

        // Check if on same tile
        if (playerTile.x == ghostTile.x && playerTile.y == ghostTile.y) {
            if (ghost->isFrightened()) {
                // Pacman eats the ghost - calculate score based on consecutive ghosts eaten
                m_ghostsEatenThisEnergizer++;

                // Score doubles for each consecutive ghost: 200, 400, 800, 1600
                static constexpr i32 GHOST_SCORES[] = {200, 400, 800, 1600};
                i32 scoreIndex = std::min(m_ghostsEatenThisEnergizer - 1, 3);
                i32 ghostScore = GHOST_SCORES[scoreIndex];

                m_scores[m_currentPlayer] += ghostScore;
                if (m_scores[m_currentPlayer] > m_highScore) {
                    m_highScore = m_scores[m_currentPlayer];
                }

                // Configure score popup to display at ghost position
                m_scorePopup->setScore(ghostScore);
                m_scorePopup->setPosition(ghost->getPosition());

                // Mark ghost as eaten (will transition to Eyes after pause)
                ghost->setEaten();
                m_eatenGhost = ghost;

                // Pause the game to show score
                m_gameState = GameState::GhostEaten;
                m_stateTimer = 0.0f;
                return;  // Only process one collision per frame
            } else if (!ghost->isEyes()) {
                // Ghost catches Pacman
                handlePacmanDeath();
                return;
            }
        }
    }
}

void Game::handlePacmanDeath() {
    // Start death sequence with pause
    m_gameState = GameState::Dying;
    m_stateTimer = 0.0f;
}

void Game::resetPositions() {
    // Reset player to starting position
    m_player->reset();

    // Reset ghosts to starting positions
    m_blinky->reset();
    m_pinky->reset();
    m_inky->reset();
    m_clyde->reset();

    // Reset dot counter for ghost release
    m_dotsEatenThisLife = 0;

    // Release ghosts with dot limit of 0 immediately
    if (m_pinky->getDotLimit() == 0) {
        m_pinky->releaseFromHouse();
    }
}

void Game::startNewGame() {
    // Reset scores and lives for all players
    m_scores[0] = 0;
    m_scores[1] = 0;
    m_lives[0] = constants::PACMAN_START_LIVES;
    m_lives[1] = constants::PACMAN_START_LIVES;
    m_currentPlayer = 0;

    // Reset maze (restore all pellets)
    m_maze->loadClassicMaze();

    // Reset positions
    resetPositions();

    // Reset game state
    m_gameState = GameState::Ready;
    m_stateTimer = 0.0f;
    m_ghostsEatenThisEnergizer = 0;
    m_eatenGhost = nullptr;
}

}  // namespace pacman
