// ifndef todo

/* This class represents the top-down drawing of lanes and cars. Most of the coordinates are
   based off the bottom, where the main car is, because the height depends on the window size.
   Specifically, Road.cpp has constants for the x/y offset of the bottom center of everything*/

class Road{

	SDL_Rect carMain;
	SDL_Rect carL;
	SDL_Rect carM;
	SDL_Rect carR;
	float dashOffset;

	void updateRect();
	void drawDashes(SDL_Renderer *renderer, int x);
	void Road::drawAllLines(SDL_Renderer *renderer);

public:
	float angle = 10;

	float toMarking_LL = -6;
	float toMarking_ML = -3;
	float toMarking_MR = 2;
	float toMarking_RR = 6.5;
	float dist_LL = 5;
	float dist_MM = 10;
	float dist_RR = 20;

	float toMarking_L = -2.5;
	float toMarking_M = .3;
	float toMarking_R = 3.8;
	float dist_L = 10;
	float dist_R = 15;

	float dashSpeed;

	// w and h are the width and height of the viewport used
	Road(SDL_Renderer *renderer, int w, int h);

	float carPosition = 0; // just for testing

	void tick();

	void draw(SDL_Renderer *renderer);

};