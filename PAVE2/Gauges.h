#include <SDL.h>

class Gauges{
private:
	SDL_Texture *wheel;
public:
	float wheelAngle;
	float gas;
	float brake;

	void draw(SDL_Renderer *renderer);
};