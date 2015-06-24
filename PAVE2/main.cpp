#include <SDL.h>
#include <iostream>
#include <vector>
#include "Road.h"
#include "Text.h"
#include "Gauges.h"
#include <fstream>
#include <stdlib.h>

using namespace std;

bool checkEvents();
void getInput();
void tick();
bool InitEverything();
bool InitSDL();
bool CreateWindow();
bool CreateRenderer();
void SetupRenderer();

void render();
void RunGame();


// Window pos
int posX = 300;
int posY = 100;
int sizeX = 1280;
int sizeY = 720;

ifstream source;
int frameCount = 0;

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Texture *testImage;
int testImageCounter = 3;
SDL_Rect roadViewport;
SDL_Rect gaugesViewport;

Road *road;
Gauges *gauges;

int main(int argc, char* args[])
{
	if (!InitEverything())
		return -1;

	RunGame();
	return 0;
}

void RunGame()
{
	bool loop = true;

	while (loop)
	{
		// input, logic, render, wait
		loop = checkEvents();
		getInput();
		tick();
		render();
		SDL_Delay(16);
		frameCount++;
	}
}
bool checkEvents(){
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			return false;
	}
	return true;
}

void getInput(){
	//Todo

	// simulate choppy stream
	if (rand() % 10 >= 3){
		//return;
	}

	if (frameCount % 6 != 0){
		return;
	}

	if (!source.eof()){
		source >> road->angle;
		source >> road->toMarking_LL;
		source >> road->toMarking_ML;
		source >> road->toMarking_MR;
		source >> road->toMarking_RR;
		source >> road->dist_LL;
		source >> road->dist_MM;
		source >> road->dist_RR;
	}

	if (testImage != NULL){
		SDL_DestroyTexture(testImage);
	}

	if (testImageCounter <= 33){
		// i think char[45] is enough but i'm playing it safe
		char filename[50];
		sprintf_s(filename, sizeof(filename), "C:\\Users\\Daniel\\Desktop\\TestImages\\img%02d.bmp", testImageCounter);
		SDL_Surface *temp = SDL_LoadBMP(filename);
		if (temp == NULL){
			cout << "Could not load image";
		}
		else{
			testImage = SDL_CreateTextureFromSurface(renderer, temp);
			testImageCounter += 1;
			if (testImageCounter == 34){
				testImageCounter = 3;
			}
		}
		SDL_FreeSurface(temp);
	}

}
void tick(){
	road->tick();
	//gauges->wheelAngle++;
	if (gauges->speed < 60)	gauges->speed += .05;
	road->dashSpeed = gauges->speed;
}

void render()
{
	// Clear the window 
	SDL_RenderSetViewport(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	SDL_RenderSetViewport(renderer, &roadViewport);
	road->draw(renderer);

	SDL_RenderSetViewport(renderer, &gaugesViewport);
	gauges->draw(renderer);

	// video stream
	SDL_Rect tempViewport = { 0, 0, 280 * 2, 210 * 2 };
	SDL_RenderSetViewport(renderer, &tempViewport);
	SDL_RenderCopy(renderer, testImage, NULL, NULL);

	SDL_RenderPresent(renderer);
}


bool InitEverything()
{
	if (!InitSDL())
		return false;

	if (!CreateWindow())
		return false;

	if (!CreateRenderer())
		return false;

	SetupRenderer();

	road = new Road(renderer, 300, 720);
	roadViewport = { 980, 0, 300, 720 };

	Text_init(renderer);

	gauges = new Gauges();
	gauges->init(renderer);
	gaugesViewport = { 560, 0, 420, 420 };

	source.open("data.txt", ios_base::in);  // open data
	if (!source)  {                     // if it does not work
		cerr << "Can't open Data!\n";
		return false;
	}


	return true;
}
bool InitSDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		std::cout << " Failed to initialize SDL : " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}
bool CreateWindow()
{
	window = SDL_CreateWindow("Display", posX, posY, sizeX, sizeY, 0);

	if (window == nullptr)
	{
		std::cout << "Failed to create window : " << SDL_GetError();
		return false;
	}

	return true;
}
bool CreateRenderer()
{
	renderer = SDL_CreateRenderer(window, -1, 0);

	if (renderer == nullptr)
	{
		std::cout << "Failed to create renderer : " << SDL_GetError();
		return false;
	}

	return true;
}
void SetupRenderer()
{
	// Set size of renderer to the same as window
	SDL_RenderSetLogicalSize(renderer, sizeX, sizeY);

	// Set color of renderer to red
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
}