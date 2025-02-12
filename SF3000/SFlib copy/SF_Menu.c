// File : SF_Menu.c		-	Menu selection routines

#include "SF_Star3000.h"
#include "SF_Menu.h"
#include "SF_War.h"
#include "SF_CelUtils.h"
#include "SF_ScreenUtils.h"
#include "SF_Palette.h"
#include "SF_Io.h"
#include "SF_Access.h"
#include "SF_ArmCell.h"
#include "SF_ArmAnim.h"
#include "SF_Control.h"
#include "SF_Message.h"
#include "SF_Music.h"
#include "SF_NVRam.h"
#include "SF_Utility.h"
#include "SF_Sound.h"
#include "SF_Pyramid.h"
#include "SF_Joystick.h"
#include "SF_Video.h"
#include "Setup_Tables.h"
#include "Sound_Control.h"

#include <string.h>


/***************************************************************************************************/
/*											MENU DEFINITIONS									   */
/***************************************************************************************************/

// Text size definitions

#define MENU_YSIZE	18						// Y Size of text multiplier 
#define	MENU_YOFFS	41						// Y Offset for menu text

// Icon definitions for menus

#define	BASECEL_LEVEL_L1			0		// Icon definitions for gameset 'Select Level'
#define	BASECEL_LEVEL_L2			1
#define	BASECEL_LEVEL_L3			2
#define	BASECEL_LEVEL_L4			3

#define BASECEL_LANGUAGE_ENGLISH	0		// Icon definitions for gameset 'Language'
#define BASECEL_LANGUAGE_FRENCH		1
#define BASECEL_LANGUAGE_GERMAN		2

#define	BASECEL_MUSIC_CD			0		// Icon definitions for gameset 'Music'
#define	BASECEL_BIG_CD				5
#define	BASECEL_BIG_FX				6
#define	BASECEL_BARS				7
#define	BASECEL_NUMBERS				15

#define	BASECEL_CONTROL_TICK		0		// Icon definitions for gameset 'Control'
#define	BASECEL_CONTROL_ARROWS		1
#define	BASECEL_CONTROL_WHEELIN		5
#define	BASECEL_CONTROL_WHEELOUT	6
#define	BASECEL_CONTROL_CURSOR		7
#define	BASECEL_CONTROL_CENTRE		8

// Definitions for saving gamefiles to NVRAM

#define	GAMEFILE_PREFIX		"SF#"
#define	GAMEFILE_VERSION	2

// GLOBAL VARIABLES

char			pilot_widths [16]	=	{
										0,
										27,
										39,
										31,
										33,
										33,
										43,
										27,
										21,
										31,
										35,
										23,
										27,
										39,
										24,
										24
										};
