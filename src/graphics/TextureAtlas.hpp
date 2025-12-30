#pragma once

#include "utils/Types.hpp"

#include <SDL3/SDL.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

#include <SDL3_image/SDL_image.h>

namespace pacman {

// Represents a rectangular region within a texture atlas
struct SpriteRegion {
    i32 x{};
    i32 y{};
    i32 w{};
    i32 h{};

    constexpr SpriteRegion() = default;
    constexpr SpriteRegion(i32 x, i32 y, i32 w, i32 h) : x(x), y(y), w(w), h(h) {}
};

// Texture atlas for efficient sprite rendering
// Loads a single texture and allows defining named regions for individual sprites
class TextureAtlas {
public:
    TextureAtlas(SDL_Renderer* renderer, std::string_view imagePath);
    ~TextureAtlas() = default;

    // Non-copyable
    TextureAtlas(const TextureAtlas&) = delete;
    TextureAtlas& operator=(const TextureAtlas&) = delete;

    // Movable
    TextureAtlas(TextureAtlas&&) noexcept = default;
    TextureAtlas& operator=(TextureAtlas&&) noexcept = default;

    // Define a named sprite region within the atlas
    void defineRegion(std::string_view name, i32 x, i32 y, i32 w, i32 h);
    void defineRegion(std::string_view name, const SpriteRegion& region);

    // Define a grid of regions (useful for animation frames)
    // Names will be: baseName_0, baseName_1, etc.
    void defineGrid(std::string_view baseName, i32 startX, i32 startY, i32 cellWidth,
                    i32 cellHeight, i32 columns, i32 rows = 1, i32 paddingX = 0, i32 paddingY = 0);

    // Get a region by name
    [[nodiscard]] const SpriteRegion& getRegion(std::string_view name) const;
    [[nodiscard]] bool hasRegion(std::string_view name) const;

    // Get the underlying texture
    [[nodiscard]] SDL_Texture* getTexture() const { return m_texture.get(); }

    // Get atlas dimensions
    [[nodiscard]] i32 getWidth() const { return m_width; }
    [[nodiscard]] i32 getHeight() const { return m_height; }

private:
    struct TextureDeleter {
        void operator()(SDL_Texture* texture) const {
            if (texture) {
                SDL_DestroyTexture(texture);
            }
        }
    };

    std::unique_ptr<SDL_Texture, TextureDeleter> m_texture;
    std::unordered_map<std::string, SpriteRegion> m_regions;
    i32 m_width{};
    i32 m_height{};
};

}  // namespace pacman
