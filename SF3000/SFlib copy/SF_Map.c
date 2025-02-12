// File : SF_Map
// Map display routines

#include "SF_Map.h"
#include "SF_io.h"
#include "SF_Palette.h"
#include "SF_ARMCell.h"
#include "SF_Access.h"
#include "SF_War.h"
#include "SF_Control.h"
#include "SF_ArmUtils.h"
#include "SF_ArmAnim.h"
#include "Sound_Control.h"
#include "Weapons.h"

/**************************************/

// Icon definitions for gameset 'Map'

#define	BASECEL_MAP_DIRECTION		0
#define	BASECEL_MAP_PALETTE			8
#define	BASECEL_MAP_TYPE_VEHICLE	11
#define	BASECEL_MAP_TYPE_MOTHERSHIP	12
#define	BASECEL_MAP_TYPE_SPACESHIP	13
#define	BASECEL_MAP_TYPE_PARACHUTE	14
#define	BASECEL_MAP_TYPE_MISSILE	15
#define	BASECEL_MAP_TYPE_FIGHTER	16
#define	BASECEL_MAP_TYPE_SATELLITE	19
#define	BASECEL_MAP_TARGET			20
#define	BASECEL_MAP_CHEAT			23
#define	BASECEL_MAP_BACKDROP		22
#define	BASECEL_MAP_ZOOM			26
#define	BASECEL_MAP_NUMBERS			29
#define	BASECEL_MAP_LETTERS			37
#define	BASECEL_MAP_STATUS			45
#define	BASECEL_MAP_QUESTION		47
#define	BASECEL_MAP_WEAPON			49
#define	BASECEL_MAP_CONTROL			56
#define	BASECEL_MAP_ENGINE			58
#define	BASECEL_MAP_SHIELDS			60
#define	BASECEL_MAP_LASERBAR		62
#define	BASECEL_MAP_GRAPH			63
#define	BASECEL_MAP_MODE			66
#define	BASECEL_MAP_HEIGHT			67
#define	BASECEL_MAP_KILL			68
#define	BASECEL_MAP_ECM				69
#define	BASECEL_MAP_LIVES			70

// Definitions for map display

#define	MAPBASEX	5				// Offsets from (0,0) - Centre of screen
#define	MAPBASEY	-10

// Global Variables

long	map_x,						// Map X, Y Offsets and zoom
		map_y,
		map_zoom,
		cross_x,
		cross_y,
		item_xpos,
		item_ypos;
		
ship_stack	*nearest_ship = NULL;

long		nearest_found = TRACKING_NOTHING,
			tracking_x = 0,
			tracking_y = 0,
			nearest_distance;
			
/**************************************/

void	map_show (void)

// Purpose : Display movable, zoomable map, targets etc.
// Accepts : Nothing
// Returns : Nothing