gamefile		save_gamefile;
char			user_savefilename [16];
long			menu_setfixedcamera;
long			menu_countdown = 0;
menu_info		menu_status = {NO_MENU,0,0,0,0};
menu			menus [MAX_MENUS] =
			
	{
	
		// Main Menu (0)
		// -------------
		
		{
		MMNU0 + 0,
		0,	"",
		ControlStart,
		WHITE_15,
		-1,
		
		100,		
		20,
		
		MAIN_MENU,	
		3,	{{BLUE_PAL, 0,1},	{RED_PAL,	4, 5},	{GREY_PAL, 7,7}},
		
		4,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	MAIN_START,	NULL,						0,				0,	0,	YELLOW_15, 	{MMNU0 + 1}},
		{-1,	-1,	-1,	ITEM_TN,	1,	ITEM_OK,	MAIN_START,	control_startgame,			0,				0,	0,	YELLOW_15, 	{MMNU0 + 2}},
		{-1,	-1,	-1,	ITEM_TN,	4,	ITEM_OK,	0,			menu_savegamefile,			0,				0,	0,	WHITE_15, 	{MMNU16 + 2}},
		{-1,	-1,	-1,	ITEM_TN,	5,	ITEM_OK,	0,			menu_movedown,				LOAD_MENU,		0,	0,	WHITE_15, 	{MMNU0 + 3}},
		{-1,	-1,	-1,	ITEM_TN,	7,	ITEM_OK,	0,			menu_movedown,				CONFIGURE_MENU,	0,	0,	RED_15, 	{MMNU0 + 4}},
		{-1,	-1,	-1,	ITEM_TN,	2,	ITEM_OK,	0,			menu_movedown,				LEVEL_MENU,		0,	0,	ORANGE_15, 	{MMNU0 + 5}}
			}
		},

		
		// Formation Controls (1)
		// ----------------------
		
		{
		MMNU1 + 0,
		0,	"",
		ControlX,
		WHITE_15,
		-1,
		
		100,		
		20,
		
		NO_MENU,	
		2,	{{GREY_PAL, 0,3},	{BLUE_PAL, 5, 8}},
		
		3,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	0,	menu_changeformmode,	-1,	2,	0,	WHITE_15, 	{MMNU1 + 1, MMNU1 + 2, MMNU1 + 3}},
		{-1,	-1,	-1,	ITEM_TN,	1,	ITEM_OK,	0,	menu_formationgroup,	0,	0,	0,	ORANGE_15, 	{MMNU1 + 4}},
		{-1,	-1,	-1,	ITEM_TN,	2,	ITEM_OK,	0,	menu_formationattack,	0,	0,	0,	ORANGE_15, 	{MMNU1 + 5}},
		{-1,	-1,	-1,	ITEM_TN,	3,	ITEM_OK,	0,	menu_formationattack,	1,	0,	0,	ORANGE_15, 	{MMNU1 + 6}}	
			}
		},
		
		// Pause Menu (2)
		// --------------
		
		{
		MMNU2 + 0,
		0,	"",
		ControlStart,
		WHITE_15,
		-1,
		
		100,
		20,

		NO_MENU,
		3,	{{RED_PAL, 0,1},	{BLUE_PAL,	3,3},	{GREY_PAL,	5,5}},
				
		3,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	0,	menu_movedown,		PLAYERTYPE_MENU,	0,	0,	YELLOW_15, 	{MMNU2 + 1}},
		{-1,	-1,	-1,	ITEM_TN,	1,	ITEM_OK,	0,	menu_movedown,		GAMETYPE_MENU,		0,	0,	YELLOW_15, 	{MMNU2 + 2}},
		{-1,	-1,	-1,	ITEM_TN,	3,	ITEM_OK,	0,	menu_movedown,		FORMATION_MENU,		0,	0,	YELLOW_15, 	{MMNU2_FORMATION}},
		{-1,	-1,	-1,	ITEM_TN,	5,	ITEM_OK,	0,	menu_quitgame, 		-1,					2,	0,	ORANGE_15,	{MMNU2 + 3, MMNU2 + 4, MMNU2 + 5}},
		
			}
		},
		
		// Select camera menu FOR PLAYER (3)
		// ---------------------------------
		
		{
		MMNU3 + 0,
		0,	"",
		ControlStart,
		WHITE_15,
		-1,
		
		100,	
		20,
		
		PAUSE_MENU,
		2,	{{BLUE_PAL, 0,3},	{GREY_PAL, 5,5}},
		
		4,
		0,
			{
		
		{BASECEL_TICK,	32,	ITEM_PR,	ITEM_TE,	0,	ITEM_OK,	0,	menu_setplayercamera,	0,				0,	0, 	YELLOW_15, 	{MMNU3 + 1}},
		{BASECEL_TICK,	32,	ITEM_PR,	ITEM_TE,	1,	ITEM_OK,	0,	menu_setplayercamera,	1,				0,	0, 	YELLOW_15, 	{MMNU3 + 2}},
		{BASECEL_TICK,	32,	ITEM_PR,	ITEM_TE,	2,	ITEM_OK,	0,	menu_setplayercamera,	2,				0,	0, 	YELLOW_15, 	{MMNU3 + 3}},
		{BASECEL_TICK,	32,	ITEM_PR,	ITEM_TE,	3,	ITEM_OK,	0,	menu_setplayercamera,	3,				0,	0, 	YELLOW_15, 	{MMNU3 + 4}},
		{-1,			-1,	ITEM_PR,	ITEM_TN,	5,	ITEM_OK,	0,	menu_moveup,			0,				0,	0,	BLUE_15,	{MMNU3 + 5}}
			}	
		},
		
		// Select view menu FOR GAME (4)
		// -----------------------------
		
		{
		MMNU4 + 0,
		0,	"",
		ControlStart,
		WHITE_15,
		-1,
		
		100,		
		20,
		
		GAMETYPE_MENU,
		2, {{BLUE_PAL, 0,3},	{GREY_PAL, 5,5}},
		
		4,
		0,
			{
		{BASECEL_TICK,	32,	ITEM_PR,	ITEM_TE,	0,	ITEM_OK,	0,	menu_setgameview,	0,				0,	0, 	YELLOW_15, 	{MMNU4 + 1}},
		{BASECEL_TICK,	32,	ITEM_PR,	ITEM_TE,	1,	ITEM_OK,	0,	menu_setgameview,	1,				0,	0, 	YELLOW_15, 	{MMNU4 + 2}},
		{BASECEL_TICK,	32,	ITEM_PR,	ITEM_TE,	2,	ITEM_OK,	0,	menu_setgameview,	2,				0,	0, 	YELLOW_15, 	{MMNU4 + 3}},
		{BASECEL_TICK,	32,	ITEM_PR,	ITEM_TE,	3,	ITEM_OK,	0,	menu_setgameview,	3,				0,	0, 	YELLOW_15, 	{MMNU4 + 4}},
		{-1,			-1,	-1,			ITEM_TN,	5,	ITEM_OK,	0,	menu_moveup,		0,				0,	0,	BLUE_15,	{MMNU4 + 5}}
			}
					
		},
		
		// Select camera menu FOR GAME (5)
		// -------------------------------
		
		{
		MMNU5 + 0,
		0,	"",
		ControlStart,
		WHITE_15,
		-1,
		
		100,	
		20,
		
		PAUSE_MENU,
		3,	{{BLUE_PAL, 0,3},	{GREEN_PAL, 5,5},	{GREY_PAL, 7,7}},
		
		5,
		0,
			{
		{BASECEL_TICK,	32,	ITEM_PR,	ITEM_TE,	0,	ITEM_OK,	0,	menu_setgamecamera,	0,				0,	0, 	YELLOW_15, 	{MMNU5 + 1}},
		{BASECEL_TICK,	32,	ITEM_PR,	ITEM_TE,	1,	ITEM_OK,	0,	menu_setgamecamera,	1,				0,	0, 	YELLOW_15, 	{MMNU5 + 2}},
		{BASECEL_TICK,	32,	ITEM_PR,	ITEM_TE,	2,	ITEM_OK,	0,	menu_setgamecamera,	2,				0,	0, 	YELLOW_15, 	{MMNU5 + 3}},
		{BASECEL_TICK,	32,	ITEM_PR,	ITEM_TE,	3,	ITEM_OK,	0,	menu_setgamecamera,	3,				0,	0, 	YELLOW_15, 	{MMNU5 + 4}},
		{-1,			-1,	-1,			ITEM_TN,	5,	ITEM_OK,	0,	menu_movedown,		GAMEVIEW_MENU,	0,	0,	YELLOW_15,	{MMNU5 + 5}},
		{-1,			-1,	ITEM_PR,	ITEM_TN,	7,	ITEM_OK,	0,	menu_moveup,		0,				0,	0,	BLUE_15,	{MMNU5 + 6}}
			}	
		},
		
		// Camera Player Position (6)
		// --------------------------
		
		{
		MMNU6 + 0,
		0,	"",
		ControlStart,
		WHITE_15,
		-1,
		
		100,	
		20,
		
		PLAYERTYPE_MENU,
		0,{},
		
		-1,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	0,	menu_moveup,	0,	0,	0,	0,		{0}}
			}
		},
		
		// Camera Game Position (7)
		// ------------------------
		
		{
		MMNU7 + 0,
		0,	"",
		ControlStart,
		WHITE_15,
		-1,
		
		100,	
		20,
		
		GAMETYPE_MENU,
		0,{},
		
		-1,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	0,	menu_moveup,	0,	0,	0,	0,		{0}}
			}
		},
		
		// Select Mission (8)
		// -------------------
		
		{
		MMNU8 + 0,
		0,	"",
		ControlStart,
		WHITE_15,
		-1,
		
		100,	
		20,
		
		MISSION_MENU,
		0,{},
		
		-1,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	0,	NULL,			0,	0,	0,	0,		{0}}
			}
		},
		
		// Main Level Select (9)
		// ---------------------
		
		{
		MMNU9 + 0,
		1,	"Level",
		ControlStart,
		WHITE_15,
		0,
		
		100,		
		20,
				
		MAIN_MENU,
		2,{{BLUE_PAL, 0,3},		{GREY_PAL, 5,5}},
		
		4,
		0,
			{
		{BASECEL_LEVEL_L1,	32,	ITEM_PB,	ITEM_TE,	0,	ITEM_OK,	0,	menu_setlevel,	0,	0,	0,	WHITE_15,	{MMNU9 + 1}},
		{BASECEL_LEVEL_L2,	32,	ITEM_PB,	ITEM_TE,	1,	ITEM_OK,	0,	menu_setlevel,	1,	0,	0,	WHITE_15,	{MMNU9 + 2}},
		{BASECEL_LEVEL_L3,	32,	ITEM_PB,	ITEM_TE,	2,	ITEM_OK,	0,	menu_setlevel,	2,	0,	0,	WHITE_15,	{MMNU9 + 3}},
		{BASECEL_LEVEL_L4,	32,	ITEM_PB,	ITEM_TE,	3,	ITEM_OK,	0,	menu_setlevel,	3,	0,	0,	WHITE_15,	{MMNU9 + 4}},
		{-1,				-1,	-1,			ITEM_TN,	5,	ITEM_OK,	0,	menu_moveup,	0,	0,	0,	BLUE_15,	{MMNU9 + 5}}
			}
		},
		
		// Sound Menu (10)
		// ---------------
		
		{
		MMNU10 + 0,
		1,	"Music",
		ControlStart,
		WHITE_15,
		-1,
		
		100,	
		20,
		
		CONFIGURE_MENU,
		2,	{{BLUE_PAL, 0,2},	{GREY_PAL, 4,4}},
		
		3,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	0,	menu_musicset,	-1,				1,	1,	WHITE_15,	{MMNU10 + 1, MMNU10 + 2}},
		{-1,	-1,	-1,	ITEM_TN,	1,	ITEM_OK,	0,	menu_soundset,	-1,				1,	1, 	WHITE_15, 	{MMNU10 + 3, MMNU10 + 4}},
		{-1,	-1,	-1,	ITEM_TN,	2,	ITEM_OK,	0,	menu_movedown,	JUKEBOX_MENU,	0,	0,	YELLOW_15, 	{MMNU10 + 5}},
		{-1,	-1,	-1,	ITEM_TN,	4,	ITEM_OK,	0,	menu_moveup,	0,				0,	0,	BLUE_15,	{MMNU10 + 6}}
			}	
		},
		
		// Jukebox Menu (11)
		// -----------------
		
		{
		MMNU11 + 0,
		1,	"Music",
		ControlStart,
		WHITE_15,
		-1,
		
		100,	
		20,
		
		SOUND_MENU,
		2,	{{BLUE_PAL, 0,6},	{GREY_PAL, 8,8}},
		
		7,
		0,
			{
		{-1,	-1,	-1,	ITEM_TM_S,	0,	ITEM_OK,	0,	menu_trackset,	0,	0,	0,	YELLOW_15,	{MMNU11 + 1}},
		{-1,	-1,	-1,	ITEM_TM_S,	1,	ITEM_OK,	0,	menu_trackset,	1,	0,	0,	YELLOW_15,	{MMNU11 + 2}},
		{-1,	-1,	-1,	ITEM_TM_S,	2,	ITEM_OK,	0,	menu_trackset,	2,	0,	0,	YELLOW_15,	{MMNU11 + 3}},
		{-1,	-1,	-1,	ITEM_TM_S,	3,	ITEM_OK,	0,	menu_trackset,	3,	0,	0,	YELLOW_15,	{MMNU11 + 4}},
		{-1,	-1,	-1,	ITEM_TM_S,	4,	ITEM_OK,	0,	menu_trackset,	4,	0,	0,	YELLOW_15,	{MMNU11 + 5}},
		{-1,	-1,	-1,	ITEM_TM_S,	5,	ITEM_OK,	0,	menu_trackset,	5,	0,	0,	YELLOW_15,	{MMNU11 + 6}},
		{-1,	-1,	-1,	ITEM_TM_S,	6,	ITEM_OK,	0,	menu_trackset,	6,	0,	0,	YELLOW_15,	{MMNU11 + 7}},
		{-1,	-1,	-1,	ITEM_TN,	8,	ITEM_OK,	0,	menu_moveup,	0,	0,	0,	BLUE_15,	{MMNU11 + 8}}
			}	
		},
		
		// Load Game (12)
		// --------------
		
		{
		MMNU12 + 0,
		0,	"",
		ControlStart,
		WHITE_15,
		0,
		
		100,		
		20,
				
		MAIN_MENU,
		2,	{{BLUE_PAL, 0,6},		{GREY_PAL, 8,8}},
		
		7,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	MAIN_LOADSTART,	menu_loadgame,	0,	0,	0,	ORANGE_15,	{MMNU12 + 1}},
		{-1,	-1,	-1,	ITEM_TN,	1,	ITEM_OK,	MAIN_LOADSTART,	menu_loadgame,	1,	0,	0,	ORANGE_15,	{MMNU12 + 2}},
		{-1,	-1,	-1,	ITEM_TN,	2,	ITEM_OK,	MAIN_LOADSTART,	menu_loadgame,	2,	0,	0,	ORANGE_15,	{MMNU12 + 3}},
		{-1,	-1,	-1,	ITEM_TN,	3,	ITEM_OK,	MAIN_LOADSTART,	menu_loadgame,	3,	0,	0,	ORANGE_15,	{MMNU12 + 4}},
		{-1,	-1,	-1,	ITEM_TN,	4,	ITEM_OK,	MAIN_LOADSTART,	menu_loadgame,	4,	0,	0,	ORANGE_15,	{MMNU12 + 5}},
		{-1,	-1,	-1,	ITEM_TN,	5,	ITEM_OK,	MAIN_LOADSTART,	menu_loadgame,	5,	0,	0,	ORANGE_15,	{MMNU12 + 6}},
		{-1,	-1,	-1,	ITEM_TN,	6,	ITEM_OK,	MAIN_LOADSTART,	menu_loadgame,	6,	0,	0,	ORANGE_15,	{MMNU12 + 7}},
		{-1,	-1,	-1,	ITEM_TN,	8,	ITEM_OK,	0,				menu_moveup,	0,	0,	0,	BLUE_15,	{MMNU12 + 8}}
			}
		},

		// Save Game (13)
		// --------------
		
		{
		MMNU13 + 0,
		0,	"",
		ControlStart,
		WHITE_15,
		0,
		
		100,		
		20,
				
		MAIN_MENU,
		2,	{{BLUE_PAL, 0,6},		{GREY_PAL, 8,8}},
		
		7,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	0,	menu_savegame,	0,	0,	0,	ORANGE_15,	{MMNU13 + 1}},
		{-1,	-1,	-1,	ITEM_TN,	1,	ITEM_OK,	0,	menu_savegame,	1,	0,	0,	ORANGE_15,	{MMNU13 + 2}},
		{-1,	-1,	-1,	ITEM_TN,	2,	ITEM_OK,	0,	menu_savegame,	2,	0,	0,	ORANGE_15,	{MMNU13 + 3}},
		{-1,	-1,	-1,	ITEM_TN,	3,	ITEM_OK,	0,	menu_savegame,	3,	0,	0,	ORANGE_15,	{MMNU13 + 4}},
		{-1,	-1,	-1,	ITEM_TN,	4,	ITEM_OK,	0,	menu_savegame,	4,	0,	0,	ORANGE_15,	{MMNU13 + 5}},
		{-1,	-1,	-1,	ITEM_TN,	5,	ITEM_OK,	0,	menu_savegame,	5,	0,	0,	ORANGE_15,	{MMNU13 + 6}},
		{-1,	-1,	-1,	ITEM_TN,	6,	ITEM_OK,	0,	menu_savegame,	6,	0,	0,	ORANGE_15,	{MMNU13 + 7}},
		{-1,	-1,	-1,	ITEM_TN,	8,	ITEM_OK,	0,	menu_moveup,	0,	0,	0,	BLUE_15,	{MMNU13 + 8}}
			}
		},
		
		// Configure Menu (14)
		// -------------------
		
		{
		MMNU14 + 0,
		0,	"",
		ControlStart,
		WHITE_15,
		-1,
		
		100,
		20,
		
		MAIN_MENU,
		3,	{{RED_PAL, 0,2},	{BLUE_PAL, 4, 5},	{GREY_PAL, 7,7}},
		
		5,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	0,	menu_movedown,		SOUND_MENU,		0,	0,	YELLOW_15, 	{MMNU14 + 1}},
		{-1,	-1,	-1,	ITEM_TN,	1,	ITEM_OK,	0,	menu_movedown,		CONTROLS_MENU,	0,	0,	YELLOW_15, 	{MMNU14 + 2}},
		{-1,	-1,	-1,	ITEM_TN,	2,	ITEM_OK,	0,	menu_movedown,		LANGUAGE_MENU,	0,	0,	YELLOW_15,	{MMNU14 + 3}},
		{-1,	-1,	-1,	ITEM_TN,	4,	ITEM_OK,	0,	menu_setpilotname,	0,				0,	0,	YELLOW_15,	{MMNU_GETNAME+1}},
		{-1,	-1,	-1,	ITEM_TN,	5,	ITEM_OK,	0,	menu_configdefault,	0,				0,	0,	YELLOW_15,	{MMNU14 + 4}},
		{-1,	-1,	-1,	ITEM_TN,	7,	ITEM_OK,	0,	menu_moveup,		0,				0,	0,	BLUE_15,	{MMNU14 + 5}}
			}
		},

		// Controls Menu (15)
		// ------------------
		
		{
		MMNU15 + 0,
		1,	"Controls",
		ControlStart,
		WHITE_15,
		0,
		
		100,
		20,
		
		CONFIGURE_MENU,
		3,	{{BLUE_PAL, 0,1},	{RED_PAL, 3,4},	{GREY_PAL, 8,8}},
		
		4,
		0,
			{
		{BASECEL_CONTROL_TICK,	32,	ITEM_PR,	ITEM_TE,	0,	ITEM_OK,	0,	menu_setcontrol,	0,					0,	0,	WHITE_15, 	{MMNU15 + 1}},
		{BASECEL_CONTROL_TICK,	32,	ITEM_PR,	ITEM_TE,	1,	ITEM_OK,	0,	menu_setcontrol,	1,					0,	0,	WHITE_15, 	{MMNU15 + 2}},
		{-1,					-1,	-1,			ITEM_TN,	3,	ITEM_OK,	0,	menu_setupstick,	0,					0,	0,	YELLOW_15,	{MMNU15 + 3}},
		{-1,					-1,	-1,			ITEM_TN,	4,	ITEM_OK,	0,	menu_enter,			FLIGHTCONTROL_MENU,	0,	0,	YELLOW_15,	{MMNU15 + 4}},
		{-1,					-1,	-1,			ITEM_TN,	8,	ITEM_OK,	0,	menu_moveup,		0,					0,	0,	BLUE_15,	{MMNU15 + 5}}
			}
		},
		
		// Debrief Menu (16)
		// -----------------
		
		{
		MMNU16 + 0,
		1,	"Alphabet",
		ControlStart,
		WHITE_15,
		0,
		
		100,
		20,
		
		DEBRIEF_MENU,
		2,	{{RED_PAL, 0,1},	{BLUE_PAL, 4,6}},
		
		-1,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	0,	NULL,			0,	0,	0,	0,		{0}}
			}
		},
		
		// Set Language Menu (17)
		// ----------------------
		
		{
		MMNU17 + 0,
		1,	"Language",
		ControlStart,
		WHITE_15,
		-1,
		
		100,	
		20,
		
		LANGUAGE_MENU,
		1,	{{BLUE_PAL, 1,6}},
		
		2,
		0,
			{
		{BASECEL_LANGUAGE_ENGLISH,	48,	ITEM_PB | ITEM_PA,	ITEM_TE,	1,	ITEM_OK,	0,	menu_setlanguage,	0,	0,	0,	YELLOW_15,	{MMNU17 + 1}},
		{BASECEL_LANGUAGE_FRENCH,	48,	ITEM_PB | ITEM_PA,	ITEM_TE,	3,	ITEM_OK,	0,	menu_setlanguage,	1,	0,	0,	YELLOW_15,	{MMNU17 + 2}},
		{BASECEL_LANGUAGE_GERMAN,	48,	ITEM_PB | ITEM_PA,	ITEM_TE,	5,	ITEM_OK,	0,	menu_setlanguage,	2,	0,	0,	YELLOW_15,	{MMNU17 + 3}}
			}	
		},
		
		// High Score Game (18)
		// --------------------
		
		{
		MMNU18 + 0,
		1,	"Alphabet",
		ControlStart,
		WHITE_15,
		0,
		
		100,		
		20,
				
		MAIN_MENU,
		0,{},
		
		-1,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	0,	NULL,	0,	0,	0,	ORANGE_15,	{0}}
			}
		},
		
		// Flight Controls Menu (19)
		// -------------------------
		
		{
		MMNU19 + 0,
		0,	"",
		ControlStart,
		WHITE_15,
		-1,
		
		100,	
		20,
		
		CONTROLS_MENU,
		2,	{{BLUE_PAL, 0,7},	{GREY_PAL, 8,8}},
		
		8,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	0,	menu_setflightc,	1,	0,	0,	YELLOW_15,	{MMNU19 + 1}},
		{-1,	-1,	-1,	ITEM_TN,	1,	ITEM_OK,	0,	menu_setflightc,	1,	0,	0,	YELLOW_15,	{MMNU19 + 2}},
		{-1,	-1,	-1,	ITEM_TN,	2,	ITEM_OK,	0,	menu_setflightc,	1,	0,	0,	YELLOW_15,	{MMNU19 + 3}},
		{-1,	-1,	-1,	ITEM_TN,	3,	ITEM_OK,	0,	menu_setflightc,	1,	0,	0,	YELLOW_15,	{MMNU19 + 4}},
		{-1,	-1,	-1,	ITEM_TN,	4,	ITEM_OK,	0,	menu_setflightc,	1,	0,	0,	YELLOW_15,	{MMNU19 + 5}},
		{-1,	-1,	-1,	ITEM_TN,	5,	ITEM_OK,	0,	menu_setflightc,	1,	0,	0,	YELLOW_15,	{MMNU19 + 6}},
		{-1,	-1,	-1,	ITEM_TN,	6,	ITEM_OK,	0,	menu_setflightc,	1,	0,	0,	YELLOW_15,	{MMNU19 + 7}},
		{-1,	-1,	-1,	ITEM_TN,	7,	ITEM_OK,	0,	menu_setflightc,	1,	0,	0,	YELLOW_15,	{MMNU19 + 8}},
		{-1,	-1,	-1,	ITEM_TN,	8,	ITEM_OK,	0,	menu_saveflightc,	0,	0,	0,	WHITE_15,	{MMNU19 + 9}}
			}	
		},
		
		// Calibrate Joystick (20)
		// -----------------------
		
		{
		MMNU20 + 0,
		
		0,	"",
		ControlStart,
		WHITE_15,
		-1,
		
		100,	
		20,
		
		CONTROLS_MENU,
		0,{},
		
		-1,
		0,
			{
		{-1,	-1,	-1,	ITEM_TN,	0,	ITEM_OK,	0,	NULL,			0,	0,	0,	0,		{0}}
			}
		}
};

									
/***************************************************************************************************/
/*										MENU UPDATE ROUTINES									   */
/***************************************************************************************************/

