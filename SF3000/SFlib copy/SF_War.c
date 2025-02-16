// File : SF_War
// Ship setup, update & AI control routines

#include "SF_War.h"
#include "SF_Control.h"
#include "SF_Message.h"
#include "SF_Palette.h"
#include "SF_ARMUtils.h"
#include "Plot_Graphic.h"
#include "Stdio.h"

/**************************************/

// Global variables

long				cockpit_missiles = 0,	// Counter for incoming missiles
					cockpit_enemybig = 0,	// Counter for enemy big ships
					cockpit_enemysmall = 0;	// Counter for enemy fighters attacking you
					
// FORMATION LAYOUT IS :								      X
//															1   2
//														  3   5   4
//															6   7

formation_ship	formation [__WAR_MF] =	{
										{NULL,		0,				0,				0,			},	// NULL POSITION
										{NULL,		-8<<__WAR_FS,	2<<__WAR_FS,	+0<<__WAR_FS},	// Position 1
										{NULL,		+8<<__WAR_FS,	2<<__WAR_FS,	+0<<__WAR_FS},	// Position 2
										{NULL,		-16<<__WAR_FS,	4<<__WAR_FS,	+0<<__WAR_FS},	// Position 3
										{NULL,		+16<<__WAR_FS,	4<<__WAR_FS,	+0<<__WAR_FS},	// Position 4
										{NULL,		+0<<__WAR_FS,	4<<__WAR_FS,	+4<<__WAR_FS},	// Position 5
										{NULL,		-8<<__WAR_FS,	6<<__WAR_FS,	+5<<__WAR_FS},	// Position 6
										{NULL,		+8<<__WAR_FS,	6<<__WAR_FS,	+5<<__WAR_FS}	// Position 7
										};

// Pleb special data block (always attack player)

ship_sdb	pleb_special_data = 	{
									SDB_MODE_ATTACK,		// control_mode;
									SDB_SHIP_ALIVE,			// alive;
									SDB_SIDE_ENEMY,			// side;
									SDB_KILL_NULL,			// killstatus;
									0,						// flightpath_point;			// Don't Care
									0,						// command_override;			// Don't Care
									0,						// formation_position;			// Don't Care
									SDB_INCOMING_IGNORE,	// fire_instruction;
									SDB_NO_DISTANCECHECK,	// distance_check;
									0,						// pilot_name;					// Don't Care
									SDB_CLOAKING_OFF,		// cloaking_device;
									0,						// start_direction;				// Don't Care
									0,						// bomb_dropper;				// Don't Care
									0,						// bomb_number;					// Don't Care
									0,						// bomb_timer;					// Don't Care
									0,						// ship_type;					// Don't Care
									0,						// init_ref;					// Don't Care
									0,						// x_start;						// Don't Care
									0,						// y_start;						// Don't Care
									0,						// z_start;						// Don't Care
									SDB_TYPE_PLEB,			// sdb type;
									0,						// free1;
									0,						// free2;
									0,						// free3;
									NULL,					// ship_address;				// Don't Care
									NULL,					// command_address;				// Don't Care
									0,						// x_offset;					// Don't Care
									0,						// y_offset;					// Don't Care
									0						// z_offset;					// Don't Care
									};

ship_sdb	player_special_data = 	{
									0,						// control_mode;				// Don't Care
									SDB_SHIP_ALIVE,			// alive;
									SDB_SIDE_FRIEND,		// side;
									0,						// killstatus;					// Don't Care
									0,						// flightpath_point;			// Don't Care
									0,						// command_override;			// Don't Care
									0,						// formation_position;			// Don't Care
									SDB_INCOMING_IGNORE,	// fire_instruction;			// Don't Care
									SDB_NO_DISTANCECHECK,	// distance_check;				// Don't Care
									0,						// pilot_name;					// Don't Care
									SDB_CLOAKING_OFF,		// cloaking_device;
									0,						// start_direction;				// Don't Care
									0,						// bomb_dropper;				// Don't Care
									0,						// bomb_number;					// Don't Care
									0,						// bomb_timer;					// Don't Care
									0,						// ship_type;					// Don't Care
									0,						// init_ref;					// Don't Care
									0,						// x_start;						// Don't Care
									0,						// y_start;						// Don't Care
									0,						// z_start;						// Don't Care
									SDB_TYPE_PLAYER,		// sdb type;
									0,						// free1;
									0,						// free2;
									0,						// free3;
									NULL,					// ship_address;
									NULL,					// command_address;				// Don't Care
									0,						// x_offset;					// Don't Care
									0,						// y_offset;					// Don't Care
									0						// z_offset;					// Don't Care
									};
									
/**************************************/

void	war_addmissionships (performance_data *players_performance)

// Purpose : Creates & initialises all enemy / friendly ships at the start of a mission
//			 Including the players ship (added first, linked to passed performance datablock) - SUSPENDED IF DEMO MISSION
//			 If starting a mission docked, it sets status.docked to address of mothership, else sets it to NULL (For Hutch)

// Accepts : Nothing
// Returns : Nothing

