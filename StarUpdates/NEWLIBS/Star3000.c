/***************************************************************************************************
 *                    					  STAR FIGHTER - 3DO                               	 	   *
 *										  ------------------									   *
 *																								   *
 *									  (c) Krisalis software 1995								   *
 *																								   *
 *							  Written by Fednet - T.D.Parry & A.P.Hutchings						   *
 *																								   *
 ***************************************************************************************************/

/****************************************************************************************************
/												INCLUDE FILES										/
****************************************************************************************************/

#include "SFlib/SF_ARMLink.h"							// Tim's header files
#include "SFlib/SF_ARMCell.h"
#include "SFlib/SF_ARMSky.h"
#include "SFlib/SF_ARMBurn.h"
#include "SFlib/SF_ARMAnim.h"
#include "SFlib/SF_ARMUtils.h"

#include "SFlib/SF_Palette.h"
#include "SFlib/SF_Menu.h"
#include "SFlib/SF_Screenutils.h"
#include "SFlib/SF_Font.h"
#include "SFlib/SF_io.h"
#include "SFlib/SF_Access.h"
#include "SFLib/SF_NVRam.h"
#include "SFlib/SF_Parameters.h"
#include "SFlib/SF_Status.h"
#include "SFlib/SF_Control.h"
#include "SFlib/SF_Message.h"
#include "SFlib/SF_Map.h"
#include "SFlib/SF_War.h"
#include "SFlib/SF_Sound.h"
#include "SFlib/SF_Music.h"
#include "SFlib/SF_Video.h"
#include "SFlib/SF_Pyramid.h"
#include "SFlib/SF_Utility.h"
#include "SFlib/SF_Joystick.h"

#include "SFlib/SF_Star3000.h"

#include "SFlib/Draw_Frame.h"							// Hutch's header files
#include "SFlib/Update_Frame.h"
#include "SFlib/Setup_Tables.h"
#include "SFlib/Camera_Struct.h"
#include "SFlib/Plot_Graphic.h"
#include "SFlib/Sound_Control.h"
#include "SFlib/Misc_Struct.h"
#include "SFlib/Maths_Stuff.h"
#include "SFlib/Global_2.h"

#include <timerutils.h>									// System routines
#include <string.h>
#include <stdlib.h>	
#include <audio.h>

/****************************************************************************************************
/											GLOBAL VARIABLES										/
****************************************************************************************************/


char*			cel_game;								// Pointer to game textures list
char*			cel_list32;								// Pointer to 32x32 texture sprites
char*			cel_list16;								// Pointer to 16x16 ground sprites
char*			cel_creation;							// Pointer to created 16x16 land cels
char*			cel_palette;							// Pointer to palette entries
char*			cel_codedpalette;						// Pointer to coded6 palette entries
char*			cel_plutdata;							// Pointer to general plut data
CCB*			cel_plut;								// Pointer to plut initialising CCB
CCB*			cel_plotlist [CEL_MAXTEMP];				// Temp cel plotting list
cel_celdata		cel_quad;								// Cel adder description block
zsort_list		plot_list;								// Graphic ZSort plot table

// Cache tables for creation cels
// ------------------------------

char			cache_lookup [64*64 * 2];				// Created cels lookup block
char			cache_free	 [CEL_MAXCREATION];			// Created cels free list


// Planet information / maps
// -------------------------

planet_data		planet_info;							// Planet type information

char			sprite_map [256] [256];					// Map [sprite] data
char			height_map [256] [256];					// Map [height] data
char			poly_map [128] [128];					// Map [polygon] data

char			*map512;								// Low res map (512 x 512) - Coded6
char			*map128;								// Low res map (128 x 128) - Coded6
char			*map32;									// Low res map (32  x 32) - Coded6
char			cels4x4 [16 * CEL_MAX4];				// Pointer to 4x4 compacted land cels
char			animate_poly [1024];					// Pointer to texture animations file

// System Variables
// ----------------

long			configure_waiting = 0;					// Initial configure load ? Decides language / main menu
ScreenContext	screenbanks,
				*screen=&screenbanks;					// Screen

Item			ioreq_timer;							// IO requests
Item			VRAMIOReq;
Item			VBLIOReq;
Item			parent_taskref;							// Task ref of parent

// Sound System Variables
// ----------------------

long			music_background=0;
		
// Game status declarations
// ------------------------

game_configuration	configuration;							// Current game configuration
game_status			status;									// Mission status
game_parameters		parameters = 	{						// Game Parameters
									0,						// Current selected level to play on
									DISPLAY_NORMAL,			// Overlay menu type (MAIN MENU, HI-SCORE etc)
									MENUSWITCH_TIMEOUT,		// Counter to next switch
									0,						// Flip counter to demo
									0,						// Current hi-score level display
									0,						// Do credits first
									};
mission_data		mission;								// Mission data

long	cheat_feature1 = 0,
		cheat_feature2 = 0,
		cheat_feature3 = 0,
		cheat_feature4 = 0;
		
// Performance datablocks for players ship
// ---------------------------------------

performance_data	player_performance;						// Players performance data

performance_data	player_resetperformance = 	{			// Players performance reset data, *=not applicable for player
												0, 0, 2,	// Engine, Control, Shields
													{
													2, 10, 10,		// LASER, ATA, ATG
													0, 0, 0,	 	// Mega Bombs, Beam Lasers, Multi Missiles
													0, 3, 0,		// Mines, E-C-M's, Free 2
													0, 0, 0,		// Free 3, Free 4, Wing Pods
													0				// Max can launch*
													},
												0, 0, 0		// Laser Rate*, Missile Rate*, Launch Rate*
												};

performance_data	player_megaperformance = 	{			// Players mega performance data, *=not applicable for player
												7, 7, 7,	// Engine, Control, Shields
													{
													7, 1000, 1000,		// LASER, ATA, ATG
													1000, 1000, 1000, 	// Mega Bombs, Beam Lasers, Multi Missiles
													1000, 50, 0,		// Mines, E-C-M's, Free 2
													0, 0, 0,			// Free 3, Free 4, Wing Pods
													0					// Max can launch*
													},
												0, 0, 0		// Laser Rate*, Missile Rate*, Launch Rate*
												};

