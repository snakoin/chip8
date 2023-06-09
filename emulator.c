#include <stdbool.h>
#include <SDL.h>
#include "chip8.h"
#include "log.h"
#include "assembly.h"

bool debug = false;

int main(int argc, char *argv[]) {
    chip8_init();
    for (int arg = 1; arg < argc; arg++) {
        if (strcmp(argv[arg], "--log") == 0) {
            logging = true;
        } else if (strcmp(argv[arg], "--debug") == 0) {
            debug = true;
        } else if (!chip8_load_rom(argv[arg])) {
            fprintf(stderr, "rom %s not found\n", argv[arg]);
            return EXIT_FAILURE;
        }
    }

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH * PIXEL_SIZE, SCREEN_HEIGHT * PIXEL_SIZE, SDL_WINDOW_RESIZABLE, &window, &renderer);

    int numkeys;
    const uint8_t *keyboard = SDL_GetKeyboardState(&numkeys);
    uint8_t *previous_keyboard = malloc(numkeys);
    while (true) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) break;
        chip8_update(keyboard, previous_keyboard, debug);
        for (int i = 0; i < numkeys; i++) {
            previous_keyboard[i] = keyboard[i];
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        chip8_render(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(3);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    chip8_logging_end();

    return 0;
}