void	menu_moveup (long nothing)

// Purpose : Enters previous menu without resetting it's position to top
// Accepts : Nothing
// Returns : Nothing

{

	// If we are moving up from flightcontrol menu, check there are no clashes, if there are, do not move
	
	if (menu_status.current_menu == FLIGHTCONTROL_MENU)
		if (menu_endflightc (0) == 0)
			return;
	
	// O.K. move to previous menu
	
	menu_enter (menus [menu_status.current_menu].previous, 0);
}

/**************************************/

void	menu_movedown (long new_menu)

// Purpose : Enters sub menu (setting it's position to top)
// Accepts : Nothing
// Returns : Nothing

{
	menu_enter (new_menu, 1);
}

/**************************************/

void	menu_enter (long current_menu, long reset_to_top)	

// Purpose : Setup menu parameters & text when we enter a new menu (or close all menus)
// Accepts : New menu to enter (-1 to close all), and whether to reset cursor position to top (1 = yes)
// Returns : Nothing

{

long		cel_base=0,
			option_draw,
			width,
			height,
			item_ypos,
			item_variable;

char		item_variabletext [16];
menu_item	*item_draw;
	
	//	CLEAR ALL MENU TEXT & STOP ENGINE SOUNDS
	
	stop_engine_sounds();
	
	for (cel_base = 0; cel_base < FONT_MAXMENU; cel_base++)
		{
		SetTextCelSize (cel_text [cel_base], 0, 0);
		(cel_text [cel_base] -> tc_CCB) -> ccb_Flags &= ~CCB_SKIP;
		}
		
	//	SET THIS MENU AS THE CURRENT ONE, AND RETURN IF WE ARE LEAVING MENU (-1)

	if (menu_status.current_menu != current_menu)
		{
		menu_status.hidden = 0;
		menu_status.current_menu = current_menu;
		}
	
	if (current_menu == NO_MENU)
		{
		// PUT IN CALL TO RE-ENABLE SOUNDS HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		return;
		}
	// SHOULD WE BE LOADING IN A GAMESET OF SPRITES FOR THIS MENU ?
	
	if (menus [current_menu].reload_gamesprites == 1)
		load_gamecels (menus [current_menu].gamesprites);

	//	MOVING DOWN TO A SUBMENU ? IF SO, RESET IT'S SELECTED OPTION TO TOP
	
	if (reset_to_top == 1)
		menus [current_menu].cur_option = 0;
		
	// CHECK TO UPDATE OPTIONS IN RESPECT OF CURRENT STATUS
		
	switch (current_menu)
		{

		// If pause menu, if not on option 2 (quit), cancel 'confirm'
		
		case PAUSE_MENU :
			if (menus [PAUSE_MENU].cur_option != 3)
				{
				menus [PAUSE_MENU].items [3].cur_selection = 0;				// If we're not on 'Quit' then reset it !
				menus [PAUSE_MENU].items [3].item_colour = ORANGE_15;
				}
			
			menus [PAUSE_MENU].items [2].selectable = (menu_displayformation (0) == 0) ? ITEM_NOTOK : ITEM_OK;	// Form Control ?
			break;
			
		case PLAYERTYPE_MENU :
			control_changecamera (1);
			menus [PLAYERTYPE_MENU].tick_position = camera[1].type;
			menus [PLAYERTYPE_MENU].cur_option = camera[1].type;
			if (camera [1].type == 0)
				strcpy (message_decode (menus [PLAYERTYPE_MENU].items [0].string [0], 1), message_decode (MMNU5_MOVE_CAM, 1));
			else
				strcpy (message_decode (menus [PLAYERTYPE_MENU].items [0].string [0], 1), message_decode (MMNU5_EXTERNAL_CAM, 1));
			break;

		case GAMETYPE_MENU :
			control_changecamera (2);
			menus [GAMETYPE_MENU].tick_position = camera[2].type;
			menus [GAMETYPE_MENU].cur_option = camera[2].type;
			if (camera [2].type == 0)
				strcpy (message_decode (menus [GAMETYPE_MENU].items [0].string [0], 1), message_decode (MMNU5_MOVE_CAM, 1));
			else
				strcpy (message_decode (menus [GAMETYPE_MENU].items [0].string [0], 1), message_decode (MMNU5_EXTERNAL_CAM, 1));
			break;
		
		// If game view menu, set tick & current option to current camera setting
		
		case GAMEVIEW_MENU :
			menus [GAMEVIEW_MENU].tick_position = camera[2].view;
			menus [GAMEVIEW_MENU].cur_option = camera[2].view;
			break;
		
		// If formation control, make formation controls unselectable if control mode is not normal
		
		case FORMATION_MENU :
			switch (status.formation_mode)
				{
				case SDB_FORMATION_NORMAL :
					menus [FORMATION_MENU].items[1].selectable = ITEM_OK;
					menus [FORMATION_MENU].items[2].selectable = ITEM_OK;
					menus [FORMATION_MENU].items[3].selectable = ITEM_OK;
					break;
				
				default :
					menus [FORMATION_MENU].items[1].selectable = ITEM_NOTOK;
					menus [FORMATION_MENU].items[2].selectable = ITEM_NOTOK;
					menus [FORMATION_MENU].items[3].selectable = ITEM_NOTOK;
					break;
				}
		
		// If camera pos or game pos menu, set fixed camera to zero
		
		case GAMEPOS_MENU :
		case PLAYERPOS_MENU :
			menu_setfixedcamera=0;
			break;

		
		// If nvram load, load in directory into menu items
		
		case LOAD_MENU :
			nvram_directory (LOAD_MENU, 0, 6, ITEM_NOTOK, GAMEFILE_PREFIX);
			break;

		// If controls menu, check for flightstick connection
		
		case CONTROLS_MENU :
			
			// If control is set to flightstick & flightstick not connected, then set back to joypad
			
			if (joystick_registerports () == 0)
				if (configuration.control_method == 1)
					configuration.control_method = 0;

			// Set tick to current control selected
			
			menus [CONTROLS_MENU].items [1-configuration.control_method].selectable = ITEM_OK;
			menus [CONTROLS_MENU].items [configuration.control_method].selectable = ITEM_OK;
			menus [CONTROLS_MENU].tick_position = configuration.control_method;
			
			// Shade out 'Setup flightstick' if not selected
			
			if (configuration.control_method == 1)
				menus [CONTROLS_MENU].items [2].selectable = ITEM_OK;
			else
				menus [CONTROLS_MENU].items [2].selectable = ITEM_NOTOK;
			break;
				
		// If set flight controls menu, check for clashes, if none, allow exit
		
		case FLIGHTCONTROL_MENU :
			sprintf (message_decode (menus [FLIGHTCONTROL_MENU].items [8].string [0], 1), "%s", (menu_endflightc (0) == 0) ? message_decode (MMNU19_EXITS+0, 1) : message_decode (MMNU19_EXITS+1, 1));
			break;
		
		// If jukebox menu, set items selected value to configured track list
		
		case JUKEBOX_MENU :
			for (option_draw = 0; option_draw < 7; option_draw++)
				menus [JUKEBOX_MENU].items [option_draw].tick_status = (configuration.music_tracks [option_draw] == 1) ? ITEM_TM_S : ITEM_TM_C;				
		}
		
	//	DISPLAY CURRENT MENU

	menu_status.cel_textccb = cel_text [0] -> tc_CCB;
	cel_base=0;
	
	// DRAW MENU TITLE
	
	SetTextCelColor(cel_text [cel_base], 0, menus [current_menu].title_colour);
	UpdateTextInCel(cel_text [cel_base], TRUE, message_decode (menus [current_menu].title, 1));
	GetTextCelSize (cel_text [cel_base], &width, &height);
	SetTextCelCoords(cel_text [cel_base], 160-(width>>1), (long) menus [current_menu].ypos -2);
	(cel_text [cel_base]->tc_CCB) -> ccb_Flags &= ~CCB_LAST;
	cel_base++;
	
	// DRAW MENU OPTIONS (IF IT HAS ANY)
	
	if (menus [current_menu].max_option >=0)
		{
		for (option_draw=0; option_draw <= menus [current_menu].max_option; option_draw++)
			{
			
			// DRAW MENU OPTION
			
			item_draw = & (menus [current_menu].items [option_draw]);
			
			// SHOULD WE SHADE THE ITEM BECAUSE IT'S ONE OF A SET NOT CURRENTLY SELECTED ?
			
			if (item_draw->tick_status == ITEM_TM_S || item_draw->tick_status == ITEM_TN || (item_draw->tick_status == ITEM_TE && option_draw == menus [current_menu].tick_position))
				SetTextCelColor(cel_text [cel_base], 0, item_draw->item_colour);
			else
				SetTextCelColor(cel_text [cel_base], 0, GREY_15);
			
			// SHOULD WE SHADE THE ITEM BECAUSE IT'S NOT SELECTABLE ?
			
			if (item_draw->selectable == ITEM_NOTOK)
				SetTextCelColor(cel_text [cel_base], 0, DARK_15);
				
			// ADD TEXT ITEM
			// NOTE : 	If we are in the setup flight controls menu, put the options to the LEFT
			//			and insert the current flightcontrol for the option on the RIGHT
			
			UpdateTextInCel(cel_text [cel_base], TRUE, message_decode (item_draw->string [item_draw->cur_selection], 1));
				
			if (current_menu == FLIGHTCONTROL_MENU && option_draw <=7)
				SetTextCelCoords(cel_text [cel_base], 50, MENU_YOFFS + (MENU_YSIZE*item_draw->y_position)+menus [current_menu].ypos);
			else
				{
				GetTextCelSize (cel_text [cel_base], &width, &height);
				SetTextCelCoords(cel_text [cel_base], 160-(width>>1), MENU_YOFFS + (MENU_YSIZE*item_draw->y_position)+menus [current_menu].ypos);
				}
				
			(cel_text [cel_base] -> tc_CCB) -> ccb_Flags &= ~CCB_LAST;
			item_draw->tccbptr = cel_text [cel_base];
			cel_base++;
			}
			
		// Draw all control methods for set controls ?
		
		if (current_menu == FLIGHTCONTROL_MENU)
			for (option_draw = 0; option_draw <=7; option_draw++)
				{
				item_draw = & (menus [current_menu].items [option_draw]);
				SetTextCelColor(cel_text [cel_base], 0, WHITE_15);
				UpdateTextInCel(cel_text [cel_base], TRUE, message_decode (MMNU19_CONTROLS + configuration.flight_controls [option_draw] + ((option_draw == 7) ? 7 : 0), 1));
				GetTextCelSize (cel_text [cel_base], &width, &height);
				SetTextCelCoords(cel_text [cel_base], 270-width, MENU_YOFFS + (MENU_YSIZE*item_draw->y_position)+menus [current_menu].ypos);
				(cel_text [cel_base] -> tc_CCB) -> ccb_Flags &= ~CCB_LAST;
				cel_base++;
				}			
			
		// Draw end of mission stats ?
		
		if (current_menu == DEBRIEF_MENU)
			{
			for (option_draw = 0; option_draw <=4; option_draw++)
				{
				switch (option_draw)
					{
					case 0 :
						item_variable = status.score;
						item_ypos = 0;
						break;
					case 1 :
						item_variable = status.missions_completed;
						item_ypos = 1;
						break;
					case 2 : 
						item_variable = status.hits_taken;
						item_ypos = 4;
						break;
					case 3 : 
						item_variable = status.enemy_aircount;
						item_ypos = 5;
						break;
					default : 
						item_variable = status.enemy_groundcount;
						item_ypos = 6;
						break;
					}
				
				SetTextCelColor(cel_text [cel_base], 0, (option_draw < 2) ? WHITE_15 : GREEN_15);
				UpdateTextInCel(cel_text [cel_base], TRUE, message_decode (MMNU16_INFO + option_draw, 1));
				SetTextCelCoords(cel_text [cel_base], 50, MENU_YOFFS + (MENU_YSIZE*(option_draw+4))+menus [current_menu].ypos);
				(cel_text [cel_base] -> tc_CCB) -> ccb_Flags &= ~CCB_LAST;
				cel_base++;
				
				SetTextCelColor(cel_text [cel_base], 0, (option_draw < 2) ? YELLOW_15 : RED_15);
				sprintf (item_variabletext, "%d", item_variable);
				UpdateTextInCel(cel_text [cel_base], TRUE, item_variabletext);
				GetTextCelSize (cel_text [cel_base], &width, &height);
				SetTextCelCoords(cel_text [cel_base], 270-width, MENU_YOFFS + (MENU_YSIZE*(option_draw+4))+menus [current_menu].ypos);
				(cel_text [cel_base] -> tc_CCB) -> ccb_Flags &= ~CCB_LAST;
				cel_base++;
				}
			}
	}			
	
	(cel_text [cel_base-1] -> tc_CCB) -> ccb_Flags |= CCB_LAST;
}

