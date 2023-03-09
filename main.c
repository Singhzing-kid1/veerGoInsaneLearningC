#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

int main(int argc, char *argv[]){
    SDL_Window* window = NULL;// pointer to the window
    SDL_Renderer* renderer = NULL;


    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("HAHA LOSER SDL INIT: Error %s", SDL_GetError());
        return 1;
    }

    if(TTF_Init() < 0){
        printf("HAHA LOSER TTF INIT: Error %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", 24);
    SDL_Color color = {255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, "Hello World", color);

    if(!surface){
        printf("HAHA LOSER CREATE SURFACE: Error %s", SDL_GetError());
        return 1;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstrect = {0, 0, surface->w, surface->h};



    if(!window){
        printf("HAHA LOSE CREATE WINDOW: Error %s", SDL_GetError());
        return 1;
    }

    bool quit = 0;
    SDL_Event event;

    while(!quit){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                quit = 1;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
