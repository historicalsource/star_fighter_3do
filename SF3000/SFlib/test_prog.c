#include "SF_Mission.h"
#include "Smoke_Control.h"
#include "Graphic_Struct.h"
#include "Plot_Graphic.h"
#include "Collision.h"
#include "Laser_Control.h"
#include "Explosion.h"
#include "SF_ArmLink.h"
#include "Stdio.h"
#include "Weapons.h"
#include "Sound_Control.h"
#include "Maths_Stuff.h"
#include "Misc_Struct.h"
#include "SF_Control.h"
#include "Ship_Control.h"
#include "Ship_Command.h"
#include "Plot_Graphic.h"
#include "Collision.h"
#include "Graphics_Set.h"

#include "SF_Status.h"
#include "SF_Message.h"

//################################################
//#                                              #
//#       Fast turn fighters	  			     #
//#                                              #
//################################################

void new_flight_path_control( ship_stack* ) ;
void new_formation_control( ship_stack* ) ;


void new_fighter_control( ship_stack * ship )

{

rotate_node node_data ;
rotate_node node_data2 ;
target_struct target ;
long x_aim , y_aim , z_aim , aim ;
long x_dist , y_dist , z_dist , dist ;
long temp_long , temp_long2 , temp_long3 ;
long x_rot_rel , y_rot_rel ;
long max_speed = 2048 ;
long min_speed = 0 ;
long z_roll ;
long coll_check = 0 ;
long goto_x , goto_y , goto_z ;
long loop ;
long y_limit , mode ;

ship_stack* temp_ship ;

temp_ship = (ship_stack*) ( (ship_sdb*) ship->special_data )->command_address ;

mode = ship->special_data->control_mode ;

if (mode == SDB_MODE_FLIGHTPATH )
{
	new_flight_path_control( ship ) ;
	return ;
}


if (mode == SDB_MODE_FORMATION)
{
	new_formation_control( ship ) ;
	ship->command = COMMAND_ENTER_FORMATION ;
	return ;
}


//if ( mode != SDB_MODE_ATTACK ) return ; 

// Attack mode controller


// Test remove fire requests
// ship->fire_request = WEAPON_NOTHING ;


ship->who_owns_me = (long) ship ;

ship->goto_x = temp_ship->x_pos ;
ship->goto_y = temp_ship->y_pos ;
ship->goto_z = temp_ship->z_pos ;

// Find the greatest unsigned dist
x_dist = (ship->x_pos) - (ship->goto_x) ;
y_dist = (ship->y_pos) - (ship->goto_y) ;
z_dist = (ship->z_pos) - (ship->goto_z) ;

if ( x_dist < 0 ) x_dist = -x_dist ;
if ( y_dist < 0 ) y_dist = -y_dist ;
if ( z_dist < 0 ) z_dist = -z_dist ;

if ( x_dist > y_dist )
{
	dist = x_dist ;
}
else
{
	dist = y_dist ;
}

if ( z_dist > dist ) dist = z_dist ;

ship->aim_counter -= 1 ;
if ( ship->aim_counter < 0 )
{
	// Slight chance of short burst of mega steering
	if ( arm_random()&3 == 0 )
	{
		ship->aim_counter = 32 + (arm_random()&31) ;
		ship->aim_x_vel = 512 + (arm_random()&511) ;
	}
	else
	{
		ship->aim_counter = 256 + (arm_random()&511) ;
		ship->aim_x_vel = 256 + (arm_random()&255) ;
	}	
}


// Avoid hills
if ( ship->z_pos < (40<<24) && ship->z_pos > -(16<<24) && which_graphics_set != SPACE_GRAPHICS )
{
	//coll_check = find_ground_height(	ship->x_pos + (ship->x_vel<<3) , 
	//									ship->y_pos + (ship->y_vel<<3) );
	
	
	for (loop = 0 ; loop <= 3 ; loop += 1)
	{
		goto_x =	ship->x_pos -( sine_table [ (ship->x_rot)>>10 ] <<(12+loop) ) ;

		goto_y =	ship->y_pos -( cosine_table [ (ship->x_rot)>>10 ] <<(12+loop) ) ;

		coll_check = find_ground_height(	goto_x , 
											goto_y );
		
		if ( coll_check > ( ship->z_pos + (ship->z_vel<<(2+loop) ) - ( 1<<(23+loop) ) ) )
		{
	
			// 4096 speed = 1 sprite unit per frame
			//goto_x =	ship->x_pos -( sine_table [ (ship->x_rot)>>10 ] <<14 ) ;
	
			//goto_y =	ship->y_pos -( cosine_table [ (ship->x_rot)>>10 ] <<14 ) ;
	
			//goto_z = find_ground_height( goto_x , goto_y ) + ( 1<<(23+loop) ) ;
			
			goto_z = coll_check + ( 1<<(23+loop) ) ;
		
			ship->aim_counter = 10 ;
			ship->aim_x_vel = 512 ;
			coll_check = 1 ;
			break ;
		}
		else
		{
			//goto_x =	ship->x_pos -( sine_table [ (ship->x_rot)>>10 ] <<14 ) ;
			//goto_y =	ship->y_pos -( cosine_table [ (ship->x_rot)>>10 ] <<14 ) ;
			//goto_z = ship->z_pos - (1<<22) ;
			//coll_check = 1 ;
			
			coll_check = 0 ;
		}	
	}
}



// Rotate these distances by the ships rot
if ( coll_check == 0 )
{
	if (ship->command == COMMAND_ATTACK_RUN_AWAY)
	{
		// Use these for the y aim
		node_data.x_pos =	( (ship->x_pos) - (ship->aim_goto_x) )>>6 ;
	
		node_data.y_pos =	( (ship->y_pos) - (ship->aim_goto_y) )>>6 ;
	
		node_data.z_pos =	( (ship->aim_goto_z) - (ship->z_pos) )>>6 ;
	}
	else
	{
		// Use these for the y aim
		node_data.x_pos =	( (ship->x_pos) - 
							( ship->goto_x + (((dist>>12) * temp_ship->x_vel)>>14) ))>>6 ;
	
		node_data.y_pos =	( (ship->y_pos) - 
							( ship->goto_y + (((dist>>12) * temp_ship->y_vel)>>14) ))>>6 ;
	
		node_data.z_pos =	( ( ship->goto_z + (((dist>>12) * temp_ship->z_vel)>>14) )
							- (ship->z_pos) )>>6 ;
	}
}
else
{
	node_data.x_pos = (ship->x_pos - goto_x)>>6 ;
	node_data.y_pos = (ship->y_pos - goto_y)>>6 ;
	node_data.z_pos = (goto_z - ship->z_pos)>>6 ;	
}

node_data.x_rot = ship->x_rot ;
node_data.y_rot = ((1<<20) - ship->y_rot )&ROT_LIMIT ;
node_data.z_rot = ship->z_rot ;

oppo_rotate_node_from_c( &node_data ) ;

temp_long = (( -(node_data.y_pos>>10) )-1) ;
if (temp_long < 0) temp_long = -temp_long ;
if (temp_long == 0) temp_long = 1 ;

if ( coll_check == 0 )
{
	if (ship->command == COMMAND_ATTACK_RUN_AWAY)
	{
		// Find the ideal x,y rotation to aim for
		target.x_pos = ship->x_pos ;
		target.y_pos = ship->y_pos ;
		target.z_pos = ship->z_pos ;
		target.x_aim = ship->aim_goto_x ;
		target.y_aim = ship->aim_goto_y ;
		target.z_aim = ship->aim_goto_z ;
	}
	else
	{
		// Find the ideal x,y rotation to aim for
		target.x_pos = ship->x_pos ;
		target.y_pos = ship->y_pos ;
		target.z_pos = ship->z_pos ;
		target.x_aim = ship->goto_x ;
		target.y_aim = ship->goto_y ;
		target.z_aim = ship->goto_z ;
	}
}
else
{
	target.x_pos = ship->x_pos ;
	target.y_pos = ship->y_pos ;
	target.z_pos = ship->z_pos ;
	target.x_aim = goto_x ;
	target.y_aim = goto_y ;
	target.z_aim = goto_z ;
}

target_finder( &target );

// Find the relative rotation between fighter and aim
x_aim = (target.x_rot) - (ship->x_rot) ;
y_aim = (target.y_rot) - (ship->y_rot) ;
if (x_aim > 512*1024) x_aim -= 1024*1024 ;
if (y_aim > 512*1024) y_aim -= 1024*1024 ;
if (x_aim < -512*1024) x_aim += 1024*1024 ;
if (y_aim < -512*1024) y_aim += 1024*1024 ;

// Roll based on rel x
temp_long2 = -x_aim ;

if (temp_long2 > (256*1024) ) temp_long2 -= (512*1024) ;
if (temp_long2 < (-256*1024) ) temp_long2 += (512*1024) ;
temp_long2 = (temp_long2&ROT_LIMIT) ;


if (x_aim < 0) x_aim = -x_aim ;
if (y_aim < 0) y_aim = -y_aim ;
if (x_aim>y_aim)
{
	aim = x_aim ;
}
else
{
	aim = y_aim ;
}


temp_long3 = -((node_data.x_pos/temp_long)) ;

if ( temp_long3 > -128 && temp_long3 < 128 )
{
	ship->z_control = temp_long3 ;
}
else
{
	temp_long3 = (temp_long3 - ship->z_control) ;
	if ( temp_long3 > 128 ) temp_long3 = 128 ;
	if ( temp_long3 < -128 ) temp_long3 = -128 ;
	ship->z_control += temp_long3 ;
}



z_roll = (ship->z_rot - temp_long2 ) ;
if (z_roll > (1<<19) ) z_roll -= (1<<20) ;
if (z_roll < (-(1<<19)) ) z_roll += (1<<20) ;

ship->x_control = (-(z_roll>>8)) ;


// Sort out the y control 
// Is the target behind me - yes wham in full up or down
if ( node_data.y_pos > 0)
{
	if (node_data.z_pos > 0)
	{
		ship->y_control += 32 ;
	}
	else
	{
		ship->y_control -= 32 ;
	}
}
else
{
	temp_long3 = (((node_data.z_pos/temp_long))) ;
	
	if ( temp_long3 > -128 && temp_long3 < 128 )
	{
		ship->y_control = temp_long3 ;
	}
	else
	{
		temp_long3 = (temp_long3 - ship->y_control) ;
		if ( temp_long3 > 128 ) temp_long3 = 128 ;
		if ( temp_long3 < -128 ) temp_long3 = -128 ;
		ship->y_control += temp_long3 ;
	}
}

// Max Limit for controls

// Roll - fixed limit
if (ship->x_control > 1024) ship->x_control = 1024 ;
if (ship->x_control < -1024) ship->x_control = -1024 ;

x_rot_rel = temp_ship->x_rot - ship->x_rot ;
y_rot_rel = temp_ship->y_rot - ship->y_rot ;
if (x_rot_rel > (512*1024) ) x_rot_rel -= (1024*1024) ;
if (x_rot_rel < -(512*1024) ) x_rot_rel += (1024*1024) ;

if (y_rot_rel > (512*1024) ) y_rot_rel -= (1024*1024) ;
if (y_rot_rel < -(512*1024) ) y_rot_rel += (1024*1024) ;

// Aim cos value - On line = +ve 1024 - Off line = -ve 1024
temp_long = (	((cosine_table [ ((x_aim>>10)&1023) ] * cosine_table [ ((y_aim>>10)&1023) ])>>12)
			*((cosine_table [ ((x_rot_rel>>10)&1023) ] * cosine_table [ ((y_rot_rel>>10)&1023) ])>>12)
			)>>14 ;
	
//temp_long = ship->aim_x_vel ;

//if (temp_long < 0)
//{
//	temp_long = ( (1024+64) + temp_long ) ;
//	if (temp_long > ship->aim_x_vel ) temp_long = ship->aim_x_vel ;
//	max_speed = (512*2) ;

//}
//else
//{
//	temp_long = ship->aim_x_vel ;
//}

temp_long = ship->aim_x_vel ;

//Test print up stuff
//status.score = 0 ;
//message_addscore( temp_long ) ;

// Test stuff
//ship->shields = 8192 ;
//ship->special_data->command_address = players_ship ;

// Climb
if (ship->y_control > temp_long ) ship->y_control = temp_long ;
if (ship->y_control < -temp_long ) ship->y_control = -temp_long ;

// Yaw
if (ship->z_control > temp_long ) ship->z_control = temp_long ;
if (ship->z_control < -temp_long ) ship->z_control = -temp_long ;

if (ship->command != COMMAND_ATTACK_RUN_AWAY )
{
	if ( (temp_ship->type>>4) == BIG_SHIP )
	{
		if ( dist < (8<<24) )
		{
			ship->command = COMMAND_ATTACK_RUN_AWAY ;
			ship->command_counter = 128 + (arm_random()&127) ;
			ship->aim_counter = ship->command_counter ;
			ship->aim_x_vel = 1024 ;
		}
		
		dist -= (6<<24) ;
		if (dist < 0) dist = 0 ;	
	}
	else
	{
		if ( dist < (4<<24) )
		{
			ship->command = COMMAND_ATTACK_RUN_AWAY ;
			ship->command_counter = 128 + (arm_random()&127) ;
			ship->aim_counter = ship->command_counter ;	
			ship->aim_x_vel = 1024 ;
	
			ship->aim_x_rot = ship->x_rot ;
			ship->aim_y_rot = ship->y_rot ;
			ship->aim_z_rot = (arm_random()&2047)-1024 ;
			
			// Climb or dive
			if ( ship->z_pos > (34<<24) || ship->z_pos < -(34<<24) )
			{
				if ( (arm_random()&1) == 0 )
				{
					ship->aim_goto_z = temp_ship->z_pos + (4<<24) ;
				}
				else
				{
					ship->aim_goto_z = temp_ship->z_pos - (4<<24) ;
				}
			}
			else
			{
				if ( ship->z_pos >0 )
				{
					ship->aim_goto_z = temp_ship->z_pos + (4<<24) ;
				}
				else
				{
					ship->aim_goto_z = temp_ship->z_pos - (4<<24) ;
				}
			}
		}
	
		dist -= (5<<24) ;
		if (dist < 0) dist = 0 ;
	}
}


if ( ship->command == COMMAND_ATTACK_RUN_AWAY )
{
	ship->command_counter -= 1 ;
	if ( ship->command_counter < 0 )
	{
		ship->command = COMMAND_ATTACK ;
	}

	if ( dist<(4<<24) )
	{
		// 4096 speed = 1 sprite unit per frame
		ship->aim_goto_x =	temp_ship->x_pos -(( sine_table [ (ship->x_dir)>>10 ] * 
							cosine_table [ (ship->y_dir)>>10 ] ) << 3) ;

		ship->aim_goto_y =	temp_ship->y_pos -(( cosine_table [ (ship->x_dir)>>10 ] * 
							cosine_table [ (ship->y_dir)>>10 ] ) << 3) ;
		min_speed = (1<<20) ;// Floor it
	}
	else
	{
		// 4096 speed = 1 sprite unit per frame
		ship->aim_goto_x =	temp_ship->x_pos -(( sine_table [ (ship->aim_x_rot)>>10 ] * 
							cosine_table [ (ship->aim_y_rot)>>10 ] )<<6) ;

		ship->aim_goto_y =	temp_ship->y_pos -(( cosine_table [ (ship->aim_x_rot)>>10 ] * 
							cosine_table [ (ship->aim_y_rot)>>10 ] )<<6);

		ship->aim_goto_z =	temp_ship->z_pos + ( sine_table [ (ship->aim_y_rot)>>10 ] <<18 ) ;
	
		temp_long  = ((ship->command_counter&63)-32)<<6 ;
		
		ship->aim_x_rot = ((ship->aim_z_rot+ship->aim_x_rot+temp_long)&ROT_LIMIT) ;
		ship->aim_y_rot = ((ship->aim_y_rot-(temp_long>>1) )&ROT_LIMIT) ;
		
		// Tend to levl
		if ( ship->aim_y_rot>(512<<10) ) ship->aim_y_rot -= (1024<<10) ;
		ship->aim_y_rot = (ship->aim_y_rot-(ship->aim_y_rot>>4))&ROT_LIMIT ;
		
		if ( dist<(8<<24) )
		{
			min_speed = ((temp_ship->speed>>4) + temp_ship->speed) ;
			max_speed = min_speed + 64 ;
		}
		else
		{
			min_speed = temp_ship->speed ;
			max_speed = min_speed + 64 ;
		}
		
		if ( dist>(24<<24) )
		{
			ship->command = COMMAND_ATTACK ;		
		}
	}
}


//if(			ship->command == COMMAND_ATTACK )		
//{
		//Test print up stuff
//		status.score = 0 ;
//		message_addscore( 1 ) ;
//}

if ( dist > (32<<24) ) min_speed = (1024+256) ;

if (min_speed>max_speed) max_speed=min_speed;

// Engine on or off
if ( ship->speed>max_speed)
{
	ship->thrust_control -= 128 ;
}
else
{
	if ( ship->speed<min_speed )
	{
		ship->thrust_control += 128 ;
	}
	else
	{
		if ( dist > (32<<24) )
		{
			if ( dist > (64<<24) )
			{
				ship->thrust_control += 128 ;
			}
			else
			{
				if ( ship->speed > (temp_ship->speed>>1) )
				{
					ship->thrust_control -= 128 ;
				}
				else
				{
					ship->thrust_control += 128 ;
				}
			}
		}
		else
		{
			if ( ( ((dist>>16)) -(ship->speed)) < 0 )
			{
				ship->thrust_control -= 128 ;
			}
			else
			{ 
				ship->thrust_control += 128 ;
			}
		}
	}
}

// Limit the ammount of thrust
if (ship->thrust_control < 0) ship->thrust_control = 0 ;
if (ship->thrust_control > 1024) ship->thrust_control = 1024 ;


// Allow them to shoot or wot
if (ship->fire_request != WEAPON_NOTHING)
{
	if (ship->command == COMMAND_ATTACK_RUN_AWAY) ship->fire_request = WEAPON_NOTHING ;
	
	if ( (temp_ship->type>>4) == BIG_SHIP )
	{
		if ( dist > (48<<24) || aim > (64<<10) )
		{
			ship->fire_request = WEAPON_NOTHING ;
		}
	}
	else
	{
		if ( dist > (32<<24) || aim > (32<<10) )
		{
			ship->fire_request = WEAPON_NOTHING ;
		}
	}
}

}