/**************************************/

long	menu_update (long keypad, long keypad_constant)

// Purpose : Control menu using keypad
// Accepts : Nothing
// Returns : Command selected number, (or 0 for no / nothing command)

{
menu		*current_menu;
menu_item	*item_update;

	// UPDATE MENU TIMER
	
	menu_countdown = (menu_countdown+1) & 31;
	
	// GET CURRENT MENU
	current_menu = & menus [menu_status.current_menu];
	
	// DEBOUNCE SELECTOR ?
	if (menu_status.selected == 1 && (keypad & ControlA) == 0)
		menu_status.selected=0;
			
	// UNHIDE MENU ?
	if (!(keypad_constant & ControlA))
		menu_status.hidden = 0;
	

	// MAIN PRE - UPDATE SWITCH

	switch (menu_status.current_menu)
		{
		case PLAYERPOS_MENU :
			menu_updatecamerapos (keypad_constant, keypad, 1);			// Update PLAYER camera position menu ?
			break;
		case GAMEPOS_MENU :
			menu_updatecamerapos (keypad_constant, keypad, 2);			// Update GAME camera position menu ?
			break;
		case SOUND_MENU :
			menu_updatesoundmenu (keypad, current_menu->cur_option);	// Update Sound & Music menu ?
			break;
		case JUKEBOX_MENU :
			menu_updatejukeboxmenu (keypad);							// Update jukebox menu ?
			break;
		case FLIGHTCONTROL_MENU :
			menu_updateflightc (keypad);								// Update flight controls menu ?
			break;
}
					
	// CHECK FOR RETURN TO PARENT MENU
	
	if (keypad & (ControlStart | ControlX))
		{
		keypad &= (~(ControlX | ControlStart));
		sound_playsample (SOUND_CLICK, 70, 127, 0);
		menu_moveup (0);
		return (0);
		}
			
	// UPDATE MENU SELECTIONS ETC. (only if menu has at least 1 option)
	
	if (current_menu->max_option >=0)
		{
		
		item_update = & (current_menu->items [current_menu->cur_option]);	// Current selection
		
		// CHECK FOR OPTION SELECTION (sub-menu or advance selection)
			
		if ((keypad & ControlA) && item_update->selectable == ITEM_OK)
			{
			
			sound_playsample (SOUND_CLICK, 60, 127, 0);
			
			// DECIDE WHETHER TO TICK / UNTICK THIS (AND OTHER) MENU OPTIONS
			
			switch (item_update->tick_status)
				{
				case ITEM_TE :
					current_menu->tick_position = current_menu->cur_option;
					break;
				case ITEM_TM_C :
					item_update->tick_status = ITEM_TM_S;
					break;
				case ITEM_TM_S :
					item_update->tick_status = ITEM_TM_C;
					break;
				}
			
			// SET SELECTED FLAG TO 1, AND INCREMENT SELECTIONS OPTION
			
			menu_status.selected = 1;
			if (++item_update->cur_selection > item_update->max_selection)
				item_update->cur_selection=0;
			
			// CALL FUNCTION FOR ITEM SELECTED (WITH PARAMETER)
			
			if (item_update -> function != NULL)			// Call correct function (if defined)
				{
				if (item_update->function_value == -1)
					item_update->function (item_update->cur_selection);
				else
					item_update->function (item_update->function_value);
				}
				
			if (current_menu == &menus [menu_status.current_menu])
				menu_enter (menu_status.current_menu, 0);	// Re-do menu (IF WE HAVEN'T JUST CHANGED)
				

			return (item_update -> command);				// Return
			}

		// CHECK FOR MOVING UP AN OPTION
	
		if (keypad & ControlUp)
			{
			sound_playsample (SOUND_BEEP, 60, 80, 0);
			
			if ((current_menu -> cur_option) >0 )
				current_menu -> cur_option--;
			else
				current_menu -> cur_option = current_menu -> max_option;
			}
			
		// CHECK FOR MOVING DOWN AN OPTION
	
		if (keypad & ControlDown)
			{
			sound_playsample (SOUND_BEEP, 60, 80, 0);
			
			if ((current_menu -> cur_option) < (current_menu -> max_option))
				current_menu -> cur_option++;
			else
				current_menu -> cur_option = 0;
			}
		}
		
	return (0);
}

/**************************************/

void	menu_display (long title_background)

// Purpose : Renders shaded boxes and menu pointer to the screen cel list, prior to display
// Accepts : Nothing
// Returns : Nothing

