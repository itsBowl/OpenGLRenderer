#include <iostream>
#include <SDL/SDL.h>
#include <GL/gl3w.h>
#include "Render.h"



int main(int argc, char** argv)
{
    bool quit = false;
    int width = 640;
    int height = 480;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) std::cout << "Failed SDL Init";
    auto window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    if (SDL_GL_CreateContext(window) == NULL) std::cout << "Failed: " << SDL_GetError();

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, SDL_TRUE);
    SDL_GL_SetSwapInterval(0);

    Render::init();
    auto result = Render::testPopulate();
    if (result) std::cout << "ERROR CODE: " << result << "\n";

    int wWidth = 0, wHeight = 0;
    
    SDL_GetWindowSize(window, &wWidth, &wHeight);
    glViewport(0, 0, wWidth, wHeight);
    

    do
    {
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        SDL_PumpEvents();

        SDL_Event event;
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        if (w != wWidth || h != wHeight)
        {
            glViewport(0, 0, w, h);
            wWidth = w;
            wHeight = h;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        Render::testRender();
        SDL_GL_SwapWindow(window);
    } while (!quit);
    return 0;
}