void new_flight_path_control( ship_stack * ship )

{

rotate_node node_data ;
rotate_node node_data2 ;
target_struct target ;
long x_aim , y_aim , z_aim , aim ;
long x_dist , y_dist , z_dist , dist ;
long temp_long , temp_long2 ;

long z_roll ;

long y_limit , mode ;

ship_stack* temp_ship ;


mode = ship->special_data->control_mode ;


// Test remove fire requests
// ship->fire_request = WEAPON_NOTHING ;

// Find the greatest unsigned dist
x_dist = (ship->x_pos) - (ship->goto_x) ;
y_dist = (ship->y_pos) - (ship->goto_y) ;
z_dist = (ship->z_pos) - (ship->goto_z) ;

if ( x_dist < 0 ) x_dist = -x_dist ;
if ( y_dist < 0 ) y_dist = -y_dist ;
if ( z_dist < 0 ) z_dist = -z_dist ;

if ( x_dist > y_dist )
{
	dist = x_dist ;
}
else
{
	dist = y_dist ;
}

if ( z_dist > dist ) dist = z_dist ;


// Rotate these distances by the ships rot

node_data.x_pos =	( (ship->x_pos) - 
					( ship->goto_x ) )>>6 ;

node_data.y_pos =	( (ship->y_pos) - 
					( ship->goto_y ) )>>6 ;

node_data.z_pos =	( ( ship->goto_z )
					- (ship->z_pos) )>>6 ;


node_data.x_rot = ship->x_rot ;
node_data.y_rot = ((1<<20) - ship->y_rot )&ROT_LIMIT ;
node_data.z_rot = ship->z_rot ;

oppo_rotate_node_from_c( &node_data ) ;

temp_long = (( -(node_data.y_pos>>10) )-1) ;
if (temp_long < 0) temp_long = -temp_long ;
if (temp_long == 0) temp_long = 1 ;


// Find the ideal x,y rotation to aim for
target.x_pos = ship->x_pos ;
target.y_pos = ship->y_pos ;
target.z_pos = ship->z_pos ;
target.x_aim = ship->goto_x ;
target.y_aim = ship->goto_y ;
target.z_aim = ship->goto_z ;

target_finder( &target );

// Find the relative rotation between fighter and aim
x_aim = (target.x_rot) - (ship->x_rot) ;
y_aim = (target.y_rot) - (ship->y_rot) ;
if (x_aim > 512*1024) x_aim -= 1024*1024 ;
if (y_aim > 512*1024) y_aim -= 1024*1024 ;
if (x_aim < -512*1024) x_aim += 1024*1024 ;
if (y_aim < -512*1024) y_aim += 1024*1024 ;

// Roll based on rel x
temp_long2 = -x_aim ;

if (temp_long2 > (256*1024) ) temp_long2 -= (512*1024) ;
if (temp_long2 < (-256*1024) ) temp_long2 += (512*1024) ;

temp_long2 = (temp_long2&ROT_LIMIT) ;



if (x_aim < 0) x_aim = -x_aim ;
if (y_aim < 0) y_aim = -y_aim ;
if (x_aim>y_aim)
{
	aim = x_aim ;
}
else
{
	aim = y_aim ;
}


ship->z_control = -((node_data.x_pos/temp_long)) ;



z_roll = (ship->z_rot - temp_long2 ) ;
if (z_roll > (1<<19) ) z_roll -= (1<<20) ;
if (z_roll < (-(1<<19)) ) z_roll += (1<<20) ;

ship->x_control = (-(z_roll>>8)) ;


// Sort out the y control 
// Is the target behind me - yes wham in full up or down
if ( node_data.y_pos > 0)
{
	if (node_data.z_pos > 0)
	{
		ship->y_control = 1024 ;
	}
	else
	{
		ship->y_control = -1024 ;
	}
}
else
{
	ship->y_control = (((node_data.z_pos/temp_long))) ;
}

// Max Limit for controls

// Roll
if (ship->x_control > 1024) ship->x_control = 1024 ;
if (ship->x_control < -1024) ship->x_control = -1024 ;

// Climb
if (ship->y_control > 1024 ) ship->y_control = 1024 ;
if (ship->y_control < -1024 ) ship->y_control = -1024 ;

// Yaw
if (ship->z_control > 1024 ) ship->z_control = 1024 ;
if (ship->z_control < -1024 ) ship->z_control = -1024 ;


// Engine on or off
if ( dist > (32<<24) )
{
	ship->thrust_control += 128 ;
	if (ship->thrust_control > 1024) ship->thrust_control = 1024 ;
}
else
{
	if ( ( ((dist>>16)) -(ship->speed)) < 0 )
	{
		ship->thrust_control -= 128 ;
		if (ship->thrust_control < 0) ship->thrust_control = 0 ;
	}
	else
	{ 
		ship->thrust_control += 128 ;
		if (ship->thrust_control > 1024) ship->thrust_control = 1024 ;
	}
}

if (ship->speed > (1024+256)) ship->thrust_control -= 256 ;

//	ship->fire_request = WEAPON_NOTHING ;

}