{

long	map_timer=0,
		keypad,
		keypad_constant,
		icon_loop;

long	x_draw,
		y_draw,
		cross_lock_fail;
		
long	move_speeds [5] = {	0,		// Zoom = 0	// NOT AVAILABLE ANYMORE DUE TO A TIGHT FISCAL OUTLOOK
							0,		// Zoom = 1	// NOT AVAILABLE EITHER COZ IT'S 4.40 AM
							4,		// Zoom = 2
							4,		// Zoom = 3
							8 };	// Zoom = 4
							
// INITIALISE VARIABLES AND GRAPHICS

	stop_engine_sounds();						// Turn off engine / wind effects
	screen_setswap (1);							// Set next flip to fade
	
	map_x = 128;								// Set Map X, Y Offsets from player and current Zoom
	map_y = 128;
	
	if (status.track_type == TRACKING_SHIP && status.track_status == TRACKING_STATUS_LOCKED)
		{
		cross_x = ((status.ship_tracking->x_pos >> 24)) & 255;
		cross_y = ((status.ship_tracking->y_pos >> 24)) & 255;
		}
	else
		{
		cross_x = ((players_ship->x_pos >> 24)) & 255;
		cross_y = ((players_ship->y_pos >> 24)) & 255;
		}
	
	map_zoom = 4;
	
	load_gamecels("Map");																			// Load in map sprites
		
// ----------------------

// MAIN REPEAT LOOP

	do
		{

// ----------------------

		cel_quad.temp_cels=0;									// Reset plotting list
				
		keypad = controlpad_debounce ();						// Read keypad
		keypad_constant = controlpad_constant ();	

// ----------------------
		
		// INTERPRET PLAYER COMMANDS
		
		if (keypad_constant & ControlLeft) 						// Moving map left ?
			cross_x = (cross_x - move_speeds [map_zoom]) & 255;
			
		if (keypad_constant & ControlRight)						// Moving map right ?
			cross_x = (cross_x + move_speeds [map_zoom]) & 255;
			
		if (keypad_constant & ControlDown)						// Moving map down ?
			cross_y = (cross_y - move_speeds [map_zoom]) & 255;
			
		if (keypad_constant & ControlUp) 						// Moving map up ?
			cross_y = (cross_y + move_speeds [map_zoom]) & 255;
			
		if ((keypad & ControlRightShift) && map_zoom > 2)		// Zoom map in ?
			{
			map_zoom-=1;
			if (nearest_found != TRACKING_NOTHING)
				{
				cross_x = tracking_x;
				cross_y = tracking_y;
				}
			}
			
		if ((keypad & ControlLeftShift) && map_zoom < 4)		// Zoom map out ?
			map_zoom += 1;
		
		// Set relative map position
		
		map_x = (map_zoom == 4) ? 128 : cross_x;
		map_y = (map_zoom == 4) ? 128 : cross_y;
			
		if (keypad & ControlC)									// Show brief again ?
			{
			map_showbrief ();
			}
		
		if (keypad & ControlB)									// Display ship stats ?
			if (nearest_found == TRACKING_SHIP)
				{
				map_showstatus (nearest_ship);
				}
				
		if (keypad & ControlA)									// Set tracking lock ?
			{
			switch (nearest_found)
				{
				case TRACKING_SHIP :
					if (nearest_ship != players_ship)
						{
						status.track_status = TRACKING_STATUS_LOCKED;
						status.track_type = TRACKING_SHIP;
						status.ship_tracking = nearest_ship;
						}
				break;
			
				case TRACKING_GROUND :
					status.track_status = TRACKING_STATUS_LOCKED;
					status.track_type = TRACKING_GROUND;
					status.track_xpos = tracking_x;
					status.track_ypos = tracking_y;
				break;
				
				case TRACKING_NOTHING :
					status.track_status = TRACKING_STATUS_FREE;
					status.track_type = TRACKING_NOTHING;
					break;
				}
			}
						
// ----------------------

		// DRAW BIG & SMALL MAPS ON SCREEN (ONLY IF NOT SPACE) , + BORDER
		
		screen_base (S_WAIT);									// Clear backdrop screen
		map_backdrop ();										// Draw map backdrop
		
		// DRAW BIG MAP OR BLANK OUT SPACE AREA
		
		if (planet_info.space_mission != 1)
			map_draw512 ();
		else
			{
			y_draw = 15;	x_draw = -16;										// Bottom Left
			cel_quad.x_pos0 = (x_draw*6) - ((6 - (y_draw*6))>>1);
			cel_quad.y_pos0 = (6 - (y_draw*6));
						
			y_draw = 15;	x_draw = 15;
			cel_quad.x_pos1 = 6 + ((x_draw*6) - ((6 - (y_draw*6))>>1));			// Bottom Right	
			cel_quad.y_pos1 = (6 - (y_draw*6));
						
			y_draw = -16;	x_draw = 15;
			cel_quad.x_pos2 = (6 + 3 + ((x_draw*6) - ((6 - (y_draw*6))>>1)));	// Top Right
			cel_quad.y_pos2 = (6-(y_draw*6)) - 6;
						
			y_draw = -16;	x_draw = -16;
			cel_quad.x_pos3 = (3 + ((x_draw*6) - ((6 - (y_draw*6))>>1)));		// Top Left
			cel_quad.y_pos3 = (6-(y_draw*6)) - 6;
					
			cel_quad.shade 	= 0;
			arm_addmonocel (&cel_quad, 0, GREY_PAL, 1);
			}
		
		// DRAW ZOOM ICONS
			
		for (icon_loop = 0; icon_loop < 3; icon_loop++)
			{
			cel_quad.x_pos0 = 94 + (icon_loop << 4);
			cel_quad.y_pos0 = 85;
			cel_quad.shade = (icon_loop == (4-map_zoom)) ? 11 : 5;
			arm_addgamecel (&cel_quad, BASECEL_MAP_ZOOM + icon_loop, 1024, 1024);
			}
			
		// DRAW HUNTING CROSS / LOCK
		
		cel_quad.shade = 11;
		
		if (nearest_found == TRACKING_NOTHING)
			{
			item_xpos = cross_x<<24;												// Set pos
			item_ypos = cross_y<<24;
			map_getoffsets (&item_xpos, &item_ypos);
			cel_quad.x_pos0 = item_xpos - 16;
			cel_quad.y_pos0 = item_ypos - 16; 
			arm_addgamecel (&cel_quad, BASECEL_MAP_QUESTION + 1, 1024, 1024);		// Draw it
			}
		else
			{
			cross_lock_fail = 0;
			
			if (status.track_type == nearest_found)
				{
				if (nearest_found == TRACKING_SHIP)
					{
					if (status.ship_tracking == nearest_ship)
						cross_lock_fail = 1;
					}
				else
					{
					if (status.track_xpos == tracking_x && status.track_ypos == tracking_y)
						cross_lock_fail = 1;
					}
				}
					
			if (cross_lock_fail == 0)
				{
				item_xpos = tracking_x << 24;										// Get ship's / gobj x & y pos
				item_ypos = tracking_y << 24;
				if (map_getoffsets (&item_xpos, &item_ypos) == 1)
					{
					cel_quad.x_pos0 = item_xpos-6;
					cel_quad.y_pos0 = item_ypos-6;
					arm_addgamecel (&cel_quad, BASECEL_MAP_QUESTION, 1024, 1024);	// Draw it (target found)
					}
				}
			}
			
// ----------------------

		// DRAW GROUND OBJECTS, SHIPS, FLASHING TARGETS, ICONS ON SCREEN THEN RENDER AND DISPLAY SCREEN
		
		if (mission.scanner_bust == 0)
			{
			arm_drawmaptargets (poly_map, map_zoom, &cel_quad, map_x | (map_y<<8));				// Draw map objects
			}
		
		map_drawships (map_timer & 4);															// Draw ships
		map_drawtargets (map_timer & 4);														// Draw targets
		screen_update (S_CEL | S_BANK | S_WAIT);												// Render screen
		
// ----------------------

		// UPDATE TIMERS
		
		map_timer++;

		}
	while (!(keypad & (ControlStart | ControlX)));

// ----------------------

	// RETURN TO GAME
	
	cel_quad.temp_cels=0;
	load_gamecels(NULL);
	screen_setswap (1);
}

/**************************************/

void	map_draw512 (void)

