//---------------------------------- C PREPROCESSOR --------------------------


#ifndef INCLUDE_LOCK
#define INCLUDE_LOCK
#include "main.h"
#endif



//------------------------------ FUNCTIONS -----------------------------------
/*
	char *loadTextFile(const char *filename, int *success):
	Loads a file in a safe way and creates a string from it

*/

char *loadTextFile( const char *filename , int *success )
{

	fprintf( stderr , "Loading file %s... \n" , filename );
	int fileDescriptor, fileSize, i;
	char *fileContents;
	FILE *jsonFile;
	
	fileDescriptor = open( filename , O_RDONLY , 0600 );//avoids the race condition issues
	if( fileDescriptor == -1 )
	{
		fprintf( stderr , "open has failed on %s: %s \n" , filename , strerror( errno ) );
		*success = FAIL;
		return NULL;
	}
	
	jsonFile = fdopen( fileDescriptor , "rb" );
	if( !jsonFile )
	{
		fprintf( stderr , "fopen has failed : %s \n" , strerror( errno ) );
		*success = FAIL;
		return NULL;
	}
	fileSize = getFileSize( jsonFile , success );
	if( !fileSize )
	{
		return NULL;
	
	}
	fileContents = calloc( 1 , fileSize + 1 );
	if( !fileContents )
	{
		fprintf( stderr , "malloc has failed at %s, %s, line %d : %s\n" , __FUNCTION__ , __FILE__ , __LINE__ , strerror( errno ) );
		*success = FAIL;
		return NULL;
	
	}
	fread( fileContents , fileSize , 1 , jsonFile );
	fclose( jsonFile );
	for(i = 0; i < fileSize; i++)
	{
		if(fileContents[i] == ']')
		{
			fileContents[i + 1] = '\0';
			break;
		}
	
	}
	if( strstr( fileContents , "%x" ) != NULL)
	{
		fprintf( stderr , "stack change formatter detected in file provided, nice try\n" );
		*success = FAIL;
		return NULL;
	
	}
	return fileContents;


}
/*
	int getFileSize(FILE *sizeToGet, int *success):
	Gets the size of a file

*/

int getFileSize( FILE *sizeToGet , int *success )
{
	fprintf( stderr , "Getting File size...\n" );
	int fileSize = 0;
	fseek( sizeToGet , 0 , SEEK_END );
	fileSize = ftell( sizeToGet );
	rewind( sizeToGet );//back to the start
	if(errno > 0)
	{
		fprintf( stderr , "ftell has failed : %s" , strerror( errno ) );
		*success = FAIL;
		return 0;
	}
	return fileSize;

}
/*
	optionsData *initOptions(char *fileContents, int *success):
	loads the options file in JSON to a optionsData structure

*/

optionsData initOptions( char *fileContents , int *success )
{
	fprintf( stderr , "Loading options... \n" );
	optionsData tempOpt;
	json_t *tempJsonHandle, *optionsData;
	json_error_t errorHandle;
	
	tempJsonHandle = json_loads( fileContents , 0 , &errorHandle );//loads the JSON file into Jansson 
	if( !tempJsonHandle )
	{
		fprintf( stderr , "json_loads has failed : %s \n" , errorHandle.text );
		*success = FAIL;
		json_decref(tempJsonHandle);
		return tempOpt;
	}
	
	optionsData = json_array_get( tempJsonHandle , 0 );
	if( !json_is_object(optionsData) )//makes sure that what is being opened is actually a JSON object
	{
		fprintf( stderr , "json_object_get failed, didn't get an object\n" );
		*success = FAIL;
		json_decref(tempJsonHandle);
		return tempOpt;
	
	}
	//gets the program options
	tempOpt.SCREEN_WIDTH = json_integer_value( json_object_get ( optionsData , "SCREEN_WIDTH" ) );
	tempOpt.SCREEN_HEIGHT = json_integer_value( json_object_get( optionsData , "SCREEN_HEIGHT" ) );
	tempOpt.WINDOW_TITLE = (char *)json_string_value( json_object_get( optionsData ,  "WINDOW_TITLE" ) );
	tempOpt.SAMPLE_SIZE = json_integer_value( json_object_get( optionsData , "SAMPLE_SIZE" ) );
	tempOpt.SAMPLE_FREQUENCY = json_integer_value( json_object_get( optionsData , "SAMPLE_FREQUENCY" ) );
	tempOpt.NO_CHANNELS = json_integer_value( json_object_get( optionsData , "NO_CHANNELS" ) );
	tempOpt.FONT_SIZE = json_integer_value( json_object_get( optionsData , "FONT_SIZE" ) );
	tempOpt.DEFAULT_FONT = (char *)json_string_value( json_object_get( optionsData , "DEFAULT_FONT" ) );
	
	return tempOpt;
}

