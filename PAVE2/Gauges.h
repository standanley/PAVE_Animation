#include <SDL.h>

class Gauges{
public:
	float wheelAngle = 20;
	float gas = 200;
	float brake = 50;
	float speed = 0;

	bool init(SDL_Renderer *renderer); //TODO: move this to the constructor
	void draw(SDL_Renderer *renderer);
};