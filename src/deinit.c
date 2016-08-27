#ifndef INCLUDE_LOCK
#define INCLUDE_LOCK
#include "main.h"
#endif


void endSDL( SDL_Renderer *render , SDL_Window *screen , TTF_Font *font )
{
	fprintf( stderr , "Deinitialising SDL2, extensions libraries, hardware rendering and window....\n" );
	//runtime errors
	SDL_DestroyRenderer( render );
	SDL_DestroyWindow( screen );
	TTF_CloseFont( font );
	//other bits
	IMG_Quit();
	Mix_Quit();
	TTF_Quit();
	SDL_Quit();

}