/*
	SDL_Window *initSDL(optionsData *opt, int *success):
	initialise SDL2 and associated library

*/

SDL_Window *initSDL( optionsData *opt , int *success )
{
	fprintf( stderr , "Initialising SDL2 and SDL2 Extension Libraries....\n" );
	SDL_Window *temp;
	int SDL_Flags, IMG_Flags;
	SDL_Flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO;
	IMG_Flags = IMG_INIT_JPG | IMG_INIT_PNG;
	//Main SDL2 Library
	if( SDL_Init( SDL_Flags ) < 0 )
	{
		fprintf( stderr , "SDL_Init has failed : %s \n" , SDL_GetError() );
		*success = FAIL;
		return NULL;
	}
	//TTF Font Library
	if( TTF_Init() != 0 )
	{
		fprintf( stderr , "TFF_Init has failed : %s \n" , TTF_GetError() );
		*success = FAIL;
		return NULL;
	}
	//Image library
	if( !( IMG_Init( IMG_Flags ) & IMG_Flags ) )
	{
		fprintf( stderr , "IMG_Init has failed, %s \n" , IMG_GetError() );
		*success = FAIL;
		return NULL;
	
	}
	//Audio library
	if( Mix_OpenAudio( opt->SAMPLE_FREQUENCY , MIX_DEFAULT_FORMAT , opt->NO_CHANNELS , opt->SAMPLE_SIZE ) < 0 )//first arg : frequency of audio, second arg : format, third arg : Number of audio channels, forth arg : sample size
	{
		fprintf( stderr , "Mix_OpenAudio has failed, %s \n" , Mix_GetError() );
		*success = FAIL;
		return NULL;
	
	}
	//initialise window
	temp = SDL_CreateWindow( opt->WINDOW_TITLE , SDL_WINDOWPOS_UNDEFINED , SDL_WINDOWPOS_UNDEFINED , opt->SCREEN_WIDTH ,
	opt->SCREEN_HEIGHT , SDL_WINDOW_SHOWN );
	if( !temp )
	{
		fprintf( stderr , "SDL_CreateWindow has failed : %s \n" , SDL_GetError() );
		*success = FAIL;
		return NULL;
	}
	
	return temp;
}
/*
	SDL_Renderer *createRenderer(SDL_Window *screen, int *success):
	initialise hardware renderer

*/

SDL_Renderer *createRenderer( SDL_Window *screen , int *success )
{
	fprintf( stderr , "Creating SDL2 Renderer....\n" );
	SDL_Renderer *temp;
	int Render_Flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;//Hardware acceleration and a frame rate capped by the refresh rate of the monitor
	temp = SDL_CreateRenderer( screen , -1 , Render_Flags );
	if( !temp )
	{
		fprintf( stderr , "SDL_CreateRenderer has failed : %s \n" , SDL_GetError() );
		*success = FAIL;
		return NULL;
	}

	return temp;

}
/*
	Mix_Music *loadMusic(const char *filename, int *success):
	load music 

*/