char	*pilot_names [16] =
							{	"",
								"Callum Blaze",
								"Luke Forester",
								"Jane Holly-Dean",
								"Hazel Phoenix",
								"Dave Valiant",
								"Stefan Lancaster",
								"Crusher",
								"Allied Pilot",
								"Leslie Jacobs",
								"Jake Phillips",
								"Spyder",
								"Jasmine Laker",
								"Kelly Forester",
								"Trainee Pilot 1",
								"Trainee Pilot 2"
							};				

long	keypad,
		keypad_constant,
		menu_command;
		
// MAIN GAME CODE

/****************************************************************************************************
/													MAIN											/
****************************************************************************************************/

int main()

{

long	selection,
		config_version;

	game_initialise ();											// Initialise basic system resources
		
	// Load in Configuration file
	
	load_gamedata (DATA_CONFIGURE);								// Load in config file of CD
	config_version = configuration.version;						// Get current version of config file
	
	configure_waiting = nvram_load ("StarFighter.Config", "SFC:", (char*) &configuration, sizeof (game_configuration));
	
	if (configure_waiting == 0 || config_version != configuration.version)		// No file or out of date one ?
		load_gamedata (DATA_CONFIGURE);											// NO - load default configure from CD
		
	control_setupfromconfigure ();								// Setup game from configuration file
	game_intro ();												// Do initial intro

	// TO INITIALISE CHEATS - DO IT HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	// printf ("WARNING ! PYRAMID MOVE AND ENTER CHEATS ARE ACTIVE !\n");
	// cheat_feature1 = 1;
	// menus [CONFIGURE_MENU].items[3].function_value = 1;			// Yes - from now on, cheat !
	
	pyramid_initialisegame ();									// Clear all pyramid levels
	arm_celinitialisecreation(cel_creation, CEL_MAXCREATION);	// Initialise creation cels
	font_initialise(cel_text);									// Init menu fonts
	
	armzsort_create (&plot_list, MAX_GRAPHICPLOT);				// Initialise plotting list
	io_resetvideocounter (MAX_WAITBEFOREVIDEO);					// Reset timeout counter
	control_initialisecameras ();								// Reset default cameras at start of game
	control_startgame (0);										// Setup player for new game
	
/****************************************************************************************************
/											MAIN MENU LOOP											/
****************************************************************************************************/
	
	parameters.display_timer = MENUSWITCH_TIMEOUT;				// Reset main menu timeout
	
	do
		{
		
		// Could we find the configuration file ?
		
		message_initialise (0);
		menu_resetmenutext ();
	
		load_backdrop (NULL, 1);								// Load in random backdrop
		
		// THIS CODE BELOW ENTERS THE SELECT LANGUAGE MENU IF THE GAME COULD NOT LOAD A NVRAM FILE
		// FROM NVRAM (FIRST TIME RUN ?) - IT HAS BEEN COMMENTED OUT FOR THE AMERICAN VERSION WHICH
		// GOES STRAIGHT INTO THE MAIN MENU. (NO LANGUAGE SELECT)
		
		// IF LANGUAGE SELECT IS PUT BACK IN, DON'T FORGET TO ALTER IN MENU (STRUCTURE) AND CHEAT
		// ACTIVATOR BECAUSE OF EXTRA LINE
		
		menu_enter (MAIN_MENU, 1);								// Enter main menu
		
		//if (configure_waiting == 1)							// YES - go straight into main menu
		//	menu_enter (MAIN_MENU, 1);
		//else
		//	menu_enter (LANGUAGE_MENU, 1);
					
		screen_setswap (1);										// Set screen swap
		parameters.menu_display = DISPLAY_MENU;					// Set hiscore toggle to menu display
				
		do
			{
			
// RESET PLOTTING DATA

			cel_quad.temp_cels=0;

// READ KEYPAD, CHECK FOR CHEAT ACTIVATE !

			keypad = controlpad_debounce ();
			keypad_constant = controlpad_constant ();
							
// UPDATE MENU & MENU SWITCHER

			menu_command = menu_update (keypad, keypad_constant);	// Update menu
			menuswitch_update (keypad);								// Switch on timer / keypress ?
			
// RENDER SCREEN

			screen_base (S_IMAGE);							// Plot backdrop image
			menu_display (BLUE_PAL);						// Overlay menu
			screen_update(S_CEL | S_BANK | S_MENU);			// Render screen
			}
		
		while (menu_command == MAIN_NOTHING);
		parameters.display_timer = MENUSWITCH_TIMEOUT;		// Reset main menu timeout
		
/****************************************************************************************************
/											SELECT MISSION LOOP										/
****************************************************************************************************/

		status.score = 0;									// Reset players score !
		
		do
			{
			
			screen_setswap (1);
			cel_setgamearea (1);							// Set gamecels area to temp
			
			pyramid_initialiseselect (parameters.level);	// Initialise pyramid mission selection
			load_backdrop ("Mission_Select", 1);			// Load in backdrop picture
			
			// WAIT FOR PLAYER SELECTION

			do
				{
				cel_quad.temp_cels=0;
				screen_base (S_IMAGE);
				menu_display (BLUE_PAL);
				
				cel_quad.y_pos0 = -105;
				
				cel_quad.shade = 7;
				cel_quad.x_pos0 = -120-8;
				arm_addgamecel (&cel_quad, BASECEL_PYRAMID_LEVEL + parameters.level, 1024, 1024);
				
				cel_quad.x_pos0 = 120-8;
				arm_addgamecel (&cel_quad, BASECEL_PYRAMID_LEVEL + parameters.level, 1024, 1024);
				
				keypad = controlpad_debounce ();
				keypad_constant = controlpad_constant ();
				
				selection = pyramid_read (keypad, parameters.level);
				message_update ();
				screen_update(S_CEL | S_BANK | S_MENU | S_INFO);
				}
			while (selection == PYRAMID_CONTINUE);
						
			// ARE WE STARTING A MISSION , IF SO, LOAD IT, DISPLAY DETAILS ETC. RUN MISSION
						
			if (selection == PYRAMID_START)							// Starting a mission ?
				{
				if (game_missionstart (0) == BRIEF_QUIT)			// Have we quit brief ?
					{
					pyramid_initialiseselect (parameters.level);	// Started & Quit brief ? - Initialise pyramid mission selection
					selection = PYRAMID_CONTINUE;
					}
				}
			else
				{
				cel_setgamearea (0);						// Set gamecels area to normal
				}
			}
		while (status.status != STATUS_GAMEOVER && selection != PYRAMID_QUIT);
		}
	while (TRUE);
}

