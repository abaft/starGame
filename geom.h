//Header File
typedef struct _vect
{
  //form r = i + j
  double i,j;
}vect;

typedef struct _point
{
  //point on cartesian plane
  double x, y;
}point;

typedef struct _cline
{
  //Form ax + by + c = 0
  double a, b, c;
}cline;

typedef struct _vline
{
  //in the form r = start + t* direction
  vect start, direction;
}vline;
#define PI 3.1415926535897


vline polarVline(point p, double theta);

point vlineMove(vline l, double t);

double clineCompute(cline line, point p);

cline vlineToCline(vline vln);

cline normal(cline l, point p);

_Bool isSide(cline l, vect n, point p);

double distBetweenPoints (point p1, point p2);

double distToCline (cline line, point p);

point vectToPoint(vect v);