// Purpose : Draw large scale map centred around the player (from 512x512 map)
// Accepts : Nothing
// Returns : Nothing

// NOTE    : Zoom of 0 is FULL SCREEN, 1 is HALF SCREEN

{

long	x_draw,
		y_draw,
		
		actual_mapxpos,
		actual_mapypos,
		
		x_screen,
		y_screen;
		
long	zoom = 4-map_zoom;
long	shift_zoom = 3-zoom;
long	height_shift = map_zoom;

	// Main render loop for segments
	
	if (mission.scanner_bust == 1)
		cel_quad.shade = 2;
				
	for (y_draw = -16; y_draw <= 15; y_draw++)
		{
		actual_mapypos = (map_y + (y_draw << shift_zoom)) & 255;
		for (x_draw = -16; x_draw <= 15; x_draw++)
			{
			y_screen = 6-(y_draw*6);
			x_screen = (x_draw*6) - (y_screen>>1);
			
			actual_mapxpos = (map_x + (x_draw << shift_zoom)) & 255;
			
			cel_quad.x_pos0 = MAPBASEX + x_screen;
			cel_quad.y_pos0 = MAPBASEY + y_screen - ((height_map [actual_mapypos] [actual_mapxpos])>>height_shift);
			
			cel_quad.x_pos1 = MAPBASEX + x_screen+6;
			cel_quad.y_pos1 = MAPBASEY + y_screen - ((height_map [actual_mapypos] [(actual_mapxpos+(1<<shift_zoom))&255])>>height_shift);
			
			cel_quad.x_pos2 = MAPBASEX + x_screen+6+3;
			cel_quad.y_pos2 = MAPBASEY + y_screen-6 - ((height_map [(actual_mapypos+(1<<shift_zoom))&255] [(actual_mapxpos+(1<<shift_zoom))&255])>>height_shift);
			
			cel_quad.x_pos3 = MAPBASEX + x_screen+3;
			cel_quad.y_pos3 = MAPBASEY + y_screen-6 - ((height_map [(actual_mapypos+(1<<shift_zoom))&255] [actual_mapxpos])>>height_shift);

			if (mission.scanner_bust == 0)
				cel_quad.shade = 8+((height_map [actual_mapypos] [actual_mapxpos]) >> 3);
			
			arm_addcelfrom512map (&cel_quad, actual_mapxpos, actual_mapypos, -zoom);
			}
		}

	// Draw bottom map edge
		
	y_draw = -16;
	for (x_draw = -16; x_draw <= 15; x_draw++)
		{
		y_screen = 6-(y_draw*6);
		x_screen = (x_draw*6) - (y_screen>>1);
	
		actual_mapxpos = (map_x + (x_draw << shift_zoom)) & 255;
		actual_mapypos = (map_y + (y_draw << shift_zoom)) & 255;
			
		cel_quad.x_pos0 = MAPBASEX + x_screen;
		cel_quad.y_pos0 = MAPBASEY + y_screen - ((height_map [actual_mapypos] [actual_mapxpos])>>height_shift);
			
		cel_quad.x_pos1 = MAPBASEX + x_screen+6;
		cel_quad.y_pos1 = MAPBASEY + y_screen - ((height_map [actual_mapypos] [(actual_mapxpos+(1<<shift_zoom))&255])>>height_shift);
		
		cel_quad.x_pos2 =  cel_quad.x_pos1;
		cel_quad.x_pos3 =  cel_quad.x_pos0;
		
		cel_quad.y_pos2 = MAPBASEY + y_screen + 8;
		cel_quad.y_pos3 = MAPBASEY + y_screen + 8;
		
		cel_quad.shade = (x_draw & 1);		
		arm_addmonocel (&cel_quad, 0, RED_PAL, 0);
		}
		
	// Draw side map edge
		
	x_draw = 15;
	for (y_draw = -16; y_draw <= 15; y_draw++)
		{
		y_screen = 6-(y_draw*6);
		x_screen = (x_draw*6) - (y_screen>>1);
	
		actual_mapxpos = (map_x + (x_draw << shift_zoom)) & 255;
		actual_mapypos = (map_y + (y_draw << shift_zoom)) & 255;
			
		cel_quad.x_pos0 = MAPBASEX + x_screen+6;
		cel_quad.y_pos0 = MAPBASEY + y_screen - ((height_map [actual_mapypos] [(actual_mapxpos+(1<<shift_zoom))&255])>>height_shift);
			
		cel_quad.x_pos1 = MAPBASEX + x_screen+6+3;
		cel_quad.y_pos1 = MAPBASEY + y_screen-6 - ((height_map [(actual_mapypos+(1<<shift_zoom))&255] [(actual_mapxpos+(1<<shift_zoom))&255])>>height_shift);
		
		cel_quad.x_pos2 =  cel_quad.x_pos1;
		cel_quad.x_pos3 =  cel_quad.x_pos0;
		
		cel_quad.y_pos2 = MAPBASEY + y_screen + 8;
		cel_quad.y_pos3 = MAPBASEY + y_screen + 8+6;
		
		cel_quad.shade = 1+(y_draw & 1);		
		arm_addmonocel (&cel_quad, 0, RED_PAL, 0);
		}
		
	// Draw letters on map bottom
	
	cel_quad.shade = 11;
	for (x_draw = -4; x_draw <= 3; x_draw++)
		{
		y_screen = map_y<<24;
		actual_mapxpos = (x_draw<<29) + (8<<24);
		
		if (map_getoffsets (&actual_mapxpos, &y_screen) == 1)
			{
			cel_quad.x_pos0 = actual_mapxpos - 48;
			cel_quad.y_pos0 = MAPBASEY + 102;
			arm_addgamecel (&cel_quad, BASECEL_MAP_LETTERS + (x_draw & 7), 1024, 1024);
			}
		}
		
	// Draw numbers on map side
	
	for (y_draw = -4; y_draw <= 3; y_draw++)
		{
		x_screen = map_x<<24;
		actual_mapypos = (y_draw<<29) + (8<<24);
		
		if (map_getoffsets (&x_screen, &actual_mapypos) == 1)
			{
			cel_quad.x_pos0 = (MAPBASEX + 90) - (actual_mapypos >> 1);
			cel_quad.y_pos0 = actual_mapypos + 4;
			arm_addgamecel (&cel_quad, BASECEL_MAP_NUMBERS + (y_draw & 7), 1024, 1024);
			}
		}
}

