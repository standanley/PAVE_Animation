#include "Text.h"
#include <iostream>
using namespace std;

#define debug(var) cout << #var << " = " << var << "\n"

SDL_Texture* Text_sprites;
SDL_Rect Text_cookieCutter;
SDL_Rect Text_destination;

bool Text_init(SDL_Renderer *renderer){
	Text_cookieCutter = { 0, 0, 24, 32 };
	Text_destination = { 0, 0, 24, 32 };

	// use sprites4 if you are drawing on a colored background, it doesn't have smoothed edges
	// if you need colored text use sprites5, set black as transparent, and use color modulation
	SDL_Surface *temp = SDL_LoadBMP("sprites2.bmp"); 
	SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 255, 255, 255)); // tells it to use white as the transparent color
	if (temp == NULL){
		cout << "Could not load text sprites image\n";
		SDL_FreeSurface(temp);
		return false;
	}
	else{
		Text_sprites = SDL_CreateTextureFromSurface(renderer, temp);
		SDL_FreeSurface(temp);
		return true;
	}
}


bool Text_renderText(SDL_Renderer *renderer, char *text, int x, int y){
	Text_destination.x = x;
	Text_destination.y = y;
	bool success = true;
	char* num = text;
	while (*num != NULL){
		// look at ascii code : 0-9 are 48-57, . is 46, - is 45, : is 58
		int ascii = (int)(*num);
		int position = 0;
		if (ascii >= 48 && ascii<=57){
			position = ascii - 48;
		}else if (ascii == 46){
			position = 10;
		}
		else if (ascii == 45){
			position = 11;
		}
		else if (ascii == 58){
			position = 12;
		}
		else{
			//uh oh, sprite sheet doen't have that character
			success = false;
			position = 0;
		}

		Text_cookieCutter.x = position * 24;
		SDL_RenderCopy(renderer, Text_sprites, &Text_cookieCutter, &Text_destination);
		Text_destination.x += 24;
		num++;
	}
	return success;
}