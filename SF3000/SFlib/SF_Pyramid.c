// File : SF_Pyramid
// Mission Selection Routines


// Includes

#include "SF_Pyramid.h"
#include "SF_Palette.h"
#include "SF_Utility.h"
#include "SF_Access.h"
#include "SF_IO.h"
#include "SF_Menu.h"
#include "SF_ScreenUtils.h"
#include "SF_ARMCell.h"

/**************************************/

// Planet type definitions

#define	PYR_EARTH			0					// Pyramid description types
#define	PYR_EARTHSEA		1
#define	PYR_SPACE			2
#define	PYR_WARRIOR			3
#define	PYR_CHEMICAL		4
#define	PYR_STAR			5
#define	PYR_ICE				6

// Mission type definitions

#define	PYR_DEFENSIVE		0					// Mission types
#define	PYR_MIXEDCOMBAT		1
#define	PYR_GROUNDCOMBAT	2
#define	PYR_AERIALCOMBAT	3
#define	PYR_TRAINING		4

/**************************************/

// Global Variables

long	flash_count,
		planet_count,
		pyramid_count,
		planet_sine,
		pyramid_pause,
		selector_x,
		selector_y,
		pause_description;

pyramid	pyramids [4] =	{	{4,	0,	0, {}},
							{4,	0,	0, {}},
							{4,	0,	0, {}},
							{4,	0,	0, {}}
						};

													
/**************************************/

void	pyramid_initialiselevel (long	level_number)

// Purpose : Clears a game level pyramid (if -ve, Clears current pyramid, as in parameters.level)
// Accepts : Nothing
// Returns : Nothing

{
long	mission_loop;
	
	if (level_number < 0)
		level_number = parameters.level;
		
	for (mission_loop=0; mission_loop < MAX_MISSIONS; mission_loop++)
		pyramids [level_number].mission [mission_loop] = MISSION_UNFINISHED;
		
	pyramids [level_number].y_level = 0;
	pyramids [level_number].x_level = 0;
	
}

/**************************************/

void	pyramid_initialisegame (void)

// Purpose : Clears all 4 game level pyramids
// Accepts : Nothing
// Returns : Nothing

{
long	level_loop;

	for (level_loop=0; level_loop <=3; level_loop++)		// Clear all pyramids
		pyramid_initialiselevel (level_loop);

}

/**************************************/

void	pyramid_initialiseselect (long level_number)

// Purpose : Sets up pyramid selection vars
// Accepts : Nothing
// Returns : Nothing

{
	pyramid_count = 0;										// Clear counters
	planet_count = 0;
	planet_sine = 512;
	pyramid_pause = 0;
	flash_count = 0;
	pause_description = 35;
	
	selector_x = pyramids [level_number].x_level;			// Set selector position
	selector_y = pyramids [level_number].y_level;
	
	message_initialise (0);									// Reset text messages
	menu_resetmenutext ();
	pyramid_showmission ();									// Set Mission Title
	menu_enter (MISSION_MENU, 1);
	load_gamecels ("Pyramid");
}

/**************************************/

long	pyramid_read (long keypad, long level_number)

// Purpose : Checks for and updates pyramid for user mission selection
// Accepts : Keypad reading
// Returns : Mission number selected, -1 for nothing

