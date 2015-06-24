#include <SDL.h>
#include <string>


bool Text_renderText(SDL_Renderer *renderer, char *text, int x, int y); // returns false if passed invalid character (0123456789.-:)
bool Text_init(SDL_Renderer *renderer);