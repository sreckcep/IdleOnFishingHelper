#include <stdio.h>
#include <time.h>
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_syswm.h"

/**
 * @brief makes a window transparent with a specified color
 * 
 * @param window pointer to the window
 * @param colorKey color to make transparent
 * @return BOOL 
 */
BOOL MakeWindowTransparent(SDL_Window* window, COLORREF colorKey) {
    // Get window handle (https://stackoverflow.com/a/24118145/3357935)
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);  // Initialize wmInfo
    SDL_GetWindowWMInfo(window, &wmInfo);
    HWND hWnd = wmInfo.info.win.window;

    // Change window type to layered (https://stackoverflow.com/a/3970218/3357935)
    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

    // Set transparency color
    return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
}

int main(int argc, char* argv[])
{
    //start SDL
    if(0 != SDL_Init(SDL_INIT_VIDEO)){
        fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError());
        goto Quit;
    }
    //create main window
    SDL_Window *window = SDL_CreateWindow("IdleOnHelper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 100, SDL_WINDOW_SHOWN|SDL_WINDOW_ALWAYS_ON_TOP);
    if(window == NULL){
        fprintf(stderr, "Erreur de creation de la fenetre : %s\n", SDL_GetError());
        goto Quit;
    }

    MakeWindowTransparent(window, RGB(255,255,255));

    //create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == NULL){
        fprintf(stderr, "Erreur de creation du renderer : %s\n", SDL_GetError());
        goto Quit;
    }

    //import the image on a temporary surface
    SDL_Surface *surface= SDL_LoadBMP("img/fishingHelper.bmp");
    if(surface == NULL){
        fprintf(stderr, "Erreur SDL_CreateTextureFromSurface : %s", SDL_GetError());
        goto Quit;
    }    
    
    //create the texute from the surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if(texture == NULL){
        fprintf(stderr, "Erreur SDL_CreateTextureFromSurface : %s", SDL_GetError());
        goto Quit;
    }
    //we don't need this anymore
    SDL_FreeSurface(surface);
    
    //left click recording 
    int mouseDown = 0;
    clock_t start;
    clock_t end ;
    clock_t real_time;
    
    //window movemement
    SDL_Rect displayBounds;
    SDL_GetDisplayUsableBounds(0, &displayBounds);
    int windowSizeX, windowSizeY;
    int window_x, window_y;
    SDL_GetWindowSize(window, &windowSizeX, &windowSizeY);
    int leftLimit = displayBounds.x;
    int rightLimit = displayBounds.x + displayBounds.w - windowSizeX;
    int upLimit = displayBounds.y;
    int downLimit = displayBounds.y + displayBounds.h - windowSizeY; 
    
    
    SDL_Event e;

    //main loop
    while (1) {
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

        //record the length of mouse clicks
        if (SDL_GetGlobalMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){
            if (!mouseDown){
                mouseDown = 1;
                start = clock();
            }
        }
        else if (mouseDown){
            end = clock() - start;
            real_time = end * 1000 / CLOCKS_PER_SEC;
            mouseDown = 0;
            printf("left click detected: time = %ld ms\n", real_time);
        }

        //applying movement from user input
        const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
        SDL_GetWindowPosition(window, &window_x, &window_y);
        /* first idea
        if (keyboardState[SDL_SCANCODE_LEFT] && window_x > leftLimit ) SDL_SetWindowPosition(window, window_x-1 , window_y );
        SDL_GetWindowPosition(window, &window_x, &window_y);
        if (keyboardState[SDL_SCANCODE_UP] && window_y > upLimit ) SDL_SetWindowPosition(window, window_x , window_y-1 );
        SDL_GetWindowPosition(window, &window_x, &window_y);
        if (keyboardState[SDL_SCANCODE_RIGHT] && window_x < rightLimit) SDL_SetWindowPosition(window, window_x+1 , window_y );
        SDL_GetWindowPosition(window, &window_x, &window_y);
        if (keyboardState[SDL_SCANCODE_DOWN] && window_y < downLimit) SDL_SetWindowPosition(window, window_x , window_y+1 );
        */
        //a more optimized way to do it; keep the old code in case of bugs I haven't thought about
        SDL_SetWindowPosition(
            window,
            (window_x - (keyboardState[SDL_SCANCODE_LEFT] && window_x > leftLimit) + (keyboardState[SDL_SCANCODE_RIGHT] && window_x < rightLimit)),
            (window_y - (keyboardState[SDL_SCANCODE_UP] && window_y > upLimit) + (keyboardState[SDL_SCANCODE_DOWN] && window_y < downLimit))
        );

        // ~60 hz so it doesn't run like crysis but keeps click recording accurate
        SDL_Delay(16);
	}

    
    
Quit:
    if (texture != NULL) SDL_DestroyTexture(texture);
    if (renderer != NULL) SDL_DestroyRenderer(renderer);
    if (window != NULL) SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;

}