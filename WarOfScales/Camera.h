#pragma once
//idk why i'm making this its own file i just don't know where else to put camera oh jeez this projects includes is gonna be a clusterfuck isnt it oh no
#include "Point.h"
typedef struct Camera {
	Point pos;
	double scale;
}Camera;

void Camera_MapToScreen(Point* output, Camera* camera, Point* in);
void Camera_ScreenToMap(Point* output, Camera* camera, Point* in);