{

long		item_width,
			item_halfwidth,
			menu_displayloop;
menu_item	*item_display;

	if (menu_status.hidden == 0)
		{

		// GET CURRENT MENU
		menu	*current_menu = &menus [menu_status.current_menu];		
					
		// DRAW BACKGROUND SHADING FOR MENU TITLE
		cel_rectangle (25, current_menu->ypos+16, 319-25, current_menu->ypos-8, title_background, REC_BORDER | REC_BOX, 0);

		// DRAW BACKGROUND BORDER SHADES FOR REST OF MENU
		for (menu_displayloop=0; menu_displayloop < current_menu->border_count; menu_displayloop++)
			cel_rectangle (40, current_menu->ypos + (MENU_YSIZE*current_menu->borders [menu_displayloop].option_end)+58, 319-40, current_menu->ypos + (MENU_YSIZE*current_menu->borders [menu_displayloop].option_start)+34, current_menu->borders [menu_displayloop].colour, REC_BORDER | REC_BOX, 1);
		
		// HIGHLIGHT CURRENT SELECTION ?
		if (current_menu->max_option >=0)
			{
			item_display = &(current_menu->items [current_menu->cur_option]);
			item_width = ((item_display->tccbptr->tc_CCB->ccb_Width))+10;
			item_halfwidth = ((item_display->tccbptr->tc_CCB->ccb_Width)>>1)+10;
			
			if (menu_status.current_menu == FLIGHTCONTROL_MENU && current_menu->cur_option <=7)
				cel_rectangle (40, current_menu->ypos + (MENU_YSIZE*item_display->y_position)+58, 50+item_width, current_menu->ypos + (MENU_YSIZE*item_display->y_position)+34, RED_PAL, REC_BORDER | REC_BOX, 1);
			else
				cel_rectangle (160-item_halfwidth, current_menu->ypos + (MENU_YSIZE*item_display->y_position)+58, 160+item_halfwidth, current_menu->ypos + (MENU_YSIZE*item_display->y_position)+34, RED_PAL, REC_BORDER | REC_BOX, 1);
			}

		cel_quad.shade = 11;
		
		// ARE WE DISPLAYING AN EXCLUSIVE TICK ICON ?
		
		if (current_menu->tick_position >=0)
			{
			item_display = & current_menu->items [current_menu->tick_position];
			cel_quad.y_pos0 = (current_menu->ypos + (MENU_YSIZE*item_display->y_position))-82;
			
			if (item_display->item_icon >=0 && (item_display->item_plottype & ITEM_PR))
				{
				cel_quad.x_pos0 = 70;
				arm_addgamecel (&cel_quad, item_display->item_icon, 1024, 1024);
				}
			if (item_display->item_icon >=0 && (item_display->item_plottype & ITEM_PL))
				{
				cel_quad.x_pos0 = -(70+item_display->item_iconwidth);
				arm_addgamecel (&cel_quad, item_display->item_icon, 1024, 1024);
				}
			}
		
		// ARE WE DISPLAYING ICONS AGAINST ANY OF THE OPTIONS IN THE MENU
		
		for (menu_displayloop = 0; menu_displayloop <= current_menu-> max_option; menu_displayloop++)
			{
			item_display = & current_menu->items [menu_displayloop];
			if (item_display->tick_status == ITEM_TM_S || (item_display->item_plottype & ITEM_PA))
				{
				cel_quad.y_pos0 = (current_menu->ypos + (MENU_YSIZE*item_display->y_position))-82;
			
				if (item_display->item_icon >=0 && (item_display->item_plottype & ITEM_PR))
					{
					cel_quad.x_pos0 = 70;
					arm_addgamecel (&cel_quad, item_display->item_icon, 1024, 1024);
					}
				if (item_display->item_icon >=0 && (item_display->item_plottype & ITEM_PL))
					{
					cel_quad.x_pos0 = -(70+item_display->item_iconwidth);
					arm_addgamecel (&cel_quad, item_display->item_icon, 1024, 1024);
					}
				}
			}
		
		// ARE WE PRINTING ANYTHING ELSE FOR OTHER MENU'S ?

		switch (menu_status.current_menu)
			{
			case FORMATION_MENU :
				menu_displayformation (1);					// Display formation ?
				break;
				
			case SOUND_MENU :
				menu_displaymusic ();						// Display volumes etc ?
				break;
			
			case JUKEBOX_MENU :
				menu_displayjukebox ();						// Display currently playing track ?
				break;
			}
		}
}

/***************************************************************************************************/
/*										   MAIN MENU FUNCTIONS									   */
/***************************************************************************************************/

void	menu_setlevel (long level_select)

// Purpose : Select next level
// Accepts : Nothing
// Returns : Nothing

{
	parameters.level = level_select;
	menu_moveup (0);
}

/***************************************************************************************************/
/*										   PAUSE MENU FUNCTIONS									   */
/***************************************************************************************************/

void	menu_quitgame (long confirmed)

// Purpose : Quits game and returns to main menu
// Accepts : Nothing
// Returns : Nothing

{
	if (confirmed == 1)
		menus [PAUSE_MENU].items [3].item_colour = WHITE_15;	// Set 'Confirm' colour to white
	else
		{
		status.status = STATUS_GAMEOVER;						// Else quit game (confirmed)
		menu_moveup(0);
		}
}

/***************************************************************************************************/
/*									   SET CAMERA-VIEW MENU FUNCTIONS							   */
/***************************************************************************************************/

void	menu_setgameview (long viewtype)

// Purpose : Set a new game camera type OR cycle through instances of a type
// Accepts : Selected camera type
// Returns : Nothing

{
	menu_status.hidden = 1;							// Hide menu temporarily, until A is released
	
	if (viewtype == camera[2].view)					// We are already looking at this type, change to next instance
		{
		control_searchcamera (2, ++camera[2].instance);
		}
	else
		{
		camera[2].view = viewtype;					// We are changing to this camera type, set to 1st instance
		control_searchcamera (2, 0);				// Get 1st instance of this type
		}
		message_addcamera (2);						// Display new camera message
}

/***************************************************************************************************/
/*										 	CAMERA MENU FUNCTIONS								   */
/***************************************************************************************************/

void	menu_setplayercamera (long cameratype)

// Purpose : Set a new camera view for the player camera /  or Calls set-camera-position
// Accepts : New camera view type
// Returns : Nothing

{
	if (cameratype == 0 && camera [1].type == 0)	// Are we being asked to select 'external camera' & already on it ?
		menu_movedown (PLAYERPOS_MENU);				// YES, SET CAM POSITION INSTEAD
	else
		{
		menu_status.hidden = 1;						// Hide menu temporarily, until A is released
		camera[1].type = cameratype;				// Set temp camera to look at new type
		message_addcamera (1);						// Display new [PLAYER] camera message
		}
}

/**************************************/

void	menu_setgamecamera (long cameratype)

// Purpose : Set a new camera view for the game camera /  or Calls set-camera-position
// Accepts : New camera view type
// Returns : Nothing

{
	if (cameratype == 0 && camera [2].type == 0)	// Are we being asked to select 'external camera' & already on it ?
		menu_movedown (GAMEPOS_MENU);				// YES, SET CAM POSITION INSTEAD
	else
		{
		menu_status.hidden = 1;						// Hide menu temporarily, until A is released
		camera[2].type = cameratype;				// Set temp camera to look at new type
		message_addcamera (2);						// Display new [GAME] camera message
		}
}

/**************************************/

void	menu_updatecamerapos (long keypad_constant, long keypad, long camera_num)

// Purpose : Allow player to position camera around view for either PLAYER or GAME camera
// Accepts : Keypad presses
// Returns : Nothing

{
		
	if (keypad_constant & ControlRight)					// Rotate CLOCKWISE around view ?
		camera [camera_num].x_rot -= 32<<10;
		
	if (keypad_constant & ControlLeft)					// Rotate ANTI-CLOCKWISE around view ?
		camera [camera_num].x_rot += 32<<10;
		
	if (keypad_constant & ControlDown)					// Rotate UNDERNEATH view ?
		if ((camera [camera_num].y_rot >> 10) < 256)
			camera [camera_num].y_rot+=12<<10;
			
	if (keypad_constant & ControlUp)					// Rotate ABOVE view ?
		if ((camera [camera_num].y_rot >> 10) > -256)
			camera [camera_num].y_rot-=12<<10;
			
	if (keypad_constant & ControlLeftShift)				// Zoom OUT from view ?		(Zoom speed depends on distance)
		if (camera [camera_num].zoom < 6000)
			{
			if (camera [camera_num].zoom < 2000)
				camera [camera_num].zoom +=80;
			else
				camera [camera_num].zoom +=240;
			}
			
	if (keypad_constant & ControlRightShift)			// Zoom IN to view ?		(Zoom speed depends on distance)
		if (camera [camera_num].zoom > 1000)
			{
			if (camera [camera_num].zoom > 2000)
				camera [camera_num].zoom -=240;
			else
				camera [camera_num].zoom -=80;
			}
			
	if (keypad & ControlA)								// Get Fixed Camera View ?
		{
		camera [camera_num].x_rot=menu_setfixedcamera<<18;
		camera [camera_num].y_rot=0;
		camera [camera_num].zoom =1100;
		camera [camera_num].type = CAMERA_NORMAL;
		menu_setfixedcamera=(menu_setfixedcamera+1) & 3;
		}
		
	if (keypad_constant == 0)
		menu_status.hidden = 0;
	else
		menu_status.hidden = 1;
}


/***************************************************************************************************/
/*									   FORMATION CONTROL MENU FUNCTIONS							   */
/***************************************************************************************************/

void	menu_changeformmode (long	new_mode)

// Purpose : Sets new control mode for formation
// Accepts : New control mode
// Returns : Nothing

{
	status.formation_mode = new_mode;

	switch (new_mode)
		{
		case SDB_FORMATION_NORMAL :
			break;
			
		case SDB_FORMATION_DEFENSIVE :
			menu_formationgroup(0);
			break;
			
		case SDB_FORMATION_AGGRESSIVE :
			menu_formationattack (1);
			break;
		}
}

/**************************************/

long	menu_displayformation (long	formation_display)

// Purpose : Display formation's current positions on screen & Returns number of formation
// Accepts : (0 to just count & return number, 1 to display as well)
// Returns : Nothing

{

ship_stack	*ship = (ship_stack*) (ships.info).start_address;

char	names_attack	[16];

long	form_xpos = 0-16,
		form_ypos = 65-16,
		form_search,
		
		friendly_total = 0,
		friendly_totalout = 0,
		friendly_attacking = 0,
		friendly_loop;
	
	while ((ship->header).status == 1)
		{
		if (ship->special_data != NULL)
			{
			if (ship->special_data->side == SDB_SIDE_FRIEND && ship->type >>4 == SMALL_SHIP)
				{
				friendly_total +=1;
				
				if (ship->special_data->formation_position == 0 && friendly_totalout < 8)
					{
					names_attack [friendly_totalout++] = ship->special_data->pilot_name;
					if (ship->special_data->control_mode == SDB_MODE_ATTACK)
						friendly_attacking+=1;
					}
				}
			}	
		ship = (ship_stack*) (ship->header).next_address;
		}
	
	if (formation_display == 0)
		return (friendly_total);
		
	cel_quad.shade = 10;
	
	// DISPLAY PLAYERS SHIP
	
	cel_quad.x_pos0 = form_xpos;
	cel_quad.y_pos0 = form_ypos;
	arm_addgamecel (&cel_quad, BASECEL_FORMATION, 1024, 1024);
	
	// DISPLAY FORMATION
	
	for (form_search = 1; form_search < __WAR_MF; form_search++)
		if (formation [form_search].occupied != NULL)
			{
			cel_quad.x_pos0 = form_xpos + ((formation [form_search].x_offset)>>20);
			cel_quad.y_pos0 = form_ypos + ((formation [form_search].y_offset)>>20);
			arm_addgamecel (&cel_quad, BASECEL_FORMATION+1, 1024, 1024);
			
			cel_quad.y_pos0+=8;
			cel_quad.x_pos0+=16 - (pilot_widths [formation [form_search].occupied->special_data->pilot_name] >> 1);
			arm_addgamecel (&cel_quad, BASECEL_PILOTS + formation [form_search].occupied ->special_data->pilot_name, 1024, 1024);			
			}
	
	// DISPLAY FRIENDLY'S ON FLIGHTPATHS AND IN COMBAT
	
	for (friendly_loop = 0; friendly_loop < friendly_totalout; friendly_loop++)
		{
		cel_quad.x_pos0 = 2+((34*friendly_loop)-((friendly_totalout*34)>>1));
		cel_quad.y_pos0 = 25;
		arm_addgamecel (&cel_quad, BASECEL_FORMATION+1, 1024, 1024);
		
		cel_quad.x_pos0+=7;
		cel_quad.y_pos0+=8;
			
		if (friendly_loop < friendly_attacking)
			arm_addgamecel (&cel_quad, BASECEL_FORMATION+2, 1024, 1024);
		else
			arm_addgamecel (&cel_quad, BASECEL_FORMATION+3, 1024, 1024);
		
		cel_quad.x_pos0+= 9 - (pilot_widths [names_attack [friendly_loop]] >> 1);
		arm_addgamecel (&cel_quad, BASECEL_PILOTS + names_attack [friendly_loop], 1024, 1024);	
		}
	
	return (friendly_total);
}