{

long	sublevel_loop,
		sublevel_count,
		mission_loop,
		planet_size,
		planet_scalar,
		planet_posscalar,
		sub_cosine,
		mission_num,
		planet_number,
		temp_selector_x;
	
	// ADVANCE PYRAMID TIMER & PLANET ROT
	
	flash_count = (flash_count+1) & 1023;
	
	if (pyramid_count != 128)
		pyramid_count +=2;
	
	if (planet_sine < 2048)
		planet_sine+=8;
			
	planet_count+=1;
	planet_scalar = (pyramid_count <= 128) ? pyramid_count : 128;
	planet_posscalar = (pyramid_count <= 128) ? pyramid_count : 128;
	
	if (pyramid_pause++ >= 1024)
		{
		pyramid_pause =0;
		planet_sine = -1024;
		}
	
	// DISPLAY ALL PLANETS IN THE PYRAMID
	
	sublevel_count=0;
	
	for (sublevel_loop = 0; sublevel_loop <= pyramids [level_number].sub_levels; sublevel_loop ++)
		{
		
		sublevel_count+=sublevel_loop;
		sub_cosine = planet_sine + (sublevel_loop << 6);
		
		if (sub_cosine < 0)
			sub_cosine=0;
			
		if (sub_cosine > 1024)
			{
			sub_cosine -=1024;
			if (sub_cosine > 1024)
				sub_cosine = 0;
			}
		
		// LOOP TO PLOT ALL PLANETS IN A ROW
		
		for (mission_loop = 0; mission_loop <= sublevel_loop; mission_loop++)
			{
			
			mission_num = (sub_cosine >=512) ? mission_loop : sublevel_loop - mission_loop;
			planet_size = (planet_posscalar <<3) - ((*(sine_table +sub_cosine) >> 4) * (mission_num - (sublevel_loop>>1)));
			planet_number = sublevel_count + mission_num;
			
			// CALCULATE PLANETS X,Y POSITION AND SHADE
			
			cel_quad.x_pos0 = (((((mission_num * planet_posscalar) - ((sublevel_loop * planet_posscalar) >>1 )) /3) * (cosine_table [sub_cosine])) >> 12) - (planet_size >>6);
			cel_quad.y_pos0 = 9+(-(((sublevel_loop * 8) - ((pyramids [level_number].sub_levels * 8) >> 1)) *planet_posscalar) >>5) - (planet_size >>6);
			cel_quad.shade = (mission_num == selector_x && sublevel_loop == selector_y) ? 16 : 5+ ((planet_scalar >>5) - ((*(sine_table +sub_cosine) >> 10) * (mission_num - (sublevel_loop>>1))));
			
			// DRAW THE PLANET (SPINNING EITHER WAY)
			
			if ((mission_loop + sublevel_loop)&1)
				arm_addgamecel (&cel_quad, BASECEL_PYRAMID_PLANETS+(((planet_count >>2) + ((sublevel_loop+mission_num)*7)) & 15), planet_size, planet_size);
			else
				arm_addgamecel (&cel_quad, BASECEL_PYRAMID_PLANETS+((((sublevel_loop+mission_num)*7)-(planet_count >>2)) & 15), planet_size, planet_size);

			// DRAW THE SELECTOR (IF THIS IS THE ONE THE CURSOR IS OVER)
			
			if (mission_num == selector_x && sublevel_loop == selector_y)
				arm_addgamecel (&cel_quad, BASECEL_PYRAMID_POINTER + ((pyramid_pause>>4)&3), planet_size, planet_size);
								
			// DRAW NUMBERS ON SCREEN (ONLY UNCOMPLETED)
			
			if ((flash_count>>3) > planet_number  && pyramids [parameters.level] .mission [planet_number] == MISSION_UNFINISHED)
				{
				cel_quad.x_pos0+=16;
				cel_quad.y_pos0+=16;
				if (sublevel_loop != selector_y || (((flash_count>>4)&1) == 1))
					cel_quad.shade-=4;
				arm_addgamecel (&cel_quad, BASECEL_PYRAMID_NUMBERS + planet_number, planet_size, planet_size);
				}
				
			// DRAW FLAG IF COMPLETED
			
			if (pyramids [parameters.level] .mission [planet_number] == MISSION_FINISHED)
				 {
				 cel_quad.shade = 7;
				 arm_addgamecel (&cel_quad, BASECEL_PYRAMID_COMPLETED + sublevel_loop, planet_size, planet_size);
				}
			}
		}

	// HAS PLAYER MADE SELECTION ?, RETURN CORRECT COMMAND
	
	if (keypad != 0)
		pyramid_pause =0;
		
	if (keypad & ControlX)
		return (PYRAMID_QUIT);
	
	if (keypad & (ControlStart | ControlA))
		if (pyramids [level_number].mission [return_missionpos (selector_x, selector_y)] == MISSION_UNFINISHED)
			{
			pyramid_showmission ();
			return (PYRAMID_START);
			}
			
	// NOTE : UP, DOWN & FINISH ARE ONLY AVAILABLE IF CHEAT FEATURE 1 HAS BEEN ENABLED

	if (cheat_feature1 == 1)
		{
		if ((keypad & ControlUp) && selector_y < 4)					selector_y+=1;		// Moving up ?
		if ((keypad & ControlDown) && selector_y > 0)									// Moving down ?
			{
			selector_y-=1;
			if (selector_x > selector_y)
				selector_x = selector_y;
			}
		
		if (keypad & ControlLeftShift)
			{
			pyramid_registercompleted();
			pyramid_initialiseselect(parameters.level);
			}
		}
		
	// ARE WE MOVING CURSOR ?
	
	if ((keypad & ControlLeft) && selector_x > 0)									// Moving Left ? Find next available
		{
		temp_selector_x = selector_x;
		do
			if (pyramids [level_number].mission [return_missionpos (--temp_selector_x, selector_y)] == MISSION_UNFINISHED)
				selector_x = temp_selector_x;
		while (selector_x != temp_selector_x && temp_selector_x > 0);
		}
	
		
	if ((keypad & ControlRight) && selector_x < selector_y)							// Moving Right ? Find next available
		{
		temp_selector_x = selector_x;
		do
			if (pyramids [level_number].mission [return_missionpos (++temp_selector_x, selector_y)] == MISSION_UNFINISHED)
				selector_x = temp_selector_x;
		while (selector_x != temp_selector_x && temp_selector_x < selector_y);
		}	
					
	// HAVE WE MOVED THE CURSOR ?
		
	if (selector_x != pyramids [level_number].x_level || selector_y != pyramids [level_number].y_level)
		{
		pyramids [level_number].x_level = selector_x;							// Set new x position in pyramid
		pyramids [level_number].y_level = selector_y;							// Set new y position in pyramid
		pause_description = 20;													// Display new missions details
		}

	// SHOULD WE DISPLAY MISSION TITLE & STUFF (WITH QUICK DELAY)
	
	if (pause_description >0)
		if (--pause_description == 0)
			pyramid_showmission();
			
	return (PYRAMID_CONTINUE);
}

