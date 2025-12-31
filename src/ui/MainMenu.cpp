#include "ui/MainMenu.hpp"

#include "core/Renderer.hpp"
#include "utils/Constants.hpp"

#include <SDL3/SDL.h>

namespace pacman {

MainMenu::MainMenu(TextureAtlas& atlas, BitmapFont& font)
    : m_atlas(atlas), m_font(font), m_blinkySprite(atlas), m_pinkySprite(atlas),
      m_inkySprite(atlas), m_clydeSprite(atlas), m_pacmanSprite(atlas), m_superPacmanSprite(atlas),
      m_frightenedGhostSprite(atlas) {
    // Set up ghost animations (facing left for character intro)
    m_blinkySprite.setAnimationFrames({"blinky_left_0", "blinky_left_1"});
    m_blinkySprite.setAnimationSpeed(GHOST_ANIM_SPEED);

    m_pinkySprite.setAnimationFrames({"pinky_left_0", "pinky_left_1"});
    m_pinkySprite.setAnimationSpeed(GHOST_ANIM_SPEED);

    m_inkySprite.setAnimationFrames({"inky_left_0", "inky_left_1"});
    m_inkySprite.setAnimationSpeed(GHOST_ANIM_SPEED);

    m_clydeSprite.setAnimationFrames({"clyde_left_0", "clyde_left_1"});
    m_clydeSprite.setAnimationSpeed(GHOST_ANIM_SPEED);

    // Pac-Man animation (facing left for being chased)
    m_pacmanSprite.setAnimationFrames(
        {"pacman_closed", "pacman_left_half", "pacman_left_open", "pacman_left_half"});
    m_pacmanSprite.setAnimationSpeed(10.0f);

    // Super Pac-Man (large, facing right for chasing)
    m_superPacmanSprite.setAnimationFrames(
        {"pacman_large_closed", "pacman_large_half", "pacman_large_open", "pacman_large_half"});
    m_superPacmanSprite.setAnimationSpeed(10.0f);

    // Frightened ghost
    m_frightenedGhostSprite.setAnimationFrames({"ghost_frightened_0", "ghost_frightened_1"});
    m_frightenedGhostSprite.setAnimationSpeed(GHOST_ANIM_SPEED);
}

bool MainMenu::processInput(SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            case SDLK_UP:
            case SDLK_DOWN:
                // Toggle between 1 and 2 players
                m_selectedPlayers = (m_selectedPlayers == 1) ? 2 : 1;
                return false;

            case SDLK_RETURN:
            case SDLK_SPACE:
                return true;

            case SDLK_1:
                m_selectedPlayers = 1;
                return true;

            case SDLK_2:
                m_selectedPlayers = 2;
                return true;

            default:
                break;
        }
    }
    return false;
}

void MainMenu::update(f32 deltaTime) {
    m_stateTimer += deltaTime;

    // Blink cursor
    m_blinkTimer += deltaTime;
    if (m_blinkTimer >= BLINK_INTERVAL) {
        m_blinkTimer -= BLINK_INTERVAL;
        m_showCursor = !m_showCursor;
    }

    // Blink power pellet
    m_powerPelletTimer += deltaTime;
    if (m_powerPelletTimer >= POWER_PELLET_BLINK) {
        m_powerPelletTimer -= POWER_PELLET_BLINK;
        m_showPowerPellet = !m_showPowerPellet;
    }

    // Update sprite animations
    m_blinkySprite.update(deltaTime);
    m_pinkySprite.update(deltaTime);
    m_inkySprite.update(deltaTime);
    m_clydeSprite.update(deltaTime);
    m_pacmanSprite.update(deltaTime);
    m_superPacmanSprite.update(deltaTime);
    m_frightenedGhostSprite.update(deltaTime);

    // State-specific updates
    switch (m_state) {
        case AttractState::CharacterIntro:
            updateCharacterIntro(deltaTime);
            break;
        case AttractState::PelletPoints:
            if (m_stateTimer >= PELLET_DISPLAY_TIME) {
                advanceToNextState();
            }
            break;
        case AttractState::ChaseSequence:
            updateChaseSequence(deltaTime);
            break;
        case AttractState::PlayerSelect:
            if (m_stateTimer >= PLAYER_SELECT_TIME) {
                advanceToNextState();
            }
            break;
    }
}

