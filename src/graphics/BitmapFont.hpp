#pragma once

#include "graphics/TextureAtlas.hpp"
#include "utils/Types.hpp"

#include <memory>
#include <string_view>
#include <unordered_map>

namespace pacman {

class Renderer;

// Font color options (each color takes 2 rows in the font sprite sheet)
enum class FontColor { White = 0, Red = 1, Pink = 2, Cyan = 3, Orange = 4, Cream = 5, Yellow = 6 };

// Renders text using bitmap font characters from a dedicated font texture
class BitmapFont {
public:
    static constexpr i32 CHAR_WIDTH = 8;
    static constexpr i32 CHAR_HEIGHT = 8;
    static constexpr i32 CHARS_PER_ROW = 16;

    BitmapFont(SDL_Renderer* renderer, std::string_view fontPath);
    ~BitmapFont() = default;

    // Non-copyable, non-movable
    BitmapFont(const BitmapFont&) = delete;
    BitmapFont& operator=(const BitmapFont&) = delete;
    BitmapFont(BitmapFont&&) = delete;
    BitmapFont& operator=(BitmapFont&&) = delete;

    // Render text at position with specified color
    void drawText(Renderer& renderer, std::string_view text, i32 x, i32 y,
                  FontColor color = FontColor::White) const;

    // Render a number (right-aligned, padded with spaces)
    void drawNumber(Renderer& renderer, i32 number, i32 x, i32 y, i32 minDigits = 1,
                    FontColor color = FontColor::White) const;

    // Get width of text in pixels
    [[nodiscard]] i32 getTextWidth(std::string_view text) const;

private:
    void defineCharacterRegions();
    [[nodiscard]] i32 getCharIndex(char c) const;

    std::unique_ptr<TextureAtlas> m_fontAtlas;
    std::unordered_map<char, i32> m_charToIndex;
};

}  // namespace pacman
