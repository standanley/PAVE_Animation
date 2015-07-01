/* Holds data about the road drawing, including pretty much everything coming out of the NN*/
#include <SDL.h>
#include "Road.h"
#include "Prediction.h"
#include <iostream>
#include <math.h>
using namespace std;

#define debug(var) cout << #var << " = " << var << "\n"

const float Road::PIXELS_PER_METER = 20;

const float inLaneThreshhold = 20;
const float changeLaneThreshhold = 10;
const float farLaneThreshhold = 1;

int carLength;
int carWidth;

int w;
int h;
int basex;
int basey;

bool inLane;
bool changeLane;

bool laneToLeft;
bool laneToRight;

SDL_Rect dash;
SDL_Rect line;

// A one pixel texture used for the color of the main car (for now)
SDL_Texture *carImage;
SDL_Texture *inLaneImage;
SDL_Rect inLaneDest;
SDL_Texture *checkX;
SDL_Rect cookieCutter;
SDL_Rect ilDest;
SDL_Rect omDest;

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
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(color, 1, 1, 32, 4, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff); // may not be portable?
	carImage = SDL_CreateTextureFromSurface(renderer, surface);
	free(color);
	SDL_FreeSurface(surface);


	SDL_Surface *temp = SDL_LoadBMP("inLane.bmp");
	if (temp == NULL){
		cout << "Could not load In Lane vs On Marking image\n";
		SDL_FreeSurface(temp);
	}
	else{
		inLaneImage = SDL_CreateTextureFromSurface(renderer, temp);
		SDL_FreeSurface(temp);
	}
	inLaneDest = { w / 2 - 288 / 2, 10, 288, 32 };

	temp = SDL_LoadBMP("checkX.bmp");
	SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 255, 255, 255)); // tells it to use white as the transparent color
	if (temp == NULL){
		cout << "Could not load Check and X image\n";
		SDL_FreeSurface(temp);
	}
	else{
		checkX = SDL_CreateTextureFromSurface(renderer, temp);
		SDL_FreeSurface(temp);
	}
	cookieCutter = {0, 0, 230, 230};
	ilDest = { w / 2 - 288 / 2 +94, 14, 24, 24};
	omDest = { w / 2 - 288 / 2 + 252, 14, 24, 24 };
}

void Road::updateRect(){
	if (inLane){
		carL.y = basey - dist_LL*PIXELS_PER_METER;
		carM.y = basey - dist_MM*PIXELS_PER_METER;
		carR.y = basey - dist_RR*PIXELS_PER_METER; 
		carL.x = basex - carL.w / 2 + (toMarking_LL + toMarking_ML) / 2.0*PIXELS_PER_METER;
		carM.x = basex - carM.w / 2 + (toMarking_ML + toMarking_MR) / 2.0*PIXELS_PER_METER;
		carR.x = basex - carR.w / 2 + (toMarking_MR + toMarking_RR) / 2.0*PIXELS_PER_METER;
	}
	else{
		carL.y = basey - dist_L*PIXELS_PER_METER;
		carR.y = basey - dist_R*PIXELS_PER_METER;
		carL.x = basex - carL.w / 2 + (toMarking_L + toMarking_M) / 2.0*PIXELS_PER_METER;
		carR.x = basex - carR.w / 2 + (toMarking_M + toMarking_R) / 2.0*PIXELS_PER_METER;
	}
}

void Road::tick(){
	//decide which scenarios are active
	float inlanescore = toMarking_LL*toMarking_LL +
		toMarking_ML*toMarking_ML +
		toMarking_MR*toMarking_MR +
		toMarking_RR*toMarking_RR +
		dist_LL*dist_LL +
		dist_MM*dist_MM +
		dist_RR*dist_RR;
	float changelanescore = toMarking_L*toMarking_L +
		toMarking_M*toMarking_M+
		toMarking_R * toMarking_R +
		dist_L*dist_L +
		dist_R*dist_R; 

	(inlanescore > inLaneThreshhold) ? inLane = true : inLane = false;
	(changelanescore > changeLaneThreshhold) ? changeLane = true : changeLane = false;

	if (inLane == false && changeLane == false){
		// Not supposed to happen
		cout << "In Lane and On Marking systems both inactive\n";
		if (inlanescore / inLaneThreshhold > changelanescore / changeLaneThreshhold){
			cout << "Guessing In Lane\n";
			inLane = true;
		}
		else{
			cout << "Guessing On Marking\n";
			changeLane = true;
		}

	}

	if (inLane){
		(toMarking_LL < -1 * farLaneThreshhold) ? laneToLeft = true : laneToLeft = false;
		(toMarking_RR > farLaneThreshhold) ? laneToRight = true : laneToRight = false;
	}
	
	updateRect();
	dashOffset = fmodf(Road::dashOffset + dashSpeed*.1 + dash.h * 2, dash.h * 2) - dash.h * 2;
}

void Road::drawDashes(SDL_Renderer *renderer, int x){
	int y = dashOffset;
	dash.x = x - dash.w / 2;
	while (y < h){
		dash.y = y;
		y += dash.h * 2;
		SDL_RenderFillRect(renderer, &dash);
	}
}

void Road::drawAllLines(SDL_Renderer *renderer){
	if (inLane){
		if (laneToLeft){
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
			line.x = basex + toMarking_LL * PIXELS_PER_METER - line.w / 2;
			SDL_RenderFillRect(renderer, &line);
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			drawDashes(renderer, basex + toMarking_ML* PIXELS_PER_METER);
		}
		else{
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
			line.x = basex + toMarking_ML * PIXELS_PER_METER - line.w / 2;
			SDL_RenderFillRect(renderer, &line);
		}

		if (laneToRight){
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			drawDashes(renderer, basex + toMarking_MR* PIXELS_PER_METER);
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
			line.x = basex + toMarking_RR * PIXELS_PER_METER - line.w / 2;
			SDL_RenderFillRect(renderer, &line);
		}
		else{
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
			line.x = basex + toMarking_MR * PIXELS_PER_METER - line.w / 2;
			SDL_RenderFillRect(renderer, &line);
		}
	}
	else{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		drawDashes(renderer, basex + toMarking_L* PIXELS_PER_METER);
		drawDashes(renderer, basex + toMarking_M* PIXELS_PER_METER);
		drawDashes(renderer, basex + toMarking_R* PIXELS_PER_METER);
	}
}

void Road::draw(SDL_Renderer *renderer){
	// black road
	SDL_Rect testRect = { 0, 0, w, h };
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &testRect);

	drawAllLines(renderer);

	if (inLane){
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &carL);
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		SDL_RenderFillRect(renderer, &carM);
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderFillRect(renderer, &carR);
	}
	else{
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &carL);
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderFillRect(renderer, &carR);
	}

	Prediction::drawTopDown(renderer, this);
	
	// main car
	SDL_Rect tempRect = { 0, 0, carWidth, carLength };
	SDL_RenderCopyEx(renderer, carImage, &tempRect, &carMain, angle, NULL, SDL_FLIP_NONE);


	changeLane = false; // not sure why this is here
	SDL_RenderCopy(renderer, inLaneImage, NULL, &inLaneDest);
	(inLane) ? cookieCutter.x = 0 : cookieCutter.x = 282;
	SDL_RenderCopy(renderer, checkX, &cookieCutter, &ilDest);
	(changeLane) ? cookieCutter.x = 0 : cookieCutter.x = 282;
	SDL_RenderCopy(renderer, checkX, &cookieCutter, &omDest);

}