void MainMenu::updateCharacterIntro([[maybe_unused]] f32 deltaTime) {
    // Each ghost has 3 steps: sprite, character name, nickname
    // Total of 12 steps for 4 ghosts
    constexpr i32 TOTAL_STEPS = 12;

    i32 newStep = static_cast<i32>(m_stateTimer / INTRO_STEP_TIME);
    if (newStep > TOTAL_STEPS) {
        newStep = TOTAL_STEPS;
    }
    m_introStep = newStep;

    // After all steps complete, wait a bit then advance
    f32 totalIntroTime = TOTAL_STEPS * INTRO_STEP_TIME + INTRO_HOLD_TIME;
    if (m_stateTimer >= totalIntroTime) {
        advanceToNextState();
    }
}

void MainMenu::updateChaseSequence(f32 deltaTime) {
    constexpr f32 SCREEN_WIDTH = static_cast<f32>(constants::GAME_WIDTH);
    constexpr f32 CHASE_Y = 140.0f;

    switch (m_chasePhase) {
        case ChasePhase::BlinkyChasesPacman:
            // Pac-Man runs left, Blinky chases
            m_pacmanChaseX -= CHASE_SPEED * deltaTime;
            m_ghostChaseX -= CHASE_SPEED * deltaTime;

            m_pacmanSprite.setPosition(m_pacmanChaseX, CHASE_Y);
            m_blinkySprite.setPosition(m_ghostChaseX, CHASE_Y);

            // When both exit screen left, switch to phase 2
            if (m_ghostChaseX < -20.0f) {
                m_chasePhase = ChasePhase::SuperPacmanChasesBlinky;
                // Start from left side
                m_pacmanChaseX = -40.0f;
                m_ghostChaseX = 20.0f;
            }
            break;

        case ChasePhase::SuperPacmanChasesBlinky:
            // Super Pac-Man chases frightened Blinky to the right
            // Both move at same speed to maintain spacing
            m_pacmanChaseX += CHASE_SPEED * deltaTime;
            m_ghostChaseX += CHASE_SPEED * deltaTime;

            // Super Pac-Man is 32x32, so offset Y to center with ghost
            m_superPacmanSprite.setPosition(m_pacmanChaseX, CHASE_Y - 8.0f);
            m_frightenedGhostSprite.setPosition(m_ghostChaseX, CHASE_Y);

            // When Super Pac-Man exits screen right, advance state
            // Super Pac-Man is 32 pixels wide, so wait until fully off screen
            if (m_pacmanChaseX > SCREEN_WIDTH + 32.0f) {
                m_chasePhase = ChasePhase::Done;
                advanceToNextState();
            }
            break;

        case ChasePhase::Done:
            break;
    }
}

void MainMenu::advanceToNextState() {
    m_stateTimer = 0.0f;

    switch (m_state) {
        case AttractState::CharacterIntro:
            m_state = AttractState::PelletPoints;
            break;
        case AttractState::PelletPoints:
            m_state = AttractState::ChaseSequence;
            startChaseSequence();
            break;
        case AttractState::ChaseSequence:
            m_state = AttractState::PlayerSelect;
            break;
        case AttractState::PlayerSelect:
            // Loop back to character intro
            m_state = AttractState::CharacterIntro;
            m_introStep = 0;
            break;
    }
}

void MainMenu::startChaseSequence() {
    m_chasePhase = ChasePhase::BlinkyChasesPacman;
    // Start from right side of screen
    m_pacmanChaseX = static_cast<f32>(constants::GAME_WIDTH) - 40.0f;
    m_ghostChaseX = static_cast<f32>(constants::GAME_WIDTH);

    // Set up animations for chase (facing left)
    m_blinkySprite.setAnimationFrames({"blinky_left_0", "blinky_left_1"});
    m_blinkySprite.setAnimationSpeed(GHOST_ANIM_SPEED);
    m_pacmanSprite.setAnimationFrames(
        {"pacman_closed", "pacman_left_half", "pacman_left_open", "pacman_left_half"});
    m_pacmanSprite.setAnimationSpeed(10.0f);
}