{

ship_stack			*ship;					// Pointer to a ship
ship_sdb			*ship_sdb_ptr;			// Pointer to a ship's special data block
flight_path			*ship_flight_ptr;		// Pointer to a ship's flightpath data
performance_data	*performance;			// Pointer to a ship's performance datablock
long				ship_count;				// General ship counter

unsigned	long	ship_type,				// Type of ship
					ship_instance,			// Instance of type
					ship_loop;				// Loop variable
									
	// CLEAR ALL FORMATION POSITIONS, AND SET status.docked (Starting docked flag) to NULL
	
	status.docked = NULL;														// Set start status to undocked
	for (ship_loop=0; ship_loop<7; ship_loop++)
		formation [ship_loop].occupied = NULL;
			
	// RE-INITIALISE THE LINKED LIST AND ADD THE PLAYERS SHIP - NOTE : THE PLAYERS SHIP PERFORMANCE IS NOT ADDED HERE :-
	
	armlink_initialise( MAX_SHIPS , &ships , sizeof (ship_stack) , 0 );
	
	players_ship = add_ship (	mission.x_startpos<<24,							// Xpos
								mission.y_startpos<<24,							// Ypos
								mission.z_startpos<<24,							// Zpos
								mission.x_startrotation<<17,					// Xrot
								PLAYERS_SHIP<<4,								// Type
								&player_special_data,							// Special player's SDB
								players_performance);							// Performance

	player_special_data.ship_address = players_ship;							// Make Players SDB point to players ship
	pleb_special_data.command_address = players_ship;							// Make pleb SDB point to players ship
	
	add_wing_pod( players_ship );
	add_wing_pod( players_ship );
	add_wing_pod( players_ship );
	add_wing_pod( players_ship );
	
	
	if (mission.demo_mission == 1)												// If demo mission, suspend player from list
		armlink_suspenditem (players_ship, &ships);
		
	// ADD ALL THE OTHER SPECIAL SHIPS FROM THE MISSION FILE
		
	if (mission.special_ships >0)
		{
		for (ship_count=0; ship_count < mission.special_ships; ship_count++)
			{
		
			// GET THE TYPE OF SHIP
		
			ship_type = (unsigned) ((mission.ship_special [ship_count] . ship_type )>>4);			// Type
			ship_instance = (unsigned) ((mission.ship_special [ship_count] . ship_type ) & 15);		// Instance of type

			// INC MISSION SPECIFIC STATUS COUNTERS

			switch (mission.ship_special [ship_count]. killstatus)
				{
				
				case SDB_KILL_MUSTDIE :
				case SDB_KILL_MUSTSTOPFLIGHT :
					status.flag_allshipdead+=1;										// Inc number of special ships to destroy
					break;
					
				case SDB_KILL_MUSTSURVIVEFLIGHT :
					status.flag_defendland+=1;										// Inc number of special ships to land alive
					break;
				}
				

			// DECIDE ON PERFORMANCE POINTER & SET SINGLE / MULTIPLE TARGET TYPES
		
			mission.ship_special [ship_count].target_type = SDB_TARGETTYPE_NORMAL;			// Set default to single target
			
			switch (ship_type)
				{
				case SMALL_SHIP :
					performance = &mission.performances [1+ship_instance];	// Little ship performances
					break;
				
				case BIG_SHIP  :
					mission.ship_special [ship_count].target_type = SDB_TARGETTYPE_MULTIPLE;	// Set to multiple targets
					mission.ship_special [ship_count].fire_instruction = SDB_INCOMING_KEEPFLIGHTPATH;
					performance = &mission.performances [5+ship_instance];	// Big ship performances
					break;
				
				case SATELLITE :
					mission.ship_special [ship_count].target_type = SDB_TARGETTYPE_MULTIPLE;	// Set to multiple targets
					mission.ship_special [ship_count].fire_instruction = SDB_INCOMING_IGNORE;	// Ignore incoming
					performance = &mission.performances [8];				// Satellite Performances
					break;
				
				case CAR :
					performance = &mission.performances [9];				// Vehicle Performance
					printf ("CAR ADDED\n");
					break;
				
				default :
					performance = NULL;										// Default is NULL
					printf("Adding ship in war.c with no performance block - type is %d\n", ship_type);
					break;
				}
	

			// ADD THE SHIP TO THE LINKED LIST
		
			ship = add_ship (	(unsigned) (mission.ship_special [ship_count] . x_start<<24),			// Xpos (Scale up to 32bit map ref)
								(unsigned) (mission.ship_special [ship_count] . y_start<<24),			// Ypos
								(unsigned) (mission.ship_special [ship_count] . z_start<<24),			// Zpos
								(unsigned) (mission.ship_special [ship_count] . start_direction << 17),	// Xrot
								mission.ship_special [ship_count] . ship_type,							// Type
								&mission.ship_special [ship_count],										// SDB
								performance );															// Performance
		
			
			
			// IF ADDED (ALWAYS SHOULD BE), INITIALISE SHIPS STATS, DEPENDING ON ITS STARTING CONTROL MODE
		
			if (ship != NULL)
				{
				ship_sdb_ptr = &mission.ship_special [ship_count];								// Pointer to this SDB
				ship_sdb_ptr->alive = SDB_SHIP_ALIVE;											// Set ship to alive
				ship_sdb_ptr->ship_address = ship;												// Link SpecialDB -> Ship

				if (mission.starting_docked == 1 && ship->type == MOTHER_SHIP)					// Is this the mothership & we are
					status.docked = ship;														// Starting docked ? If so, register
				
				if (performance != NULL)
					ship_sdb_ptr->max_canlaunch = (char) performance->weapons [WEAPON_LAUNCH_SHIP];	// Set maxcanlaunch in SDB
				}
			}
		
		// NOW INITIALISE ALL SPECIAL SHIPS
		
		for (ship_count=0; ship_count < mission.special_ships; ship_count++)
			{
			ship_sdb_ptr = &mission.ship_special [ship_count];								// Get Pointer to ship's SDB
			ship = (ship_stack*) ship_sdb_ptr->ship_address;								// Get the ship it refers to
			ship_sdb_ptr->formation_position = 0;											// Clear it's formation position
			
			switch (ship_sdb_ptr->control_mode)
				{
				case SDB_MODE_ATTACK :														// Initialise ship in attack
					if (ship_sdb_ptr->init_ref != 255)
						war_settoattack (ship, (ship_stack*) (mission.ship_special [ship_sdb_ptr->init_ref].ship_address));	// Get address of ship it is attacking
					else
						war_settoattack (ship,(ship_stack*) players_ship);
					war_updateship_attack(ship, ship_sdb_ptr);
					break;
				
				case SDB_MODE_FORMATION :													// Initialise ship in formation
					if (ship_sdb_ptr->init_ref != 255)
						{
						ship_sdb_ptr->command_address = mission.ship_special [ship_sdb_ptr->init_ref].ship_address;	// Get address of ship it is attacking
						war_updateship_formation(ship, ship_sdb_ptr);						// Just set it's GOTOXYZ Co-ordinates
						}
					else
						{
						ship_sdb_ptr->command_address = players_ship;

						if (ship_sdb_ptr->side == SDB_SIDE_FRIEND)							// FF Formation ?
							{
							war_addshiptoformation (ship, 0);								// Set this ship to formation pos
							war_updateship_formation(ship, ship_sdb_ptr);					// Set it's GOTOXYZ Co-ordinates
							ship->x_pos = ship->goto_x;										// Set It's X,Y,Z pos to Rotated Offsets
							ship->y_pos = ship->goto_y;
							ship->z_pos = ship->goto_z;
							}
						else
							war_updateship_formation(ship, ship_sdb_ptr);					// Just set it's GOTOXYZ Co-ordinates
						}
					break;

				case SDB_MODE_FLIGHTPATH :																		// Initialise ship in flightpath mode ? SET INITIAL GO-TO POINTS
					ship_flight_ptr = &mission.flight_paths [(long) (ship_sdb_ptr->init_ref)];					// Get the flightpath Address
					ship_sdb_ptr->command_address = ship_flight_ptr;											// Store it in com address
					ship_settoflightpoint (ship, ship_sdb_ptr);																// Set it's flightpt														
					break;
				}
			}
		}
}
	
/**************************************/

void	war_updatemissionships ()

// Purpose : Updates all mission & pleb ships, Attack, Flightpath, Formation etc. Also does counts & cockpit display info
// Accepts : Nothing
// Returns : Nothing

