#include "starHandler.h"
#include <stdlib.h>
#include <time.h>

int compareX (const void* one, const void* two)
{
  const star* s1 = one;
  const star* s2 = two;
  return (s1->x - s2->x);
}

int compareY (const void* one, const void* two)
{
  const star* s1 = one;
  const star* s2 = two;
  return (s1->y - s2->y);
}

star* star_INIT(int n)
{
  srand(time(NULL));
  star* rtn = malloc(sizeof(star) * n);

  int xs[n]; //Gives us enough memory
  int ys[n];
  for (int j = 0; j < n; j++)
  {
    xs[j] = 100; //These numbers are not real coodinates
    ys[j] = 100;
  }

  int i = 0;
  int x, y;
  _Bool test = 1;
  while (i < n)
  {
    test = 1;
    x = rand()%100;
    y = rand()%100;
    for (int j = 0; j < n; j++)
    {
      if (x == xs[j] && y == ys[j])
      {
        test = 0;
      }
    }
    if (test)
    {
      rtn[i].x = x;
      rtn[i].y = y;
      rtn[i].life = rand()%1000 + 10;
      ys[i] = y;
      xs[i] = x;
      i++;
    }
  }
  //This will sort the stars by there x coods, used for future algurithm for
  //finding the closest
  qsort (rtn, n,sizeof(star), compareX);

  for (int i = 0; i < n; i++)
    rtn[i].ID = i;
  return rtn;
}

star* star_PINIT(int n, star* stars, int numStars)
{
  srand(time(NULL));
  star* rtn = malloc(sizeof(star) * n);

  int xs[n]; //Gives us enough memory
  int ys[n];
  for (int j = 0; j < n; j++)
  {
    xs[j] = 100; //These numbers are not real coodinates
    ys[j] = 100;
  }

  int i = 0;
  int x, y;
  _Bool test = 1;
  while (i < n)
  {
    test = 1;
    x = rand()%100;
    y = rand()%100;
    for (int j = 0; j < n; j++)
    {
      if (x == xs[j] && y == ys[j])
      {
        test = 0;
      }
    }
    for (int j = 0; j < numStars; ++j)
    {
      if (x == stars[j].x && y == stars[j].y)
      {
        test = 0;
      }
    }
    if (test)
    {
      rtn[i].x = x;
      rtn[i].y = y;
      rtn[i].life = rand()%1000 + 10;
      ys[i] = y;
      xs[i] = x;
      i++;
    }
  }
  //This will sort the stars by there x coods, used for future algurithm for
  //finding the closest
  qsort (rtn, n,sizeof(star), compareX);

  for (int i = 0; i < n; i++)
    rtn[i].ID = i;
  return rtn;
}

int star_cycleLife(star** ptrStars, int n,star** deadStarsArray)
{
  int deadStars = 0;
  for (int i = 0; i < n; ++i)
  {
    (*ptrStars)[i].life -= 1;
    if ((*ptrStars)[i].life < 0)
      deadStars++;
  }

  star* newArray = malloc(sizeof(star) * (n - deadStars));
  (*deadStarsArray) = malloc(sizeof(star) * deadStars);

  for (int i = 0, c = 0, d = 0; i < n; ++i)
    if ((*ptrStars)[i].life >= 0)
      newArray[c] = (*ptrStars)[i], ++c;
    else
      (*deadStarsArray)[d] = (*ptrStars)[i], ++d;

  free(*ptrStars);
  *ptrStars = newArray;
  return n - deadStars;
}
