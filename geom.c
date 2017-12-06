#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "geom.h"

vline polarVline(point p, double theta)
{
  vline rtn;
  vect start = {.i = p.x, .j = p.y};
  vect direct = {.i = sin(theta), .j = cos(theta)};
  rtn.start = start;
  rtn.direction = direct;
  return rtn;
}

point vlineMove(vline l, double t)
{
  point rtn;
  rtn.x = l.start.i + (l.direction.i * t);
  rtn.y = l.start.j + (l.direction.j * t);
  return rtn;
}

double clineCompute(cline line, point p)
{
  return (line.a * p.x) + (line.b * p.y) + line.c;
}

cline vlineToCline(vline vln)
{
  cline rtn;
  rtn.a = vln.direction.j;
  rtn.b = -vln.direction.i;
  rtn.c = (vln.direction.i * vln.start.j) - (vln.direction.j * vln.start.i);
  return rtn;
}

cline normal(cline l, point p)
{
  cline rtn;
  rtn.a = -l.b;
  rtn.b = l.a;
  rtn.c = (l.b * p.x) + (l.a * p.y);
  return rtn;
}

_Bool isSide(cline l, vect n, point p)
{
  vline veceqn = { .start.i = 0, .start.j = -(l.c/l.b), .direction = n};
  point Correct = vlineMove( veceqn, 1);
  double compute = clineCompute(l, p);
  double c = clineCompute(l, Correct);
  if (compute == 0)
    return 0;
  if (c < 0)
  {
    if (compute < 0)
      return 1;
    else
      return 0;
  }
  else //(c > 0)
  {
    if (compute > 0)
      return 1;
    else
      return 0;
  }
}

double distBetweenPoints (point p1, point p2)
{
  return sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
}

double distToCline (cline line, point p) //computationally expencive
{
  return fabs((line.a * p.x) + (line.b * p.y) + line.c)/
        (sqrt(pow(line.a,2) + pow(line.b,2)));
}

point vectToPoint(vect v)
{
  point rtn = 
  {
    v.i,
    v.j
  };
  return rtn ;
}
