#include <SDL.h>
#include "Road.h"

struct Point{
	float x;
	float y;
};

class Prediction{
private:
	static int numPoints; // should be 2*num from updateSurface
	static Point* topDown;
	static Sint16 *xpoints;
	static Sint16 *ypoints;
	static Sint16 *xpoints2;
	static Sint16 *ypoints2;
	static void createPoints(Point *input);
	static Point* transformPoints(Point* input);
	static void updateLists(Point* input);
	static void updateLists2(Point* input, Road *road);
public:
	static float topDownScale;
	 // positions: the given predictions for the car's trajectory
	// num: the number of positions given as predictions
	static void updateSurface(Point *positions, int num);
	static void drawPerspective(SDL_Renderer *Renderer);
	static void drawTopDown(SDL_Renderer *Renderer, Road *road);

};