#include <SDL.h>

class Gauges{
private:
	SDL_Texture *wheel;
public:
	float wheelAngle = 0;
	float gas = 100;
	float brake = 30;

	bool init(SDL_Renderer *renderer); //TODO: move this to the constructor
	void draw(SDL_Renderer *renderer);
};