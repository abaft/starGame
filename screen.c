#define _USE_MATH_DEFINES
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include "starHandler.h"
#include "geom.h"

#define WINDOW_WIDTH (2000)
#define WINDOW_HEIGHT (2000)
#define NUM_STARS 1
#define NUM_PLANETS 1000

// speed in pixels/second
#define SPEED (300)

SDL_Rect* positionStars(star* stars, int n)
{
  SDL_Rect* dest = malloc(sizeof(SDL_Rect) * n);
  for (int i = 0; i < n; ++i){
    float x_pos = stars[i].x * (WINDOW_WIDTH/100);
    float y_pos = stars[i].y * (WINDOW_HEIGHT/100);
    dest[i].x = (int) x_pos;
    dest[i].y = (int) y_pos; 
    dest[i].w = WINDOW_WIDTH/100;
    dest[i].h = WINDOW_WIDTH/100;
  }
  return dest;
}
point starToPoint(star s)
{
  point rtn = 
  {
    s.x,
    s.y
  };
  return rtn;
}
int removePlanetCollision(star** ptrPlanets, int n, vline vecline)
{
  cline carline = vlineToCline(vecline);
  cline norm = normal(carline, vectToPoint(vecline.start));

  star* newArray = malloc(sizeof(star) * n);
  int c = 0;
  for (int i = 0; i < n; ++i)
  {
    //if (isSide(norm, vecline.direction, starToPoint((*ptrPlanets)[i]))){
      if (distToCline (carline, starToPoint((*ptrPlanets)[i])) > 4)
        newArray[c] = (*ptrPlanets)[i], ++c;
      else
        printf("%lf\n", distToCline (carline, starToPoint((*ptrPlanets)[i])));
    //}
    //else
    //  newArray[c] = (*ptrPlanets)[i], ++c;

  }
  free(*ptrPlanets);
  *ptrPlanets = newArray;
  return c;
}

int main(void)
{
  srand(time(NULL));
  star* stars = star_INIT(NUM_STARS);
  star* planets = star_PINIT(NUM_PLANETS, stars, NUM_STARS);

  // attempt to initialize graphics and timer system
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
  {
    printf("error initializing SDL: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* win = SDL_CreateWindow("Star Game",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH, WINDOW_HEIGHT,0);
  if (!win)
  {
    printf("error creating window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // create a renderer, which sets up the graphics hardware
  Uint32 render_flags = SDL_RENDERER_ACCELERATED;
  SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
  if (!rend)
  {
    printf("error creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  // load the image into memory using SDL_image library function
  SDL_Surface* surface = IMG_Load("hello.png");
  if (!surface)
  {
    printf("error creating surface\n");
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  // load the image data into the graphics hardware's memory
  SDL_Texture* startex = SDL_CreateTextureFromSurface(rend, surface);
  SDL_FreeSurface(surface);

  if (!startex)
  {
    printf("error creating texture: %s\n", SDL_GetError());
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }
  
  surface = IMG_Load("Planet.png");
  if (!surface)
  {
    printf("error creating surface\n");
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  // load the image data into the graphics hardware's memory
  SDL_Texture* plantex = SDL_CreateTextureFromSurface(rend, surface);
  SDL_FreeSurface(surface);

  if (!plantex)
  {
    printf("error creating texture: %s\n", SDL_GetError());
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  SDL_Rect* dest = positionStars(stars, NUM_STARS);

  // set to 1 when window close button is pressed
  int close_requested = 0;
  // animation loop
  int numStars = NUM_STARS;
  int numPlanets = NUM_PLANETS;

  SDL_Rect* destPlanets = positionStars(planets, NUM_PLANETS);

  while (!close_requested)
  {
    // process events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        close_requested = 1;
      }
    }

    // clear the window
    SDL_SetRenderDrawColor(rend, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(rend);

    // draw the image to the window
    for (int i = 0; i < numStars; ++i)
      SDL_RenderCopy(rend, startex, NULL, &dest[i]);
    for (int i = 0; i < numPlanets; ++i)
      SDL_RenderCopy(rend, plantex, NULL, &destPlanets[i]);

    free(destPlanets);
    free(dest);

    star* deadStars;
    int tmp = numStars;
    numStars = star_cycleLife(&stars, numStars, &deadStars);
    int deadStarCount = tmp - numStars;
    
    SDL_SetRenderDrawColor(rend , 255, 255, 255, SDL_ALPHA_OPAQUE);
    for (int i = 0; i < deadStarCount; ++i)
    {
      point loc = 
      {
        deadStars[i].x,
        deadStars[i].y
      };

      
      vline line = polarVline(loc, fmod(rand(), 2 * PI));
      loc = vlineMove(line, 2000);
      
      numPlanets = removePlanetCollision(&planets, numPlanets, line);
      SDL_RenderDrawLine(rend, 
          loc.x * WINDOW_WIDTH/100,
          loc.y * WINDOW_WIDTH/100,
          deadStars[i].x * WINDOW_WIDTH/100,
          deadStars[i].y * WINDOW_WIDTH/100);
    }

    //if (numStars == 0)
    //  close_requested = 1;
    dest = positionStars(stars, numStars);
    destPlanets = positionStars(planets, numPlanets);
    free(deadStars);

    SDL_RenderPresent(rend);
    // wait 1/60th of a second
    SDL_Delay(1000/60);
  }

  // clean up resources before exiting
  SDL_DestroyTexture(startex);
  SDL_DestroyTexture(plantex);
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();
}
