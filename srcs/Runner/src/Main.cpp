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
#include "Controllable.hpp"
#include "Destroyable.hpp"
#include "Hitbox.hpp"
#include "Collider.hpp"
#include "./../ECS/Systems/Position_System.hpp"
#undef main

int main() {
    Registry reg;
    reg.register_component<Texture>();
    reg.register_component<Scale>();
    reg.register_component<Position>();
    reg.register_component<MovementPattern>();
    reg.register_component<Velocity>();
    reg.register_component<Controllable>();
    reg.register_component<Destroyable>();
    reg.register_component<Hitbox>();

    Entity Space_background = reg.spawn_entity();
    reg.add_component<Texture>(Space_background, {"./assets/sprites/Beep_Background.png", 0, -0, 514, 160});
    reg.add_component<Scale>(Space_background, {7, 7});
    reg.add_component<Position>(Space_background, {0, 0});
    reg.add_component<MovementPattern>(Space_background, {STRAIGHTLEFT});
    reg.add_component<Velocity>(Space_background, {1});

    Entity Player = reg.spawn_entity();
    reg.add_component<Texture>(Player, {"./assets/sprites/Beep.png", 0, 0, 45, 75});
    reg.add_component<Position>(Player, {100, 700});
    reg.add_component<Scale>(Player, {3, 3});
    reg.add_component<Velocity>(Player, {10});
    reg.add_component<MovementPattern>(Player, {NONE});
    reg.add_component<Controllable>(Player, {"1"});
    reg.add_component<Destroyable>(Player, {3});
    reg.add_component<Hitbox>(Player, {45, 75});

    Entity Obstacle = reg.spawn_entity();
    reg.add_component<Texture>(Obstacle, {"./assets/sprites/Coyotte.png", 360, 0, 60, 80});
    reg.add_component<Scale>(Obstacle, {2, 2});
    reg.add_component<Position>(Obstacle, {1920, 700});
    reg.add_component<MovementPattern>(Obstacle, {STRAIGHTLEFT});
    reg.add_component<Velocity>(Obstacle, {20});
    reg.add_component<Hitbox>(Obstacle, {60, 80});


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

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Position_System posSys;

    bool isRunning = true;
    SDL_Event event;
    while (isRunning) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }
        SDL_RenderClear(renderer);
        posSys.position_SystemRunner(reg);
        auto textures = reg.get_components<Texture>();
        auto scales = reg.get_components<Scale>();
        auto positions = reg.get_components<Position>();
        auto patterns = reg.get_components<MovementPattern>();
        auto hitboxes = reg.get_components<Hitbox>();
        for (std::size_t i = 0; i < textures.size(); ++i)
        {
            auto &texture = textures[i];
            auto &scale = scales[i];
            auto &position = positions[i];
            auto &pattern = patterns[i];
            auto &hitbox = hitboxes[i];
            if (texture) {
                SDL_Surface* sprite = IMG_Load(texture->_path.c_str());
                if (sprite == NULL) {
                    SDL_Log("Erreur lors du chargement de l'image : %s", IMG_GetError());
                    return 84;
                }
                SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, sprite);
                SDL_Rect srcRect = {texture->_left, texture->_top, texture->_width, texture->_height };
                if (hitbox && pattern) {
                    if (position->_x <= 0) {
                        position->_x += 1920;
                    }
                }
                SDL_Rect destRect = {static_cast<int>(position->_x), static_cast<int>(position->_y), static_cast<int>(texture->_width * scale->_scaleX), static_cast<int>(texture->_height * scale->_scaleY) };
                SDL_RenderCopy(renderer, imageTexture, &srcRect, &destRect);
                SDL_FreeSurface(sprite);
            }
        }
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}