Mix_Music *loadMusic( const char *filename , int *success )
{
	fprintf( stderr , "Loading music from %s....\n" , filename );
	Mix_Music *temp = Mix_LoadMUS( filename );
	if( !temp )
	{
		fprintf( stderr , "Mix_LoadMUS has failed : %s \n" , Mix_GetError() );
		*success = FAIL;
		return NULL;
	}

	return temp;

}
/*
	Mix_Chunk *loadEffect(const char *filename, int *success):
	load sound effects

*/
Mix_Chunk *loadEffect( const char *filename , int *success )
{
	fprintf( stderr , "Loading effect from %s....\n" , filename );
	Mix_Chunk *temp = Mix_LoadWAV( filename );
	if(!temp)
	{
		fprintf( stderr , "Mix_LoadWAV has failed : %s \n" , Mix_GetError() );
		*success = FAIL ;
		return NULL;
	}
	return temp;
}
/*
	TTF_Font *loadFont(options *opt, int *success):
	load a font structure

*/
TTF_Font *loadFont( optionsData *opt , int *success )
{
	fprintf( stderr , "Loading font from %s....\n" , opt->DEFAULT_FONT );
	if(opt->FONT_SIZE <= 0)
	{
		fprintf(stderr, "FATAL ERROR : Font size invalid\n");
		return NULL;
	
	}
	TTF_Font *temp = TTF_OpenFont( opt->DEFAULT_FONT , opt->FONT_SIZE );
	if( !temp )
	{
		fprintf( stderr , "TTF_OpenFont has failed : %s \n" , TTF_GetError() );
		*success = FAIL;
		return NULL;
	}
	return temp;
}
/*
	SDL_Texture *loadImage(const char *filename, SDL_Renderer *render, SDL_Rect *dimen, int *success):
	load an image, translates it to a hardware renderable texture and returns the original image's dimensions

*/
SDL_Texture *loadImage( const char *filename , SDL_Renderer *render , SDL_Rect *dimen , int *success )
{
	fprintf( stderr , "Loading image and converting it to texture from %s....\n" , filename );
	SDL_Surface *temp;
	SDL_Texture *tempTex;
	
	temp = IMG_Load( filename );//image library used to load things other than bitmaps
	if(!temp)
	{
		fprintf( stderr , "IMG_LoadBMP has failed: %s \n" , IMG_GetError() );
		*success = FAIL;
		return NULL;
	
	}
	SDL_SetColorKey( temp , SDL_TRUE , SDL_MapRGB( temp->format , 255 , 0 ,255 ) );//makes the background colour transparent
	tempTex = SDL_CreateTextureFromSurface( render , temp );//converts to renderable type
	if( !tempTex )
	{
		fprintf( stderr , "SDL_CreateTextureFromSurface has failed : %s \n" , SDL_GetError() );
		*success = FAIL;
		return NULL;
	}
	dimen->w = temp->w;
	dimen->h = temp->h;
	SDL_FreeSurface( temp );
	return tempTex;
}
/*
mapConstruct *loadMap(char *mapText, SDL_Texture *floor, SDL_Texture *wall, int *success):
Loads a map from a JSON file


*/
mapConstruct *loadMap(char *mapText, SDL_Texture *floor, SDL_Texture *wall, int *success)
{
	json_t *tempJsonHandle, *mapData;
	json_error_t errorHandle;
	char *mapString;
	int i, j, yPos, xPos;
	mapConstruct *map = malloc(sizeof(mapConstruct));
	if(!map)
	{
		fprintf(stderr, "loadMap has failed: Unable to allocate memory for MapConstruct\n");
		*success = FAIL;
		return NULL;
	}
	tempJsonHandle = json_loads( mapText , 0 , &errorHandle );//loads the JSON file into Jansson 
	if( !tempJsonHandle )
	{
		fprintf( stderr , "json_loads has failed : %s \n" , errorHandle.text );
		*success = FAIL;
		return NULL;
	}
	
	mapData = json_array_get( tempJsonHandle , 0 );
	if( !json_is_object(mapData) )//makes sure that what is being opened is actually a JSON object
	{
		fprintf( stderr , "json_object_get failed, didn't get an object\n" );
		*success = FAIL;
		return NULL;
	}
	//json_integer_value( json_object_get ( optionsData , "SCREEN_WIDTH" ) );
	map->xSize = json_integer_value(json_object_get( mapData, "xSize" ));
	map->ySize = json_integer_value(json_object_get( mapData, "ySize" ));
	strncpy(mapString,json_string_value(json_object_get( mapData, "MapLayout" )), map->ySize * map->xSize);
	
	map->mapDetails = malloc(sizeof(basicEntity *) * map->ySize);
	xPos = 0;
	yPos = 0;
	
	for(i = 0; i < map->ySize; i++)
	{
		xPos = 0;
		map->mapDetails[i] = malloc(sizeof( basicEntity ) * map->xSize);
		
		for(j = 0; j < map->xSize; j++)
		{
			map->mapDetails[i][j].rectangle.x = xPos;
			map->mapDetails[i][j].rectangle.y = yPos;
			map->mapDetails[i][j].rectangle.w = TILE_SIZE;
			map->mapDetails[i][j].rectangle.h = TILE_SIZE;
			map->mapDetails[i][j].type = mapString[(i * map->ySize) + j] - '0';
			
			if(map->mapDetails[i][j].type == TILE_WALKABLE)
			{
				map->mapDetails[i][j].tileDisplay = floor;
			}
			else
			{
				map->mapDetails[i][j].tileDisplay = wall;
			}
			xPos += TILE_SIZE;
		}
		
		yPos += TILE_SIZE;
	
	}
	return map;
}
/*
basicEntity **loadTiles(int *success):
Load all of the mapTiles

*/

