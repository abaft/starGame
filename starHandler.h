typedef struct _star
{
  int ID;
  double x;
  double y;
  int life; // units of time till Supernova
}star;

star* star_INIT(int number_Of_Stars);
int star_cycleLife(star** ptrStars, int n, star** deadStarArrayRtn);
star* star_PINIT(int n, star* stars, int numStars);