/****************************************************************************************************
/									SWITCH MENU'S ON TIMER / KEYPRESS ?								/
****************************************************************************************************/

void menuswitch_update (long keypad)

// Purpose : Decides whether to switch displays ?
// Accepts : Current keypresses
// Returns : Nothing

{

	switch (parameters.menu_display)
		{
		case DISPLAY_MENU :

			if (keypad != 0)
				{
				parameters.display_timer = MENUSWITCH_TIMEOUT;
				parameters.demo_counter = 0;
				return;
				}
			else
				if (--parameters.display_timer <=0)
					menuswitch_next (DISPLAY_HISCORES);
			break;
			
		case DISPLAY_HISCORES :
		
			cel_rendercharactermap (1);
			if (keypad != 0 || --parameters.display_timer <=0)
				menuswitch_next (DISPLAY_MENU);
			break;
		}
}

// ***********************************

void menuswitch_next (long nextdisplaytype)

// Purpose : Switches menu displays
// Accepts : ?
// Returns : nothing

{

static long	last_menu;
char		hiscore_string [4] = 	{
									MTXT__TRAINING,
									MTXT__EASY,
									MTXT__MEDIUM,
									MTXT__HARD
									};

	parameters.display_timer = MENUSWITCH_TIMEOUT;					// Reset main menu timeout
	parameters.menu_display = nextdisplaytype;						// Set display to selected one
	
	// ENTERING 'DISPLAY HISCORES' ?
	
	if (nextdisplaytype == DISPLAY_HISCORES)
		{
		long	entry, hs_ypos;
		char	hs_score [10];

		last_menu = menu_status.current_menu;							// Remember last menu
		
		if (++parameters.demo_counter >= 4)								// Do demo or credits ?
			{
			parameters.menu_display = DISPLAY_MENU;
			parameters.demo_counter = 0;
			if (parameters.demo_or_credits == 1)
				game_demo();
			else
				game_missionstart (1);
			parameters.demo_or_credits = 1- parameters.demo_or_credits;
			}
		else
			{
			cel_resetcharactermap();									// Reset alphatext map
			sprintf (message_decode (menus [HIGHSCORE_MENU].title, 1), "%s : %s", message_decode (MTXT__HIGHSCORE, 0), message_decode (hiscore_string [parameters.display_hiscoretable], 0));		// Do menu title
			menu_enter (HIGHSCORE_MENU, 0);								// Enter menu
			cel_setalphaaddmode (ALPHA_CIRCLE);							// Set text add to circle
		
			for (entry = 0; entry < 5; entry++)							// Put in high score table
				{
				hs_ypos = 64 + (32*entry);
				cel_addalphastring (configuration.high_scores [parameters.display_hiscoretable] [entry].name, 35, hs_ypos, 1024);
				sprintf (hs_score, "%d", configuration.high_scores [parameters.display_hiscoretable] [entry].score);
				cel_addalphastring (hs_score, 235, hs_ypos, 1024);
				}
			
			if (++parameters.display_hiscoretable >= 4)						// Make so next shows next level, if >4, reset to zero
				parameters.display_hiscoretable = 0;
			}
		}
	
	// ENTERING 'MAIN MENU' ?
	
	if (nextdisplaytype == DISPLAY_MENU)
		{
		menu_enter (last_menu, 0);
		}
}

/****************************************************************************************************
/								  DISPLAY MISSION DETAILS PRIOR TO START							/
****************************************************************************************************/

long game_missionstart (long show_credits)

// Purpose : Loads in mission & Prints up all mission details prior to start, then calls game_play
// Accepts : Nothing
// Returns : Nothing

{
long	brief_status;

	if (show_credits == 0)
		{
		load_mission (parameters.level, 1+return_mission (&pyramids [parameters.level]));	// Load in mission & Initialise briefing
		screen_setswap (1);										// Set screen swap
		}
	
	control_showmission (show_credits, message_initialisemissionfiles (configuration.language, (show_credits == 0) ? parameters.level : -1, 1+return_mission (&pyramids [parameters.level])));
		
	do
		{
		cel_quad.temp_cels =0;										// Reset plotting list
		screen_base (S_IMAGE | S_WAIT);								// Draw backdrop screen
		brief_status = control_showmission (show_credits+2, 1);		// Do brief frame
		
		if (show_credits == 0)										// If doing mission brief, display insignia across menu title
			{
			cel_quad.y_pos0 = -105;
			cel_quad.shade = 7;
			cel_quad.x_pos0 = -120-8;
			arm_addgamecel (&cel_quad, BASECEL_PYRAMID_LEVEL + parameters.level, 1024, 1024);
			cel_quad.x_pos0 = 120-8;
			arm_addgamecel (&cel_quad, BASECEL_PYRAMID_LEVEL + parameters.level, 1024, 1024);
			screen_update (S_CEL | S_MENU | S_BANK | S_WAITFRAME);								// Render screen + title + cels
			}
		else
			screen_update (S_BANK | S_WAITFRAME);												// Render screen
		}
		
	while (brief_status	== BRIEF_CONTINUE);			// Loop until quit / start
	
	if (show_credits == 1)							// If doing credits, just return here
		{
		menu_enter (menu_status.current_menu, 0);
		return (0);
		}
	
	if (configuration.sound_on == 1)
		{
		music_terminate ();
		music_initialise (0);						// Re-Initialise music player
		}
		
	if (brief_status == BRIEF_START)				// Are we starting game from brief ? 
		{
		cel_setgamearea (0);						// Set gamecels area to normal
		menu_enter (NO_MENU, 1);					// Clear menu
		load_missionfiles ();						// Load in mission files	
		game_play (menu_command);					// Play game
		}
	
	else
	
		{
		if (configuration.music_on == 1 && configuration.sound_on == 1)
			music_play (MUSIC_MOVETRACK, 0, -1);	// Continue playlist for return to main menu
		}
	
	return (brief_status);
}