{

ship_stack	*ship;															// Pointer to a ship
ship_sdb	*ship_sdb_ptr;													// Pointer to a ship's special data block
long		player_st;														// Player increase shields timer

	// RESET DISPLAY COUNTERS FOR INFO
	
	cockpit_missiles = 0;
	cockpit_enemybig = 0;
	cockpit_enemysmall = 0;
	
	// UPDATE ALL SHIPS WITH SDB'S EXCEPT PLAYER
	
	ship = (ship_stack*) (ships.info).start_address;						// Get start of ship linked list
	while ((ship->header).status == 1)										// While active ship, search
		{
		ship_sdb_ptr = ship->special_data;
		
		if (ship_sdb_ptr == NULL)											// Does ship NOT have a SDB ?
			{
			if (ship->type >>4 == WEAPON)									// Is it a weapon ?
				{
				if (ship->target == players_ship)							// Is it going for the players ship ?
					cockpit_missiles+=1;									// Yes, register it
				}
			}
		
		else
		
			{
		
			if (ship_sdb_ptr->alive == SDB_SHIP_ALIVE)						// Yes, is it alive ?
				{
				
				ship->last_fire_request = ship->fire_request;				// Copy over fire requests
				ship->fire_request = WEAPON_NOTHING;						// Clear current fire request
				
				if (ship_sdb_ptr->sdb_type != SDB_TYPE_PLAYER)				// If NOT Player then update commands
					{
					war_updatemissionship (ship, ship_sdb_ptr);
					}
				else
					{
					if (ship->who_hit_me != -1 && status.docked == NULL)			// Player been hit ? - NOT DOCKED
						status.hits_taken +=1;
					}
					
				if (ship_sdb_ptr->target_type == SDB_TARGETTYPE_MULTIPLE)			// Attack Multiple Targets ?
					war_update_multipleattackrequests (ship, ship_sdb_ptr);
				
				if (ship_sdb_ptr->side == SDB_SIDE_FRIEND)
					{
					if (ship != players_ship)										// UPDATE SHIELDS FOR FRIEND ?
						{
						if ((status.clock2 & 3) == 3)							
							if (ship->shields < (ship->performance->shields << 5) && ship->shields >0)
								ship->shields+=1;
						}
					else
						{
						player_st = (status.docked == NULL) ? 7 : 1;				// UPDATE SHIELDS FOR PLAYER ?
						if ((status.clock2 & player_st) == player_st)
							if (ship->shields < (ship->performance->shields << 5) && ship->shields >0)
								ship->shields+=1;
						}
					}
				}
			}
		ship = (ship_stack*) (ship->header).next_address;							// Get next ship in linked list
		}
}

/**************************************/

void	war_updatemissionship (ship_stack *ship, ship_sdb *ship_sdb_ptr)

// Purpose : Updates a single mission ship
// Accepts : Pointer to ship to update, pointer to it's special data block
// Returns : Nothing

{

	// HAS THE SHIP BEEN FIRED UPON THIS FRAME (and is NOT set to Ignore Attack) ?
	
	if (ship->who_hit_me > 16384 && ship_sdb_ptr->fire_instruction != SDB_INCOMING_IGNORE)
		{
		if (ship_sdb_ptr->side == SDB_SIDE_FRIEND)
			war_update_attacked_friend (ship, ship_sdb_ptr);
		else
			war_update_attacked_enemy (ship, ship_sdb_ptr);
		}
	
	// UPDATE THE SHIP ACCORDING TO IT'S COMMAND CONTROL MODE
	
	switch (ship_sdb_ptr->control_mode)
		{
		case SDB_MODE_ATTACK :							// Is the ship attacking someone else ?	
			war_updateship_attack (ship, ship_sdb_ptr);
			break;

		case SDB_MODE_FLIGHTPATH :						// Is the ship following as flightpath ?
			war_updateship_flightpath (ship, ship_sdb_ptr);
			break;
			
		case SDB_MODE_FORMATION :						// Is the ship in formation flight with another ship ?
			war_updateship_formation (ship, ship_sdb_ptr);
			break;
		}
}

/************************************************************************************************************************************
*												UPDATE SHIP THAT IS IN ATTACK MODE													*
************************************************************************************************************************************/

void	war_updateship_attack (ship_stack *ship, ship_sdb *ship_sdb_ptr)

// Purpose : Updates a special ship in ATTACK MODE
// Accepts : Pointer to ship to update, Pointer to it's special data block
// Returns : Nothing

//	RULE 1 : HAS THE SHIP WE ARE ATTACKING HAS DIED ?
//			 YES : (FRIENDLY)
//				{
//				DO LOCAL SEARCH FOR NEW TARGET, IF FOUND, SET TO ATTACK & CARRY ON
//				IF NOT FOUND :
//					IS CONTROL MODE 'NORMAL' - IF SO, REJOIN FORMATION
//				ELSE
//					DO WIDE SEARCH FOR NEW TARGET, IF FOUND, SET TO ATTACK & CARRY ON
//					OTHERWISE : REJOIN FORMATION
//				}

//			YES : (ENEMY)
//				{
//				DO WIDE SEARCH FOR NEW TARGET, IF FOUND, SET TO ATTACK & CARRY ON
//				ELSE : LEAVE SUSPENDED
//				}

//	RULE 2 : SET THE SHIP'S GOTO CO-ORDINATES TO THAT OF THE SHIP IT IS ATTACKING [HUTCH NOW DOES THIS]

//	RULE 3 : SHOULD WE FIRE LASER, OR A-T-A MISSILE ?

//	RULE 4 : IF SINGLE TARGETING, RETURN, ELSE :
//					{
//					SHOULD WE LAUNCH SHIP
//					DEC. TARGET COUNTER, IF <0, RESET AND RE-ALLOCATE TOP 3 TARGETS
//					}

