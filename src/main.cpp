#include "core/Game.hpp"

#include <SDL3/SDL_main.h>

#include <cstdlib>
#include <exception>
#include <iostream>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    try {
        pacman::Game game;
        game.run();
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