/**************************************/

void	map_drawships (long map_toggle)

// Purpose : Draw all ships on map centred around the player
// Accepts : Toggle
// Returns : Nothing

{
ship_stack	*ship = (ship_stack*) (ships.info).start_address,
			*nearest_found_ship = NULL;

long		distance,
			item_xpos,
			item_ypos,
			item_type;
	
	nearest_distance = -1;							// Reset search distance
	
	if (mission.scanner_bust == 0)					// Only look & display of scanner O.K
		{
		while ((ship->header).status == 1)			// Main search loop
			{
			item_type = (ship->type) >> 4;			// Get actual type of ship (big ship, fighter etc.)

			// DISPLAY DETAILS FOR SHIP
			
			if (map_toggle == 0)
				{
				
				// DISPLAYING SHIPS INFO (0) - DIRECTION, SIDE ?
			
				switch ((ship->type) >> 4)
					{
					case SMALL_SHIP :
					case BIG_SHIP :
					case CAR :
						map_addshipitem1 (ship, 0);
						break;
					
					case PARACHUTE :
						map_addshipitem2 (ship, BASECEL_MAP_TYPE_PARACHUTE);
						break;
										
					case WEAPON :
						if (ship->type == AIR_TO_AIR)
							map_addshipitem2 (ship, BASECEL_MAP_TYPE_MISSILE);
						break;				
					}
				}
			
			else
			
				{
				
				// DISPLAYING SHIPS INFO (1) - TYPE, HEIGHT ETC.
				
				switch ((ship->type) >>4)
					{
					case CAR :
						map_addshipitem2 (ship, BASECEL_MAP_TYPE_VEHICLE);
						map_addshipitem1 (ship, 1);
						break;
						
					case SMALL_SHIP :
						map_addshipitem2 (ship, BASECEL_MAP_TYPE_FIGHTER + ship->special_data->control_mode);
						map_addshipitem1 (ship, 1);
						break;
					
					case BIG_SHIP :
						if (ship->type == MOTHER_SHIP)
							map_addshipitem2 (ship, BASECEL_MAP_TYPE_MOTHERSHIP);
						else
							map_addshipitem2 (ship, BASECEL_MAP_TYPE_SPACESHIP);
						map_addshipitem1 (ship, 1);
						break;
					
					case SATELLITE :
						map_addshipitem2 (ship, BASECEL_MAP_TYPE_SATELLITE);
						map_addshipitem1 (ship, 1);
						break;
					}
				}
		
			// SHOULD THIS MARKED FOR A QUESTION MARK ?
			
			if (item_type == SMALL_SHIP || item_type == BIG_SHIP ||
				item_type == CAR 		|| item_type == PLAYERS_SHIP || item_type == SATELLITE)
				{
				item_xpos = (((map_x + cross_x) & 255) <<24) - ship->x_pos;		// Get ship's xpos relative to cross
				item_ypos = (((map_y + cross_y) & 255) <<24) - ship->y_pos;		// Get ship's ypos relative to cross
				
				item_xpos = (item_xpos - (map_x << 24)) >> 24;					// Scale to X&Y to 255 & Invert Y
				item_ypos = -(item_ypos - (map_y << 24) >> 24); 

				item_xpos = item_xpos << (4-map_zoom);							// Scale to zoom
				item_ypos = item_ypos << (4-map_zoom);
	
				distance = (item_xpos * item_xpos) + (item_ypos * item_ypos);	// Get actual distance

				if (distance < 256)												// Is it within scan range ?	
					if (distance < nearest_distance || nearest_distance == -1)	// Yes - nearer than last ?
						{
						nearest_found_ship = ship;								// Yes - remember ship
						nearest_distance = distance;							// Yes - remember distance
						}
				}
				
			// GET NEXT SHIP IN LIST
		
			ship = (ship_stack*) (ship->header).next_address;
			}
			
		nearest_found = (nearest_found_ship == NULL) ? TRACKING_NOTHING : TRACKING_SHIP;
		if (nearest_found_ship != NULL)
			{
			nearest_ship = nearest_found_ship;
			tracking_x = nearest_ship->x_pos >> 24;
			tracking_y = nearest_ship->y_pos >> 24;
			}
		}
		
		// NOW DO PLAYERS SHIP, LAST
		
		if (map_toggle == 0)
			map_addshipitem1 (players_ship, 0);
}

/**************************************/

void	map_addshipitem1 (ship_stack *ship, long info_type)

// Purpose : Displays a single item (ship, missile etc.) on the screen, with direction, side (FRAME 1 INFO)
// Accepts : Pointer to ship & info type (0 is direction, 1 is kill/save status)
// Returns : Now't