{

long weapon_firerequest;					// Random fire value
performance_data	*performance;			// Pointer to a ship's performance datablock

	// IS THE SHIP ATTACKING THE PLAYER ? IF SO, INC EMENY FIGHTER COUNTER
	
	if (ship_sdb_ptr -> command_address == players_ship)
		cockpit_enemysmall+=1;
		
	// HAS THE SHIP WE ARE ATTACKING DIED ?
	
	if ((((ship_stack*) (ship_sdb_ptr->command_address)) -> shields <=0 ) ||	// Shields <= 0 OR
	   (((ship_stack*) (ship_sdb_ptr->alive)) == SDB_SHIP_DEAD))				// SDB_Alive set to dead
			
			{
			//printf ("SHIP WE ARE ATTACKING HAS DIED\n");
			
			if (ship_sdb_ptr->side == SDB_SIDE_FRIEND)												// Find new target for FRIENDLY
				{
				if (ship_setmultipletargets (ship, __WAR_SR_LOCAL, ship_sdb_ptr->side) != 0)		// Found local target ?
					war_settoattack (ship, (ship_stack*) ship->target);								// YES - attack it
				else																				// Couldn't find local ?
					{
					if (status.formation_mode == SDB_FORMATION_NORMAL)								// Normal mode ? If so, rejoin formtn
						{
						war_addshiptoformation (ship, 1);
						return;
						}
					else
						{
						if (ship_setmultipletargets (ship, __WAR_SR_AREA, ship_sdb_ptr->side) != 0)	// Found area target ?
							war_settoattack (ship, (ship_stack*) ship->target);						// YES - attack it
						else																		// Not Found area target ?
							{
							war_addshiptoformation (ship, 1);
							return;
							}
						}
					}
				}
			
			else																					// Find new target for ENEMY
			
				{
				if (ship_setmultipletargets (ship, __WAR_SR_AREA, ship_sdb_ptr->side) != 0)			// Found area target ?
					war_settoattack (ship, (ship_stack*) ship->target);								// YES - attack it
				else
					{
					printf ("ENEMY SHIP CANNOT FIND ANYBODY TO ATTACK - It's in limbo !\n");
					return;																			// ELSE leave in limbo, return
					}
				}
			}
			
	// GO TO (SHIP WE ARE ATTACKING)'S CO-ORDINATES
	
	//ship->goto_x = ((ship_stack*) (ship_sdb_ptr->command_address))->x_pos;
	//ship->goto_y = ((ship_stack*) (ship_sdb_ptr->command_address))->y_pos;
	//ship->goto_z = ((ship_stack*) (ship_sdb_ptr->command_address))->z_pos;
	
	// FIRE A WEAPON ? (SINGLE TARGET REQUESTS)

	weapon_firerequest = arm_random() & 2047;				
	performance = ship->performance;
	
	if (weapon_firerequest < performance->laser_rate)		// FIRE LASER ?
		ship->fire_request = WEAPON_LASER;

	if (weapon_firerequest < performance->missile_rate)		// FIRE A-T-A MISSILE ?
		{
		ship->fire_request = WEAPON_ATA;
		ship->target = (ship_stack*) ship_sdb_ptr->command_address;
		}
}

		
/************************************************************************************************************************************
*										UPDATE A SHIP THAT IS IN FORMATION WITH ANOTHER SHIP										*
************************************************************************************************************************************/

void	war_updateship_formation (ship_stack *ship, ship_sdb *ship_sdb_ptr)

// Purpose : Updates a special ship in FORMATION MODE
// Accepts : Pointer to ship to update, Pointer to it's special data block
// Returns : Nothing

//	RULE 1 : ARE WE DOING A DISTANCE CHECK WITH THE PLAYER ?
//				IF SO, IS DISTANCE WITHIN RANGE ?
//					{
//					ARE WE FRIENDLY ? IF SO, ASK TO JOIN FORMATION
//					ARE WE ENEMY ? IF SO, BREAK FORMATION AND ATTACK PLAYER
//					}

//	RULE 2 : HAS OUR FORMATION LEADER BEEN HIT BY ENEMY FIRE ? IF SO (AND IT ISN'T SET TO IGNORE INCOMING FIRE) THEN 
//				{
//				SET TO ATTACK FIREE (DEFEND FORMATION IF FRIENDLY)
//				UPDATE IT USING updateship_attack() INSTEAD
//				RETURN
//				}
				
//	RULE 3 : SET THE SHIP'S GOTO CO-ORDINATES TO THAT OF THE SHIP IT IS FOLLOWING (+OFFSETS)
//	RULE 4 : HAS THE SHIP WE ARE FOLLOWING FIRED ? IF SO, THEN FIRE AS WELL - ONLY IF FOLLOWING PLAYER

{

ship_stack	*ship_following = (ship_stack*) ship_sdb_ptr->command_address;			// Get Ship we are following
	
	// ARE WE DOING DISTANCE CHECK WITH PLAYER ?
	
	if (ship_sdb_ptr->distance_check == SDB_DO_DISTANCECHECK)
		war_distancecheckwithplayer(ship, ship_sdb_ptr);
		
	// HAS LEADER BEEN FIRED UPON ? IF SO, MAYBE DEFEND FORMATION
	
	if (ship_following->who_hit_me > 16384)
		if (war_update_defend_leader (ship, ship_sdb_ptr, ship_following) == 1)
			{
			war_updateship_attack(ship, ship_sdb_ptr);
			return;
			}
	
	// SET FOLLOW CO-ORDINATES TO THAT OF THE SHIP WE ARE FOLLOWING (INCLUDING ROTATED OFFSETS)
	
	//rotation.x_rot = ship_following->x_rot;										// Get follow ship's rotations
	//rotation.y_rot = ship_following->y_rot;
	//rotation.z_rot = ship_following->z_rot;
	//rotation.x_pos = ship_sdb_ptr->x_offset;										// Get our follow offsets
	//rotation.y_pos = ship_sdb_ptr->y_offset;
	//rotation.z_pos = -ship_sdb_ptr->z_offset;
	//rotate_node_from_c (&rotation);												// Rotate the offsets
	
	//ship->goto_x = (ship_following->x_pos) - rotation.x_pos;						// Goto It's X,Y,Z pos + /Rotated Offset
	//ship->goto_y = (ship_following->y_pos) - rotation.y_pos;
	//ship->goto_z = (ship_following->z_pos) - rotation.z_pos;
	
	// MAKE OUR SHIP FIRE (IF IT WAS FOLLOWING THE PLAYER WHO FIRED & THE PLAYER WASN'T DOCKED) or something like that anyway
	
	if (ship_following->last_fire_request == WEAPON_LASER)
		if (ship_following == players_ship && docked.status == DOCKING_OUT)
			ship->fire_request = WEAPON_LASER;
}

/************************************************************************************************************************************
*												UPDATE SHIP THAT IS ON A FLIGHTPATH													*
************************************************************************************************************************************/

void	war_updateship_flightpath (ship_stack *ship, ship_sdb *ship_sdb_ptr)

// Purpose : Updates a special ship on a FLIGHTPATH
// Accepts : Pointer to ship to update, Pointer to it's special data block
// Returns : Nothing

//	RULE 1 : ARE WE DOING A DISTANCE CHECK WITH THE PLAYER ?
//				IF SO, IS DISTANCE WITHIN RANGE ?
//					{
//					ARE WE FRIENDLY ? IF SO, ASK TO JOIN FORMATION
//					ARE WE ENEMY ? IF SO, BREAK FORMATION AND ATTACK PLAYER
//					}

//	RULE 2 : SEE IF IT IS A VEHICLE OR SHIP, SET CHECK DISTANCE ACCORDINGLY
//			 IF THE SHIP IS CLOSE ENOUGH TO ITS CURRENT FLIGHT PATH POINT, MOVE IT ONTO THE NEXT FLIGHT PATH POINT