basicEntity **loadTiles(SDL_Renderer *renderer, int *success)
{
	int i;
	basicEntity **tiles = malloc( sizeof(basicEntity *) * NO_TILES);
	char filename[1000];
	for( i = 0; i < NO_LEVELS; i++)
	{
		tiles[(i * 2)] = malloc( sizeof( basicEntity) );
		sprintf( filename, "%s%s_FLOOR.png", SPRITE_DIR, GOD_NAMES[i]);
		fprintf(stdout, "%s\n", filename);
		//SDL_Texture *loadImage( const char *filename , SDL_Renderer *render , SDL_Rect *dimen , int *success );
		tiles[(i *2)]->tileDisplay = loadImage( filename, renderer, &tiles[(i * 2)]->rectangle, success);
		tiles[(i * 2) + 1] = malloc( sizeof( basicEntity) );
		sprintf( filename, "%s%s_WALL.png", SPRITE_DIR, GOD_NAMES[i]);
		//SDL_Texture *loadImage( const char *filename , SDL_Renderer *render , SDL_Rect *dimen , int *success );
		tiles[(i *2) + 1]->tileDisplay = loadImage( filename, renderer, &tiles[(i * 2) + 1]->rectangle, success);
	}
	return tiles;
}
/*
basicEntity **loadEnemies(SDL_Renderer *renderer, int *success)
loads all the enemy components

*/

