#include "core/Renderer.hpp"

#include "graphics/TextureAtlas.hpp"
#include "utils/Constants.hpp"

#include <stdexcept>

namespace pacman {

Renderer::Renderer(const Window& window) : m_renderer(nullptr) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window.get(), nullptr);

    if (!renderer) {
        throw std::runtime_error(std::string("Failed to create renderer: ") + SDL_GetError());
    }

    m_renderer.reset(renderer);

    // Set logical presentation size for automatic scaling
    // Game renders at GAME_WIDTH x GAME_HEIGHT and SDL scales to window size
    SDL_SetRenderLogicalPresentation(m_renderer.get(), constants::GAME_WIDTH,
                                     constants::GAME_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // Use nearest neighbor scaling for crisp pixel art
    SDL_SetTextureScaleMode(nullptr, SDL_SCALEMODE_NEAREST);

    // Enable VSync
    SDL_SetRenderVSync(m_renderer.get(), 1);
}

void Renderer::clear() {
    SDL_RenderClear(m_renderer.get());
}

void Renderer::present() {
    SDL_RenderPresent(m_renderer.get());
}

void Renderer::setDrawColor(u8 r, u8 g, u8 b, u8 a) {
    SDL_SetRenderDrawColor(m_renderer.get(), r, g, b, a);
}

void Renderer::drawTextureRegion(SDL_Texture* texture, const SpriteRegion& region, i32 x, i32 y) {
    SDL_FRect srcRect{static_cast<f32>(region.x), static_cast<f32>(region.y),
                      static_cast<f32>(region.w), static_cast<f32>(region.h)};
    SDL_FRect dstRect{static_cast<f32>(x), static_cast<f32>(y), static_cast<f32>(region.w),
                      static_cast<f32>(region.h)};
    SDL_RenderTexture(m_renderer.get(), texture, &srcRect, &dstRect);
}

}  // namespace pacman
