#define _USE_MATH_DEFINES
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include "starHandler.h"
#include "geom.h"
#include "SDL_FontCache/SDL_FontCache.h"
//LEAVE THE WINDOW SIZE ALONE
#define WINDOW_WIDTH (2000)
#define WINDOW_HEIGHT (2000)

#define SCALE 0.5 //is 2000x2000
#define NUM_STARS 1000
#define NUM_PLANETS 100

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

int removePlanetCollision( star** ptrPlanets, 
    int n, vline vecline, 
    star** deadPlanets, 
    int* deadPlanetsCtr,
    int* winState)
{
  cline carline = vlineToCline(vecline);
  cline norm = normal(carline, vectToPoint(vecline.start));

  star* newArray = malloc(sizeof(star) * n);
  int c = 0;
  for (int i = 0; i < n; ++i)
  {
    if (isSide(norm, vecline.direction, starToPoint((*ptrPlanets)[i]))){
      if (distToCline (carline, starToPoint((*ptrPlanets)[i])) > 0.1)
        newArray[c] = (*ptrPlanets)[i], ++c;
      else
      {
        (*deadPlanets)[*deadPlanetsCtr] = (*ptrPlanets)[i], ++(*deadPlanetsCtr);
        if ((*ptrPlanets)[i].life == -24601)
          *winState = 0;
      }
    }
    else
      newArray[c] = (*ptrPlanets)[i], ++c;

  }
  free(*ptrPlanets);
  *ptrPlanets = newArray;
  return c;
}

star* findPlanetUnder (int x, int y, point* loc, star* planets, int n)
{
  loc->x = x;
  loc->y = y;

  for (int i = 0; i < n; ++i)
    if (starToPoint(planets[i]).x == loc->x 
        && starToPoint(planets[i]).y == loc->y)
      return planets + i;

  return NULL;
}

char string[100];

int main(int argc, char** argv)
{
  if (argc != 2)
    exit (1);
  if (atoi(argv[1]) < 1)
    exit (1);
  int numStars = atoi (argv[1]);
  srand(time(NULL));
  star* stars = star_INIT(numStars);
  star* planets = star_PINIT(NUM_PLANETS, stars, numStars);
  TTF_Init();

  FC_Font* font = FC_CreateFont();
  FC_Font* font2 = FC_CreateFont();

  // attempt to initialize graphics and timer system
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
  {
    printf("error initializing SDL: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* win = SDL_CreateWindow("Star Game",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH * SCALE, WINDOW_HEIGHT * SCALE,0);
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

  SDL_RenderSetScale(rend, (float) SCALE, (float) SCALE);
  FC_LoadFont(font2, rend, "Sans.ttf", 100, 
      FC_MakeColor(255,225,255,255), TTF_STYLE_BOLD);

  FC_LoadFont(font, rend, "Sans.ttf", 72, 
      FC_MakeColor(255,51,255,255), TTF_STYLE_NORMAL);

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

  surface = IMG_Load("deadPlanet.png");
  if (!surface)
  {
    printf("error creating surface\n");
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  // load the image data into the graphics hardware's memory
  SDL_Texture* dplantex = SDL_CreateTextureFromSurface(rend, surface);
  SDL_FreeSurface(surface);

  if (!dplantex)
  {
    printf("error creating texture: %s\n", SDL_GetError());
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  surface = IMG_Load("circle.png");
  if (!surface)
  {
    printf("error creating surface\n");
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  // load the image data into the graphics hardware's memory
  SDL_Texture* circletex = SDL_CreateTextureFromSurface(rend, surface);
  SDL_FreeSurface(surface);

  if (!dplantex)
  {
    printf("error creating texture: %s\n", SDL_GetError());
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  SDL_Rect* dest = positionStars(stars, numStars);
  SDL_Rect starSelectorDest;
  starSelectorDest.x = -100;
  starSelectorDest.y = -100; 
  starSelectorDest.w = 2 * (WINDOW_WIDTH/100);
  starSelectorDest.h = 2 * (WINDOW_WIDTH/100);

  // set to 1 when window close button is pressed
  int close_requested = 0;
  int selected = 0;
  // animation loop
  int numPlanets = NUM_PLANETS;
  int numDPlanets = 0;
  star* deadPlanets = malloc(sizeof(star) * numPlanets);

  SDL_Rect* destPlanets = positionStars(planets, NUM_PLANETS);
  SDL_Rect* destDPlanets = positionStars(deadPlanets, numDPlanets);

  while (!selected)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        close_requested = 1;
        selected = 1;
      }
      if (event.type == SDL_MOUSEBUTTONDOWN
          //&& event.button == SDL_BUTTON_LEFT
         )
      {
        point loc;
        star* planet;
        if((planet = findPlanetUnder(
                (event.motion.x/((float) WINDOW_WIDTH * SCALE)) * 100,
                (event.motion.y/((float) WINDOW_WIDTH * SCALE)) * 100,
                &loc, planets, numPlanets)) != NULL)
        {
          starSelectorDest.x = (loc.x-0.5) * (WINDOW_WIDTH/100);
          starSelectorDest.y = (loc.y-0.5) * (WINDOW_WIDTH/100);
          selected = 1;
          planet->life = -24601;
        }        
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
    for (int i = 0; i < numDPlanets; ++i)
      SDL_RenderCopy(rend, dplantex, NULL, &destDPlanets[i]);
    SDL_RenderCopy(rend, circletex, NULL, &starSelectorDest);

    SDL_RenderPresent(rend);
    SDL_Delay(1000/60);
  }
  int winstate = 1;
  clock_t start = clock();
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
    for (int i = 0; i < numDPlanets; ++i)
      SDL_RenderCopy(rend, dplantex, NULL, &destDPlanets[i]);
    if (winstate == 1)
      SDL_RenderCopy(rend, circletex, NULL, &starSelectorDest);

    free(destPlanets);
    free(destDPlanets);
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
      numPlanets = removePlanetCollision(&planets, 
          numPlanets, line, &deadPlanets, &numDPlanets, &winstate);
      loc = vlineMove(line, 2000);

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
    destDPlanets = positionStars(deadPlanets, numDPlanets);
    free(deadStars);
    //TEXT RENDERING
      clock_t diff = clock();
      diff -= start;
      long int time = (long int)( (diff * 1000)/CLOCKS_PER_SEC);

      if (winstate == 0 || winstate == -1)
      {
        sprintf(string, "%li Million Years", time);
        winstate += 3;
      }
      
    FC_Draw(font, rend, 0, 0, "%li Million Years", time);    

      if (winstate == 3)
        FC_DrawAlign(font2, rend,
           WINDOW_WIDTH/2, WINDOW_WIDTH/2, FC_ALIGN_CENTER,
           "Lost in %s", string);
      else if (numStars == 0)
      { 
        FC_DrawAlign(font2, rend,
           WINDOW_WIDTH/2, WINDOW_WIDTH/2, FC_ALIGN_CENTER,
           "Won, Planet Survived...\nHowever all else is dead" );
        winstate = -1;
      }
    

    SDL_RenderPresent(rend);
    // wait 1/60th of a second
    SDL_Delay((1000/60));
  }

  // clean up resources before exiting
  FC_FreeFont(font);
  SDL_DestroyTexture(startex);
  SDL_DestroyTexture(plantex);
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();
}
