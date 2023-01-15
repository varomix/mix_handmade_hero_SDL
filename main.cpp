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

struct sdl_offscreen_buffer {
    SDL_Texture *texture;
    void *memory;
    int width;
    int height;
    int pitch;
};

struct sdl_window_dimension {
    int width;
    int height;
};

global_variable sdl_offscreen_buffer GlobalBackbuffer;

sdl_window_dimension
SDLGetWindowDimension(SDL_Window *window) {
    sdl_window_dimension result;
    SDL_GetWindowSize(window, &result.width, &result.height);
    return (result);
}

internal void
RenderWeirdGradient(sdl_offscreen_buffer buffer, int blue_offset, int green_offset) {
    uint8 *row = (uint8 *) buffer.memory;
    for (int y = 0; y < buffer.height; ++y) {
        uint32 *pixel = (uint32 *) row;
        for (int x = 0; x < buffer.width; ++x) {
            uint8 blue = (x + blue_offset);
            uint8 green = (y + green_offset);

            *pixel++ = ((green << 8) | blue);
        }
        row += buffer.pitch;
    }
}

internal void
SDLResizeTexture(sdl_offscreen_buffer *buffer, SDL_Renderer *renderer, int width, int height) {
    int bytes_per_pixel = 4;
    if (buffer->memory) {
        munmap(buffer->memory, buffer->width * buffer->height * bytes_per_pixel);
    }
    if (buffer->texture) {
        SDL_DestroyTexture(buffer->texture);
    }

    buffer->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        width,
                                        height);
    buffer->width = width;
    buffer->height = height;
    buffer->pitch = width * bytes_per_pixel;
    buffer->memory = mmap(0, width * height * bytes_per_pixel,
                          PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS,
                          -1,
                          0);

}

internal void
SDLUpdateWindow(SDL_Window *window, SDL_Renderer *renderer, sdl_offscreen_buffer buffer) {
    SDL_UpdateTexture(buffer.texture, 0, buffer.memory, buffer.pitch);
    SDL_RenderCopy(renderer, buffer.texture, 0, 0);
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
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
                    SDL_Renderer *renderer = SDL_GetRenderer(window);
                    printf("SDL_WINDOWEVENT_SIZE_CHANGED (%d, %d)\n", event->window.data1, event->window.data2);
                }
                    break;
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
                    SDLUpdateWindow(window, renderer, GlobalBackbuffer);
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
            sdl_window_dimension dimension = SDLGetWindowDimension(window);
            SDLResizeTexture(&GlobalBackbuffer, renderer, dimension.width, dimension.height);
            int x_offset = 0;
            int y_offset = 0;
            while (running) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (HandleEvent(&event)) {
                        running = false;
                    }
                }
                RenderWeirdGradient(GlobalBackbuffer, x_offset, y_offset);
                SDLUpdateWindow(window, renderer, GlobalBackbuffer);

                ++x_offset;
                y_offset += 1;
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