/**************************************/

void	pyramid_showmission (void)

// Purpose : Displays mission title or 'finished'
// Accepts : Nothing
// Returns : Nothing

{

char	string [40];
long	mission_num = return_mission (&pyramids [parameters.level]);
	
	if (pyramids [parameters.level].mission [mission_num] == MISSION_FINISHED)		// Finished ?
		sprintf (message_decode (menus [MISSION_MENU].title, 1), "- %s -", message_decode (60, 3));		// Yes
	else
		sprintf (message_decode (menus [MISSION_MENU].title, 1), "- %s -", message_decode ((parameters.level * 15) + mission_num, 3));	// No

	menu_enter (MISSION_MENU, 1);													// Re-enter menu to redo title
}

/**************************************/

long	pyramid_registercompleted (void)

// Purpose : Registers a mission as completed on currently selected pyramid level
// Accepts : Nothing
// Returns : (1) if all missions on that sub level are complete, (0) if not, (2) if entire pyramid complete
// 																			[IN WHICH CASE IT IS CLEARED & DOES ONTO NEXT LEVEL]

{

long current_level = parameters.level;
long current_mission = return_mission (&pyramids [current_level]);
long x_loop;
long all_complete=1;

	// MARK THIS MISSION AS FINISHED
		
	pyramids [current_level].mission [current_mission] = MISSION_FINISHED;
	
	// CHECK TO SEE IF WE HAVE COMPLETED ALL MISSIONS ON SUB-LEVEL, IF SO, MOVE UP & RETURN (1), IF ALL PYRAMID FINISHED, RETURN (2)
	
	for (x_loop=0; x_loop <= pyramids [current_level].y_level; x_loop++)
		if (pyramids [current_level].mission [return_missionpos (x_loop, pyramids [current_level].y_level)] != MISSION_FINISHED)
			all_complete=0;
		
	// STILL SOME MISSIONS ON THIS SUB-LEVEL LEFT ?

	if (all_complete == 0)
		return (0);
		
	// CAN WE MOVE UP ?
	
	if (pyramids [current_level].y_level < 4)	
		{
		pyramids [current_level].y_level+=1;
		return (1);
		}

	// PYRAMID IS FULL ! - RESET IT, CLEAR IT AND INCREASE PYRAMID MULTIPLIER ?
	
	pyramid_initialiselevel (current_level);	
	
	return (2);
}