/****************************************************************************************************
/												PLAY A MISSION										/
****************************************************************************************************/

void game_play (long command)

// Purpose : Plays a mission
// Accepts : ?
// Returns : nothing

{

long	shield_extent=0,							// Shield display variables
		shield_value=0,
		
		tracking_distance,							// Tracking display variables
		tracking_graphic,
									
		camera_touse,								// Camera & Crystal variables
		crystal_loop,
		crystal_size,
		cel_graphicslimit,
		
		shipheight,
		shipheighticon,
		scanlocposition,
		
		remember_score;
		
target_struct target;
		

char	intro_clip [16];

	// INITIALISE MISSION START ETC.
	
	control_startmission (0);						// Initialise players stats to mission start
	screen_setswap (1);
	load_gamecels (NULL);							// Load in planet game sprites
	
	// take a copy of players performance & status at mission start
	
	memcpy (&save_gamefile.savefile_performance, &player_performance, sizeof (performance_data));
	memcpy (&save_gamefile.savefile_status, &status, sizeof (game_status));
	
	// Play into - mission movie clip (if configured for it)
	
	if (configuration.video_on == 1)
		{
		sprintf (intro_clip, "Into%d", 1+(return_mission (&pyramids [parameters.level])) % 3);
		video_play (intro_clip);
		}
	
	// Restart music (if switched on)
	
	if (configuration.music_on == 1 && configuration.sound_on == 1)
			music_play (MUSIC_MOVETRACK, 0, -1);
			
	// Update first frame always before loop
	
	update_frame (0);
	
/****************************************************************************************************
/										MISSION IN-PROGRESS LOOP									/
****************************************************************************************************/

	do
		{

// RESET PLOTTING DATA

		armzsort_initialise ();
		cel_quad.temp_cels =0;

				
// READ KEYPAD (DEBOUNCE 1 OF THE KEYPAD VARIABLES, KEEP THE OTHER IMMEDIATE)

		keypad = controlpad_debounce ();
		keypad_constant = controlpad_constant ();
					
// GAME UPDATE ? (IF NOT IN MENU)

		if (menu_status.current_menu == NO_MENU)
			{
	
			// CHECK FOR GAME CONTROLS
			
			if (keypad & keypad_iomap [configuration.flight_controls [FLIGHT_VIEW_MAP]])			// Check for map display
				if (players_ship->shields > 0)														// Can only enter map when alive
					{
					map_show ();
					keypad = 0;
					}
				
			if (keypad & keypad_iomap [configuration.flight_controls [FLIGHT_CHANGE_CAM]])			// Check for camera change
				control_changecamera ((status.current_camera == 1) ? 2 : 1);
				
			if (keypad & keypad_iomap [configuration.flight_controls [FLIGHT_SELECT_WEAPON]])		// Check for weapon change
				control_selectweapon();
			
			if (keypad & keypad_iomap [configuration.flight_controls [FLIGHT_MENU]])				// Entering game menu ?
				{
				menu_enter (PAUSE_MENU, 1);
				keypad = 0;
				}
			}
			
// UPDATE GAME FRAME
		
		if (menu_status.current_menu == NO_MENU)
			{
			update_frame (keypad_constant);				// Update simulation
			status_updatestatus();						// Update game status
			}
			
			
// DECIDE WHAT CAMERA TO USE, THEN RENDER 3D WORLD TO CEL LIST USING THAT CAMERA

		
		if (status.status == STATUS_CRASHED || status.status == STATUS_GAMEOVERWAIT)	// If player is crashing, display using camera 0
			camera_touse=0;
		else
			{
			if (camera [status.current_camera].instance == -1)							// Otherwise, if player is using cam2 & no item, use cam1
				camera_touse=1;
			else
				camera_touse=status.current_camera;										// Otherwise, use current cam
			}
		
		draw_frame (camera_touse);														// Render world
		
// UPDATE MENU OR UPDATE CURRENT TRACKING OBJECTS ?

		if (menu_status.current_menu != NO_MENU)
			{
			menu_update (keypad, keypad_constant);										// Update menu
			}
		
		else
		
			{
			if (status.track_status == TRACKING_STATUS_FREE && status.clock2 == 0)		// Update tracking objects ?
				control_retrack ();
			}
			
// IF INTERNAL VIEW, DISPLAY COCKPIT GRAPHICS (AND NOT DOCKED)

		if (camera [camera_touse].type == CAMERA_INTERNAL)
			if (status.docked == NULL)
				{
				cel_quad.x_pos0 = -24;
				cel_quad.y_pos0 = -24;
				cel_quad.shade = 10;
				arm_addgamecel (&cel_quad, BASECEL_COCKPIT, 1024, 1024);
				}
			
// ADD IN-GAME GRAPHICS, SHIELDS ETC.

		cel_graphicslimit = cel_quad.temp_cels;
		
		if (menu_status.current_menu == NO_MENU)
			{
			
			// Display lives left ?
			
			if (status.display_lives & 4)
				{
				cel_quad.y_pos0 = -80;
				cel_quad.shade = 11;
				
				for (crystal_loop = 0; crystal_loop < status.lives; crystal_loop++)
					{
					cel_quad.x_pos0 = (20*crystal_loop) - (10*status.lives);
					arm_addgamecel (&cel_quad, BASECEL_LIVES, 1024, 1024);
					}
				}
		
			// Only add shields & direction arrow if alive
			
			if (players_ship->shields > 0)
				{
				
				// Flash shields ?
			
				cel_quad.shade = (shield_value <32 || players_ship->damage != DAMAGE_NOTHING) ? (status.clock2 & 8)+6 : 11;
								
				// Add shields (SHADED COLOUR BAR)
				
				cel_quad.x_pos0 = -135;
				cel_quad.y_pos0 = -103;
				shield_extent = players_ship->performance->shields << 5;		// Shields Rating (32 to 255)
				shield_value = (players_ship -> shields);						// Current Value (0-255)
				arm_addgamecel (&cel_quad, BASECEL_SHIELD+0, 1024, 1024);		// Put in shields bar
				arm_setcel_hw (&cel_quad, shield_value	& 255, -1);				// Set it's length to shields value
				
				// Add shields (BLUE BIT)
			
				cel_quad.x_pos0 = shield_value-135;
				arm_addgamecel (&cel_quad, BASECEL_SHIELD+1, (shield_extent-shield_value)<<7, 1024);
			
				// Add shields (Pointer)
			
				cel_quad.x_pos0 = shield_value-140;
				arm_addgamecel (&cel_quad, BASECEL_SHIELD+2, 1024, 1024);
			
				// Are we shading down (ie. damage), if so, do danger highlight bar
				
				if (cel_quad.shade == 6)
					{
					cel_quad.x_pos0 = -135;
					cel_quad.y_pos0 = -105;
					cel_quad.shade = 11;
					arm_addgamecel (&cel_quad, BASECEL_SHIELD+3, shield_extent<<7, 1024);
					}
				
				scanlocposition = -14;
				
				if (status.track_type != TRACKING_NOTHING)
					{
					if (status.track_type == TRACKING_SHIP)								// Tracking a ship ?
						{
						target.x_aim = status.ship_tracking -> x_pos;
						target.y_aim = status.ship_tracking -> y_pos;
						target.z_aim = status.ship_tracking -> z_pos;
						tracking_graphic = BASECEL_GRIDLOCK + 2;
						scanlocposition = -65;
						}
					else
						{
						target.x_aim = (status.track_xpos)<<24;							// Tracking a ground object ?
						target.y_aim = (status.track_ypos)<<24;
						target.z_aim = 0;
						tracking_graphic = BASECEL_GRIDLOCK + 3;
						scanlocposition = -55;
						}
					
					target.x_pos = players_ship->x_pos;
					target.y_pos = players_ship->y_pos;
					target.z_pos = players_ship->z_pos;
					
					target_finder(&target);
					tracking_distance = target.distance>>29;
					
					if (tracking_distance < 0)
						tracking_distance = -tracking_distance;
					}
					
				// Add direction arrow 
			
				cel_quad.x_pos0 = scanlocposition;
				cel_quad.shade = 11;
				cel_quad.y_pos0 = 90;
				arm_addgamecel (&cel_quad, BASECEL_DIRECTION+(4+(((players_ship->x_rot + (64<<10)))>>17) &7), 1024, 1024);
			
				// Add grid reference
				
				cel_quad.x_pos0+=12;
				cel_quad.y_pos0+=2;
				arm_addgamecel (&cel_quad, BASECEL_GRIDREF + (((players_ship->x_pos)>>29) &7), 1024, 1024);
				
				cel_quad.x_pos0+=9;
				arm_addgamecel (&cel_quad, BASECEL_GRIDREF + 8 + (((players_ship->y_pos)>>29) &7), 1024, 1024);				

				// Add target / lock reference
				
				if (status.track_type != TRACKING_NOTHING)
					{
					
					cel_quad.x_pos0 += 16;
					arm_addgamecel (&cel_quad, BASECEL_GRIDLOCK + status.track_status, 1024, 1024);
											
					cel_quad.x_pos0 += 24;
					arm_addgamecel (&cel_quad, tracking_graphic, 1024, 1024);						// Display tracking type
				
					cel_quad.x_pos0 += 24;
					arm_addgamecel (&cel_quad, BASECEL_GRIDREF + tracking_distance, 1024, 1024);	// Display distance
					
					cel_quad.x_pos0+=9;
					
					if (status.track_type == TRACKING_SHIP)											// Display abv / blw
						{
						shipheight = (players_ship->z_pos - status.ship_tracking->z_pos)>>25;
						shipheighticon = 2;
						if (shipheight != 0)
							shipheighticon = (shipheight > 0) ? 1 : 0;
						arm_addgamecel (&cel_quad, BASECEL_GRIDSIGN + shipheighticon, 1024, 1024);
						cel_quad.x_pos0+=27;
						}
						
					cel_quad.y_pos0-=2;																// Display relative arrow
					
					tracking_distance = ((target.x_rot + (64<<10) - players_ship->x_rot) & ((1024<<10)-1)) >> 17;
					arm_addgamecel (&cel_quad, BASECEL_DIRECTION + tracking_distance, 1024, 1024);
					}
				}
				
			// Add current weapon (flash if none of this type of weapon left)
			
			cel_quad.shade = (players_ship->counter == 0) ? 11 : 7;
			
			if (!(status.weapon != WEAPON_LASER && players_ship->performance->weapons [status.weapon] <=0 && (status.clock1 & 2)))
				{
				cel_quad.x_pos0 = 124;
				cel_quad.y_pos0 = -106;
				arm_addgamecel (&cel_quad, BASECEL_WEAPON+(status.weapon), 1024, 1024);
				}
				
			// Add collected crystals
			
			cel_quad.y_pos0 = 87;
			crystal_size = 64*(16-status.pickup_count);
			for (crystal_loop=0; crystal_loop<=3; crystal_loop++)
				if (status.pickups [crystal_loop] !=0)
					{
					cel_quad.x_pos0 = 86 + (crystal_loop*12);
					arm_addgamecel (&cel_quad, BASECEL_CRYSTAL+(status.pickups [crystal_loop]-1), crystal_size, crystal_size);
					}
					
			// Add mission timer icon
			
			if (mission.mission_timer > 0)
				{
				cel_quad.x_pos0 = -135;
				cel_quad.y_pos0 = 83;
				arm_addgamecel (&cel_quad, BASECEL_TIMER+((status.clock2>>2)&3), 1024, 1024);
				}
						
			// Display attacking stats
	
			cel_quad.x_pos0 = -(((cockpit_missiles + cockpit_enemybig + cockpit_enemysmall)*5) >> 1);
			cel_quad.y_pos0 = 86;
			cel_quad.shade = 11;
		
			while (cockpit_missiles-- >0)
				{
				if (status.clock2 & 4)
					arm_addgamecel (&cel_quad, BASECEL_COCKPIT+1, 1024, 1024);
				cel_quad.x_pos0 += 5;
				}
	
			while (cockpit_enemybig-- >0)
				{
				arm_addgamecel (&cel_quad, BASECEL_COCKPIT+2, 1024, 1024);
				cel_quad.x_pos0 += 5;
				}

			while (cockpit_enemysmall-- >0)
				{
				arm_addgamecel (&cel_quad, BASECEL_COCKPIT+3, 1024, 1024);
				cel_quad.x_pos0 += 5;
				}	
			}
			
// UPDATE TEXT MESSAGES

		message_update ();										// Update Text Messages


// IS THIS A SPACE MISSION, OR A NORMAL ONE ?

		
		if (planet_info.space_mission == 1)
			{
			screen_base (S_FLASH | S_WAIT);						// YES - Do fast backdrop frame clear
			}
		else
			{
			// armanim_update (animate, *sprite_map);			// NO - Update Ground Animations [TAKEN OUT]
			armburn_updateexplosions (*sprite_map, &cel_quad);	// NO - Update Ground Explosions
			arm_updatecache (&cel_quad);						// Update Plotting Cache
			}
	
			
// DRAW SCREEN (Only if not just about to quit)

		if (!(status.status == STATUS_GAMEOVER || status.status == STATUS_MISSIONCOMPLETED))
			{
			
			// Do build up of post-game graphics ? (ie. shields bar, co-ordinates etc. ?)
			
			if ((cel_graphicslimit + (status.clock3>>1)) < cel_quad.temp_cels)
				{
				cel_quad.temp_cels = cel_graphicslimit + (status.clock3>>1);
				if (status.clock3 & 1)
					sound_playsample (SOUND_BEEP, 60, 80, 0);
				}
			
			if (menu_status.current_menu == NO_MENU)
				screen_update(S_CEL | S_BANK | S_INFO | S_WAIT);
			else
				{
				menu_display (BLUE_PAL);
				screen_update(S_CEL | S_BANK | S_MENU | S_INFO | S_WAIT);
				}
			}
				
// UPDATE ANIMATIONS, TEXT MESSAGES

		armtex_updateall (animate_poly);					// Update Texture Animations
		if (menu_status.current_menu == NO_MENU)
			{
			if (keypad_constant & ControlA)
				armtex_update (animate_poly,0,1);			// Update Texture Animations
			else
				armtex_update (animate_poly,0,-1);
			}
		
		}

	while (status.status != STATUS_GAMEOVER && status.status != STATUS_MISSIONCOMPLETED);


	if (status.status == STATUS_GAMEOVER)	// Reset players performance if they have failed mission
		{
		remember_score = status.score;
		memcpy (&player_performance, &save_gamefile.savefile_performance, sizeof (performance_data));
		memcpy (&status, &save_gamefile.savefile_status, sizeof (game_status));
		status.status = STATUS_GAMEOVER;
		status.score = remember_score;
		}
		
// STOP SOUNDS

	stop_engine_sounds();									// Stop engine sounds
		
// MISSION COMPLETED ?

	if (status.status == STATUS_MISSIONCOMPLETED)
		{
		if (pyramid_registercompleted() == 2)				// Mark mission as completed - have we finished pyramid ?
			game_pyramidcomplete ();
		
		game_debrief();										// Show mission details
		}
	else
		game_failed();
}

