#pragma once

#include "core/Window.hpp"
#include "utils/Types.hpp"

#include <SDL3/SDL.h>

#include <memory>

namespace pacman {

struct SpriteRegion;

class Renderer {
public:
    explicit Renderer(const Window& window);
    ~Renderer() = default;

    // Non-copyable
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // Movable
    Renderer(Renderer&&) noexcept = default;
    Renderer& operator=(Renderer&&) noexcept = default;

    [[nodiscard]] SDL_Renderer* get() const { return m_renderer.get(); }

    void clear();
    void present();

    void setDrawColor(u8 r, u8 g, u8 b, u8 a = 255);

    // Texture rendering
    void drawTextureRegion(SDL_Texture* texture, const SpriteRegion& region, i32 x, i32 y);

private:
    struct RendererDeleter {
        void operator()(SDL_Renderer* renderer) const {
            if (renderer) {
                SDL_DestroyRenderer(renderer);
            }
        }
    };

    std::unique_ptr<SDL_Renderer, RendererDeleter> m_renderer;
};

}  // namespace pacman