{
		
long	item_xpos = ship->x_pos;									// Get ship's xpos
long	item_ypos = ship->y_pos;									// Get ship's ypos
long	item_rotation = (4+(((ship->x_rot + (64<<10)))>>17) &7);	// Get rotation (0 to 7)
long	item_side;
		
	if (ship->special_data == NULL)									// Ship must have SDB for this display
		return;
		
	if (map_getoffsets (&item_xpos, &item_ypos) == 1)				// Adjust co-ordinates for around players pos & zoom
		{
		
		cel_quad.shade = 11;
		cel_quad.x_pos0 = item_xpos;
		cel_quad.y_pos0 = item_ypos;
			
		if (info_type == 0)
			{
			arm_addgamecel (&cel_quad, BASECEL_MAP_DIRECTION + item_rotation, 1024, 1024);	// Display direction arrow
			if (ship != players_ship)														// Do colour for side ?
				{
				item_side = ship->special_data->side;										// Get side ship is on
				arm_setgamecelpalette (&cel_quad,BASECEL_MAP_PALETTE + 1 + item_side);		// Set palette for correct side
				}
			}
			
		else
			{
			cel_quad.x_pos0-=3;
			cel_quad.y_pos0-=3;
		
			switch (ship->special_data->killstatus)
				{
				case SDB_KILL_MUSTSURVIVE :
				case SDB_KILL_MUSTSURVIVEFLIGHT :
					arm_addgamecel (&cel_quad, BASECEL_MAP_STATUS, 1024, 1024);				// Display save icon ?
					break;
			
				case SDB_KILL_MUSTDIE :
				case SDB_KILL_MUSTSTOPFLIGHT :
					arm_addgamecel (&cel_quad, BASECEL_MAP_STATUS+1, 1024, 1024);			// Display kill icon ?
					break;
				}
				
			// DISPLAY QUESTION MARK ICON AROUND TRACK LOCKED SHIP ?
		
			if (status.track_status == TRACKING_STATUS_LOCKED &&
				status.track_type == TRACKING_SHIP &&
				status.ship_tracking == ship)
				
				{
				cel_quad.x_pos0-=3;
				cel_quad.y_pos0-=3;
				arm_addgamecel (&cel_quad, BASECEL_MAP_QUESTION, 1024, 1024);				// Display status icon
				}
			}
		}
}

/**************************************/

void	map_addshipitem2 (ship_stack *ship, long item_type_sprite)

// Purpose : Displays a single item (ship, missile etc.) on the screen, with direction, side (FRAME 2 INFO)
// Accepts : Pointer to ship, sprite to use
// Returns : Now't

{
long	item_xpos = ship->x_pos;										// Get ship's xpos
long	item_ypos = ship->y_pos;										// Get ship's ypos
		
	if (map_getoffsets (&item_xpos, &item_ypos) == 1)					// Adjust co-ordinates for around players pos & zoom
		{
		cel_quad.shade = 11;
		cel_quad.x_pos0 = item_xpos;
		cel_quad.y_pos0 = item_ypos;
		arm_addgamecel (&cel_quad, item_type_sprite, 1024, 1024);		// Display direction arrow
		}
}

/**************************************/

void	map_drawtargets (long map_toggle)

// Purpose : Displays all ground targets on the map
// Accepts : Display toggle
// Returns : Now't

{

long	object_loop,
		item_xpos,
		item_ypos,
		item_gridxpos,
		item_gridypos;

long	distance,
		item_tempnearest_x = -1,
		item_tempnearest_y = -1;
		
	if (map_toggle != 0 || mission.scanner_bust == 1)							// Don't print anything when scanner nackered or toggled off
		return;
	
	// Work out what nearest distance check should be (ie. temp tracking ship / nothing)
	
	cel_quad.shade 	= 10;
		
	for (object_loop =0 ; object_loop < mission.groundcount; object_loop++)
		{
		if (mission.groundlist [object_loop].type == GROUND_DESTROY || cheat_feature4 == 1)	// Should be just GROUND_DESTROY targets
			{
			item_gridxpos = mission.groundlist [object_loop].xpos;
			item_gridypos = mission.groundlist [object_loop].ypos;
			
			item_xpos = item_gridxpos << 24;
			item_ypos = item_gridypos << 24;
			
			if (map_getoffsets (&item_xpos, &item_ypos) == 1)					// Adjust co-ordinates for around players pos & zoom
				{
				item_ypos-=((height_map [mission.groundlist [object_loop].ypos] [mission.groundlist [object_loop].xpos]) << (4-map_zoom));
				cel_quad.x_pos0 = item_xpos;
				cel_quad.y_pos0 = item_ypos;
				arm_addgamecel (&cel_quad, (mission.groundlist [object_loop].type == GROUND_DESTROY) ? BASECEL_MAP_TARGET : BASECEL_MAP_CHEAT, 1024, 1024);		// Display direction arrow
				}
	
			distance = (item_xpos * item_xpos) + (item_ypos * item_ypos);		// Get actual distance
			
			if (distance < 256)													// Is it within scan range ?	
				if (distance < nearest_distance || nearest_distance == -1)		// Yes - nearer than last
					{
					distance = nearest_distance;
					item_tempnearest_x = item_gridxpos;
					item_tempnearest_y = 255 - item_gridypos;
					}
			
			// DISPLAY QUESTION MARK ICON AROUND TRACK LOCKED GROUND OBJECT ?
		
			if (status.track_status == TRACKING_STATUS_LOCKED &&
				status.track_type == TRACKING_GROUND &&
				status.track_xpos == item_gridxpos &&
				status.track_ypos == item_gridypos
				)
				
				{
				cel_quad.x_pos0-=8;
				cel_quad.y_pos0-=8;
				arm_addgamecel (&cel_quad, BASECEL_MAP_QUESTION, 1024, 1024);				// Display status icon
				}	
			}
		}

	if (item_tempnearest_x >=0)
		{
		nearest_found = TRACKING_GROUND;
		tracking_x = item_tempnearest_x;
		tracking_y = item_tempnearest_y;
		}
				
}

