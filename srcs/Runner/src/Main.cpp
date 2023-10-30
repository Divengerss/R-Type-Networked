/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Main
*/

#include <SDL.h>
#include <SDL2/SDL_image.h>
#include "Registry.hpp"
#include "Texture.hpp"
#include "Scale.hpp"
#include "Position.hpp"
#include "MovementPattern.hpp"
#include "Velocity.hpp"
// #include "PositionSystem.hpp"

int main() {
    // PositionSystem posSys;
    Registry reg;
    reg.register_component<Texture>();
    reg.register_component<Scale>();
    reg.register_component<Position>();
    reg.register_component<MovementPattern>();
    reg.register_component<Velocity>();

    Entity Space_background = reg.spawn_entity();
    reg.add_component<Texture>(Space_background, {"./assets/sprites/Space.png", 0, 0, 950, 200});
    reg.add_component<Scale>(Space_background, {5, 5});
    reg.add_component<Position>(Space_background, {0, 0});
    reg.add_component<MovementPattern>(Space_background, {STRAIGHTLEFT});
    reg.add_component<Velocity>(Space_background, {1});

    auto textures = reg.get_components<Texture>();
    auto scales = reg.get_components<Scale>();
    auto positions = reg.get_components<Position>();
    auto patterns = reg.get_components<MovementPattern>();
    auto velocities = reg.get_components<Velocity>();

    auto &texture = textures[0];
    auto &scale = scales[0];
    auto &position = positions[0];
    auto &pattern = patterns[0];
    auto &velocity = velocities[0];

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Erreur lors de l'initialisation de SDL : %s", SDL_GetError());
        return 84;
    }

    SDL_Window* window = SDL_CreateWindow("SDL2 Runner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_Log("Erreur lors de la création de la fenêtre : %s", SDL_GetError());
        return 84;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        SDL_Log("Erreur lors de l'initialisation de SDL_image : %s", IMG_GetError());
        return 84;
    }

    SDL_Surface* space = IMG_Load(texture->_path.c_str());
    if (space == NULL) {
        SDL_Log("Erreur lors du chargement de l'image : %s", IMG_GetError());
        return 84;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, space);
    SDL_Rect srcRect = { texture->_left, texture->_top, texture->_width, texture->_height };
    SDL_Rect destRect = { 0, 0, static_cast<int>(texture->_width * scale->_scaleX), static_cast<int>(texture->_height * scale->_scaleY) };

    SDL_FreeSurface(space);

    bool isRunning = true;
    SDL_Event event;
    while (isRunning) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }
        // posSys.positionSystemClient(reg);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, imageTexture, &srcRect, &destRect);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyTexture(imageTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