/**************************************/

void	menu_formationgroup (long nothing)

// Purpose : Regroup friendly formation
// Accepts : Nothing
// Returns : Nothing

{
ship_stack	*ship;
ship_sdb	*ship_sdb_ptr;

	war_squashformationup();

	ship = (ship_stack*) (ships.info).start_address;						// Get start of ship linked list
	while ((ship->header).status == 1)										// While active ship, search
		{
		ship_sdb_ptr = ship->special_data;
		if (ship_sdb_ptr != NULL)											// Does ship have a SDB ?
			if (ship_sdb_ptr->alive == SDB_SHIP_ALIVE)						// Yes, is it alive ?
				if (ship_sdb_ptr->sdb_type != SDB_TYPE_PLAYER)				// Yes, is it not player ?
					if (ship_sdb_ptr->side == SDB_SIDE_FRIEND)				// Yes, Is it a friend ?
						if (ship_sdb_ptr->control_mode == SDB_MODE_ATTACK)	// Yes, Not currently in formation ?
							printf("Recalling ship ->%d\n",war_addshiptoformation (ship, 0));						// Yes, Add it to the formation
					
		ship = (ship_stack*) (ship->header).next_address;					// Get next ship in linked list
		}
}

/**************************************/

void	menu_formationattack (long	all)

// Purpose : Send friendly formation into attack
// Accepts : Flag to send ALL formation into attack (1), or just enough for a 1 to 1 attack (0)
// Returns : Nothing

{

// #define __WAR_SR_LOCAL
// #define __WAR_SR_AREA

long		formation_scan;
ship_stack	*ship;

	for (formation_scan = 0; formation_scan < __WAR_MF; formation_scan++)						// Scan through formation
		{
		ship = formation [formation_scan].occupied;
		if (ship != NULL)																		// Formation pos occupied ?
			if (ship_setmultipletargets (ship, __WAR_SR_AREA, ship->special_data->side) == 1)	// Can we find target ?
				if (war_settoattack (ship, (ship_stack*) ship->target) == 1)					// Could we attack it ?
					{
					ship->special_data->formation_position = 0;									// Clear it's formation pos in SDB
					formation [formation_scan].occupied = NULL;									// Clear ref in formation block
					}
		}
}

/***************************************************************************************************/
/*									 	  MUSIC CONTROL MENU FUNCTIONS							   */
/***************************************************************************************************/

void	menu_trackset (long track_number)

// Purpose : Toggle a track on / off
// Accepts : Nothing
// Returns : Nothing

{
	configuration.music_tracks [track_number] = 1- configuration.music_tracks [track_number];	// Toggle track status
	control_saveconfigure();																	// Try to save it to NVRAM
	
	if (configuration.music_tracks [track_number] == 0)											// Taking track out of PL ?
		music_takefromplaylist (track_number);
	else
		{
		music_addtoplaylist (track_number);														// Putting track in PL !
		if (music_query() == -1 && configuration.music_on == 1)									// Not playing ? Music on ?
			music_play (MUSIC_PLAYFROMTRACK, 0, -1);											// Then start
		}
}

/**************************************/

void	menu_musicset (long music_status)

// Purpose : Change music mode
// Accepts : Nothing
// Returns : Nothing

{
	configuration.music_on = music_status;			// Set music on / off status in configure file
	control_saveconfigure();						// Try to save it to NVRAM
	if (music_status == 1)
		music_play (MUSIC_PLAYFROMTRACK, 0, -1);	// Play tracks in list repeatedly
	else
		music_stop ();								// Stop music playing
}

/**************************************/

void	menu_soundset (long sound_status)

// Purpose : Change sound mode
// Accepts : Nothing
// Returns : Nothing

{
	configuration.sound_on = sound_status;			// Set sound on / off status in configure file
	sound_switchonoroff (sound_status);				// Switch SFX on / off
	control_saveconfigure();						// Try to save it to NVRAM
}

/**************************************/

void	menu_setmusictracks (void)

// Purpose : Builds play list from configured tracks
// Accepts : Nothing
// Returns : Nothing

{

long	loop;
	
	music_resetplaylist();							// Reset tunes to play list
	
	for (loop = 0; loop <= 6; loop++)
		{
		if (configuration.music_tracks [loop] == 1)
			music_addtoplaylist (loop);
		}
}

/**************************************/

void	menu_displaymusic (void)

// Purpose : Displays current music status
// Accepts : Nothing
// Returns : Nothing

{
long	loop;
	
	// DRAW BIG CD-ICON AND MUSIC VOLUME
	
	cel_quad.shade = (configuration.music_on == 1) ? 11 : 4;	// Set shade depending on on / off status
		
	// Draw big-cd icon
	
	cel_quad.x_pos0 = -84-28;
	cel_quad.y_pos0 = -59;
	arm_addgamecel (&cel_quad, BASECEL_BIG_CD, 1024, 1024);
	
	// Draw music volume bars
	
	cel_quad.y_pos0 = -58;
	for (loop = 0; loop < 8; loop++)
		{
		cel_quad.x_pos0 = (29 -(loop * 7)) - 80-24;
		if (loop <= configuration.music_volume)
			arm_addgamecel (&cel_quad, BASECEL_BARS + loop, 1024, 1024);
		}
		
	// DRAW BIG FX-ICON AND SOUND VOLUME
	
	cel_quad.shade = (configuration.sound_on == 1) ? 11 : 4;	// Set shade depending on on / off status
		
	// Draw big-fx icon
	
	cel_quad.x_pos0 = 84-20;
	cel_quad.y_pos0 = -59;
	arm_addgamecel (&cel_quad, BASECEL_BIG_FX, 1024, 1024);
	
	// Draw sound volume bars
	
	cel_quad.y_pos0 = -58;
	for (loop = 0; loop < 8; loop++)
		{
		cel_quad.x_pos0 = (loop * 7) + 100-24;
		if (loop <= configuration.sound_volume)
			arm_addgamecel (&cel_quad, BASECEL_BARS + loop, 1024, 1024);
		}
}

/**************************************/

void	menu_updatesoundmenu (long keypad, long selection)

// Purpose : Updates sound & music menu
// Accepts : Current keypresses
// Returns : Nothing

{

	// Adjusting music bars ?
	
	if (selection == 0 && configuration.music_on == 1)
		{
		if (keypad & ControlRight)
			if (configuration.music_volume > 0)
				{
				configuration.music_volume -=1;									// Music volume down ?
				control_saveconfigure();										// Try to save it to NVRAM
				music_setmastervolume ((configuration.music_volume+1)*16);
				sound_playsample (SOUND_BEEP, 60, 80, 0);
				}
				
		if (keypad & ControlLeft)
			if (configuration.music_volume < 7)
				{
				configuration.music_volume +=1;									// Music volume up ?
				control_saveconfigure();										// Try to save it to NVRAM
				music_setmastervolume ((configuration.music_volume+1)*16);
				sound_playsample (SOUND_BEEP, 60, 80, 0);
				}
		}
		
	// Adjusting sound bars ?
	
	if (selection == 1 && configuration.sound_on == 1)
		{
		if (keypad & ControlLeft)
			if (configuration.sound_volume > 0)
				{
				configuration.sound_volume -=1;									// Sound volume down ?
				control_saveconfigure();										// Try to save it to NVRAM
				sound_setmastervolume ((configuration.sound_volume+1)*16);
				sound_playsample (SOUND_BEEP, 60, 80, 0);
				}
				
		if (keypad & ControlRight)
			if (configuration.sound_volume < 7)
				{
				configuration.sound_volume +=1;									// Sound volume up ?
				control_saveconfigure();										// Try to save it to NVRAM
				sound_setmastervolume ((configuration.sound_volume+1)*16);
				sound_playsample (SOUND_BEEP, 60, 80, 0);
				}
		}
}

/**************************************/

void	menu_updatejukeboxmenu (long keypad)

// Purpose : Updates jukebox menu
// Accepts : Current keypresses
// Returns : Nothing

{

	if (keypad & ControlLeft)
		{
		music_play (MUSIC_MOVETRACK, -1, -1);
		sound_playsample (SOUND_BEEP, 60, 80, 0);
		}
		
	if (keypad & ControlRight)
		{
		music_play (MUSIC_MOVETRACK, +1, -1);
		sound_playsample (SOUND_BEEP, 60, 80, 0);
		}
		
}

/**************************************/

void	menu_displayjukebox (void)

// Purpose : Displays current music status in jukebox menu (currently playing track)
// Accepts : Nothing
// Returns : Nothing

{

long	loop,
		track_no = music_query();

	// Draw big-cd icon
	
	cel_quad.x_pos0 = -84-28;
	cel_quad.y_pos0 = -59;
	cel_quad.shade = (configuration.music_on == 1) ? 11 : 4;	// Set shade depending on on / off status
	arm_addgamecel (&cel_quad, BASECEL_BIG_CD, 1024, 1024);
	
	// Draw music volume bars
	
	cel_quad.y_pos0 = -58;
	for (loop = 0; loop < 8; loop++)
		{
		cel_quad.x_pos0 = (29 -(loop * 7)) - 80-24;
		if (loop <= configuration.music_volume)
			arm_addgamecel (&cel_quad, BASECEL_BARS + loop, 1024, 1024);
		}
		
	// Draw tracks
	
	for (loop = 0; loop < 7; loop++)
		{
		cel_quad.shade = (configuration.music_tracks [loop] == 1) ? 11 : 2;
		cel_quad.y_pos0 = menus[JUKEBOX_MENU].ypos + (MENU_YSIZE*menus[JUKEBOX_MENU].items [loop].y_position)-82;
		cel_quad.x_pos0 = 90;
		
		if (loop == track_no)
			{
			arm_addgamecel (&cel_quad, BASECEL_MUSIC_CD+((menu_countdown>>1) & 3), 1024, 1024);
		
			cel_quad.shade = ((menu_countdown>>2) & 7) + 4;
			cel_quad.x_pos0 = 72 + (((menu_countdown>>2)&7)<<2);
			arm_addgamecel (&cel_quad, BASECEL_MUSIC_CD+4, 1024, 1024);
			}
		else
			arm_addgamecel (&cel_quad, BASECEL_MUSIC_CD, 1024, 1024);
		}
		
	// Draw playlist
	
	cel_quad.x_pos0 = 72;
	cel_quad.shade = 11;
	
	for (loop = 0; loop < MUSIC_PROGRAMLENGTH; loop++)
		{
		track_no = MUSIC_PROGRAM [loop];
		cel_quad.y_pos0 = menus[JUKEBOX_MENU].ypos + (MENU_YSIZE*menus[JUKEBOX_MENU].items [track_no].y_position)-80;
		arm_addgamecel (&cel_quad, BASECEL_NUMBERS + loop, 1024, 1024);
		}
}

/***************************************************************************************************/
/*									 	  NVRAM CONTROL MENU FUNCTIONS							   */
/***************************************************************************************************/

void	menu_loadgame (long slot_toload)

// Purpose : Load in a saved game from NVRAM
// Accepts : Menu item number holding filename
// Returns : Nothing

