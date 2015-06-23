/* Holds data about the road drawing, including pretty much everything coming out of the NN*/
#include <SDL.h>
#include "Road.h"
#include <iostream>
#include <math.h>

#define debug(var) cout << #var << " = " << var << "\n"

using namespace std;
const float PIXELS_PER_METER = 20;
int carLength;
int carWidth;

int w;
int h;
int basex;
int basey;

SDL_Rect dash;
SDL_Rect line;

// A one pixel texture used for the color of the main car (for now)
SDL_Texture *carImage;

Road::Road(SDL_Renderer *renderer, int w1, int h1){
	w = w1;
	h = h1;

	dashOffset = 0;

	carLength = int(3.5 * PIXELS_PER_METER);
	carWidth = int(2.0 * PIXELS_PER_METER);
	dash = { 0, 0, int(.3 * PIXELS_PER_METER), int(3 * PIXELS_PER_METER) };
	line = { 0, 0, int(.3 * PIXELS_PER_METER), h };
	basex = w / 2; // base is centered left/right and at the nose of the main car
	basey = h - 2 * carLength;


	carMain = { basex - carWidth / 2, basey, carWidth, carLength };
	carL = { basex - 3.7*PIXELS_PER_METER - carWidth / 2, basey, carWidth, carLength };
	carM = { basex - carWidth / 2, basey, carWidth, carLength };
	carR = { basex + 3.7*PIXELS_PER_METER - carWidth / 2, basey, carWidth, carLength };

	// Initialize a surface to be used for tilted rectangles, just one pixel big
	// if you need to change the color a lot, make it white and use color modulation?
	void *color = new int(0x00ff00ff);
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(color, 1, 1, 32, 4, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	carImage = SDL_CreateTextureFromSurface(renderer, surface);
	free(color);
	SDL_FreeSurface(surface);
}

void Road::updateRect(){
	carL.y = basey - dist_LL*PIXELS_PER_METER;
	carM.y = basey - dist_MM*PIXELS_PER_METER;
	carR.y = basey - dist_RR*PIXELS_PER_METER;
}

void Road::tick(){
	float DASH_SPEED = 5.0;
	Road::updateRect();
	Road::dashOffset = fmodf(Road::dashOffset + DASH_SPEED + dash.h * 2, dash.h * 2) - dash.h * 2;
}

void Road::drawDashes(SDL_Renderer *renderer, int x){
	int y = dashOffset;
	dash.x = x - dash.w/2;
	while (y < h){
		dash.y = y;
		y += dash.h * 2;
		SDL_RenderFillRect(renderer, &dash);
	}
}

void Road::drawAllLines(SDL_Renderer *renderer){
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	line.x = basex - toMarking_LL * PIXELS_PER_METER - line.w / 2;
	SDL_RenderFillRect(renderer, &line);
	line.x = basex + toMarking_RR * PIXELS_PER_METER - line.w / 2;
	SDL_RenderFillRect(renderer, &line);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	drawDashes(renderer, basex - toMarking_ML* PIXELS_PER_METER);
	drawDashes(renderer, basex + toMarking_MR* PIXELS_PER_METER);
}

void Road::draw(SDL_Renderer *renderer){

	//SDL_RenderClear(renderer);

	SDL_Rect testRect = { 0, 0, w, h };
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &testRect);

	SDL_Rect tempRect = { 0, 0, carWidth, carLength };
	SDL_RenderCopyEx(renderer, carImage, &tempRect, &carMain, angle, NULL, SDL_FLIP_NONE);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &carL);
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(renderer, &carM);
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
	SDL_RenderFillRect(renderer, &carR);


	drawAllLines(renderer);

}