void new_formation_control( ship_stack * ship )

{

rotate_node node_data ;
rotate_node node_data2 ;
target_struct target ;
long x_aim , y_aim , z_aim , aim ;
long x_dist , y_dist , z_dist , dist ;
long temp_long , temp_long2 ;

long z_roll ;

long y_limit , mode ;

ship_stack* temp_ship ;

temp_ship = (ship_stack*) ( (ship_sdb*) ship->special_data )->command_address ;

mode = ship->special_data->control_mode ;

if ( mode != SDB_MODE_FORMATION ) return ; 

// Formation controller

//########################################################

// Has this ship just entered a formation
// Note : Only fix formation if flightpath start <> 255


if (ship->command == COMMAND_FIXED_FORMATION)
{
	if (ship->special_data->flightpath_point != 255)
	{
		node_data.x_pos = ( ship->special_data )->x_offset ;
		node_data.y_pos = -( ( ship->special_data )->y_offset +(1<<24) ) ;
		node_data.z_pos = ( ship->special_data )->z_offset ;
	
		// Is this ship in formation with the player in the mother ship
		if (	((temp_ship->type)>>4) == PLAYERS_SHIP &&
				docked.status != DOCKING_OUT )
		{
			node_data.y_pos = -( ( ship->special_data )->y_offset + (4<<24) ) ;
		}
	
		node_data.x_rot = temp_ship->x_rot ;
		node_data.y_rot = temp_ship->y_rot ;
		node_data.z_rot = temp_ship->z_rot ;
	
		rotate_node_from_c( &node_data );
	
		ship->x_pos = temp_ship->x_pos + node_data.x_pos ;
		ship->y_pos = temp_ship->y_pos + node_data.y_pos ;
		ship->z_pos = temp_ship->z_pos + node_data.z_pos ;
	}
}

//########################################################

if ( mode == SDB_MODE_FORMATION ) ship->command = COMMAND_ENTER_FORMATION ;

ship->who_owns_me = (long) ship->special_data->command_address ;

if (ship->command == COMMAND_ENTER_FORMATION)
{

	// Formation offset stuff again but with offset y	
	node_data.x_pos = ( ship->special_data )->x_offset ;
	
	// Offset for the ships y if in formation with the player when in the mother ship
	if (	((temp_ship->type)>>4) == PLAYERS_SHIP &&
			docked.status != DOCKING_OUT )
	{	
		node_data.y_pos = -( ( ship->special_data )->y_offset +(2<<24) ) ;
	}
	else
	{
		node_data.y_pos = -( ( ship->special_data )->y_offset -(1<<24) ) ;
	}
	
	
	node_data.z_pos = ( ship->special_data )->z_offset ;
	
	node_data.x_rot = temp_ship->x_rot ;
	node_data.y_rot = temp_ship->y_rot ;
	node_data.z_rot = temp_ship->z_rot ;
	
	rotate_node_from_c( &node_data );
	
	ship->goto_x = temp_ship->x_pos + node_data.x_pos ;
	ship->goto_y = temp_ship->y_pos + node_data.y_pos ;
	ship->goto_z = temp_ship->z_pos + node_data.z_pos ;

}


// Find the greatest unsigned dist
x_dist = (ship->x_pos) - (ship->goto_x) ;
y_dist = (ship->y_pos) - (ship->goto_y) ;
z_dist = (ship->z_pos) - (ship->goto_z) ;

if ( x_dist < 0 ) x_dist = -x_dist ;
if ( y_dist < 0 ) y_dist = -y_dist ;
if ( z_dist < 0 ) z_dist = -z_dist ;

if ( x_dist > y_dist )
{
	dist = x_dist ;
}
else
{
	dist = y_dist ;
}

if ( z_dist > dist ) dist = z_dist ;


ship->aim_counter -= 1 ;
if (ship->aim_counter < 0)
{

// Find rate of change between last goto coords and present
ship->aim_x_vel = (ship->goto_x - ship->aim_goto_x) ;
ship->aim_y_vel = (ship->goto_y - ship->aim_goto_y) ;
ship->aim_z_vel = (ship->goto_z - ship->aim_goto_z) ;

// Check they are not too massive
if (ship->aim_x_vel > (64<<24)) ship->aim_x_vel = 0 ;
if (ship->aim_x_vel < -(64<<24)) ship->aim_x_vel = 0 ;

if (ship->aim_y_vel > (64<<24)) ship->aim_y_vel = 0 ;
if (ship->aim_y_vel < -(64<<24)) ship->aim_y_vel = 0 ;

if (ship->aim_z_vel > (64<<24)) ship->aim_z_vel = 0 ;
if (ship->aim_z_vel < -(64<<24)) ship->aim_z_vel = 0 ;

// Keep a copy of the last goto coords
ship->aim_goto_x = ship->goto_x ;
ship->aim_goto_y = ship->goto_y ;
ship->aim_goto_z = ship->goto_z ;

ship->aim_counter = 7 ;

}


ship->aim_x_vel = 1024 ;
ship->aim_y_vel = 0 ;
ship->aim_z_vel = 0 ;


// Rotate these distances by the ships rot

// Use these for the y aim
node_data.x_pos =	( (ship->x_pos) - 
					(temp_ship->x_pos + node_data.x_pos + ship->aim_x_vel ))>>6 ;

node_data.y_pos =	( (ship->y_pos) - 
					(temp_ship->y_pos + node_data.y_pos + ship->aim_y_vel ))>>6 ;

node_data.z_pos =	( (temp_ship->z_pos + node_data.z_pos + ship->aim_z_vel )
					- (ship->z_pos) )>>6 ;

// Set these up before node data is rotated
//target.x_aim = ( temp_ship->x_pos + node_data.x_pos + ship->aim_x_vel ) ;
//target.y_aim = ( temp_ship->y_pos + node_data.y_pos + ship->aim_y_vel ) ;
//target.z_aim = ( temp_ship->z_pos + node_data.z_pos + ship->aim_z_vel ) ;

node_data.x_rot = ship->x_rot ;
node_data.y_rot = ((1<<20) - ship->y_rot )&ROT_LIMIT ;
node_data.z_rot = ship->z_rot ;

oppo_rotate_node_from_c( &node_data ) ;

// Set these to the goto coords
target.x_aim = ship->goto_x ;
target.y_aim = ship->goto_y ;
target.z_aim = ship->goto_z ;

// Find the ideal x,y rotation to aim for
target.x_pos = ship->x_pos ;
target.y_pos = ship->y_pos ;
target.z_pos = ship->z_pos ;

target_finder( &target );

// Find the relative rotation between fighter and aim
x_aim = (target.x_rot) - (ship->x_rot) ;
y_aim = (target.y_rot) - (ship->y_rot) ;
if (x_aim > 512*1024) x_aim -= 1024*1024 ;
if (y_aim > 512*1024) y_aim -= 1024*1024 ;
if (x_aim < -512*1024) x_aim += 1024*1024 ;
if (y_aim < -512*1024) y_aim += 1024*1024 ;

if (x_aim < 0) x_aim = -x_aim ;
if (y_aim < 0) y_aim = -y_aim ;
if (x_aim>y_aim)
{
	aim = x_aim ;
}
else
{
	aim = y_aim ;
}



temp_long = (( -(node_data.y_pos>>10) )-1) ;
if (temp_long < 0) temp_long = -temp_long ;
if (temp_long == 0) temp_long = 1 ;



ship->z_control = -((node_data.x_pos/temp_long)) ;



z_roll = (ship->z_rot - temp_ship->z_rot ) ;
if (z_roll > (1<<19) ) z_roll -= (1<<20) ;
if (z_roll < (-(1<<19)) ) z_roll += (1<<20) ;

ship->x_control = (-(z_roll>>8)) ;

if (ship->x_control > 1024) ship->x_control = 1024 ;
if (ship->x_control < -1024) ship->x_control = -1024 ;

if (ship->z_control > 1024) ship->z_control = 1024 ;
if (ship->z_control < -1024) ship->z_control = -1024 ;


// Sort out the y control 
// Is the target behind me - yes wham in full up or down
if ( node_data.y_pos > 0)
{
	if (node_data.z_pos > 0)
	{
		ship->y_control = 1024 ;
	}
	else
	{
		ship->y_control = -1024 ;
	}
}
else
{
	ship->y_control = (((node_data.z_pos/temp_long))) ;
}


// Max Limit for y control
if (ship->y_control > 1024) ship->y_control = 1024 ;
if (ship->y_control < -1024) ship->y_control = -1024 ;


dist -= (1<<24) ;
if (dist < 0) dist = 0 ;


// Engine on or off
if ( dist > (32<<24) )
{
	ship->thrust_control = 1024 ;
}
else
{
	if ( ( ((dist>>16)) -(ship->speed)) < 0 )
	{
		ship->thrust_control -= 128 ;
		if (ship->thrust_control < 0) ship->thrust_control = 0 ;
	}
	else
	{ 
		ship->thrust_control += 128 ;
		if (ship->thrust_control > 1024) ship->thrust_control = 1024 ;
	}
}

if ( ship->fire_request != WEAPON_NOTHING )
{
	if (aim > (16<<10) || dist > (4<<24) )
	{
		ship->fire_request = WEAPON_NOTHING ;
	}
}

}





