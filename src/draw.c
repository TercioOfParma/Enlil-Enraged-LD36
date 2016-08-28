#ifndef INCLUDE_LOCK
#define INCLUDE_LOCK
#include "main.h"
#endif



void drawMap(mapConstruct *toDraw, SDL_Renderer *render, SDL_Rect camera, optionsData *opt)
{
	int i,j;
	SDL_Rect temp;
	temp.w = TILE_SIZE;
	temp.h = TILE_SIZE;
	for( i = 0; i < toDraw->ySize; i++)
	{
		for( j = 0; j < toDraw->xSize; j++)
		{
			temp.x = toDraw->mapDetails[i][j].rectangle.x + camera.x;
			temp.y = toDraw->mapDetails[i][j].rectangle.y + camera.y;
			if(temp.x >= 0 - TILE_SIZE && temp.x <= opt->SCREEN_WIDTH + TILE_SIZE && temp.y >= 0 - TILE_SIZE && temp.y <= opt->SCREEN_HEIGHT + TILE_SIZE)
			{
				SDL_RenderCopy(render, toDraw->mapDetails[i][j].tileDisplay, NULL,  &temp);
			}
		}
		
	}


}
