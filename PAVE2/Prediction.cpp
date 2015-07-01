
//#include <SDL.h>
#include "Prediction.h"
#include "Road.h"
#include "SDL2_gfxPrimitives.h"
//#include <stdio.h>
#include <iostream>
#include <math.h>
using namespace std;

#define debug(var) cout << #var << " = " << var << "\n"

Sint16 *Prediction::xpoints = NULL;
Sint16 *Prediction::ypoints = NULL;
Sint16 *Prediction::xpoints2 = NULL;
Sint16 *Prediction::ypoints2 = NULL;
Point *Prediction::topDown = NULL;
int Prediction::numPoints = 0;

// center of the video feed viewport
const int centerx = 280;
const int centery = 210;

float Prediction::topDownScale = 100; // should be updated by road
const float SCALE = 200;
const float CAR_WIDTH = 2.2; //meters
const float CAMERA_HEIGHT = 1;
const float CAMERA_BACK = 0.2; //distance from camera to the first prediction point (0,0), bad things happen if it's 0

// returns points of either side of the path, based on positions array
void Prediction::createPoints(Point *input){
	//struct Point *toReturn = (Point*)malloc(sizeof(struct Point)*numPoints); // no room for null, use numPositions to get length
	// the resulting order is counterclockwise so we can draw a polygon
	for (int i = 0; i < numPoints/2; i++){ // i should go up to the original number of input points
		Point current = input[i]; // does this create a copy in the current stack? I guess it doesn't really matter
		Point *newR = &topDown[i];
		Point *newL = &topDown[numPoints - 1 - i];
		float angle = M_PI/2; // straight ahead is 90 degrees
		if (i != 0){
			Point old = input[i - 1];
			angle = atan2(current.y - old.y, current.x - old.x);
		}
		newR->x = current.x + CAR_WIDTH / 2 * sin(angle);
		newR->y = current.y - CAR_WIDTH / 2 * cos(angle);
		newL->x = current.x - CAR_WIDTH / 2 * sin(angle);
		newL->y = current.y + CAR_WIDTH / 2 * cos(angle);
	}
	//return toReturn;
}

// take points from top-down 2D in meters to camera perspective in pixels
Point* Prediction::transformPoints(Point* input){
	struct Point *toReturn = (Point*)malloc(sizeof(struct Point)*numPoints); // no room for null, use numPositions to get length
	for (int i = 0; i < numPoints; i++){
		Point *older = &input[i];
		Point *newer = &toReturn[i];

		float dist = older->y + CAMERA_BACK; // horizontal distance to this point on the road

		newer->x = (older->x / dist)*SCALE+centerx;
		newer->y = (CAMERA_HEIGHT / dist)*SCALE+centery; 
	}
	return toReturn;
}

void Prediction::updateLists(Point* input){
	xpoints = (Sint16*)realloc(xpoints, sizeof(Sint16)*numPoints);
	ypoints = (Sint16*)realloc(ypoints, sizeof(Sint16)*numPoints);
	for (int i = 0; i < numPoints; i++){
		xpoints[i] = (Sint16)input[i].x;
		ypoints[i] = (Sint16)input[i].y;
	}
}

void Prediction::updateLists2(Point* input, Road *road){
	xpoints2 = (Sint16*)realloc(xpoints2, sizeof(Sint16)*numPoints);
	ypoints2 = (Sint16*)realloc(ypoints2, sizeof(Sint16)*numPoints);
	for (int i = 0; i < numPoints; i++){
		xpoints2[i] = (Sint16)(input[i].x*Road::PIXELS_PER_METER+road->carMain.x + road->carMain.w/2);
		ypoints2[i] = (Sint16)(-input[i].y*Road::PIXELS_PER_METER+road->carMain.y);
	}
}

void Prediction::updateSurface(Point *positions, int num){
	numPoints = num * 2;
	topDown = (Point*)realloc(topDown, sizeof(Point)*numPoints);
	createPoints(positions);
	Point* perspective = transformPoints(topDown);
	//updateLists2(topDown); // now done in draw method
	updateLists(perspective);

	free(perspective);
}

void Prediction::drawPerspective(SDL_Renderer *renderer){
	// xpoints and ypoints should be up to date because updateSurface was called
	filledPolygonRGBA(renderer, xpoints, ypoints, numPoints, 255, 0, 0, 63);
}
void Prediction::drawTopDown(SDL_Renderer *renderer, Road *road){
	updateLists2(topDown, road);
	filledPolygonRGBA(renderer, xpoints2, ypoints2, numPoints, 255, 0, 0, 127);
}