/**************************************/

long	map_getoffsets (long *item_xpos, long *item_ypos)

// Purpose : Centralises points around the player, and does zoom
// Accepts : Pointers to X,Y pos
// Returns : 1 if still on map screen , else 0

{
	*item_xpos = (*item_xpos - (map_x << 24)) >> 24;				// Scale to X&Y to 255 & Invert Y
	*item_ypos = -(*item_ypos - (map_y << 24) >> 24); 

	*item_xpos = *item_xpos << (4-map_zoom);						// Scale to zoom
	*item_ypos = *item_ypos << (4-map_zoom);
	
	if (*item_xpos >= -128 && *item_ypos >= -128 && *item_xpos <= 128 && *item_ypos <= 128)
		{
		*item_xpos -= (*item_ypos >> 1);
		*item_xpos = MAPBASEX + (*item_xpos >> 1) + (*item_xpos >> 2);
		*item_ypos = MAPBASEY + (*item_ypos >> 1) + (*item_ypos >> 2);
		return (1);
		}
	
	return (0);
}

/**************************************/

void	map_showbrief (void)

// Purpose : Shows mission brief again
// Accepts : Nothing
// Returns : Nothing

{
long	brief_status;

	control_showmission (1, -1);								// Setup mission brief
	
	do
		{
		screen_base (S_WAIT);									// Wait to stop flicker
		cel_quad.temp_cels=0;									// Reset plotting list
		map_backdrop ();										// Do backdrop
		brief_status = control_showmission (0, 0);				// Do brief
		screen_update(S_CEL | S_MENU | S_BANK | S_WAITFRAME);	// Render screen, flip banks with wait
		}
	while (brief_status == BRIEF_CONTINUE);						// Loop until quit / finish
	
	music_terminate ();
	music_initialise (0);										// Initialise music player
	if (configuration.music_on == 1)
		music_play (MUSIC_MOVETRACK, 0, -1);					// Continue playlist
	menu_enter (NO_MENU, 1);									// Clear menu
}

/**************************************/

void	map_backdrop (void)

// Purpose : Draws map backdrop on screen
// Accepts : Nothing
// Returns : Nothing

{
long	x_backdrop,
		y_backdrop;
		
	cel_quad.shade = 5;
	for (x_backdrop = 0; x_backdrop <=3; x_backdrop++)
		for (y_backdrop = 0; y_backdrop <=2; y_backdrop++)
			{
			cel_quad.x_pos0 = (x_backdrop * 100)-160;
			cel_quad.y_pos0 = (y_backdrop * 80)-120;
			arm_addgamecel (&cel_quad, BASECEL_MAP_BACKDROP, 1024, 1024);
			}
}

/**************************************/

void	map_showstatus (ship_stack *ship)

// Purpose : Shows status for ship
// Accepts : Ship
// Returns : Nothing

#define	icon_xposition			-75
#define	icon_ystartposition		-68
#define	icon_ygap				21
#define	icon_xgap				24