void MainMenu::render(Renderer& renderer) {
    renderHeader(renderer);

    switch (m_state) {
        case AttractState::CharacterIntro:
            renderCharacterIntro(renderer);
            break;
        case AttractState::PelletPoints:
            renderCharacterIntro(renderer);  // Keep showing characters
            renderPelletPoints(renderer);
            break;
        case AttractState::ChaseSequence:
            renderChaseSequence(renderer);
            break;
        case AttractState::PlayerSelect:
            renderPlayerSelect(renderer);
            break;
    }

    renderCredits(renderer);
}

void MainMenu::renderHeader(Renderer& renderer) {
    // Score display area (mimics gameplay HUD)
    m_font.drawText(renderer, "1UP", 24, 0, FontColor::White);
    m_font.drawText(renderer, "HIGH SCORE", 72, 0, FontColor::White);
    m_font.drawText(renderer, "2UP", 176, 0, FontColor::White);

    // Placeholder scores
    m_font.drawText(renderer, "00", 40, 8, FontColor::White);
    m_font.drawText(renderer, "00", 96, 8, FontColor::White);
}

void MainMenu::renderCharacterIntro(Renderer& renderer) {
    // Title
    m_font.drawText(renderer, "CHARACTER / NICKNAME", 24, 40, FontColor::White);

    constexpr i32 startY = 64;
    constexpr i32 lineHeight = 24;
    constexpr i32 spriteX = 24;
    constexpr i32 dashX = 48;
    constexpr i32 characterX = 56;
    constexpr i32 nicknameX = 128;

    // Each ghost has 3 steps: sprite (step 0), character name (step 1), nickname (step 2)
    // Blinky: steps 0-2, Pinky: steps 3-5, Inky: steps 6-8, Clyde: steps 9-11

    // Blinky - "SHADOW" "BLINKY" (Red)
    if (m_introStep >= 1) {
        m_blinkySprite.setPosition(static_cast<f32>(spriteX), static_cast<f32>(startY));
        m_blinkySprite.render(renderer);
    }
    if (m_introStep >= 2) {
        m_font.drawText(renderer, "-", dashX, startY + 4, FontColor::Red);
        m_font.drawText(renderer, "SHADOW", characterX, startY + 4, FontColor::Red);
    }
    if (m_introStep >= 3) {
        m_font.drawText(renderer, "\"BLINKY\"", nicknameX, startY + 4, FontColor::Red);
    }

    // Pinky - "SPEEDY" "PINKY" (Pink)
    if (m_introStep >= 4) {
        m_pinkySprite.setPosition(static_cast<f32>(spriteX), static_cast<f32>(startY + lineHeight));
        m_pinkySprite.render(renderer);
    }
    if (m_introStep >= 5) {
        m_font.drawText(renderer, "-", dashX, startY + lineHeight + 4, FontColor::Pink);
        m_font.drawText(renderer, "SPEEDY", characterX, startY + lineHeight + 4, FontColor::Pink);
    }
    if (m_introStep >= 6) {
        m_font.drawText(renderer, "\"PINKY\"", nicknameX, startY + lineHeight + 4, FontColor::Pink);
    }

    // Inky - "BASHFUL" "INKY" (Cyan)
    if (m_introStep >= 7) {
        m_inkySprite.setPosition(static_cast<f32>(spriteX),
                                 static_cast<f32>(startY + lineHeight * 2));
        m_inkySprite.render(renderer);
    }
    if (m_introStep >= 8) {
        m_font.drawText(renderer, "-", dashX, startY + lineHeight * 2 + 4, FontColor::Cyan);
        m_font.drawText(renderer, "BASHFUL", characterX, startY + lineHeight * 2 + 4,
                        FontColor::Cyan);
    }
    if (m_introStep >= 9) {
        m_font.drawText(renderer, "\"INKY\"", nicknameX, startY + lineHeight * 2 + 4,
                        FontColor::Cyan);
    }

    // Clyde - "POKEY" "CLYDE" (Orange)
    if (m_introStep >= 10) {
        m_clydeSprite.setPosition(static_cast<f32>(spriteX),
                                  static_cast<f32>(startY + lineHeight * 3));
        m_clydeSprite.render(renderer);
    }
    if (m_introStep >= 11) {
        m_font.drawText(renderer, "-", dashX, startY + lineHeight * 3 + 4, FontColor::Orange);
        m_font.drawText(renderer, "POKEY", characterX, startY + lineHeight * 3 + 4,
                        FontColor::Orange);
    }
    if (m_introStep >= 12) {
        m_font.drawText(renderer, "\"CLYDE\"", nicknameX, startY + lineHeight * 3 + 4,
                        FontColor::Orange);
    }
}

