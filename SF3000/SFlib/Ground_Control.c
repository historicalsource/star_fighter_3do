#include "SF_Mission.h"
#include "Ship_Control.h"
#include "Laser_Control.h"
#include "Maths_Stuff.h"
#include "Graphic_Struct.h"
#include "Weapons.h"
#include "Stdio.h"
#include "Ground_Control.h"
#include "SF_Message.h"
#include "SF_Palette.h"
#include "Sound_Control.h"
#include "SF_ArmUtils.h"

#define FIND_OBJECT_POSITION \
x_pos = (unsigned) x_grid<<25 ;\
y_pos = (unsigned) y_grid<<25 ;\
z_pos = (long) ((height_map [ (y_grid<<1) ] [ (x_grid<<1) ])-17)<<21 ;\
//if (z_pos < 0) z_pos = 0 ;\ 

void ground_update(void)

{

// Check for any active ground objects around the players ship
// ie. look for any objects with ref. no. 1-8
// 1-3 ground laser bases
// 4-6 ground sam bases
// 7-8 ground enemy fighter hangers

static long hangar_counter = 0 ;
static long missile_counter = 0 ;

long x_grid , y_grid ;
long x_pos , y_pos , z_pos ;
long type ;
long loop ;

ship_stack* pleb_ship ;

laser_details_header*			laser_header ;
laser_details*					laser_data ;
missile_details_header*			missile_header ;
missile_details*				missile_data ;
ship_details*					ship_data ;

graphics_details *details = (graphics_details*) static_graphics_adr ;
long *found = temp_store ;


x_grid = (((players_ship->x_pos)>>25)-16)&127 ;
y_grid = (((players_ship->y_pos)>>25)-16)&127 ;

// Scan the poly map and find the poly grid ref for all objects <= 8
// This list contains 0 - 16383 poly_map grid refs - negative terminates the list

scan_poly_map( x_grid , y_grid , found );

// Count these counters down to zero - only fire when reset
if (missile_counter > 0) missile_counter -= 1;
if (hangar_counter > 0) hangar_counter -= 1;

while (*found  >= 0)
{

x_grid = (*found)&127 ;
y_grid = (*found)>>7 ;
type = poly_map [ y_grid ] [ x_grid ] ;

// Fire a ground laser or not ?
if (type <= 3 && (arm_random()&1023) < ground_laser_rate && ((*found)&1) == laser_counter )
{

	FIND_OBJECT_POSITION

	laser_header = (laser_details_header*) (details+type)->laser_data ;
	laser_data = (laser_details*) ( laser_header + 1 ) ;

	// If it's a 2 laser launch site then arm_randomomly pick one	
	if (laser_header->counter > 0) laser_data += (arm_random()&1) ;
	
		aim_laser( 	x_pos - laser_data->x_pos ,
					y_pos - laser_data->y_pos ,
					z_pos + laser_data->z_pos ,
					players_ship->x_pos ,
					players_ship->y_pos , 
					players_ship->z_pos ,
					0 , 0 , 0 ,				// Gun base velocity fixed
					players_ship->x_vel ,
					players_ship->y_vel ,
					players_ship->z_vel ,
					ground_laser_type + laser_header->counter + (type-1) , *found ) ;

	make_sound( x_pos , y_pos , z_pos , LASER_SOUND ) ;
}  

// Launch missiles or wot
if (type>=4 && type<=6 && (arm_random()&1023) < ground_sam_rate && missile_counter == 0 )
{

	FIND_OBJECT_POSITION

	missile_header = (missile_details_header*) (details+type)->missile_data ;
	missile_data = (missile_details*) ( missile_header + 1 ) ;

	// If it's a 2 missile launch site then arm_randomomly pick one
	if (missile_header->counter > 0) missile_data += (arm_random()&1) ;
	
		add_sam( 	x_pos - missile_data->x_pos ,
					y_pos - missile_data->y_pos ,
					z_pos + missile_data->z_pos ,
					players_ship ,
					(long) *found ) ;

	// Reset the counter like
	missile_counter = 8 ;
	
	// Display warning
	message_add(MESSAGE_TOP, MTXT__WARNING_INCOMING_SAM , RED_15 , MESSAGE_FLASH , 24 , MESSAGE_NOCURSOR );
}

// Launch a ship or wot
if (type>=7 && (arm_random()&1023) < ground_ship_rate && hangar_counter == 0 && 
	collision_map [ y_grid ] [ (x_grid+1)&127 ] > 0 )
{
	
	collision_map [ y_grid ] [ (x_grid+1)&127 ] -= 1 ;
	
	ship_data = (ship_details*)  (details+type)->ship_data ;
	
	// Are there any ships launched from a ground hangar	
	if (ship_data->type == SHIP_RUNWAY_TAKEOFF )
	{
	//printf("Pleb takes off from runway");
		FIND_OBJECT_POSITION

		pleb_ship = add_ship( 	x_pos - ship_data->x_pos , 
								y_pos - ship_data->y_pos , 
								z_pos + ship_data->z_pos ,
								(512*1024) ,
								(SMALL_SHIP<<4)+ground_ship_type ,
								&pleb_special_data ,
								&mission.performances[ground_ship_type+1]
								) ;
		if (pleb_ship != NULL)
		{
			pleb_ship->who_owns_me = (long) *found ;
			pleb_ship->command = COMMAND_TAXI_TURN ;
			pleb_ship->command_counter = COMMAND_TAXI_TURN_COUNTER ;

			// Set this pleb to attack the player
			war_settoattack( pleb_ship , players_ship ) ;
		}
	}

	// Are there any ships launched from a aerial hangar or an aircraft carrier
	if (ship_data->type == SHIP_SOUTH_LAUNCH )
	{
	//printf("Pleb hanger south launch");
		FIND_OBJECT_POSITION

		pleb_ship = add_ship( 	x_pos - ship_data->x_pos , 
								y_pos - ship_data->y_pos , 
								z_pos + ship_data->z_pos ,
								0 , (SMALL_SHIP<<4)+ground_ship_type ,
								&pleb_special_data ,
								&mission.performances[ground_ship_type+1]
								) ;
		if (pleb_ship != NULL)
		{
			pleb_ship->who_owns_me = (long) *found ;
			pleb_ship->command = COMMAND_TAKEOFF_CLIMB ;
			pleb_ship->command_counter = COMMAND_TAKEOFF_CLIMB_COUNTER ;
			
			// Set this pleb to attack the player
			war_settoattack( pleb_ship , players_ship ) ;
		}
	}
	
	// Reset the counter
	hangar_counter = 64 ;
	
	// Display warning
	message_add(MESSAGE_TOP, MTXT__ENEMY_SHIP_LAUNCHED , RED_15 , MESSAGE_FLASH , 24 , MESSAGE_NOCURSOR );
}

found += 1;
}

}
