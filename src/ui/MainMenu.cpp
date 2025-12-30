#include "ui/MainMenu.hpp"

#include "core/Renderer.hpp"
#include "utils/Constants.hpp"

#include <SDL3/SDL.h>

namespace pacman {

MainMenu::MainMenu(TextureAtlas& atlas, BitmapFont& font)
    : m_atlas(atlas), m_font(font), m_blinkySprite(atlas), m_pinkySprite(atlas),
      m_inkySprite(atlas), m_clydeSprite(atlas), m_pacmanSprite(atlas) {
    // Set up ghost animations
    m_blinkySprite.setAnimationFrames({"blinky_right_0", "blinky_right_1"});
    m_blinkySprite.setAnimationSpeed(GHOST_ANIM_SPEED);

    m_pinkySprite.setAnimationFrames({"pinky_right_0", "pinky_right_1"});
    m_pinkySprite.setAnimationSpeed(GHOST_ANIM_SPEED);

    m_inkySprite.setAnimationFrames({"inky_right_0", "inky_right_1"});
    m_inkySprite.setAnimationSpeed(GHOST_ANIM_SPEED);

    m_clydeSprite.setAnimationFrames({"clyde_right_0", "clyde_right_1"});
    m_clydeSprite.setAnimationSpeed(GHOST_ANIM_SPEED);

    m_pacmanSprite.setAnimationFrames(
        {"pacman_closed", "pacman_right_half", "pacman_right_open", "pacman_right_half"});
    m_pacmanSprite.setAnimationSpeed(10.0f);
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
            case SDLK_1:
                // Start game with 1 player
                if (event.key.key == SDLK_1) {
                    m_selectedPlayers = 1;
                }
                return true;

            case SDLK_2:
                // Start game with 2 players
                m_selectedPlayers = 2;
                return true;

            default:
                break;
        }
    }
    return false;
}

void MainMenu::update(f32 deltaTime) {
    m_animTimer += deltaTime;

    // Blink cursor
    m_blinkTimer += deltaTime;
    if (m_blinkTimer >= BLINK_INTERVAL) {
        m_blinkTimer -= BLINK_INTERVAL;
        m_showCursor = !m_showCursor;
    }

    // Update sprite animations
    m_blinkySprite.update(deltaTime);
    m_pinkySprite.update(deltaTime);
    m_inkySprite.update(deltaTime);
    m_clydeSprite.update(deltaTime);
    m_pacmanSprite.update(deltaTime);
}

void MainMenu::render(Renderer& renderer) {
    renderTitle(renderer);
    renderCharacterShowcase(renderer);
    renderPlayerSelection(renderer);
    renderCredits(renderer);
}

void MainMenu::renderTitle(Renderer& renderer) {
    // Draw "CHARACTER / NICKNAME" header
    m_font.drawText(renderer, "CHARACTER / NICKNAME", 24, 40, FontColor::White);

    // Large PAC-MAN title - we'll use text since we don't have a logo sprite
    // Position it in the upper portion of the screen
}

void MainMenu::renderCharacterShowcase(Renderer& renderer) {
    // Display each ghost with their name and nickname
    // Format: [ghost sprite] -"NICKNAME" CHARACTERNAME

    constexpr i32 startY = 64;
    constexpr i32 lineHeight = 24;
    constexpr i32 spriteX = 32;
    constexpr i32 textX = 56;

    // Blinky - "SHADOW" BLINKY (Red)
    m_blinkySprite.setPosition(static_cast<f32>(spriteX), static_cast<f32>(startY));
    m_blinkySprite.render(renderer);
    m_font.drawText(renderer, "-SHADOW", textX, startY + 4, FontColor::Red);
    m_font.drawText(renderer, "\"BLINKY\"", textX + 72, startY + 4, FontColor::Red);

    // Pinky - "SPEEDY" PINKY (Pink)
    m_pinkySprite.setPosition(static_cast<f32>(spriteX), static_cast<f32>(startY + lineHeight));
    m_pinkySprite.render(renderer);
    m_font.drawText(renderer, "-SPEEDY", textX, startY + lineHeight + 4, FontColor::Pink);
    m_font.drawText(renderer, "\"PINKY\"", textX + 72, startY + lineHeight + 4, FontColor::Pink);

    // Inky - "BASHFUL" INKY (Cyan)
    m_inkySprite.setPosition(static_cast<f32>(spriteX), static_cast<f32>(startY + lineHeight * 2));
    m_inkySprite.render(renderer);
    m_font.drawText(renderer, "-BASHFUL", textX, startY + lineHeight * 2 + 4, FontColor::Cyan);
    m_font.drawText(renderer, "\"INKY\"", textX + 72, startY + lineHeight * 2 + 4, FontColor::Cyan);

    // Clyde - "POKEY" CLYDE (Orange)
    m_clydeSprite.setPosition(static_cast<f32>(spriteX), static_cast<f32>(startY + lineHeight * 3));
    m_clydeSprite.render(renderer);
    m_font.drawText(renderer, "-POKEY", textX, startY + lineHeight * 3 + 4, FontColor::Orange);
    m_font.drawText(renderer, "\"CLYDE\"", textX + 72, startY + lineHeight * 3 + 4,
                    FontColor::Orange);

    // Pellet point values
    constexpr i32 pelletY = startY + lineHeight * 5;
    m_font.drawText(renderer, "10 PTS", 80, pelletY, FontColor::White);
    m_font.drawText(renderer, "50 PTS", 80, pelletY + 16, FontColor::White);

    // Draw pellet sprites
    const auto& pelletRegion = m_atlas.getRegion("pellet");
    const auto& powerPelletRegion = m_atlas.getRegion("power_pellet");
    renderer.drawTextureRegion(m_atlas.getTexture(), pelletRegion, 56, pelletY);
    renderer.drawTextureRegion(m_atlas.getTexture(), powerPelletRegion, 56, pelletY + 16);
}

void MainMenu::renderPlayerSelection(Renderer& renderer) {
    constexpr i32 selectionY = 200;

    // Player selection options
    if (m_showCursor && m_selectedPlayers == 1) {
        m_font.drawText(renderer, ">", 56, selectionY, FontColor::White);
    }
    m_font.drawText(renderer, "1 PLAYER", 72, selectionY, FontColor::Cyan);

    if (m_showCursor && m_selectedPlayers == 2) {
        m_font.drawText(renderer, ">", 56, selectionY + 16, FontColor::White);
    }
    m_font.drawText(renderer, "2 PLAYERS", 72, selectionY + 16, FontColor::Cyan);

    // Instructions
    m_font.drawText(renderer, "PRESS ENTER TO START", 24, selectionY + 48, FontColor::Yellow);
}

void MainMenu::renderCredits(Renderer& renderer) {
    // Credits at bottom
    m_font.drawText(renderer, "@ 1980 MIDWAY MFG.CO.", 24, 264, FontColor::Pink);
}

void MainMenu::reset() {
    m_selectedPlayers = 1;
    m_animTimer = 0.0f;
    m_blinkTimer = 0.0f;
    m_showCursor = true;
}

}  // namespace pacman