{
long	loop;
long	delete_loadfile;
long	load_success = 0;

	delete_loadfile = (cheat_feature3 == 0) ? 0 : 1;
	
	// Load file (if delete feature not set)
	
	if (cheat_feature3 == 0)
		{
		if (nvram_load (message_decode (menus [LOAD_MENU].items [slot_toload].string [0], 1), GAMEFILE_PREFIX,(char*)  &save_gamefile, sizeof (gamefile)) == 1)
			{
			
			// CHECK VERSION NUMBER IS VALID, IF SO, COPY OVER
			
			if (save_gamefile.savefile_version == GAMEFILE_VERSION)
				{
				parameters.level = save_gamefile.savefile_level;
				memcpy (&player_performance, &save_gamefile.savefile_performance, sizeof (performance_data));
				memcpy (&status, &save_gamefile.savefile_status, sizeof (game_status));
				memcpy (&camera [1], &save_gamefile.savefile_cameras [0], sizeof (camera_data));
				memcpy (&camera [2], &save_gamefile.savefile_cameras [1], sizeof (camera_data));
				
				for (loop=0; loop < 4; loop++)
					memcpy (&pyramids [loop], &save_gamefile.savefile_pyramids [loop], sizeof (pyramid));
				
				load_success = MAIN_LOADSTART;
				}
			else
				{
				delete_loadfile = 1;
				}
			}
		}
	
	if (delete_loadfile == 1)
		nvram_delete (message_decode (menus [LOAD_MENU].items [slot_toload].string [0], 1), GAMEFILE_PREFIX);

	menus [LOAD_MENU].items [slot_toload].command = load_success;
}

/**************************************/

void	menu_savegamefile (long nothing)

// Purpose : Gets filename to save file as - Then saves file OR enters SAVEGAME menu if an existing file needs to be replaced
// Accepts : Nothing
// Returns : Nothing

{
	menu_getname (user_savefilename, 16, message_decode (MMNU_GETNAME+2, 1), 0);
	if (nvram_directory (SAVE_MENU, 0, 6, ITEM_OK, GAMEFILE_PREFIX) >=7)
		menu_movedown (SAVE_MENU);
	else
		menu_savegame (-1);
}

/**************************************/

void	menu_savegame (long slot_tosave)

// Purpose : Deletes any file already in the slot (unless slot < 0), then Save a saved game to NVRAM
// Accepts : Menu item number holding saveslot
// Returns : (0) If failed, (1) If successful

{
long	loop;
	
	// COPY OVER GAME DETAILS INTO SAVEFILE

	save_gamefile.savefile_version = GAMEFILE_VERSION;
	save_gamefile.savefile_level = parameters.level;
	memcpy (&save_gamefile.savefile_performance, &player_performance, sizeof (performance_data));
	memcpy (&save_gamefile.savefile_status, &status, sizeof (game_status));
	memcpy (&save_gamefile.savefile_cameras [0], &camera [1], sizeof (camera_data));
	memcpy (&save_gamefile.savefile_cameras [1], &camera [2], sizeof (camera_data));
	
	for (loop=0; loop < 4; loop++)
		memcpy (&save_gamefile.savefile_pyramids [loop], &pyramids [loop], sizeof (pyramid));
	
	// DELETE FILE THAT ALREADY OCCUPIES SLOT (IF >=0)
	if (slot_tosave >=0)
		printf ("RESULT OF NVRAM DELETE IS %d\n", nvram_delete (message_decode (menus [SAVE_MENU].items [slot_tosave].string [0], 1), GAMEFILE_PREFIX));
		
	// SAVE FILE TO NVRAM
	printf ("RESULT OF NVRAM SAVE IS %d\n", nvram_save (user_savefilename, GAMEFILE_PREFIX, (char*) &save_gamefile, sizeof (gamefile)));
	menu_enter (MAIN_MENU, 1);
}

/***************************************************************************************************/
/*									 	  FLIGHT CONTROLS MENU FUNCTIONS						   */
/***************************************************************************************************/

void	menu_setcontrol (long control_type)

// Purpose : Sets current control method
// Accepts : New Control Method
// Returns : Nothing

{
	configuration.control_method = control_type;		// Change control method
	control_saveconfigure();							// Try to save it to NVRAM
	if (control_type == 1)
		control_setupfromconfigure ();					// If switching to joystick, set it up
}

/**************************************/

void	menu_setupstick (long nothing)

// Purpose : Sets control boundaries for flightstick
// Accepts : Nothing
// Returns : Nothing

{

long	calibrate_stage = 0;
long	keypad,
		icon_flash = 0,
		pro_x,
		pro_y;

static	long	stick_icon [6] [3] = 	{
										{(-110)-16,	(+70)-16,	BASECEL_CONTROL_ARROWS+0},
										{(-110)-16,	(-70)-16,	BASECEL_CONTROL_ARROWS+1},
										{(+110)-16, (-70)-16,	BASECEL_CONTROL_ARROWS+2},
										{(+110)-16,	(+70)-16,	BASECEL_CONTROL_ARROWS+3},
										{0,		0,				BASECEL_CONTROL_WHEELIN},
										{0,		0,				BASECEL_CONTROL_WHEELOUT}
										};

	BS_SetStickMinX (&joystick, 32767);							// Reset max's
	BS_SetStickMaxX (&joystick, -32767);
	BS_SetStickMinY (&joystick, 32767);
	BS_SetStickMaxY (&joystick, -32767);
	BS_SetStickMinZ (&joystick, 32767);
	BS_SetStickMaxZ (&joystick, -32767);
	
	do
		{
		switch (calibrate_stage)
			{
			case 0 :
			case 1 :
			case 2 :
			case 3 :
			
				sprintf (message_decode (menus [CALIBRATE_JOYSTICK_MENU].title, 1), message_decode (MMNU20_MODE+0, 1));
				break;
			
			case 4 :
				sprintf (message_decode (menus [CALIBRATE_JOYSTICK_MENU].title, 1), message_decode (MMNU20_MODE+1, 1));
				break;
				
			case 5 :
				sprintf (message_decode (menus [CALIBRATE_JOYSTICK_MENU].title, 1), message_decode (MMNU20_MODE+2, 1));
				break;
			}
		
		menu_enter (CALIBRATE_JOYSTICK_MENU, 0);					// Enter calibrate menu
		
		do
			{
			keypad = controlpad_debounce ();
			
			screen_base (S_IMAGE | S_WAIT);							// Plot backdrop image
			cel_quad.temp_cels = 0;
			
			menu_display (BLUE_PAL);								// Overlay menu
			
			cel_quad.shade = 11;
			
			// Draw centre cross
			
			cel_quad.x_pos0 = -16;
			cel_quad.y_pos0 = -16;
			arm_addgamecel (&cel_quad, BASECEL_CONTROL_CENTRE, 1024, 1024);
			
			// Draw cursor
			
			pro_x = pro_stick.x -160;
			pro_y = pro_stick.y -120;
			
			cel_quad.x_pos0 = (pro_x - (pro_x>>2)) -16;
			cel_quad.y_pos0 = (pro_y - (pro_y>>2)) -16;
			
			if (calibrate_stage >=4)
				arm_addgamecel (&cel_quad, stick_icon [calibrate_stage] [2], 1024, 1024);
			else
				arm_addgamecel (&cel_quad, BASECEL_CONTROL_CURSOR, 1024, 1024);
			
			// Draw arrows in corners ?
			
			if ((++icon_flash & 8) && calibrate_stage < 4)
				{
				cel_quad.x_pos0 = stick_icon [calibrate_stage] [0];
				cel_quad.y_pos0 = stick_icon [calibrate_stage] [1];
				arm_addgamecel (&cel_quad, stick_icon [calibrate_stage] [2], 1024, 1024);
				}
			
			// Draw centre rectangle
			
			cel_rectangle (-24+160, 24+120, 24+160, -24+120, (  pro_stick.x >= (-24)+160 && pro_stick.x <= (24)+160 &&
																pro_stick.y >= (-24)+120 && pro_stick.y <= (24)+120) ? RED_PAL : BLUE_PAL, REC_BORDER | REC_BOX, 1);
			
			// Update screen
			
			screen_update(S_CEL | S_BANK | S_MENU | S_WAITFRAME);		// Render screen
			}
		while (!(keypad & ControlA)); 
		calibrate_stage++;
		}
	while (calibrate_stage <= 5);
	
	configuration.stick_x_min = (long) BS_GetStickMinX (&joystick);		// Put in config file
	configuration.stick_x_max = (long) BS_GetStickMaxX (&joystick);
	configuration.stick_y_min = (long) BS_GetStickMinY (&joystick);
	configuration.stick_y_max = (long) BS_GetStickMaxY (&joystick);
	configuration.stick_z_min = (long) BS_GetStickMinZ (&joystick);
	configuration.stick_z_max = (long) BS_GetStickMaxZ (&joystick);
	
	control_saveconfigure();											// Save config file
	menu_moveup(0);														// Move up previous menu
}
		
/**************************************/

void	menu_setflightc (long dir_tomove)

// Purpose : Sets up an individual flight control
// Accepts : Nothing
// Returns : Nothing

{
long	control_key = configuration.flight_controls [menus [FLIGHTCONTROL_MENU].cur_option] + dir_tomove;
long	max_setting = (menus [FLIGHTCONTROL_MENU].cur_option == 7) ? 1 : 6;

	if (control_key < 0)
		control_key = max_setting;
		
	if (control_key > max_setting)
		control_key = 0;

	configuration.flight_controls [menus [FLIGHTCONTROL_MENU].cur_option] = control_key;
}

/**************************************/

void	menu_updateflightc (long keypad)

// Purpose : Updates set flight controls menu
// Accepts : Keypad presses
// Returns : Nothing

{

	if (keypad & ControlLeft)
		{
		menu_setflightc (-1);
		menu_enter (FLIGHTCONTROL_MENU, 0);
		}
		
	if (keypad & ControlRight)
		{
		menu_setflightc (+1);
		menu_enter (FLIGHTCONTROL_MENU, 0);
		}
}

/**************************************/

void	menu_saveflightc (void)

// Purpose : Saves config file and moves up a menu
// Accepts : Nothing
// Returns : Nothing

{
	control_saveconfigure();
	menu_moveup (0);
}

/**************************************/

long	menu_endflightc (long nothing)

// Purpose : Checks for clashes in control setup
// Accepts : Nothing
// Returns : 1 if no clashes, 0 if clashes

{
char	clash_check [7] = {0,0,0,0,0,0,0};
long	clash_loop,
		clash_fail = 1;
		
	for (clash_loop = 0; clash_loop < 7; clash_loop++)
		clash_check [configuration.flight_controls [clash_loop]] = 1;
		
	for (clash_loop = 0; clash_loop < 7; clash_loop++)
		if (clash_check [clash_loop] == 0)
			clash_fail = 0;
			
	return (clash_fail);
}

/***************************************************************************************************/
/*									 	 	 CONFIGURE MENU FUNCTIONS							   */
/***************************************************************************************************/

void	menu_configdefault (long nothing)

// Purpose : Resets game to default configuration
// Accepts : Nothing
// Returns : Nothing

{
char current_language;

	current_language = configuration.language;	// Retain current language
	load_gamedata (DATA_CONFIGURE);				// Load in standard configuration
	configuration.language = current_language;	// Restore old language setting
	control_saveconfigure();					// Try to save it to NVRAM
	control_setupfromconfigure ();				// Set up to new configuration
	menu_moveup (0);							// Move up to next menu
}

/**************************************/

void	menu_setlanguage (long language_toset)

// Purpose : Sets new language for the game
// Accepts : New language number
// Returns : Nothing

{
	configuration.language = language_toset;						// Set for Selected Language
	message_initialiselanguagefiles (configuration.language);		// Initialise correct message files
	control_saveconfigure();										// Try to save it to NVRAM

	if (configure_waiting == 0)										// Is this 1st menu screen coz couldn't load config file from NVR
		{
		configure_waiting = 1;										// Yes - clear flag
		menu_enter (MAIN_MENU, 1);									// Go back to the main menu
		}
	else
		menu_enter (CONFIGURE_MENU, 0);								// Else - go back to configure menu
}

