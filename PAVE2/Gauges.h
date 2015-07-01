#ifndef GAUGES_DEFINED
#define GAUGES_DEFINED

#include <SDL.h>

class Gauges{
private:
	bool init(SDL_Renderer *renderer);

public:
	float wheelAngle = 20;
	float gas = 200;
	float brake = 50;
	float speed = 0;

	Gauges(SDL_Renderer *renderer);
	void draw(SDL_Renderer *renderer);
};
#endif