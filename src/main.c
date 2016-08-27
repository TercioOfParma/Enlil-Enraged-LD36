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
	char *optionsFile;
	int succeededInit;
	SDL_Window *mainWindow;
	SDL_Renderer *mainRenderer;
	TTF_Font *mainFont;
	optionsData opt;
	FILE *errorToFile = freopen(ERROR_LOG, "w", stderr);
	
	//======== First Options Loadup ==============
	succeededInit = SUCCESS;
	optionsFile = loadTextFile(OPTIONS_FILE, &succeededInit);
	opt = initOptions( optionsFile, &succeededInit);
	
	mainWindow = initSDL(&opt, &succeededInit);
	mainRenderer = createRenderer(mainWindow, &succeededInit);
	mainFont = loadFont( &opt, &succeededInit);
	
	endSDL(mainRenderer, mainWindow, mainFont);
	return 0;
}