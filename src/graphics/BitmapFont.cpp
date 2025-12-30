#include "graphics/BitmapFont.hpp"

#include "core/Renderer.hpp"

#include <sstream>

namespace pacman {

BitmapFont::BitmapFont(SDL_Renderer* renderer, std::string_view fontPath)
    : m_fontAtlas(std::make_unique<TextureAtlas>(renderer, fontPath)) {
    defineCharacterRegions();
}

void BitmapFont::defineCharacterRegions() {
    // Font layout (each row has 16 characters, 8x8 pixels each):
    // Row 0: A B C D E F G H I J K L M N O (15 chars)
    // Row 1: P Q R S T U V W X Y Z ! (c) pts (14 chars)
    // Row 2: 0 1 2 3 4 5 6 7 8 9 / - " (13 chars)
    // Row 3: NAMCO
    // Each color variant repeats this pattern

    // Map characters to their index in the font grid
    // Row 0: A-O (indices 0-14)
    m_charToIndex['A'] = 0;
    m_charToIndex['B'] = 1;
    m_charToIndex['C'] = 2;
    m_charToIndex['D'] = 3;
    m_charToIndex['E'] = 4;
    m_charToIndex['F'] = 5;
    m_charToIndex['G'] = 6;
    m_charToIndex['H'] = 7;
    m_charToIndex['I'] = 8;
    m_charToIndex['J'] = 9;
    m_charToIndex['K'] = 10;
    m_charToIndex['L'] = 11;
    m_charToIndex['M'] = 12;
    m_charToIndex['N'] = 13;
    m_charToIndex['O'] = 14;

    // Row 1: P-Z then ! (indices 16-27)
    m_charToIndex['P'] = 16;
    m_charToIndex['Q'] = 17;
    m_charToIndex['R'] = 18;
    m_charToIndex['S'] = 19;
    m_charToIndex['T'] = 20;
    m_charToIndex['U'] = 21;
    m_charToIndex['V'] = 22;
    m_charToIndex['W'] = 23;
    m_charToIndex['X'] = 24;
    m_charToIndex['Y'] = 25;
    m_charToIndex['Z'] = 26;
    m_charToIndex['!'] = 27;

    // Row 2: 0-9 then / - " (indices 32-44)
    m_charToIndex['0'] = 32;
    m_charToIndex['1'] = 33;
    m_charToIndex['2'] = 34;
    m_charToIndex['3'] = 35;
    m_charToIndex['4'] = 36;
    m_charToIndex['5'] = 37;
    m_charToIndex['6'] = 38;
    m_charToIndex['7'] = 39;
    m_charToIndex['8'] = 40;
    m_charToIndex['9'] = 41;
    m_charToIndex['/'] = 42;
    m_charToIndex['-'] = 43;
    m_charToIndex['"'] = 44;

    // Space doesn't need an index - we just skip drawing
    m_charToIndex[' '] = -1;
}

i32 BitmapFont::getCharIndex(char c) const {
    auto it = m_charToIndex.find(c);
    if (it != m_charToIndex.end()) {
        return it->second;
    }
    return -1;  // Unknown character
}

void BitmapFont::drawText(Renderer& renderer, std::string_view text, i32 x, i32 y,
                          FontColor color) const {
    i32 cursorX = x;
    // Each color variant takes 4 rows (A-O, P-Z!, 0-9/-, NAMCO)
    constexpr i32 ROWS_PER_COLOR = 4;
    i32 colorRowOffset = static_cast<i32>(color) * ROWS_PER_COLOR * CHAR_HEIGHT;

    for (char c : text) {
        // Convert lowercase to uppercase
        if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }

        i32 charIndex = getCharIndex(c);
        if (charIndex >= 0) {
            // Calculate position in font texture
            i32 row = charIndex / CHARS_PER_ROW;
            i32 col = charIndex % CHARS_PER_ROW;

            // Source rectangle in font texture
            SpriteRegion region;
            region.x = col * CHAR_WIDTH;
            region.y = row * CHAR_HEIGHT + colorRowOffset;
            region.w = CHAR_WIDTH;
            region.h = CHAR_HEIGHT;

            renderer.drawTextureRegion(m_fontAtlas->getTexture(), region, cursorX, y);
        }
        // Always advance cursor (even for space or unknown chars)
        cursorX += CHAR_WIDTH;
    }
}

void BitmapFont::drawNumber(Renderer& renderer, i32 number, i32 x, i32 y, i32 minDigits,
                            FontColor color) const {
    std::ostringstream oss;
    oss << number;
    std::string numStr = oss.str();

    // Pad with spaces if needed
    while (static_cast<i32>(numStr.length()) < minDigits) {
        numStr = " " + numStr;
    }

    drawText(renderer, numStr, x, y, color);
}

i32 BitmapFont::getTextWidth(std::string_view text) const {
    return static_cast<i32>(text.length()) * CHAR_WIDTH;
}

}  // namespace pacman