//	RULE 3 : IF THE SHIP HAS REACHED THE END OF IT'S FLIGHTPATH :
//		{
//		SET IT BACK TO 1ST POINT ON THE FLIGHTPATH
//			IF SDB_KILL_MUSTSTOPFLIGHT		- MARK 'GAME OVER' BECAUSE ENEMY REACHED FLIGHTPOINT
//			IF SDB_KILL_MUSTSURVIVEFLIGHT	- SUB STATUS MUST-SURVIVE FLIGHT COUNTER, & SET STATUS TO DON'T CARE
//		SEE IF SHIP SHOULD BE SET TO FROZEN (COMMAND OVERRIDE - CANNOT MOVE)
//		}

{

long	dist;
long	check_limit;

flight_path	*ship_flight_ptr;

	// IS THE SHIP AN ENEMY ? IF SO, INCREMENT COUNT
	
	if (ship_sdb_ptr->side == SDB_SIDE_ENEMY)
		cockpit_enemybig+=1;
	
	// ARE WE DOING DISTANCE CHECK WITH PLAYER ?
	
	if (ship_sdb_ptr->distance_check == SDB_DO_DISTANCECHECK)
		war_distancecheckwithplayer(ship, ship_sdb_ptr);
		
	// IS THIS SHIP DROPPING SOME BOMBS ?
	
	if (ship_sdb_ptr->bomb_todrop > 0)											// Any bombs to drop ?
		if (ship_sdb_ptr->bomb_timer-- == 0)									// Yes - is timer 0 ?
			{																	// Yes - reset timer, --bombs & drop bomb
			ship_sdb_ptr->bomb_timer = 15;
			ship_sdb_ptr->bomb_todrop-=1;
			ship->fire_request = WEAPON_MEGA_BOMB;
			}
				
	// HAS SHIP / VEHICLE REACHED IT'S FLIGHTPATH POINT ?
	
	check_limit = (((ship->type) >> 4) == CAR) ? 4 : 4;							// Set check limit for vehicles / ships
	
	dist = (ship->goto_x - ship->x_pos) >> 22;
	if (dist < 0) dist = -dist;
	if (dist > check_limit)	return;												// Are we too far away in the X ?

	dist = (ship->goto_y - ship->y_pos) >> 22;
	if (dist < 0) dist = -dist;
	if (dist > check_limit)	return;												// Are we too far away in the Y ?
	
	if (((ship->type) >> 4) != CAR)
		{
		dist = (ship->goto_z - ship->z_pos) >> 22;
		if (dist < 0) dist = -dist;
		if (dist > check_limit)	return;											// Are we too far away in the Z ? ONLY CHECK FOR SHIPS
		}
	
	// YES, GET NEXT FLIGHTPATH POINT, IF FINISHED FLIGHTPATH, DO END OF FLIGHTPATH CODE
	
	ship_flight_ptr = (flight_path*)ship_sdb_ptr->command_address;				// Get the flightpath we are on from the SDB
	
	if (++(ship_sdb_ptr->flightpath_point) >= ship_flight_ptr->flight_points)	// Have we finished the flightpath ? - Do e-o-fp code
		{

		// HAS ENEMY REACHED END OF FLIGHTPATH WHEN IT SHOULDN'T HAVE ?
		
		if (ship_sdb_ptr->killstatus == SDB_KILL_MUSTSTOPFLIGHT)
			{
			message_add (MESSAGE_TOP, (MTXT__ENEMY_VESSEL_HAS_LANDED), RED_15, MESSAGE_FLASH, 64, MESSAGE_NOCURSOR);
			control_gameover();
			}
			
		// HAS FRIENDLY SURVIVED TO END OF FLIGHTPATH ?
		
		if (ship_sdb_ptr->killstatus == SDB_KILL_MUSTSURVIVEFLIGHT)
			{
			message_add (MESSAGE_TOP, (MTXT__FRIENDLY_VESSEL_HAS_LANDED), WHITE_15, MESSAGE_FLASH, 64, MESSAGE_NOCURSOR);
			ship_sdb_ptr->killstatus = SDB_KILL_NULL;
			status.flag_defendland-=1;
			}
			
		ship_sdb_ptr->flightpath_point = 0;										// Go back to start of flightpath
		}
	
	ship_settoflightpoint (ship, ship_sdb_ptr);									// Get next flightpoint in GOTOXYZ
}

	
/************************************************************************************************************************************
*											UPDATE FRIENDLY SHIP THAT HAS BEEN FIRED UPON											*
************************************************************************************************************************************/

void	war_update_attacked_friend (ship_stack *ship, ship_sdb *ship_sdb_ptr)

// Purpose : Decides what to do if a friendly ship has been fired upon (Ship cannot be set to IGNORE ATTACK to get here)
// Accepts : Pointer to ship to update, Pointer to it's special data block
// Returns : Nothing

//	RULE 1 : IF COMMAND OVERRIDE IS SET TO BIG_SHIP_COMMAND_STATIC_FREE (STATIC UNTIL FIRED ON) FREE IT !

//	RULE 2 : IF THE SHIP IS BEING FIRED UPON BY THE PLAYER :
//				{
//				UPDATE FRIENDLY-FIRE COUNTER
//				IF FF COUNTER IS >= MAX ALLOWED AND RND(4)
//					{
//					SET TO ENEMY
//					SET MODE TO ATTACK (IF POSSIBLE)
//					DISPLAY 'TERMINATING SQUADRON LEADER' MESSAGE
//					}
//				ELSE
//					DISPLAY 'DO NOT FIRE ON FRIENDLY SHIPS' MESSAGE
//				}

//	RULE 3 : IF THE SHIP IS BEING FIRED ON BY ANOTHER FRIENDLY, IGNORE THE ATTACK AND RETURN

//	RULE 4 : IF SHIP'S SHIELDS ARE LOW , DISPLAY 'MAYDAY MAY MESSAGE'

//	RULE 5 : IF IT'S THE MOTHERSHIP BEING FIRED UPON, DO MESSAGE + WARNING FOR LOW SHIELDS

//	RULE 6 : IF THE SHIP IS NOT IN FORMATION WITH PLAYER, RETURN

//	RULE 7 : IF FORMATION MODE IS 'DEFENSIVE' - DISPLAY 'UNDER FIRE' MESSAGE - DO NOT LEAVE FORMATION
//			 
//			ELSE
//				{
//				IF THE SHIP IS IN FORMATION WITH THE PLAYER, LEAVE THE FORMATION & DISPLAY MESSAGE
//				ATTACK THE ENEMY SHIP (IF POSSIBLE)
//				}