/***************************************************************************************************
 *                                            MISSION FAILED                                       *
 ***************************************************************************************************/

void game_failed (void)

// Purpose : Does end game video - checks to see if it's a high score
// Accepts : nothing
// Returns : nothing

{

	screen_setswap (1);
	if (configuration.video_on == 1)
		video_play ("Failure");					// Play failure video
	screen_setswap (1);
	game_checkforhighscore (1);					// Did player get a high score ?
}

/***************************************************************************************************
 *                                     MISSION SUCCESSFUL (DEBRIEF)                                *
 ***************************************************************************************************/

void game_debrief (void)

// Purpose : Totals up score, bonuses etc. when a mission is completed
// Accepts : nothing
// Returns : nothing

{
	status.missions_completed+=1;						// Missions completed
	
	screen_setswap (1);
	
	if (configuration.video_on == 1)
		{
		if (return_mission (&pyramids [parameters.level]) & 1)
			video_play ("Victory1");					// Play victory video (1 or the other)
		else
			video_play ("Victory0");
		}
		
	menus [DEBRIEF_MENU].items [1].selectable = ITEM_OK;
	
	message_initialise (0);								// Reset text messages
	menu_resetmenutext ();
	load_backdrop (NULL, 1);							// Load in backdrop
	
	menu_enter (DEBRIEF_MENU, 1);
	screen_setswap (1);
	
	do
		{

// RESET PLOTTING DATA

		cel_quad.temp_cels=0;
			
// READ AND DEBOUNCE KEYPAD

		keypad = controlpad_debounce ();
		keypad_constant = controlpad_constant ();

// UPDATE MENU

		menu_command = menu_update (keypad, keypad_constant);	// Update menu

// RENDER SCREEN

		screen_base (S_IMAGE);									// Plot backdrop image
		menu_display (BLUE_PAL);								// Overlay menu
		screen_update(S_CEL | S_BANK | S_MENU);					// Render screen
		}
		
	while (keypad == 0);
}

