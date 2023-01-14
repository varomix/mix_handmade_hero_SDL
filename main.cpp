#include <stdio.h>
#include <SDL2/SDL.h>

bool HandleEvent(SDL_Event *event) {
    bool should_quit = false;

    switch (event->type) {
        case SDL_QUIT: {
            printf("SDL_QUIT\n");
            should_quit = true;
        }
            break;

        case SDL_WINDOWEVENT: {
            switch (event->window.event) {
                case SDL_WINDOWEVENT_RESIZED: {
                    printf("SDL_WINDOWEVENT_RESIZED (%d, %d)\n", event->window.data1, event->window.data2);
                }
                    break;

                case SDL_WINDOWEVENT_FOCUS_GAINED: {
                    printf("SDL_WINDOWEVENT_FOCUS_GAINED\n");
                }
                    break;
                case SDL_WINDOWEVENT_EXPOSED: {
                    SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
                    SDL_Renderer *renderer = SDL_GetRenderer(window);
                    static bool is_white = true;
                    if (is_white) {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        is_white = false;
                    } else {
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        is_white = true;
                    }
                    SDL_RenderClear(renderer);
                    SDL_RenderPresent(renderer);
                }
                    break;

            }
        }
            break;
    }
    return (should_quit);
}

int main() {
//    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Handmade Hero", "This is Handmade Hero", 0);
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Handmade Hero",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          640,
                                          480,
                                          SDL_WINDOW_RESIZABLE);

    if (window) {
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

        if (renderer) {
            for (;;) {
                SDL_Event event;
                SDL_WaitEvent(&event);
                if (HandleEvent(&event)) {
                    break;
                }
            }
        } else {
            // TODO(varomix): Logging
        }
    } else {
        // TODO(varomix): Logging
    }

    SDL_Quit();
    return 0;
}