basicEntity **loadEnemies(SDL_Renderer *renderer, int *success)
{
	int i;
	basicEntity **tiles = malloc( sizeof(basicEntity *) * ((NO_ENEMIES * 2) + 1));
	char filename[1000];
	for( i = 0; i < NO_ENEMIES; i++)
	{
		tiles[(i * 2)] = malloc( sizeof( basicEntity) );
		sprintf( filename, "%s%sHEAD.png", SPRITE_DIR, ENEMY_NAMES[i]);
		fprintf(stdout, "%s\n", filename);
		tiles[(i *2)]->tileDisplay = loadImage( filename, renderer, &tiles[(i * 2)]->rectangle, success);
		tiles[(i * 2) + 1] = malloc( sizeof( basicEntity) );
		sprintf( filename, "%s%sBODY.png", SPRITE_DIR, ENEMY_NAMES[i]);
		tiles[(i *2) + 1]->tileDisplay = loadImage( filename, renderer, &tiles[(i * 2) + 1]->rectangle, success);
		tiles[(i *2) + 1]->tileWidth = tiles[(i * 2)]->rectangle.w / 2;//width of the tiles for animation
	}
	tiles[(i * 2)] = malloc( sizeof( basicEntity) );
	sprintf( filename, "%sGIANT_OWL.png", SPRITE_DIR);
	fprintf(stdout, "%s\n", filename);
	//SDL_Texture *loadImage( const char *filename , SDL_Renderer *render , SDL_Rect *dimen , int *success );
	tiles[(i *2)]->tileDisplay = loadImage( filename, renderer, &tiles[(i * 2)]->rectangle, success);
	tiles[(i *2)]->tileWidth = tiles[(i * 2)]->rectangle.w / 2;
	return tiles;
}
/*
basicEntity **loadWeapons(SDL_Renderer *renderer, int *success)
loads all the weapons components

*/
basicEntity **loadWeapons(SDL_Renderer *renderer, int *success)
{
int i;
	basicEntity **tiles = malloc( sizeof(basicEntity *) * ((NO_WEAPONS * 2) + 1));
	char filename[1000];
	for( i = 0; i < NO_WEAPONS; i++)
	{
		tiles[(i * 2)] = malloc( sizeof( basicEntity) );
		sprintf( filename, "%s%sGUN.png", SPRITE_DIR, WEAPON_NAMES[i]);
		fprintf(stdout, "%s\n", filename);
		//SDL_Texture *loadImage( const char *filename , SDL_Renderer *render , SDL_Rect *dimen , int *success );
		tiles[(i *2)]->tileDisplay = loadImage( filename, renderer, &tiles[(i * 2)]->rectangle, success);
		tiles[(i *2)]->tileWidth = tiles[(i * 2)]->rectangle.w / 2;
		tiles[(i * 2) + 1] = malloc( sizeof( basicEntity) );
		sprintf( filename, "%s%sAMMO.png", SPRITE_DIR, WEAPON_NAMES[i]);
		fprintf(stdout, "%s\n", filename);
		//SDL_Texture *loadImage( const char *filename , SDL_Renderer *render , SDL_Rect *dimen , int *success );
		tiles[(i *2) + 1]->tileDisplay = loadImage( filename, renderer, &tiles[(i * 2) + 1]->rectangle, success);
	}
	tiles[(i * 2)] = malloc( sizeof( basicEntity) );
	sprintf( filename, "%sSPEAR.png", SPRITE_DIR);
	fprintf(stdout, "%s\n", filename);
	//SDL_Texture *loadImage( const char *filename , SDL_Renderer *render , SDL_Rect *dimen , int *success );
	tiles[(i *2)]->tileDisplay = loadImage( filename, renderer, &tiles[(i * 2)]->rectangle, success);
	tiles[(i *2)]->tileWidth = tiles[(i * 2)]->rectangle.w / 2;
	return tiles;
}