/***************************************************************************************************
 *                                          ENTER NAME IN HISCORE                                  *
 ***************************************************************************************************/
 
void game_checkforhighscore (long load_backdropfile)

// Purpose : Looks to see if a hi-score should be entered, if so, gets it and enters it
// Accepts : Pointer to backdrop to load (ignored if null)
// Returns : nothing

{
long	score_search,
		score_shift,
		score_positionfound = -1;
	
	// Can we find a location to put the score in ?
	
	score_search = 0;
	do
		{
		if (status.score >= configuration.high_scores [parameters.level] [score_search] .score)
			score_positionfound = score_search;
		score_search+=1;
		}
	while (score_positionfound == -1 && score_search <5);
	
	// Did we find a location ? If so, shift down scores below & enter name here
	
	if (score_positionfound != -1)
		{
		
		// Shift down scores
		
		for (score_shift = 4; score_shift > score_positionfound; score_shift--)
			{
			configuration.high_scores [parameters.level] [score_shift].score = configuration.high_scores [parameters.level] [score_shift-1].score;
			strcpy (configuration.high_scores [parameters.level] [score_shift].name, configuration.high_scores [parameters.level] [score_shift-1].name);
			}
		
		message_initialise (0);								// Reset text messages
		menu_resetmenutext ();
	
		// Load new backdrop
		
		if (load_backdropfile != 0)
			load_backdrop (NULL, 1);
		
		// Get name
		
		configuration.high_scores [parameters.level] [score_positionfound] .score = status.score;
		menu_getname (configuration.high_scores [parameters.level] [score_positionfound] .name, 16, message_decode (MMNU_GETNAME+1, 1), 1);
		
		// Force menu to flip to this levels high score table next
		
		parameters.display_timer = 0;						// Next frame will flip to hiscores
		parameters.menu_display = DISPLAY_MENU;				// Set hiscore toggle to menu display
		parameters.display_hiscoretable = parameters.level;	// Look at this hiscore next
		
		}
}

