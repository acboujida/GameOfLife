#include <iostream>
#include <SDL.h>

using std::cout;
using std::endl;

const int T = 20;

int GRID[T][T];
int TEMP_GRID[T][T];

void display()
{
    for (int i = 0; i < T; i++)
    {
        for (int j = 0; j < T; j++)
        {
            cout << GRID[i][j] << ' ';
        }
        cout << endl;
    }
}

void init()
{
    for (int i = 0; i < T; i++)
    {
        for (int j = 0; j < T; j++)
        {
            GRID[i][j] = 0;
        }
    }
}

int living_neighbours(int x, int y)
{
    int living = 0;

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
                continue;

            int nx = (x + i + T) % T;
            int ny = (y + j + T) % T;
            living += GRID[nx][ny];
        }
    }

    return living;
}

bool change(int i, int j)
{
    int living = living_neighbours(i, j);

    if (GRID[i][j] == 0 && living == 3)
    {
        return true; // Cell becomes alive
    }
    else if (GRID[i][j] == 1 && (living < 2 || living > 3))
    {
        return true; // Cell dies
    }

    return false; // No change
}

void copy_values(int src[T][T], int dest[T][T])
{
    for (int i = 0; i < T; i++)
    {
        for (int j = 0; j < T; j++)
        {
            dest[i][j] = src[i][j];
        }
    }
}

void evolve()
{
    copy_values(GRID, TEMP_GRID);

    for (int i = 0; i < T; i++)
    {
        for (int j = 0; j < T; j++)
        {
            if (change(i, j))
            {
                GRID[i][j] = 1 - GRID[i][j];
            }
        }
    }
}

void ErrorExit(const char* message)
{
    SDL_Log("ERROR: %s - %s\n", message, SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}

void SDL_display(SDL_Renderer* render)
{
    SDL_Rect rectangle = { 0, 0, 10, 10 };

    for (int i = 0; i < T; i++)
    {
        for (int j = 0; j < T; j++)
        {
            rectangle.x = j * 10;
            rectangle.y = i * 10;

            if (GRID[i][j])
            {
                if (SDL_SetRenderDrawColor(render, 255, 255, 255, 255) != 0)
                    ErrorExit("Failed to set draw color");
            }
            else
            {
                if (SDL_SetRenderDrawColor(render, 0, 0, 0, 255) != 0)
                    ErrorExit("Failed to set draw color");
            }

            SDL_RenderFillRect(render, &rectangle);
        }
    }

    SDL_RenderPresent(render);
}

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        ErrorExit("SDL Initialization failed");

    SDL_Window* window = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, T * 10, T * 10, SDL_WINDOW_OPENGL);
    if (!window)
        ErrorExit("Window creation failed");

    SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!render)
        ErrorExit("Renderer creation failed");

    init();
    //display();
    SDL_display(render); // Ensure initial render

    bool running = true;
    bool pause = true; // Start paused

    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_SPACE:
                    pause = !pause; // Toggle pause
                    break;

                case SDLK_r:
                    init(); // Reset the grid
                    SDL_display(render);
                    break;

                case SDLK_ESCAPE:
                    running = false; // Exit the game
                    break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int x = event.button.x / 10;
                    int y = event.button.y / 10;

                    if (x < T && y < T)
                    {
                        GRID[y][x] = 1 - GRID[y][x];
                    }

                    SDL_display(render);
                }
                break;

            default:
                break;
            }
        }

        if (!pause)
        {
            evolve();
            SDL_display(render);
        }

        SDL_Delay(16); // Cap to ~60 FPS
    }

    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