{
ship_stack	*ship_firee = (ship_stack*) ship->who_hit_me;
ship_sdb	*firee_sdb_ptr = ship_firee->special_data;
	
	// IS THE SHIP ON A COMMAND_OVERRIDE STATIC UNTIL FIRED UPON ?
	
	if (ship_sdb_ptr->command_override == BIG_SHIP_COMMAND_STATIC_FREE)
		ship_sdb_ptr->command_override = BIG_SHIP_COMMAND_NORMAL;

	// PLAYERS SHIP THAT FIRED ON FRIENDLY ?
		
	if (ship_firee == players_ship)
		{
		if (++status.friendly_fire > __WAR_MFF && ((arm_random() & 3) == 0))
			{
			war_settoattack (ship, (ship_stack*) players_ship);
			ship_sdb_ptr->side = SDB_SIDE_ENEMY;
			message_pilot (ship_sdb_ptr->pilot_name, (MTXT__REQUEST_TERMINATION_OF_LEADER), ORANGE_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
			}
		else
			message_add (MESSAGE_BOTTOM, (MTXT__WARNING_FRIENDLY_FIRE), RED_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR1);
		}
	
	// IF FRIENDLY FIRING ON FRIENDLY, RETURN
	
	if (firee_sdb_ptr->side != SDB_SIDE_ENEMY)
		return;
	
	// WAS IT THE MOTHERSHIP THAT GOT FIRED ON ? IF SO, DISPLAY MESSAGE
	
	if (ship->type == MOTHER_SHIP)
		{
		if (ship->shields <= 64)
			message_pilot (ship_sdb_ptr->pilot_name, (MTXT__WARNING_MOTHERSHIP_SHIELDS_LOW), RED_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
		else
			message_pilot (ship_sdb_ptr->pilot_name, (MTXT__WARNING_MOTHERSHIP_UNDER_ATTACK), RED_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
		}
	
	// SHOULD THE SHIP ISSUE A MAYDAY ?
	
	if (ship->shields <=32)
			message_pilot (ship_sdb_ptr->pilot_name, (MTXT__MAYDAY_MAYDAY_REQUEST_ASSISTANCE), ORANGE_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
	
	// IF THE SHIP IS NOT IN FORMATION, THEN SET TO ATTACK PLAYER
	
	if (ship_sdb_ptr->formation_position == 0)
		{
		war_settoattack (ship, (ship_stack*) ship_firee);
		return;
		}
	
	// NOW WE KNOW IT'S A FRIENDLY IN FORMATION BEING ATTACKED, LOOK AT THE FORMATION CONTROL MODE TO DECIDE WHAT TO DO
	
	if (status.formation_mode == SDB_FORMATION_DEFENSIVE)
		{
		message_pilot (ship_sdb_ptr->pilot_name, (MTXT__FORMATION_UNDER_FIRE), ORANGE_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
		}
	else
		{
		formation [ship_sdb_ptr->formation_position].occupied = NULL;
		ship_sdb_ptr->formation_position = 0;
		message_pilot (ship_sdb_ptr->pilot_name, (MTXT__UNDER_FIRE_BREAKING_FORMATION), ORANGE_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
		war_settoattack (ship, (ship_stack*) ship_firee);
		}
}

/************************************************************************************************************************************
*											UPDATE ENEMY SHIP THAT HAS BEEN FIRED UPON												*
************************************************************************************************************************************/

void	war_update_attacked_enemy (ship_stack *ship, ship_sdb *ship_sdb_ptr)

// Purpose : Decides what to do if an enemy ship has been fired upon (Ship cannot be set to IGNORE ATTACK to get here)
// Accepts : Pointer to ship to update, Pointer to it's special data block
// Returns : Nothing

//	RULE 1 : IF COMMAND OVERRIDE IS SET TO BIG_SHIP_COMMAND_STATIC_FREE (STATIC UNTIL FIRED ON) FREE IT !
//	RULE 2 : IF THE SHIP IS BEING FIRED UPON BY ANOTHER ENEMY - IGNORE THE ATTACK
//	RULE 3 : IF THE SHIP IS BEING FIRED UPON BY FRIENDLY (INCLUDING PLAYER) - ATTACK THE FRIENDLY SHIP

{

ship_stack	*ship_firee = (ship_stack*) ship->who_hit_me;
ship_sdb	*firee_sdb_ptr = ship_firee->special_data;

	// IS THE SHIP ON A COMMAND_OVERRIDE STATIC UNTIL FIRED UPON ?
	
	if (ship_sdb_ptr->command_override == BIG_SHIP_COMMAND_STATIC_FREE)
		ship_sdb_ptr->command_override = BIG_SHIP_COMMAND_NORMAL;
	
	// ATTACK FRIENDLY SHIP
		
	if (firee_sdb_ptr->side == SDB_SIDE_FRIEND)
		war_settoattack (ship, (ship_stack*) ship_firee);
}

/************************************************************************************************************************************
*					SEE IF A SHIP SHOULD GO IN TO DEFEND THE FORMATION IF IT'S LEADER HAS BEEN FIRED UPON							*
************************************************************************************************************************************/

long	war_update_defend_leader (ship_stack *ship, ship_sdb *ship_sdb_ptr, ship_stack *ship_following)

// Purpose : Possibly attack ship that fired upon a formation leader
// Accepts : Pointer to ship to update, Pointer to it's special data block & Pointer to the ship it is following
// Returns : (1) if ship is now attacking the firee, or (0) if it isn't

//	RULE 1 : IF THE LEADER IS BEING FIRED UPON BY SOMEONE OF THE SAME TYPE, IGNORE THE ATTACK
//	RULE 2 : IF NOT RND(4), THEN IGNORE THE ATTACK AND CARRY ON
//	RULE 3 : IF THE LEADER IS THE PLAYERS SHIP THEN
//				{
//				DISPLAY 'DEFENDING FORMATION' MESSAGE PRIOR TO ATTACKING
//				CLEAR THE FORMATION POSITION
//				}

//			  ATTACK THE SHIP


{

ship_stack	*ship_firee = (ship_stack*) ship_following->who_hit_me;
ship_sdb	*firee_sdb_ptr = ship_firee->special_data;

	if (firee_sdb_ptr->side == ship_following->special_data->side)	// If ship is on same side, no attack
		return (0);
	
	if (arm_random () & 3 != 3)										// If not rnd(4) then no attack
		return (0);
		
	if (ship_following == players_ship)								// If leader was player (we are leaving formation)
		if (ship_sdb_ptr->formation_position != 0)
			{
			formation [ship_sdb_ptr->formation_position].occupied = NULL;
			ship_sdb_ptr->formation_position = 0;
			message_pilot (ship_sdb_ptr->pilot_name, (MTXT__GOING_IN_TO_DEFEND_FORMATION), ORANGE_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
			}
				
	return (war_settoattack (ship,(ship_stack*) ship_firee));		// Set ship to attack and return status
}

/************************************************************************************************************************************
*												SET A SHIP TO ATTACK ANOTHER SHIP													*
************************************************************************************************************************************/

long	war_settoattack (ship_stack *this_ship, ship_stack *ship_toattack)

// Purpose : Forces a ship to attack another ship (if control flags allow it) [IF ATTACKING PLAYER - DO RADAR LOCK MESSAGE]
// Accepts : Pointer to the ship to set to attack, and the ship to be attacked
// Returns : (1) if ship is now attacking the one that hit it, or (0) if it is not

//	NOTE :	 THIS ASSUMES THAT THE ATTACK FLAG HAS ALREADY BEEN CHECKED FOR 'ATTACK_IGNORE'

//	RULE 1 : DO NOT ATTACK THE SHIP IF OUR SHIP'S ATTACK FLAG IS SET TO 'ATTACK_KEEPFLIGHTPATH' AND IT IS ON A FLIGHTPATH


{
ship_sdb	*this_ship_sdb_ptr = this_ship->special_data;

	if (!(this_ship_sdb_ptr->control_mode == SDB_MODE_FLIGHTPATH && this_ship_sdb_ptr->fire_instruction == SDB_INCOMING_KEEPFLIGHTPATH))
		{
		this_ship_sdb_ptr->command_address = ship_toattack;
		this_ship_sdb_ptr->control_mode = SDB_MODE_ATTACK;
		this_ship->target = (void*) ship_toattack;
		
		// Should we do Radar Lock message (if attacking player)
		
		if (ship_toattack == players_ship)
			message_add (MESSAGE_BOTTOM, (MTXT__ENEMY_RADAR_LOCK_DETECTED), ORANGE_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
			
		return (1);
		}
	return (0);
}

/**************************************/

void	ship_settoflightpoint (ship_stack *this_ship, ship_sdb *this_ship_sdb_ptr)

// Purpose : Sets a ships GOTOXYZ Co-ordinates from it's current point in it's flightpath
// Accepts : Pointer to the ship to set & pointer to it's special data block
// Returns : Nothing

{
flight_path	*this_ship_flight_ptr = (flight_path*)this_ship_sdb_ptr->command_address;	// Get the flightpath we are on from the SDB

	// Get new X,Y,Z position
		
	this_ship->goto_x = (unsigned) ((this_ship_flight_ptr->flight_data [this_ship_sdb_ptr->flightpath_point].x_pos)<<24);
	this_ship->goto_y = (unsigned) ((this_ship_flight_ptr->flight_data [this_ship_sdb_ptr->flightpath_point].y_pos)<<24);
	this_ship->goto_z = (unsigned) ((this_ship_flight_ptr->flight_data [this_ship_sdb_ptr->flightpath_point].z_pos)<<24);

	// Is this the start of some bomb dropping ?
	
	if (this_ship_flight_ptr->flight_data [this_ship_sdb_ptr->flightpath_point].command > 0)
		{
		this_ship->special_data->bomb_todrop = this_ship_flight_ptr->flight_data [this_ship_sdb_ptr->flightpath_point].command;	// Set bombs
		this_ship->special_data->bomb_timer = 0;
		}
}

/**************************************/
	
void	war_update_multipleattackrequests (ship_stack *ship, ship_sdb *ship_sdb_ptr)

// Purpose : Updates multiple attack requests for big ships etc.
// Accepts : Pointer to ship to update, Pointer to it's special data block
// Returns : Nothing

//	RULE 1 : DEC. TARGET COUNTER, IF <0, RESET AND RE-ALLOCATE TOP 3 TARGETS
//	RULE 2 : IF WE HAVE NO TARGET SPECIFIED, RETURN
//	RULE 3 : SHOULD WE FIRE LASER, A-T-A MISSILE, OR LAUNCH SHIP ? ?


{

unsigned long 		weapon_firerequest;					// Random fire value
performance_data	*performance;						// Pointer to a ship's performance datablock

	if ((ship_sdb_ptr->target_counter--) == 0)									// Dec target counter - Realloc targets ?
		{
		ship_sdb_ptr->target_counter = __WAR_TC;								// Reset target counter
		ship_setmultipletargets (ship, __WAR_SR_LOCAL, ship_sdb_ptr->side);		// Re-allocatate new targets
		}
	
	if (ship->target == NULL)													// Nothing to fire at !
		return;
		
	// FIRE A WEAPON ? (MULTIPLE TARGET REQUESTS)
	
	weapon_firerequest = arm_random() & 4095;									// Get random fire value	
	performance = ship->performance;
	
	if (weapon_firerequest < performance->laser_rate)							// FIRE LASER ?
		{
		ship->fire_request = WEAPON_LASER;
		//printf ("BIGSHIP FIRE REQUEST - LASER\n");
		}
		
	if (weapon_firerequest < performance->missile_rate)							// FIRE A-T-A MISSILE ?
		{
		ship->fire_request = WEAPON_ATA;
		//printf ("BIGSHIP FIRE REQUEST - ATA MISSILE\n");
		}	
	
	if (ship_sdb_ptr->side == SDB_SIDE_ENEMY)
		if (weapon_firerequest < performance->launch_rate)						// LAUNCH PLEB ? ONLY IF ENEMY !
			{
			ship->fire_request = WEAPON_LAUNCH_SHIP;
			//printf ("BIGSHIP FIRE REQUEST - LAUNCH SHIP\n");
			}
}

/**************************************/

long	ship_setmultipletargets (ship_stack *this_ship, long maxsearch_distance, char search_shipside)

// Purpose : Sets nearest 3 opponent targets for ship
// Accepts : Pointer to the ship to set targets for, maxsearch start distance and the side of the ship
// Returns :(0) If No targets found, (1) if one or more targets located

{
ship_stack	*ship_search;
ship_sdb	*search_ship_sdb_ptr;

long		target_distances [3];
void		*targets [3] = {NULL, NULL, NULL};

long		xsearch_distance,
			ysearch_distance,
			zsearch_distance,
			heighest_distance,
			target_insert,
			this_ship_xpos = this_ship->x_pos,								// Get X,Y,Z position of our ship
			this_ship_ypos = this_ship->y_pos,
			this_ship_zpos = this_ship->z_pos;
	
	//printf("Doing multiple targets for ship %d, search dist is %d, side is %d\n", this_ship, maxsearch_distance>>24, search_shipside);
	
	// SET CURRENT MAX DISTANCES
	
	target_distances [0] = maxsearch_distance;
	target_distances [1] = maxsearch_distance;
	target_distances [2] = maxsearch_distance;
	 
	// START SEARCH

	ship_search = (ship_stack*) (ships.info).start_address;					// Get start of ship linked list
	while ((ship_search->header).status == 1)								// While active ship, search
		{
		search_ship_sdb_ptr = ship_search->special_data;
		if (search_ship_sdb_ptr != NULL)										// Does ship have a SDB ?
			{
			if (search_ship_sdb_ptr->alive == SDB_SHIP_ALIVE && search_ship_sdb_ptr->side != search_shipside)
				{																	
				xsearch_distance = ship_search->x_pos - this_ship_xpos;			// If Alive & Opponent, consider it as a target
				if (xsearch_distance < 0)	xsearch_distance=-xsearch_distance;
				if (xsearch_distance < maxsearch_distance)
					{
					ysearch_distance = ship_search->y_pos - this_ship_ypos;
					if (ysearch_distance < 0)	ysearch_distance=-ysearch_distance;
					if (ysearch_distance < maxsearch_distance)
						{
						zsearch_distance = ship_search->z_pos - this_ship_zpos;
						if (zsearch_distance < 0)	zsearch_distance=-zsearch_distance;
						if (zsearch_distance < maxsearch_distance)
							{
							
							// ALL 3 RELATIVE DISTANCES ARE WITHIN MAX SEARCH RANGE - SHOULD IT BE A TARGET ?
							
							heighest_distance = (xsearch_distance >= ysearch_distance) ? xsearch_distance : ysearch_distance;
							if (zsearch_distance > heighest_distance)
								heighest_distance = zsearch_distance;			// Get biggest of 3 distances
							
							// CAN IT BE 3RD TARGET (OR LESS ?)
							
							if (heighest_distance < target_distances [2])		// Is it < 3rd distance ?
								{
								target_insert = 2;
								
								// COULD IT BE 2ND TARGET ?
								
								if (heighest_distance < target_distances [1])		// Is it < 2nd distance ?
									{
									target_insert = 1;
									target_distances [2] = target_distances [1];
									targets [2] = targets [1];
									}
							
								// COULD IT BE 1ST TARGET ?
								
								if (heighest_distance < target_distances [0])		// Is it < 1st distance ?
									{
									target_insert = 0;
									target_distances [1] = target_distances [0];
									targets [1] = targets [0];
									}
							
								// REGISTER AS TARGET
								
								targets [target_insert] = ship_search;				// Register ship as relavent target
								maxsearch_distance = target_distances [2];			// Move in max-search distance
								}
							}
						}
					}
				}
			}
		ship_search = (ship_stack*) (ship_search->header).next_address;				// Get next ship in linked list
		}
	
	// RESET CURRENT TARGETS

	this_ship->target = targets [0];
	this_ship->target2 = targets [1];
	this_ship->target3 = targets [2];
	
	// printf("Targets found were %d, %d, %d\n", this_ship->target, this_ship->target2, this_ship->target3);
	
	if (targets [0] == NULL)														// Return whether we found at least 1 target
		return (0);
	else
		return (1);
}

/**************************************/

long	war_setformationposition (ship_stack *formation_ship, char formation_position, long print_message)

// Purpose : Registers a ship at a fixed position in the players formation
// Accepts : Pointer to a ship, and a formation position number
// Returns : 0 if could not add, or 1 if added

{
ship_sdb	*formation_ship_sdb_ptr = formation_ship->special_data;

	if (formation_position <__WAR_MF)														// Is pos to set < max allowed ?
		if (formation [formation_position].occupied == NULL)								// Yes, is formation pos EMPTY ?
			{
			formation_ship_sdb_ptr->x_offset = formation [formation_position].x_offset;		// Yes, set offsets to that of form pos
			formation_ship_sdb_ptr->y_offset = formation [formation_position].y_offset;
			formation_ship_sdb_ptr->z_offset = formation [formation_position].z_offset;
			formation [formation_position].occupied = formation_ship;						// Mark formation pos as occupied
			formation_ship_sdb_ptr->formation_position = formation_position;				// Set SDB in ship as occupying pos
			
			formation_ship_sdb_ptr->command_address = players_ship;							// Set to follow players ship
			formation_ship_sdb_ptr->control_mode = SDB_MODE_FORMATION;						// Set control mode to FORMATION
			
			// Display rejoin message (if print-message flag set)
			
			if (print_message == 1)
				message_pilot (formation_ship_sdb_ptr->pilot_name, (MTXT__REQUEST_TO_JOIN_FORMATION), ORANGE_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
			
			return (1);
			}
			
	return (0);
}
							
/**************************************/

long	war_addshiptoformation (ship_stack *formation_ship, long print_message)

// Purpose : Registers a ship in the players formation
// Accepts : Pointer to a ship, print-message flag (0 = no)
// Returns : 0 if could not add, 1 if added

{
char	formation_search;

	for (formation_search = 1; formation_search < __WAR_MF; formation_search++)
		if (war_setformationposition (formation_ship, formation_search, print_message) == 1)
			return (1);
	
	//printf("failed to add friendly ship in any formation position\n");
	return (0);
}

/**************************************/

void	war_squashformationup (void)

// Purpose : Takes out all spaces in formation, moves all up to front
// Accepts : Nothing
// Returns : Nothing

{
char		ship_formationplace=1,
			ship_formationcount;

ship_stack	*ship_in_formation;
	
	for (ship_formationcount=1; ship_formationcount<__WAR_MF; ship_formationcount++)	// Scan through formations
		{
		ship_in_formation = formation [ship_formationcount].occupied;
		if (ship_in_formation != NULL)													// Is there a ship in this formation pos ?
			{
			formation [ship_formationcount].occupied = NULL;							// Yes - clear ref in formation block
			ship_in_formation->special_data->formation_position = 0;					// Yes - clear it's ref in sdb
			war_setformationposition (ship_in_formation, ship_formationplace++, 0);		// Re-add it
			}
		}
}

/**************************************/

void	war_distancecheckwithplayer (ship_stack *ship, ship_sdb *ship_sdb_ptr)

// Purpose : Checks distance of ship with player, if within range, attacks or requests to join formation (depending on side)
// Accepts : Pointer to ship to update, Pointer to it's special data block
// Returns : Nothing

{
long	dist;
long	distance_length = (ship_sdb_ptr->side == SDB_SIDE_FRIEND) ? __WAR_MDC_FRIEND : __WAR_MDC_ENEMY;
	
	dist = (ship->x_pos >>24) - (players_ship->x_pos >>24);
	dist = dist < 0 ? -dist : dist;
	if (dist > distance_length)	return;													// Are we too far away in the X ?

	dist = (ship->y_pos >>24) - (players_ship->y_pos >>24);
	dist = dist < 0 ? -dist : dist;
	if (dist > distance_length)	return;													// Are we too far away in the Y ?

	dist = (ship->z_pos >>24) - (players_ship->z_pos >>24);
	dist = dist < 0 ? -dist : dist;
	if (dist > distance_length)	return;													// Are we too far away in the Z ?
	
	ship_sdb_ptr->distance_check = SDB_NO_DISTANCECHECK;								// Disable checking from now on
	
	//printf("DISTANCE CHECK - SHIP IS IN RANGE, SEARCH VALUE WAS %d\n", distance_length);
	
	if (ship_sdb_ptr->side == SDB_SIDE_FRIEND)					// Is the ship friendly ? If so, ask to join formation
		{
		war_addshiptoformation (ship, 1);
		}
	else														// Else the ship is an enemy, attack the player ! do message
		{
		war_settoattack (ship, players_ship);
		}
}