/***************************************************************************************************
 *                                      GAME INITIALISATION                                        *
 ***************************************************************************************************/

void game_initialise (void)

// Purpose : Initialise everything (system resources, base files etc.) at game start
// Accepts : nothing
// Returns : nothing, any fatal errors, program terminates by calling DONE

{

// INITIALISE OS RESOURCES

	parent_taskref = KernelBase ->kb_CurrentTask->t.n_Item;		// Put item ref# of game task in global variable
	
	if ( InitEventUtility(1, 0, LC_Observer) < 0)				// INITIALISE EVENT MANAGER
		game_end();
	
	if (OpenAudioFolio () < 0)									// INITIALISE AUDIO FOLIO
		game_end();
			
// INITIALISE SCREEN

	screen->sc_nScreens = 2;									// Get 2 screenbanks
	if (!OpenGraphics(screen,2))	game_end();					// If failed, quit game
	screen->sc_curScreen=0 ;									// Set current screen to 0
	EnableVAVG( screen->sc_Screens[0] );						// Enable sub-pixel averaging	
	EnableVAVG( screen->sc_Screens[1] );
	EnableHAVG( screen->sc_Screens[0] );			
	EnableHAVG( screen->sc_Screens[1] );
	SetCEControl (screen->sc_BitmapItems [0], 1<<26, 1<<26);	// Enable cel clipping for screen 0
	SetCEControl (screen->sc_BitmapItems [1], 1<<26, 1<<26);	// Enable cel clipping for screen 1
	
// GET IO REQUESTS

	VRAMIOReq = GetVRAMIOReq();									// Create RAM IO Request
	VBLIOReq = GetVBLIOReq();									// Create VBL IO Request
	ioreq_timer = GetTimerIOReq();								// Create Timer IO Request

// INITIALISE SOUND, CELS, FONTS, JOYSTICK READER, LOAD GAME DATA
	
	if (cel_initialise() == 0) game_end();						// Init Cel Memory Areas
	if (font_loadin() ==0) game_end();							// Load in fonts
	sound_loadsamples();										// Load in all samples
	sound_initialise();											// Initialise sample player
	joystick_initialise();										// Initialise joystick reader
	load_gamedata (DATA_FILES);									// Load in game data tables
}

/***************************************************************************************************
 *                          	            GAME TERMINATION                                       *
 ***************************************************************************************************/      
 
 void game_end (void)
 
// Purpose : Free all system resources at end of game (or fatal error)
// Accepts : nothing
// Returns : nothing

{
 	music_terminate ();
	sound_unloadsamples ();
	joystick_terminate ();
	font_terminate ();
	CloseGraphics (screen);
	DeleteIOReq (VRAMIOReq );
	DeleteIOReq (VBLIOReq );
	DeleteIOReq (ioreq_timer);
	KillEventUtility();
	CloseAudioFolio();
	exit (0);
}

/***************************************************************************************************
 *                               	         GAME INTRO           	                               *
 ***************************************************************************************************/
 
void game_intro (void)
{

// Purpose : Plays initial intro video, intro screens etc.
// Accepts : nothing
// Returns : nothing
	
	video_play ("StarLogo");						// Play star fighter logo
	video_play ("Intro");							// Play intro
	video_play ("Krisalis");						// Play Krisalis Video
	
	// LOAD IN BACKDROP IMAGE

	load_backdrop ("Fednet", 0);					// Load in backdrop picture
	
	screen_base (S_IMAGE);							// Print it
	FadeToBlack(screen, 30);
	screen_update(S_BANK);							// Display it
	FadeFromBlack(screen, 90 );						// Fade from screen
	WaitVBL (VBLIOReq, 150);						// Wait
	screen_setswap (1);								// Set screen swap
	
	// INITIALISE MUSIC PLAYER & START MUSIC PLAYING

	music_initialise (0);							// Initialise music player
	menu_setmusictracks();							// Setup tracks play list
	if (configuration.music_on == 1)
		music_play (MUSIC_PLAYFROMTRACK, 0, -1);	// Play tracks in list
}

