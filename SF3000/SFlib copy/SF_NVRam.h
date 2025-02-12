/* File : NVRam manager header file */

#ifndef __SF_NVRAM
#define __SF_NVRAM

// Includes

#include "SF_Menu.h"
#include "SF_Mission.h"
#include "SF_Pyramid.h"
#include "SF_Status.h"

// External Variables

extern	menu				menus [MAX_MENUS];
extern	pyramid				pyramids [4];
extern	performance_data	player_performance;
extern	game_status			status;	


// Function Prototypes

long	nvram_save (char*, char*, char*, long);				// Saves a file to NVRAM
long	nvram_load (char*, char*, char*, long);				// Loads a file from NVRAM
long	nvram_delete (char*, char*);						// Deletes a file from NVRAM
long	nvram_directory (long, long, long, long, char*);	// Catalogues NVRAM intro the menu

#endif