/***************************************************************************************************/
/*											GET FILENAME / HISCORE NAME 						   */
/***************************************************************************************************/

long	menu_getname (char *filename_ptr, long maxsize, char *get_message, long insert_default)

// Purpose : Gets name from user
// Accepts : Pointer to string to put filename in, max size of name, pointer to message to display, default name flag
// Returns : Length of string entered

{

long	charloop,
		cel_charoff;
		
char	charselect [2] = {0, 0};
char	*default_pointer = configuration.pilot;

long	keypad;
		
long	cursor_x = 0,
		cursor_y = 0,
		nextcharpos = 0,
		
		flash_count = 0,
		filename_length = 0,
		finished = 0;

long	stringlength_new = 0,
		stringlength_old = 0,
		string_moveby,
		alpha_filenamestart,
		shift_loop;
		
	// LOAD IN ALPHABET GAMECELS
	
	load_gamecels ("Alphabet");	
	
	// ADD ALL CHARACTERS FOR SELECT
	
	cel_resetcharactermap ();
	cel_setalphaaddmode (ALPHA_BEHIND);
	
	// ADD ALL CHARACTERS FOR SELECTION
	
	for (charloop = 0; charloop < 13; charloop++)
		{
		charselect [0] = charloop+65;															// Add A-M
		cel_charoff = -(cel_getalphastringlen (charselect)>>1);
		cel_addalphastring (charselect, 160 + cel_charoff + ((charloop-6)<<4), 90, 1024);
		
		charselect [0] = charloop+78;															// Add N-Z
		cel_charoff = -(cel_getalphastringlen (charselect)>>1);
		cel_addalphastring (charselect, 160 + cel_charoff + ((charloop-6)<<4), 120, 1024);
		}
		
	for (charloop = 0; charloop < 10; charloop++)												// Add 0-9
		{
		charselect [0] = charloop+48;
		cel_charoff = -(cel_getalphastringlen (charselect)>>1);
		cel_addalphastring (charselect, 160 + cel_charoff + ((charloop-6)<<4), 150, 1024);
		}
		
	charselect [0] = 'd';																		// Add DEL
	cel_charoff = -(cel_getalphastringlen (charselect)>>1);
	cel_addalphastring (charselect, 160 + cel_charoff + ((11-6)<<4), 150, 1024);
	
	charselect [0] = 'a';																		// Add DONE
	cel_charoff = - (cel_getalphastringlen (charselect)>>1);
	cel_addalphastring (charselect, 160 + cel_charoff + ((12-6)<<4), 150, 1024);
	
	cel_addalphadelay (128);
	cel_setalphaaddmode (ALPHA_CIRCLE);
	cel_addalphastring (get_message, 160-(cel_getalphastringlen (get_message)>>1), 198, 1024);

	alpha_filenamestart = cel_returnnextcharnumber ();
	
	// INSERT DEFAULT NAME ?
	
	if (insert_default == 1)
		{
		cel_addalphadelay (384);
		cel_setalphaaddmode (ALPHA_TOP);
		
		while (*default_pointer != 0)
			{
			nextcharpos = 160 + cel_getalphastringlen (filename_ptr) +2;	// Next char pos
			charselect [0] = *default_pointer++;							// Get character from default
			*(filename_ptr + filename_length++) = charselect [0];			// Put it in the filename
			cel_addalphastring (charselect, nextcharpos, 40, 1024);			// Add the character
			}
		*(filename_ptr + filename_length) = 0;								// Terminate string with 0
		}
		
	// MAIN DISPLAY LOOP

	do
		{
		
		stringlength_new = cel_getalphastringlen (filename_ptr)+2;			// Get length of string now
		if (stringlength_new != stringlength_old)							// Should we shift chars along to centre ?
			{
			string_moveby = (stringlength_new - stringlength_old);			// Get difference in size
			stringlength_old = stringlength_new;							// Copy new length to old
			
			for (shift_loop = alpha_filenamestart; shift_loop < cel_returnnextcharnumber (); shift_loop++)
				{
				cel_movecharacterby (shift_loop, - (string_moveby>>1) , 0);
				}
			}
		
		nextcharpos = 160 + (stringlength_new>>1);							// Next pos to put char in
		
		cel_quad.temp_cels=0;												// Reset plotting data

		// Display backdrop boxes
		
		cel_rectangle (32, 30,  319-32, 60,  RED_PAL, REC_BORDER | REC_BOX, 1);		// Top filename
		cel_rectangle (32, 220, 319-32, 190, BLUE_PAL, REC_BORDER | REC_BOX, 0);	// Message
		cel_rectangle (32, 170, 319-32, 80, GREY_PAL, REC_BORDER | REC_BOX, 1);		// Chars
		
		if (++flash_count & 8)
		
			{
			
			// Draw character selection box
			
			cel_rectangle (152 + ((cursor_x-6)<<4), 106 + (cursor_y*30), 155 + ((cursor_x-6)<<4) + 15, 85 + (cursor_y*30), BLUE_PAL, REC_BOX, 0);	// Cursor
			cel_rectangle (152 + ((cursor_x-6)<<4), 106 + (cursor_y*30), 155 + ((cursor_x-6)<<4) + 15, 85 + (cursor_y*30), RED_PAL, REC_BORDER, 0);	// Cursor
		
			// Draw top cursor
		
			cel_quad.x_pos0 = nextcharpos-160;
			cel_quad.y_pos0 = 50-120;
			cel_quad.shade = 11;
			arm_addgamecel (&cel_quad, 39, 1024, 1024);
			}
			
		keypad = controlpad_debounce ();								// Read and debounce keypad
		
		// Move cursor ?
		
		if (keypad & ControlUp)											// Moving up ?
			if (--cursor_y < 0)
				cursor_y = 2;
			
		if (keypad & ControlDown)										// Moving down ?
			if (++cursor_y > 2)
				cursor_y = 0;
			
		if (keypad & (ControlLeft | ControlLeftShift))					// Moving left (1 or 4) ?
			{
			cursor_x-=(keypad & ControlLeft) ? 1 : 4;
			if (cursor_x < 0)
				cursor_x+=13;
			}
			
		if (keypad & (ControlRight | ControlRightShift))				// Moving right (1 or 4)?
			{
			cursor_x+=(keypad & ControlRight) ? 1 : 4;
			if (cursor_x > 12)
				cursor_x-=13;
			}
		
		if (keypad & ControlStart)										// Finishing with start ?
			finished = 1;												// Yes 
			
		if (keypad & (ControlUp | ControlDown | ControlLeft | ControlRight))	// Beep if moved
			sound_playsample (SOUND_BEEP, 60, 80, 0);
		
		if (keypad & ControlA)													// Selecting letter / icon ?
			{
			sound_playsample (SOUND_CLICK, 60, 127, 0);							// Do click if item selected
			
			if (cursor_y < 2 || (cursor_y == 2 && cursor_x < 11))				// ARE WE ENTERING A LETTER ?
				{
				if (filename_length+1 < maxsize)								// YES - Is there room ?
					{
					if (cursor_y < 2)											// YES - work out what it is & add it
						charselect [0] = 'A' + cursor_x + (cursor_y * 13);
					else
						{
						if (cursor_x < 10)
							charselect [0] = '0' + cursor_x;
						else
							charselect [0] = ' ';
						}
					*(filename_ptr + filename_length++) = charselect [0];		// Put it in the filename
					cel_addalphastring (charselect, nextcharpos, 40, 1024);		// Add the character
					}
				}
			
			if (cursor_y == 2 && cursor_x == 11 && filename_length > 0)		// ARE WE DELETING A LETTER ?
				{
				cel_deleteletters (1);										// Yes - remove last added
				filename_length-=1;
				}
				
			if (cursor_y == 2 && cursor_x == 12)							// ARE WE FINISHING ?
				finished = 1;												// Yes !
			}	
			
		// RENDER SCREEN

		cel_rendercharactermap (1);
		screen_base (S_IMAGE);												// Plot backdrop image
		screen_update (S_CEL | S_BANK);										// Render screen
		
		*(filename_ptr + filename_length) = 0;								// Terminate string with 0
		}
	
	while (finished == 0);

	return (filename_length);								// Return length of string entered
}

/**************************************/

void	menu_setpilotname (long	cheat)

// Purpose : Allows player to enter default pilots name or select cheat passwords
// Accepts : nothing
// Returns : nothing

{
char	pilot_entry [24]; 

	if (cheat == 1)
		{
		menu_getname (pilot_entry, 16, message_decode (MMNU_GETNAME+0, 1), 0);	// Are we entering a cheat word ?
		
		if (strcmp (pilot_entry, message_decode (MMNU_CHEATS+0, 1)) == 0)		// CHECK FOR CHEAT 1 - PYRAMID MOVE
			cheat_feature1 = 1;
					
		if (strcmp (pilot_entry, message_decode (MMNU_CHEATS+1, 1)) == 0)		// CHECK FOR CHEAT 2 - MEGASHIP
			memcpy (&player_resetperformance,
					&player_megaperformance, sizeof (performance_data));
			
		if (strcmp (pilot_entry, message_decode (MMNU_CHEATS+2, 1)) == 0)		// CHECK FOR CHEAT 3 - SELECT LEVEL
			{
			menus [MAIN_MENU].max_option = 5;
			menus [MAIN_MENU].borders [0].option_end = 2;
			menus [MAIN_MENU].borders [0].colour = GREEN_PAL;
			}		
					
		if (strcmp (pilot_entry, message_decode (MMNU_CHEATS+3, 1)) == 0)		// CHECK FOR CHEAT 4 - 99 LIVES
			cheat_feature2 = 1;
					
		if (strcmp (pilot_entry, message_decode (MMNU_CHEATS+4, 1)) == 0)		// CHECK FOR CHEAT 5 - STRAIGHT INTO DEMO
			{
			do
				{
				game_demo();
				}
			while (controlpad_read () != ControlX);
			}
		
		if (strcmp (pilot_entry, message_decode (MMNU_CHEATS+5, 1)) == 0)		// CHECK FOR CHEAT 6 - PLAY ALL VIDEOS
			{
			video_play ("Krisalis");
			video_play ("Intro");
			video_play ("Victory0");
			video_play ("Victory1");
			video_play ("Failure");
			video_play ("Outro");
			}
	
		if (strcmp (pilot_entry, message_decode (MMNU_CHEATS+6, 1)) == 0)		// CHECK FOR CHEAT 7 - LOAD DELETES NVRAM FILES
			cheat_feature3 = 1-cheat_feature3;
		
		if (strcmp (pilot_entry, message_decode (MMNU_CHEATS+7, 1)) == 0)		// CHECK FOR CHEAT 8 - SHOWS ALL BONUSES ON MAP
			cheat_feature4 = 1-cheat_feature4;
		
		if (strcmp (pilot_entry, message_decode (MMNU_CHEATS+8, 1)) == 0)		// CHECK FOR CHEAT 9 - DISPLAY CREDITS
			game_missionstart (1);

		}
	else
		{
		menu_getname (pilot_entry, 16, message_decode (MMNU_GETNAME+1, 1), 1);	// Or entering default name ?
		if (strcmp (pilot_entry, message_decode (MMNU_CHEATS+9, 1)) == 0)		// Was default name cheat activator ?
			menus [CONFIGURE_MENU].items[3].function_value = 1;					// Yes - from now on, cheat !
		else
			{
			strcpy (configuration.pilot, pilot_entry);							// No - normal name - save it in config file
			control_saveconfigure ();
			}
		}
		
	menu_enter (CONFIGURE_MENU, 0);
}