/***************************************************************************************************
 *                               	        GAME DEMO FLIGHTS           	                       *
 ***************************************************************************************************/
 
void game_demo (void)

// Purpose : Plays a demo mission, changing tracking camera randomly for preset time
// Accepts : nothing
// Returns : nothing

{

long	keypad,
		camera_swivel = 0,
		timeout_cameratype = 128,
		timeout_cameraview = 128,
		message_timer = 256,
		message_count = 0;
			
	if (load_mission (4, 1 + (arm_random() &1)) == 0)	// Load Demo mission
		game_end ();	
	
	if (load_missionfiles () == 0)						// Load in mission files
		game_end ();		
		
	control_startmission (1);							// Initialise players stats to mission start
	load_gamecels (NULL);								// Load in planet game sprites
				
	camera [0].view = 1;								// Look at friendly !
	camera [0].type = CAMERA_TRACKING;					// Set to tracking camera
	control_searchcamera (0, 0);						// Get 1st instance in camera 0
	status.formation_mode = SDB_FORMATION_AGGRESSIVE;	// Set friendlies to aggressive
	
	do 													// Main demo loop
		{

		// Draw text messages on bottom of screen ?
		
		if (--message_timer <0)
			{
			message_add (MESSAGE_DEMO, MTXT__DEMOTEXTSTART + message_count, GREEN_15, MESSAGE_SCROLL, 96, MESSAGE_CURSOR1);
			if (++message_count >=4)
				{
				message_count = 0;
				message_timer = 512;
				}
			else
				message_timer = 96;
			}
			
		
		// Change camera instances / types ?
		
		if (camera [0] . counter2 == 0)
			{
			
			if (--timeout_cameraview < 0)					// Change instance of view ? not if ship we are watching is in trouble !
				{
				if (camera [0].view_ship->shields > 32)
					{
					if (arm_random() & 63)
						{
						timeout_cameraview = 128;
						camera [0].view = 1 + (arm_random() & 1);
						control_searchcamera (0, ++camera[0].instance);
						}
					}
				}
			
			if (--timeout_cameratype < 0)					// Pick new camera type ?
				if (arm_random() & 63)
					{
					timeout_cameratype = 128;
					
					switch (arm_random () & 7)
						{
						case 7 :															// TRACKING CAM ?
						case 6 :
						case 5 :
							camera [0].type = CAMERA_TRACKING;
							break;
						
						case 4 :															// FLYBY CAM ?
						case 3 :
						case 2 :
							camera [0].type = CAMERA_FLYBY;
							break;
							
						case 1 :															// NORMAL CAM ?
							camera [0].x_rot = ((arm_random () & 1) == 0) ? 0 : 512<<10;
							camera [0].y_rot = 0;
							camera [0].zoom  = 1100;
							camera [0].type  = CAMERA_NORMAL;
							camera_swivel = ((arm_random () & 3) == 3) ? 64 : 0;
							break;
						
						case 0 :															// INTERNAL CAM ?
							camera [0].type = CAMERA_INTERNAL;
							break;
						
						}
					}
				
			if (camera_swivel > 0)									// Swivel camera ?
				{
				camera_swivel -=1;
				camera [0].x_rot += 16<<10;
				}
			}
			
		armzsort_initialise ();										// Reset plotting data
		cel_quad.temp_cels =0;
		
		update_frame (0);											// Update simulation
		status_updatestatus();										// Update game status
		message_update ();											// Update messages
		draw_frame (0);												// Render world
		
		if (planet_info.space_mission == 1)
			{
			screen_base (S_FLASH);									// YES - Do fast backdrop frame clear
			}
		else
			{
			armburn_updateexplosions (*sprite_map, &cel_quad);		// NO - Update Ground Explosions
			arm_updatecache (&cel_quad);							// NO - Update Plotting Cache
			}
				
		if (!(status.status == STATUS_GAMEOVER))					// DRAW SCREEN (Only if not just about to quit)
			screen_update(S_CEL | S_BANK | S_WAIT | S_DEMO | S_INFO);
			
		armtex_updateall (animate_poly);							// Update Texture Animations
	
		keypad = controlpad_debounce ();
		keypad_constant = controlpad_constant ();
		}
		
	while (status.status != STATUS_GAMEOVER && keypad == 0);	

	load_backdrop (NULL, 1);									// Load in random backdrop
	parameters.menu_display = DISPLAY_MENU;						// Set hiscore toggle to menu display
	parameters.display_timer = MENUSWITCH_TIMEOUT;				// Reset main menu timeout
	menuswitch_next (DISPLAY_MENU);								// Go back to main menu
}

/***************************************************************************************************
 *                               	       	 PYRAMID FINISHED !    	      	                       *
 ***************************************************************************************************/
 
void game_pyramidcomplete (void)

// Purpose : Pyramid is finished - do something ! Move onto next level - Play outro if game finished !
// Accepts : Now't
// Returns : Now't

{

	if (parameters.level == 3)						// Have we finished the game ??? 
		{
		status.missions_completed = 0;				// Yes 		Reset missions completed
		status.score += 5000000;					//          Add on 5 million to score
		video_play ("Outro");						//			Play outro
		video_play ("Intro");						//			Replay intro !
		}
	
	else											// Or just finished a pyramid
	
		{
		status.score += 1000000;					//          Add on a million to score
		}
	
	game_checkforhighscore (1);						// Check for hiscore
	parameters.level = (parameters.level+1) &3;		// Go onto next (or back to first) level
}
