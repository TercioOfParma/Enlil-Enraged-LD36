#ifndef INCLUDE_LOCK
#define INCLUDE_LOCK
#include "main.h"
#endif


/*
	main:

	Main process
*/
int main(int argc, char *argv[])
{
	char *optionsFile, *levelFile;
	int succeededInit;
	SDL_Window *mainWindow;
	SDL_Renderer *mainRenderer;
	TTF_Font *mainFont;
	optionsData opt;
	mapConstruct *testMap;
	FILE *errorToFile = freopen(ERROR_LOG, "w", stderr);
	basicEntity **allTiles, **allEnemies, **allWeapons;
	playerCharacter *player = malloc( sizeof( playerCharacter ) );
	SDL_Event events;
	SDL_Rect cameraRect;
	
	//======== First Options Loadup ==============
	succeededInit = SUCCESS;
	optionsFile = loadTextFile(OPTIONS_FILE, &succeededInit);
	opt = initOptions( optionsFile, &succeededInit);
	
	mainWindow = initSDL(&opt, &succeededInit);
	mainRenderer = createRenderer(mainWindow, &succeededInit);
	mainFont = loadFont( &opt, &succeededInit);
	allTiles = loadTiles(mainRenderer, &succeededInit);
	allEnemies = loadEnemies(mainRenderer, &succeededInit);
	allWeapons = loadWeapons(mainRenderer, &succeededInit);
	levelFile = loadTextFile("ENKI.json", &succeededInit);
	testMap = loadMap(levelFile, allTiles[0]->tileDisplay, allTiles[1]->tileDisplay, &succeededInit);
	
	cameraRect.w = opt.SCREEN_WIDTH;
	cameraRect.h = opt.SCREEN_HEIGHT;
	cameraRect.x = 0;
	cameraRect.y = 0;
	while(succeededInit == SUCCESS)
	{
		while(SDL_PollEvent(&events) != 0)
		{
			if(events.type == SDL_KEYDOWN)
			{
				handleCamera(&events, &cameraRect);
			}
			if(events.type == SDL_QUIT)
			{
				succeededInit = FAIL;
			
			}
		
		}
		drawMap(testMap, mainRenderer,cameraRect, &opt);
		SDL_RenderPresent(mainRenderer);
		SDL_RenderClear(mainRenderer);
		
	}
	endSDL(mainRenderer, mainWindow, mainFont);
	free(testMap);
	fclose(stderr);
	return 0;
}