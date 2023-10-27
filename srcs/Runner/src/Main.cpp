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

int main(int argc, char* argv[]) {
    Registry reg;
    reg.register_component<Texture>();

    Entity Space_background = reg.spawn_entity();
    reg.add_component<Texture>(Space_background, {"./assets/sprites/Space.png", 0, 0, 950, 200});

    auto textures = reg.get_components<Texture>();
    auto &texture = textures[0];
    std::cout << texture->_path << std::endl;
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Erreur lors de l'initialisation de SDL : %s", SDL_GetError());
        return 1;
    }

    // Création de la fenêtre
    SDL_Window* window = SDL_CreateWindow("SDL2 Runner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_Log("Erreur lors de la création de la fenêtre : %s", SDL_GetError());
        return 2;
    }

    // Initialisation du sous-système de chargement d'images (SDL_image)
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        SDL_Log("Erreur lors de l'initialisation de SDL_image : %s", IMG_GetError());
        return 3;
    }

    // Chargement d'une image
    SDL_Surface* space = IMG_Load(texture->_path.c_str());
    if (space == NULL) {
        SDL_Log("Erreur lors du chargement de l'image : %s", IMG_GetError());
        return 4;
    }

    // Création d'une texture à partir de l'image
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, space);
    SDL_FreeSurface(space);

    // Boucle de jeu
    bool isRunning = true;
    SDL_Event event;
    while (isRunning) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }

        // Effacement de l'écran
        SDL_RenderClear(renderer);

        // Affichage de l'image
        SDL_RenderCopy(renderer, imageTexture, NULL, NULL);

        // Mise à jour de l'affichage
        SDL_RenderPresent(renderer);

        SDL_Delay(16);  // Limiter la boucle de jeu à environ 60 FPS (environ 16 ms par frame)
    }

    // Libération des ressources
    SDL_DestroyTexture(imageTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
