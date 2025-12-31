#include "graphics/TextureAtlas.hpp"

#include <sstream>

namespace pacman {

TextureAtlas::TextureAtlas(SDL_Renderer* renderer, std::string_view imagePath)
    : m_texture(nullptr) {
    // Load as surface first so we can set color key for transparency
    SDL_Surface* surface = IMG_Load(imagePath.data());

    if (!surface) {
        std::ostringstream oss;
        oss << "Failed to load image '" << imagePath << "': " << SDL_GetError();
        throw std::runtime_error(oss.str());
    }

    // Set black (0, 0, 0) as the transparent color key
    SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface, 0, 0, 0));

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!texture) {
        std::ostringstream oss;
        oss << "Failed to create texture '" << imagePath << "': " << SDL_GetError();
        throw std::runtime_error(oss.str());
    }

    m_texture.reset(texture);

    // Use nearest neighbor filtering to prevent texture bleeding from adjacent sprites
    SDL_SetTextureScaleMode(m_texture.get(), SDL_SCALEMODE_NEAREST);

    // Enable alpha blending for transparency
    SDL_SetTextureBlendMode(m_texture.get(), SDL_BLENDMODE_BLEND);
}

void TextureAtlas::defineRegion(std::string_view name, i32 x, i32 y, i32 w, i32 h) {
    m_regions[std::string(name)] = SpriteRegion{x, y, w, h};
}

void TextureAtlas::defineRegion(std::string_view name, const SpriteRegion& region) {
    m_regions[std::string(name)] = region;
}

void TextureAtlas::defineGrid(std::string_view baseName, i32 startX, i32 startY, i32 cellWidth,
                              i32 cellHeight, i32 columns, i32 rows, i32 paddingX, i32 paddingY) {
    i32 index = 0;
    for (i32 row = 0; row < rows; ++row) {
        for (i32 col = 0; col < columns; ++col) {
            i32 x = startX + col * (cellWidth + paddingX);
            i32 y = startY + row * (cellHeight + paddingY);

            std::ostringstream oss;
            oss << baseName << "_" << index;
            m_regions[oss.str()] = SpriteRegion{x, y, cellWidth, cellHeight};
            ++index;
        }
    }
}

const SpriteRegion& TextureAtlas::getRegion(std::string_view name) const {
    auto it = m_regions.find(std::string(name));
    if (it == m_regions.end()) {
        std::ostringstream oss;
        oss << "Sprite region '" << name << "' not found";
        throw std::runtime_error(oss.str());
    }
    return it->second;
}

bool TextureAtlas::hasRegion(std::string_view name) const {
    return m_regions.contains(std::string(name));
}

}  // namespace pacman
