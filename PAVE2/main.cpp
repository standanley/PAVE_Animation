#include <SDL.h>
#include <iostream>
#include <vector>
#include "Road.h"
#include "Text.h"
#include "Gauges.h"
#include "Prediction.h"
#include <fstream>
#include <stdlib.h>
#include "SDL2_gfxPrimitives.h"


#define debug(var) cout << #var << " = " << var << "\n"

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
int lastLocation = 0;
int frameCount = 0;

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Texture *testImage;
int testImageCounter = 3;
SDL_Rect roadViewport;
SDL_Rect gaugesViewport;

Road *road;
Gauges *gauges;
Prediction *prediction;

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
		SDL_Delay(160);
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
	// does not have image or predicted path
	float *variables[17] = { &road->angle,
		&road->toMarking_LL,
		&road->toMarking_ML,
		&road->toMarking_MR,
		&road->toMarking_RR,
		&road->dist_LL,
		&road->dist_MM,
		&road->dist_RR,
		&road->toMarking_L,
		&road->toMarking_M,
		&road->toMarking_R,
		&road->dist_L,
		&road->dist_R, 
		&gauges->speed,
		&gauges->wheelAngle,
		&gauges->gas,
		&gauges->brake};

	string mystring;
	while ((bool)getline(source, mystring)){
		int location = source.tellg();
		if (location != -1){
			// got a line
			const char *line = mystring.c_str();
			int num =0;
			float value=0;
			sscanf_s(line, "%d %f", &num, &value);
			*variables[num] = value;

			lastLocation = location;
		}
	}
	source.clear();
	source.seekg(lastLocation, source.beg);






	// simulate choppy stream
	if (rand() % 10 >= 3){
		//return;
	}

	// update every sixth frame
	if (frameCount % 6 != 0){
		return;
	}


	// Video feed

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

	Prediction::drawPerspective(renderer);

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

	gauges = new Gauges(renderer);
	//gauges->init(renderer);
	gaugesViewport = { 560, 0, 420, 420 };

	source.open("data.txt", ios_base::in);  // open data
	if (!source)  {                     // if it does not work
		cerr << "Can't open Data!\n";
		return false;
	}

	// temporarily
	//prediction = new Prediction();
	const int numPositions = 8;
	Point *positions = (Point*)malloc(sizeof(Point) * numPositions);
	positions[0] = { 0, 0 };
	positions[1] = { 0, 1 };
	positions[2] = { 0, 2 };
	positions[3] = { .3, 2.8 };
	positions[4] = { 1, 3.5 };
	positions[5] = { 1.5, 4.2 };
	positions[6] = { 1.6, 4.8 };
	positions[7] = { 1.6, 6 };
	Prediction::updateSurface(positions, numPositions);


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