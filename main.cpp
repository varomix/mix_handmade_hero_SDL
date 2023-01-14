#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <SDL2/SDL.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#define internal static
#define local_persist static
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

global_variable SDL_Texture *texture;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

internal void
RenderWeirdGradient(int blue_offset, int green_offset) {
    int width = BitmapWidth;
    int height = BitmapHeight;

    int pitch = width * BytesPerPixel;
    uint8 *row = (uint8 *) BitmapMemory;
    for (int y = 0; y < BitmapHeight; ++y) {
        uint32 *pixel = (uint32 *) row;
        for (int x = 0; x < BitmapWidth; ++x) {
            uint8 blue = (x + blue_offset);
            uint8 green = (y + green_offset);

            *pixel++ = ((green << 8) | blue);
        }

        row += pitch;
    }
}

internal void
SDLResizeTexture(SDL_Renderer *renderer, int width, int height) {
    if (BitmapMemory) {
        munmap(BitmapMemory, BitmapWidth * BitmapHeight * BytesPerPixel);
    }
    if (texture) {
        SDL_DestroyTexture(texture);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                width,
                                height);
    BitmapWidth = width;
    BitmapHeight = height;
    BitmapMemory = mmap(0, width * height * BytesPerPixel,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS,
                        -1,
                        0);

}

internal void
SDLUpdateWindow(SDL_Window *window, SDL_Renderer *renderer) {
    SDL_UpdateTexture(texture, 0, BitmapMemory, BitmapWidth * BytesPerPixel);
    SDL_RenderCopy(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);
}

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
                    SDLUpdateWindow(window, renderer);
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
            bool running = true;
            int width, height;
            SDL_GetWindowSize(window, &width, &height);
            SDLResizeTexture(renderer, width, height);
            int x_offset = 0;
            int y_offset = 0;
            while (running) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (HandleEvent(&event)) {
                        running = false;
                    }
                }
                RenderWeirdGradient(x_offset, y_offset);
                SDLUpdateWindow(window, renderer);

                ++x_offset;
                y_offset += 2;
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