{

char	text_string [48];

long	item_loop,
		item_valueloop,
		icon_perfvalue;

long	info_x,
		info_y,
		height,
		width,
		baricon,
		item_maxloop = (ship == players_ship) ? 12 : 7;

long	keypad;
		
map_performance	*data_pointer;
								
// PLAYER DISPLAY DATA

map_performance	player_items [12] =	

					{
						{BASECEL_MAP_ENGINE,		BASECEL_MAP_ENGINE+1,	0,	0,	7},
						{BASECEL_MAP_CONTROL,		BASECEL_MAP_CONTROL+1,	0,	1,	7},
						{BASECEL_MAP_SHIELDS,		BASECEL_MAP_SHIELDS+1,	0,	2,	7},
						{BASECEL_MAP_WEAPON + 0,	BASECEL_MAP_LASERBAR,	0,	3,	7},
						{BASECEL_MAP_WEAPON + 1,	-1,						-2,	4,	2},
						{BASECEL_MAP_WEAPON + 2,	-1,						-2,	5,	2},
						{BASECEL_MAP_WEAPON + 3,	-1,						-2,	6,	2},
						{BASECEL_MAP_WEAPON + 4,	-1,						7,	4,	2},
						{BASECEL_MAP_WEAPON + 5,	-1,						7,	5,	2},
						{BASECEL_MAP_WEAPON + 6,	-1,						7,	6,	2},
						{BASECEL_MAP_ECM,			-1,						-2,	7,	2},
						{BASECEL_MAP_LIVES,			-1,						7,	7,	2}
					};

// OTHER SHIP / VEHICLE DISPLAY DATA

map_performance	other_items [7] =	

					{
						{BASECEL_MAP_ENGINE,		BASECEL_MAP_ENGINE+1,	0,	0,	7},
						{BASECEL_MAP_CONTROL,		BASECEL_MAP_CONTROL+1,	0,	1,	7},
						{BASECEL_MAP_SHIELDS,		BASECEL_MAP_SHIELDS+1,	0,	2,	7},
						{BASECEL_MAP_WEAPON +0,		BASECEL_MAP_LASERBAR,	0,	3,	7},
						{BASECEL_MAP_MODE,			-1,						-3,	5,	13},
						{BASECEL_MAP_HEIGHT,		-1,						-3,	6,	13},
						{BASECEL_MAP_KILL,			-1,						-3,	7,	13}
					};
					
	cel_quad.temp_cels=0;
	
	// Shade out backdrop screen
	
	cel_quad.x_pos0 = -160;		cel_quad.y_pos0 = -120;
	cel_quad.x_pos1 = +160;		cel_quad.y_pos1 = -120;
	cel_quad.x_pos2 = +160;		cel_quad.y_pos2 = +120;
	cel_quad.x_pos3 = -160;		cel_quad.y_pos3 = +120;
	cel_quad.shade 	= 2;
	arm_addmonocel (&cel_quad, 0, GREY_PAL, 1);
	
	// Draw backdrop for ship title
	
	cel_rectangle (25, 36, 319-25, 12, (ship->special_data->side == SDB_SIDE_ENEMY) ? RED_PAL : GREEN_PAL, REC_BORDER | REC_BOX, 1);

	// Put in ship title text
	
	SetTextCelColor(cel_text [0], 0, WHITE_15);
	UpdateTextInCel(cel_text [0], TRUE, map_return_ship_title (ship, 1));
	GetTextCelSize (cel_text [0], &width, &height);
	SetTextCelCoords(cel_text [0], 160-(width>>1), 18);
	(cel_text [0]->tc_CCB) -> ccb_Flags |= CCB_LAST;
		
	// Draw all ships status items
	
	for (item_loop = 0; item_loop < item_maxloop; item_loop++)
		{
		
		data_pointer = (ship == players_ship) ? &player_items [item_loop] : &other_items [item_loop];
		
		switch (item_loop)
			{
			case 0 :
				icon_perfvalue = ship->performance->engine;
				break;
			case 1 :
				icon_perfvalue = ship->performance->control;
				break;
			case 2 :
				icon_perfvalue = ship->performance->shields;
				break;
			default :
				icon_perfvalue = ship->performance->weapons [item_loop-3];
				break;
			}
		
		info_x = icon_xposition + (data_pointer->x_pos * 16);
		info_y = icon_ystartposition + (data_pointer->y_pos * icon_ygap);
		
		// DRAW GRAPH FOR BARS ?
		
		if (data_pointer->bar != -1)
			{		
			cel_quad.x_pos0 = info_x + icon_xgap;
			cel_quad.y_pos0 = info_y;
			cel_quad.shade = 10;
			for (item_valueloop = 0; item_valueloop <= data_pointer->x_length; item_valueloop++)
				{
				baricon = BASECEL_MAP_GRAPH+1;
				if (item_valueloop == 0)
					baricon = BASECEL_MAP_GRAPH;
				if (item_valueloop == data_pointer->x_length)
					baricon = BASECEL_MAP_GRAPH+2;
				cel_quad.shade = 11;
				arm_addgamecel (&cel_quad, baricon, 1024, 1024);			// Draw graph bits
				cel_quad.x_pos0+=16;
				}
			}
			
		// DRAW RECTANGLE FOR INFO
		
		cel_rectangle (info_x+160-2, info_y+18+120, info_x + 18 +160, info_y-2+120, GREY_PAL, REC_BORDER | REC_BOX, 1);
		cel_rectangle (info_x+icon_xgap+160, info_y+18+120, info_x + icon_xgap + ((1 + data_pointer->x_length) *16) + 160, info_y-2+120, BLUE_PAL, REC_BORDER | REC_BOX, 1);
		
		// DRAW ICON GRAPHICS
		
		cel_quad.x_pos0 = info_x;
		cel_quad.y_pos0 = info_y;
		cel_quad.shade = 11;
		arm_addgamecel (&cel_quad, data_pointer->icon, 1024, 1024);
		
		// DRAW BARS FOR VALUE ?
		
		if (data_pointer->bar != -1)
			{
			cel_quad.x_pos0 = info_x + icon_xgap;
			if (icon_perfvalue < 0)		icon_perfvalue = 0;
			if (icon_perfvalue > 7)		icon_perfvalue = 7;
			
			for (item_valueloop = 0; item_valueloop < icon_perfvalue; item_valueloop++)
				{
				cel_quad.shade = 11;
				arm_addgamecel (&cel_quad, data_pointer->bar, 4096, 1024);	// Draw bar
				cel_quad.x_pos0+=16;
				}
			}	
		}
	
	screen_update (S_CEL | S_MENU);

	if (ship == players_ship)
		{
		sprintf(text_string, "%d", ship->performance->weapons [WEAPON_ATG]);
		map_statustext (&player_items [4], text_string, GREY_15);
		
		sprintf(text_string, "%d", ship->performance->weapons [WEAPON_ATA]);
		map_statustext (&player_items [5], text_string, GREY_15);
		
		sprintf(text_string, "%d", ship->performance->weapons [WEAPON_MEGA_BOMB]);
		map_statustext (&player_items [6], text_string, GREY_15);
		
		sprintf(text_string, "%d", ship->performance->weapons [WEAPON_BEAM_LASER]);
		map_statustext (&player_items [7], text_string, GREY_15);
		
		sprintf(text_string, "%d", ship->performance->weapons [WEAPON_MULTI_MISSILE]);
		map_statustext (&player_items [8], text_string, GREY_15);
		
		sprintf(text_string, "%d", ship->performance->weapons [WEAPON_MINE]);
		map_statustext (&player_items [9], text_string, GREY_15);
		
		sprintf(text_string, "%d", ship->performance->weapons [WEAPON_ECM]);
		map_statustext (&player_items [10], text_string, GREY_15);
		
		sprintf(text_string, "%d", status.lives);
		map_statustext (&player_items [11], text_string, YELLOW_15);
		}
	
	else
	
		{
		sprintf(text_string, "Mode : %s", map_return_ship_mode (ship));
		map_statustext (&other_items [4], text_string, GREY_15);
		
		sprintf(text_string, "Height : %d Metres", ((ship->z_pos - players_ship->z_pos)>>17) / 25);
		map_statustext (&other_items [5], text_string, GREY_15);
		
		sprintf(text_string, "Mission : %s", map_return_ship_killstatus (ship));
		map_statustext (&other_items [6], text_string, GREY_15);
		}
	
	screen_update (S_WAIT | S_MENU | S_BANK);
	
	do
		{
		screen_update (S_WAIT);									// Wait for frame
		keypad = controlpad_debounce ();						// Read keypad
		}
	while (keypad == 0);
}

