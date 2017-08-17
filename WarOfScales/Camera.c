#include "Camera.h"

void Camera_MapToScreen(Point* output, Camera* camera, Point* in)
{
	output->x = (in->x - camera->pos.x) * camera->scale - WINDOW_WIDTH / 2;
	output->y = (in->y - camera->pos.x) * camera->scale - WINDOW_HEIGHT / 2;
	output->z = in->z;
}
void Camera_ScreenToMap(Point* output, Camera* camera, Point* in)
{
	output->x = ((in->x + WINDOW_WIDTH / 2) / camera->scale) + camera->pos.x;
	output->y = ((in->y + WINDOW_HEIGHT / 2) / camera->scale) + camera->pos.y;
	output->z = in->z;
}