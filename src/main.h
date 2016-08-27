#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <jansson/jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


static const char *ERROR_LOG = "errorlog.txt";
static const char *OPTIONS_FILE = "options.json";

static const int SUCCESS = 0;
static const int FAIL = 1;


typedef struct
{
	int SCREEN_WIDTH, SCREEN_HEIGHT, SAMPLE_SIZE, SAMPLE_FREQUENCY, NO_CHANNELS, FONT_SIZE;
	char *WINDOW_TITLE, *DEFAULT_FONT;

}optionsData;


//===== INIT ======
char *loadTextFile( const char *filename , int *success );
int getFileSize( FILE *sizeToGet , int *success );
optionsData initOptions( char *fileContents , int *success );
SDL_Window *initSDL( optionsData *opt , int *success );
SDL_Renderer *createRenderer( SDL_Window *screen , int *success );
Mix_Music *loadMusic( const char *filename , int *success );
Mix_Chunk *loadEffect( const char *filename , int *success );
TTF_Font *loadFont( optionsData *opt , int *success );
SDL_Texture *loadImage( const char *filename , SDL_Renderer *render , SDL_Rect *dimen , int *success );

//===== DEINIT ======
void endSDL( SDL_Renderer *render , SDL_Window *screen , TTF_Font *font );