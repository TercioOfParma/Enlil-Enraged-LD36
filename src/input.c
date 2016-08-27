#ifndef INCLUDE_LOCK
#define INCLUDE_LOCK
#include "main.h"
#endif


void handleCamera(SDL_Event *events, SDL_Rect *camera)
{
	switch(events->key.keysym.sym)
	{
		case SDLK_w:
			camera->y += CAMERA_SPEED;
		break;
		case SDLK_s:
			camera->y -= CAMERA_SPEED;
		break;
		case SDLK_a:
			camera->x += CAMERA_SPEED;
		break;
		case SDLK_d:
			camera->x -= CAMERA_SPEED;
		break;
	}
	fprintf(stdout, "X: %d Y: %d\n", camera->x, camera->y);
}