#include "Gauges.h"
#include "Text.h"
#include <iostream>
using namespace std;

#define debug(var) cout << #var << " = " << var << "\n"


SDL_Texture *wheel;
SDL_Texture *speedometer;
SDL_Texture *needle;
SDL_Texture *speedText;

bool Gauges::init(SDL_Renderer *renderer){
	bool success = true;
	SDL_Surface *temp = SDL_LoadBMP("steeringWheel2.bmp");
	SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0, 255, 0)); // tells it to use green as the transparent color
	if (temp == NULL){
		cout << "Could not load steering wheel image\n";
		SDL_FreeSurface(temp);
		success = false;
	}
	else{
		wheel = SDL_CreateTextureFromSurface(renderer, temp);
		SDL_FreeSurface(temp);
	}

	SDL_Surface *temp2 = SDL_LoadBMP("speedometer2.bmp");
	//SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp2->format, 0, 255, 0));
	if (temp2 == NULL){
		cout << "Could not load speedometer image\n";
		SDL_FreeSurface(temp2);
		success = false;
	}
	else{
		speedometer = SDL_CreateTextureFromSurface(renderer, temp2);
		SDL_FreeSurface(temp2);
	}

	SDL_Surface *temp3 = SDL_LoadBMP("needle2.bmp");
	SDL_SetColorKey(temp3, SDL_TRUE, SDL_MapRGB(temp3->format, 255, 255, 255)); // tells it to use white as the transparent color
	if (temp3 == NULL){
		cout << "Could not load needle image\n";
		SDL_FreeSurface(temp3);
		success = false;
	}
	else{
		needle = SDL_CreateTextureFromSurface(renderer, temp3);
		SDL_FreeSurface(temp3);
	}

	SDL_Surface *temp4 = SDL_LoadBMP("speed.bmp");
	SDL_SetColorKey(temp4, SDL_TRUE, SDL_MapRGB(temp4->format, 255, 255, 255)); // tells it to use white as the transparent color
	if (temp4 == NULL){
		cout << "Could not load speed text image\n";
		SDL_FreeSurface(temp4);
		success = false;
	}
	else{
		speedText = SDL_CreateTextureFromSurface(renderer, temp4);
		SDL_FreeSurface(temp4);
	}

	return success;
}

void Gauges::draw(SDL_Renderer *renderer){
	// Steering wheel
	SDL_Rect center = { 139, 3, 2, 30 };
	SDL_SetRenderDrawColor(renderer, 63, 255, 0, 255);
	SDL_RenderFillRect(renderer, &center); // center marking for wheel
	SDL_Rect destination = { 0, 0, 280, 280 };
	SDL_RenderCopyEx(renderer, wheel, NULL, &destination, wheelAngle, NULL, SDL_FLIP_NONE);

	// Border for gas and brake
	//Todo: move these rectangles out of the loop?
	const int length = 260;
	const int width = 40;
	const int border = 5;
	SDL_Rect outer = { 300, 10, width, length };
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &outer);
	outer.x = 360;
	SDL_RenderFillRect(renderer, &outer);
	SDL_Rect inner = { 305, 15, width - 2 * border, length - 2 * border };
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &inner);
	inner.x = 365;
	SDL_RenderFillRect(renderer, &inner);
	// gas and brake rectangles
	SDL_Rect gasRect = { 305, 10 + border + (length - 2 * border - gas), width - 2 * border, gas };
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderFillRect(renderer, &gasRect);
	SDL_Rect brakeRect = { 365, 10+border + (length-2*border - brake), width - 2 * border, brake };
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &brakeRect);

	// speedometer & needle
	destination = { 10, 280, 260, 130 };
	SDL_RenderCopy(renderer, speedometer, NULL, &destination);
	destination = { 130, 282, 20, 130 };
	SDL_Point pivot = { 10, 125 };
	SDL_RenderCopyEx(renderer, needle, NULL, &destination, speed * 9 / 5 - 90, &pivot, SDL_FLIP_NONE);

	// digital speed
	destination = { 270, 310, 150, 50 }; // aspect ratio should be 3:1 if only taking the top half
	SDL_Rect clip = { 0, 0, 150, 50 }; // only taking the top half of the image
	SDL_RenderCopy(renderer, speedText, &clip, &destination);
	char text[5]; //enough for 9999\0, needs to be bigger if we use floats 
	sprintf_s(text, sizeof(text), "%02d", (int)speed);
	Text_renderText(renderer, text, 310, 360);
}