void MainMenu::renderPelletPoints(Renderer& renderer) {
    constexpr i32 pelletY = 176;

    // Draw pellet and power pellet with their point values
    const auto& pelletRegion = m_atlas.getRegion("pellet");
    renderer.drawTextureRegion(m_atlas.getTexture(), pelletRegion, 56, pelletY);
    m_font.drawText(renderer, "10 PTS", 80, pelletY, FontColor::White);

    if (m_showPowerPellet) {
        const auto& powerPelletRegion = m_atlas.getRegion("power_pellet");
        renderer.drawTextureRegion(m_atlas.getTexture(), powerPelletRegion, 56, pelletY + 16);
    }
    m_font.drawText(renderer, "50 PTS", 80, pelletY + 16, FontColor::White);
}

void MainMenu::renderChaseSequence(Renderer& renderer) {
    switch (m_chasePhase) {
        case ChasePhase::BlinkyChasesPacman:
            m_pacmanSprite.render(renderer);
            m_blinkySprite.render(renderer);
            break;

        case ChasePhase::SuperPacmanChasesBlinky:
            m_frightenedGhostSprite.render(renderer);
            m_superPacmanSprite.render(renderer);
            break;

        case ChasePhase::Done:
            break;
    }
}

void MainMenu::renderPlayerSelect(Renderer& renderer) {
    constexpr i32 selectionY = 120;

    // Push start button text
    m_font.drawText(renderer, "PUSH START BUTTON", 32, selectionY, FontColor::Orange);

    // Player selection options
    constexpr i32 optionY = selectionY + 32;

    if (m_showCursor && m_selectedPlayers == 1) {
        m_font.drawText(renderer, ">", 48, optionY, FontColor::White);
    }
    m_font.drawText(renderer, "1 PLAYER ONLY", 64, optionY, FontColor::Cyan);

    if (m_showCursor && m_selectedPlayers == 2) {
        m_font.drawText(renderer, ">", 48, optionY + 16, FontColor::White);
    }
    m_font.drawText(renderer, "1 OR 2 PLAYERS", 64, optionY + 16, FontColor::Cyan);

    // Bonus Pac-Man info
    constexpr i32 bonusY = optionY + 48;
    m_font.drawText(renderer, "BONUS PAC-MAN FOR", 32, bonusY, FontColor::White);
    m_font.drawText(renderer, "10000 PTS", 64, bonusY + 16, FontColor::Yellow);
}

void MainMenu::renderCredits(Renderer& renderer) {
    // Credits at bottom
    m_font.drawText(renderer, "@ 1980 MIDWAY MFG.CO.", 24, 264, FontColor::Pink);
}

void MainMenu::reset() {
    m_state = AttractState::CharacterIntro;
    m_stateTimer = 0.0f;
    m_introStep = 0;
    m_chasePhase = ChasePhase::BlinkyChasesPacman;
    m_selectedPlayers = 1;
    m_blinkTimer = 0.0f;
    m_showCursor = true;
    m_powerPelletTimer = 0.0f;
    m_showPowerPellet = true;
}

}  // namespace pacman