basicCharacter **loadEnemyStats(char *enemyFile, basicEntity **bodyAndHeads, basicEntity **weaponsAndAmmo, int *success)
{
	json_t *enemyDataFileArray, *enemyData;
	json_error_t errorHandle;
	int i, noEnemies, enemyID, weaponID;
	basicCharacter **temp;
	enemyDataFileArray = json_loads(enemyFile, 0, &errorHandle);
	if( !enemyDataFileArray )
	{
		fprintf( stderr , "json_loads has failed : %s \n" , errorHandle.text );
		*success = FAIL;
		return NULL;
	}
	
	enemyData = json_array_get( enemyDataFileArray , 0 );
	if( !json_is_object(enemyData) )//makes sure that what is being opened is actually a JSON object
	{
		fprintf( stderr , "json_object_get failed, didn't get an object\n" );
		*success = FAIL;
		return NULL;
	}
	noEnemies = json_integer_value(json_object_get(enemyData, "NO_ENEMIES")) + 1;
	temp = malloc( sizeof(basicCharacter *) * noEnemies);
	if(!temp)
	{
		fprintf(stderr, "loadEnemyStats has failed: Unable to allocate memory for basicCharacter!\n");
		*success = FAIL;
		return NULL;
	}
	
	for(i = 1; i < noEnemies; i++)
	{
		temp[i] = malloc(sizeof(basicCharacter));
		enemyData = json_array_get(enemyDataFileArray, i);
		if( !json_is_object(enemyData) )//makes sure that what is being opened is actually a JSON object
		{
			fprintf( stderr , "json_object_get failed, didn't get an object\n" );
			*success = FAIL;
			return NULL;
		}
		
		
		enemyID = json_integer_value(json_object_get(enemyData, "BODY_ID"));
		weaponID = json_integer_value(json_object_get(enemyData, "WEAPON_ID"));
		
		temp[i]->base = bodyAndHeads[enemyID *2];
		temp[i]->head = bodyAndHeads[(enemyID *2) + 1];
		temp[i]->weapon = bodyAndHeads[(weaponID * 2)];
		temp[i]->bullet = bodyAndHeads[(weaponID * 2) + 1];
		
		temp[i]->healthPoints = json_integer_value(json_object_get(enemyData, "HP"));
		temp[i]->weaponDamage = json_integer_value(json_object_get(enemyData, "WEAPON_DAMAGE"));
		temp[i]->rateOfFire = json_integer_value(json_object_get(enemyData, "SHOTS_PER_SECOND"));
		temp[i]->noShots = json_integer_value(json_object_get(enemyData, "NO_BULLETS_PER_SHOT"));
		temp[i]->speed = json_integer_value(json_object_get(enemyData, "SPEED"));
		temp[i]->headOffsetX = json_integer_value(json_object_get(enemyData, "HEAD_OFFSET_X"));
		temp[i]->headOffsetY = json_integer_value(json_object_get(enemyData, "HEAD_OFFSET_Y"));
		temp[i]->weaponOffsetX = json_integer_value(json_object_get(enemyData, "WEAPON_OFFSET_X"));
		temp[i]->weaponOffsetY = json_integer_value(json_object_get(enemyData, "WEAPON_OFFSET_Y"));
		temp[i]->alive = SUCCESS;
	}
	
	return temp;
}


gameObject *loadGame(SDL_Renderer *renderer, int *success)
{
	gameObject *temp = malloc(sizeof(gameObject));
	/*
	typedef struct
	{
	mapConstruct **maps;
	basicEntity **allWeapons, **allEnemies, **allTiles;
	basicCharacter *allEnemyData;

	}gameObject;
	*/
	
	temp->maps = malloc(sizeof( mapConstruct *) * NO_LEVELS);
	temp->allTiles = loadTiles(renderer, success);
	temp->allEnemies = loadEnemies(renderer, success);
	temp->allWeapons = loadWeapons(renderer, success);
	temp->maps[0] = loadMap(loadTextFile("ENKI.json", success), temp->allTiles[0]->tileDisplay, temp->allTiles[1]->tileDisplay, success);
	temp->allEnemyData = loadEnemyStats(loadTextFile(ENEMY_FILE, success), temp->allEnemies, temp->allWeapons, success);
	
	/*
	allTiles = loadTiles(mainRenderer, &succeededInit);
	allEnemies = loadEnemies(mainRenderer, &succeededInit);
	allWeapons = loadWeapons(mainRenderer, &succeededInit);
	levelFile = loadTextFile("ENKI.json", &succeededInit);
	testMap = loadMap(levelFile, allTiles[0]->tileDisplay, allTiles[1]->tileDisplay, &succeededInit);
	allEnemyData = loadEnemyStats(loadTextFile(ENEMY_FILE, &succeededInit),allEnemies, allWeapons,  &succeededInit);
	*/
	return temp;
}