/**************************************/

void map_statustext (map_performance *data_pointer, char *string, long colour)

// Purpose : Displays status info text
// Accepts : Pointer to status entry, string to print, colour to do it in
// Returns : Nothing

{

long	info_x = icon_xposition + (data_pointer->x_pos * 16),
		info_y = icon_ystartposition + (data_pointer->y_pos * icon_ygap);
	
	font_paint(0, colour, 160 + info_x + icon_xgap + 8, 122 + info_y, MENU_FONT, SCR_UPDATE, string);
}	

/**************************************/

char*	map_return_ship_title (ship_stack *ship, long returnside)

// Purpose : Returns pointer to title, side & pilot of ship
// Accepts : Pointer to ship
// Returns : Pointer to string

{

#define ship_returnside (returnside == 0) ? no_string : ((ship->special_data->side != SDB_SIDE_ENEMY) ? no_string : message_decode (MTXT__ENEMY, 0))
	
static	char	ship_endstring [48];
char			ship_titlestring [20];
long			ship_id = (ship->type) & 15;
char			no_string [1] = {0};

	switch ((ship->type) >> 4) 
		{
		
		case PLAYERS_SHIP :
			sprintf (ship_titlestring, "%s", message_decode (MTXT__STARFIGHTER, 0));
			break;
			
		case BIG_SHIP :
			if (ship_id != 1)
				sprintf (ship_titlestring, "%s %s", ship_returnside, message_decode (MTXT__STRATOLINER, 0));
			else
				sprintf (ship_titlestring, "%s %s", ship_returnside, message_decode (MTXT__MOTHERSHIP, 0));	
			break;

		case SMALL_SHIP :
			sprintf (ship_titlestring, "%s %s", ship_returnside, message_decode (MTXT__FIGHTER, 0));
			break;
								
		case SATELLITE :
			sprintf (ship_titlestring, "%s %s", ship_returnside, message_decode (MTXT__SATELLITE, 0));
			break;
			
		case CAR :
			if (ship_id == 0)
				sprintf (ship_titlestring, "%s %s", ship_returnside, message_decode (MTXT__VEHICLE, 0));
			else
				sprintf (ship_titlestring, "%s %s", ship_returnside, message_decode (MTXT__TANK, 0));	
			break;
		}
	
	// Put in pilot's name ?
	
	sprintf (ship_endstring, "%s", ship_titlestring);
	
	if (ship->special_data->side != SDB_SIDE_ENEMY)
		if (ship->special_data->pilot_name != 0)
			sprintf (ship_endstring, "%s - %s", ship_titlestring, pilot_names [ship->special_data->pilot_name]);
	
	return (ship_endstring);
}

/**************************************/

char*	map_return_ship_mode (ship_stack *ship)

// Purpose : Returns pointer to ships mode
// Accepts : Pointer to ship
// Returns : Pointer to string

{

static	char	ship_endstring [48];

	switch (ship->special_data->control_mode)
		{
		case SDB_MODE_ATTACK :
			if (ship->special_data->target_type == SDB_TARGETTYPE_MULTIPLE)
				sprintf (ship_endstring, "%s %s", message_decode (MTXT__ATTACKING, 0), message_decode (MTXT__MULTIPLE_TARGETS, 0));
			else
				sprintf (ship_endstring, "%s %s", message_decode (MTXT__ATTACKING, 0), map_return_ship_title ((ship_stack*) ship->special_data->command_address, 0), 0);
			break;
			
		case SDB_MODE_FLIGHTPATH :
			sprintf (ship_endstring, "%s", message_decode (MTXT__PATROL, 0));
			break;
			
		case SDB_MODE_FORMATION :
			sprintf (ship_endstring, "%s %s", message_decode (MTXT__FORMATION_WITH, 0), map_return_ship_title ((ship_stack*) ship->special_data->command_address, 0), 0);
			break;
		}
	
	return (ship_endstring);
}

/**************************************/

char*	map_return_ship_killstatus (ship_stack *ship)

// Purpose : Returns pointer to mission kill status string
// Accepts : Pointer to ship
// Returns : Pointer to string

{		

static	char	ship_endstring [16];

	switch (ship->special_data->killstatus)
		{
		case SDB_KILL_MUSTDIE :
		case SDB_KILL_MUSTSTOPFLIGHT :
			sprintf (ship_endstring, "%s", message_decode (MTXT__KILL, 0));
			break;
			
		case SDB_KILL_MUSTSURVIVEFLIGHT :
		case SDB_KILL_MUSTSURVIVE :	
			sprintf (ship_endstring, "%s", message_decode (MTXT__DEFEND, 0));
			break;
			
		default :
			sprintf (ship_endstring, "%s", message_decode (MTXT__IMMATERIAL, 0));
			break;
		}
	
	return (ship_endstring);
}
