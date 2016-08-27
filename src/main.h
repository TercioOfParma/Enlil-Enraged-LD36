﻿#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <jansson/jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


static const char *ERROR_LOG = "errorlog.txt";
static const char *OPTIONS_FILE = "options.json";
static const char *GOD_NAMES[4] = { "ENKI", "ISHTAR", "NINURTA", "ENLIL"};
static const char *SPRITE_DIR = "Sprites/";


static const int SUCCESS = 0;
static const int FAIL = 1;
static const int ENKI_LEVEL = 1;
static const int ISHTAR_LEVEL = 2;
static const int NINURTA_LEVEL = 3;
static const int ENLIL_LEVEL = 4;
static const int TILE_WALKABLE = 1;
static const int TILE_UNWALKABLE = 2;
static const int ENEMY_MELEE = 3;
static const int ENEMY_RANGED = 4;
static const int TILE_SIZE = 64;
static const int NO_TILES = 8;
static const int NO_LEVELS = 4;
static const int CAMERA_SPEED = 10;

static const int MALE_WARRIOR_MELEE = 1;
static const int MALE_WARRIOR_RANGED = 2;
static const int FEMALE_WARRIOR_MELEE = 3;
static const int FEMALE_WARRIOR_RANGED = 4;
static const int LAMASSU = 5;
static const int FISHMAN = 6;
static const int WAR_DOG = 7;
static const int ENKIDU = 8;
static const int GIANT_OWL = 9;
static const int RAM = 11;
static const int ENLIL = 12;


typedef struct
{
	int SCREEN_WIDTH, SCREEN_HEIGHT, SAMPLE_SIZE, SAMPLE_FREQUENCY, NO_CHANNELS, FONT_SIZE;
	char *WINDOW_TITLE, *DEFAULT_FONT;

}optionsData;
typedef struct
{
	SDL_Rect rectangle;
	int	type;
	SDL_Texture *tileDisplay;

}basicEntity;
typedef struct
{
	int	xSize, ySize, levelDetail;
	basicEntity **mapDetails;
}mapConstruct;

typedef struct
{
	basicEntity *base, *head, *weapon;
	int healthPoints, weaponDamage, rateOfFire, noShots, speed;
	int headOffset, weaponOffset, weaponType;
}baseCharacter;

typedef struct
{
	baseCharacter *basics;
	unsigned char hasBow, hasSpear, hasTeleport, hasSuperBow, hasWaterCannon, hasHealthRegen, hasSling, hasJavelin;
	int bowAmmo, superbowAmmo, waterCannonAmmo, slingAmmo, javelinAmmo;
}playerCharacter;

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
mapConstruct *loadMap(char *mapText, SDL_Texture *floor, SDL_Texture *wall, int *success);
basicEntity **loadTiles(SDL_Renderer *renderer, int *success);

//===== DEINIT ======
void endSDL( SDL_Renderer *render , SDL_Window *screen , TTF_Font *font );


//====== DRAW ========

void drawMap(mapConstruct *toDraw, SDL_Renderer *render, SDL_Rect camera, optionsData *opt);


//======= INPUT =======

void handleCamera(SDL_Event *events, SDL_Rect *camera);