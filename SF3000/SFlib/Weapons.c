#include "Smoke_Control.h"
#include "Collision.h"
#include "Laser_Control.h"
#include "SF_Armlink.h"
#include "Ship_Struct.h"
#include "Weapons.h"
#include "Explosion.h"
#include "Stdio.h"
#include "Ship_Control.h"
#include "Maths_Stuff.h"
#include "Misc_Struct.h"
#include "Graphic_Struct.h"
#include "Plot_Graphic.h"
#include "SF_Control.h"
#include "Collision_Update.h"
#include "String.h"
#include "Graphics_Set.h"

void weapon_control( ship_stack * weapon )
{

long type ;

type = (weapon->type);

switch ( type )
{

	case AIR_TO_GROUND :
		air_to_air_control( weapon );
	break ;

	case AIR_TO_AIR :
		air_to_air_control( weapon );
	break ;

	case MEGA_BOMB :
		mega_bomb_control( weapon );
	break ;
	
	case MINE :
		mine_control( weapon );
	break ;

	case WING_POD :
		wing_pod_control( weapon );
	break ;

	case SAM_TO_AIR :
		air_to_air_control( weapon );
	break ;

}


}

// ##########################################################

void air_to_ground_control( ship_stack *missile )
{

}

void add_air_to_ground( ship_stack *ship )
{

// Adds a atg (air to ground) missile from given ship
// going to given ground target (ship-target = grid ref 0-16383)

ship_stack *atg ;

graphics_details *details = (graphics_details*) static_graphics_adr ;
long *graphic_data ;
long aim_height , x_grid , y_grid , type ;

ship->counter = 8 ;

atg = armlink_sorttolist( &ships , ship->x_pos ) ;
if (atg != NULL)
	{

	memset( (void*) ( ((char*) atg) + (sizeof(link_header)) ) , 0 , (sizeof(ship_stack))-(sizeof(link_header)) ) ; 

	x_grid = (( (long) ship->target)&127) ;
	y_grid = (( (long) ship->target)>>7) ;
	type = poly_map [ y_grid ] [ x_grid ] ;

	aim_height = ((height_map [ y_grid<<1 ] [ x_grid<<1 ])-17)<<21 ;
	if (aim_height<0) aim_height =0 ;
	aim_height += ((details+type)->missile_aim) ;
	
	atg->x_pos = ship->x_pos ;
	atg->y_pos = ship->y_pos ;
	atg->z_pos = ship->z_pos ;

	atg->x_rot = (ship->x_rot + (16*((arm_random()&2047)-1024)) )&ROT_LIMIT ;
	atg->y_rot = (ship->y_rot + (16*((arm_random()&2047)-1024)) )&ROT_LIMIT ;
	atg->z_rot = ship->z_rot ;

	atg->x_vel = ship->x_vel ;
	atg->y_vel = ship->y_vel ;
	atg->z_vel = ship->z_vel ;
	atg->speed = ship->speed ;
	atg->type = AIR_TO_GROUND ;
	atg->shields = 120 ;
	atg->who_owns_me = (long) ship ;
	atg->goto_x = (unsigned) (x_grid<<25) ;
	atg->goto_y = (unsigned) (y_grid<<25) ;
	atg->goto_z = aim_height ;
	atg->what_hit_me = 0 ;
	atg->who_hit_me = 0 ;
	atg->special_data = NULL ;
	
	atg->collision_size = (1<<22) ;
	
	control_recheckcamera( atg ) ;
	
	}


}

// ##########################################################

void air_to_air_control( ship_stack *ata )
{

long coll_check , x_aim , y_aim ;
target_struct target ;
ship_stack *ship_hit ;
ship_stack *aim_at = (ship_stack*) (ata->target) ;

target.x_pos = ata->x_pos ;
target.y_pos = ata->y_pos ;
target.z_pos = ata->z_pos ;

if (ata->type == AIR_TO_GROUND)
{
	target.x_aim = ata->goto_x ;
	target.y_aim = ata->goto_y ;
	target.z_aim = ata->goto_z ;
}
else
{
	target.x_aim = aim_at->x_pos ;
	target.y_aim = aim_at->y_pos ;
	if ( (aim_at->type>>4) == CAR )
	{
		target.z_aim = aim_at->z_pos+((1<<23)+(1<<22)) ;	
	}
	else
	{
		target.z_aim = aim_at->z_pos ;
	}
}


target_finder( &target );

// Find the relative rotation between missile and aim
x_aim = (target.x_rot) - (ata->x_rot) ;
y_aim = (target.y_rot) - (ata->y_rot) ;
if (x_aim > 512*1024) x_aim -= 1024*1024 ;
if (y_aim > 512*1024) y_aim -= 1024*1024 ;
if (x_aim < -512*1024) x_aim += 1024*1024 ;
if (y_aim < -512*1024) y_aim += 1024*1024 ;

// Limit this value
if (x_aim > 64*1024) x_aim = 64*1024 ;
if (y_aim > 64*1024) y_aim = 64*1024 ;
if (x_aim < -64*1024) x_aim = -64*1024 ;
if (y_aim < -64*1024) y_aim = -64*1024 ;

// Add these values onto the rate of change
ata->x_control += x_aim ;
ata->y_control += y_aim ;

// Auto centre the rates of change
ata->x_control -= ((ata->x_control)>>2) ;
ata->y_control -= ((ata->y_control)>>2) ;

// Turn the missile
ata->x_rot = ((ata->x_rot)+((ata->x_control)>>4) )&ROT_LIMIT ;
ata->y_rot = ((ata->y_rot)+((ata->y_control)>>4) )&ROT_LIMIT ;
ata->z_rot = ((ata->z_rot)+ (32*1024) )&ROT_LIMIT ;

// Missiles always have their engines on
ata->speed += 48 ;

// Friction on speed
ata->speed -= ((ata->speed)>>7) ;

ata->x_vel =	-(
				(( sine_table [ (ata->x_rot)>>10 ] * 
				cosine_table [ (ata->y_rot)>>10 ] )>>12)
				* (ata->speed)
				) ;

ata->y_vel =	-(
				(( cosine_table [ (ata->x_rot)>>10 ] * 
				cosine_table [ (ata->y_rot)>>10 ] )>>12)
				* (ata->speed)
				) ;

ata->z_vel =	( sine_table [ (ata->y_rot)>>10 ] * (ata->speed) ) ;

// add velocity onto x ,y ,z positions
ata->x_pos += ata->x_vel ;
ata->y_pos += ata->y_vel ;
ata->z_pos += ata->z_vel ;

// Dec shields so missile has a limited lifespan
ata->shields -= 1 ;

// Check this missile for collisions with ground objects / hills
coll_check = check_collision( 	ata->x_pos ,
								ata->y_pos ,
								ata->z_pos ) ;

if (coll_check != 0 && coll_check != ata->who_owns_me )
	{
		if (coll_check>0) 
		{
			static_explode( coll_check , 64 ) ;
		}
		else
		{
			dent_ground( ata->x_pos , ata->y_pos , 12 ) ;
		}
		
		// Reverse velocity on impact
		ata->x_vel = -ata->x_vel ;
		ata->y_vel = -ata->y_vel ;
		ata->z_vel = -ata->z_vel ;
		ata->speed = -ata->speed ;
		
		ata->shields = 0 ;
	}

// Check for air hits
if (ata->what_hit_me != 0) missile_collision( ata ) ;

add_smoke( 	ata->x_pos , ata->y_pos , ata->z_pos ,
			ata->x_vel , ata->y_vel , ata->z_vel ,
			MISSILE_SMOKE ,
			0 ) ;

}

void add_air_to_air( ship_stack *ship , ship_stack *target )
{

// Adds a air to air missile from a given ship

ship_stack *ata ;

ship->counter = 8 ;

ata = armlink_sorttolist( &ships , ship->x_pos ) ;
if (ata != NULL)
	{
	
	memset( (void*) ( ((char*) ata) + (sizeof(link_header)) ) , 0 , (sizeof(ship_stack))-(sizeof(link_header)) ) ; 
	
	ata->x_pos = ship->x_pos ;
	ata->y_pos = ship->y_pos ;
	ata->z_pos = ship->z_pos ;
	
	ata->x_rot = ((ship->x_rot) + (16*((arm_random()&2047)-1024)) )&ROT_LIMIT ;
	ata->y_rot = ((ship->y_rot) + (16*((arm_random()&2047)-1024)) )&ROT_LIMIT ;
	ata->z_rot = ship->z_rot ;
	
	ata->x_vel = ship->x_vel ;
	ata->y_vel = ship->y_vel ;
	ata->z_vel = ship->z_vel ;
	ata->speed = ship->speed ;
	ata->type = AIR_TO_AIR ;
	ata->shields = 120 ;
	ata->target = (void*) target ;
	ata->who_owns_me = (long) ship ;
	ata->what_hit_me = 0 ;
	ata->who_hit_me = 0 ;
	ata->special_data = NULL ;
	
	ata->collision_size = (1<<22) ;
	
	control_recheckcamera( ata );
	}

}

void add_sam( 	long x_pos , long y_pos , long z_pos ,
				ship_stack *target ,
				long object_ref )
{

// Adds a sam (surface to air) missile from give x,y,z
// going to given ship - used by static ground objects
// that wish to shoot you

ship_stack *sam ;

sam = armlink_sorttolist( &ships , x_pos ) ;
if (sam != NULL)
	{
	
	memset( (void*) ( ((char*) sam) + (sizeof(link_header)) ) , 0 , (sizeof(ship_stack))-(sizeof(link_header)) ) ; 

	sam->x_pos = x_pos ;
	sam->y_pos = y_pos ;
	sam->z_pos = z_pos ;
	sam->x_rot = 0 ;
	sam->y_rot = 256*1024 ;
	sam->z_rot = 0 ;
	sam->x_vel = 0 ;
	sam->y_vel = 0 ;
	sam->z_vel = 0 ;
	sam->speed = 0 ;
	sam->type = SAM_TO_AIR ;
	sam->shields = 120 ;
	sam->target = (void*) target ;
	sam->who_owns_me = object_ref ;
	sam->what_hit_me = 0 ;
	sam->who_hit_me = 0 ;
	sam->special_data = NULL ;
	
	sam->collision_size = (1<<22) ;
	
	control_recheckcamera( sam );
	}

}


// ##########################################################

void mega_bomb_control( ship_stack * mega_bomb )

{
long coll_check ;

//Dec shields
mega_bomb->shields -= 1;

//Add on velocities
mega_bomb->x_pos += mega_bomb->x_vel ;
mega_bomb->y_pos += mega_bomb->y_vel ;
mega_bomb->z_pos += mega_bomb->z_vel ;

if (which_graphics_set != SPACE_GRAPHICS)
{
	//Friction on speed
	mega_bomb->x_vel -= ((mega_bomb->x_vel)>>7) ;
	mega_bomb->y_vel -= ((mega_bomb->y_vel)>>7) ;
	mega_bomb->z_vel -= ((mega_bomb->z_vel)>>7) ;

	//Gravity
	mega_bomb->z_vel -= (1<<17) ;

	//Nose down
	mega_bomb->y_rot = ( (mega_bomb->y_rot) - cosine_table [ (mega_bomb->y_rot)>>10 ] )&ROT_LIMIT ;
}

coll_check = check_collision( 	mega_bomb->x_pos ,
								mega_bomb->y_pos ,
								mega_bomb->z_pos ) ;

if (coll_check != 0)
	{
		if (coll_check>0) 
		{
			static_explode( coll_check , 255 ) ;
		}
		else
		{
			dent_ground( mega_bomb->x_pos , mega_bomb->y_pos , 20 ) ;
		}
		
		mega_bomb->shields = 0 ;
	}

// Check for air hits
if (mega_bomb->what_hit_me != 0) missile_collision( mega_bomb ) ;


}

void add_mega_bomb ( ship_stack *ship )	
			
{

ship_stack *mega_bomb ;

ship->counter = 16 ;

mega_bomb = armlink_sorttolist( &ships , ship->x_pos ) ;
if (mega_bomb != NULL)
	{
	
	memset( (void*) ( ((char*) mega_bomb) + (sizeof(link_header)) ) , 0 , (sizeof(ship_stack))-(sizeof(link_header)) ) ; 

	mega_bomb->x_pos = ship->x_pos ;
	mega_bomb->y_pos = ship->y_pos ;
	mega_bomb->z_pos = ship->z_pos ;
	mega_bomb->x_rot = ship->x_rot ;
	mega_bomb->y_rot = ship->y_rot ;
	mega_bomb->z_rot = ship->z_rot ;
	mega_bomb->x_vel = ship->x_vel+((ship->x_vel)>>1) ;
	mega_bomb->y_vel = ship->y_vel+((ship->y_vel)>>1) ;
	mega_bomb->z_vel = ship->z_vel+((ship->z_vel)>>1) ;
	mega_bomb->type = MEGA_BOMB ;
	mega_bomb->shields = 400 ;
	mega_bomb->who_owns_me = (long) ship ;
	mega_bomb->what_hit_me = 0 ;
	mega_bomb->who_hit_me = 0 ;
	mega_bomb->special_data = NULL ;
	
	mega_bomb->collision_size = (1<<22) ;
	
	control_recheckcamera( mega_bomb );
	}

}

// ##########################################################

void mine_control( ship_stack *mine )
{

long coll_check ;
long x_dist , y_dist , z_dist ;
ship_stack* target = (ship_stack*) mine->target ;

// Find distance from target and limit it
x_dist = mine->x_pos - target->x_pos ;
if ( x_dist < -(8*(1<<24)) ) x_dist = -(8*(1<<24)) ;
if ( x_dist > (8*(1<<24)) ) x_dist = (8*(1<<24)) ;

y_dist = mine->y_pos - target->y_pos ;
if ( y_dist < -(8*(1<<24)) ) y_dist = -(8*(1<<24)) ;
if ( y_dist > (8*(1<<24)) ) y_dist = (8*(1<<24)) ;

z_dist = mine->z_pos - target->z_pos ;
if ( z_dist < -(8*(1<<24)) ) z_dist = -(8*(1<<24)) ;
if ( z_dist > (8*(1<<24)) ) z_dist = (8*(1<<24)) ;


mine->x_pos += mine->x_vel ;
mine->y_pos += mine->y_vel ;
mine->z_pos += mine->z_vel ;
mine->z_rot = ((mine->z_rot)+64*1024)&ROT_LIMIT ;

// Head towards target
mine->x_vel -= (x_dist>>6) ;
mine->y_vel -= (y_dist>>6) ;
mine->z_vel -= (z_dist>>6) ;

mine->x_vel -= ((mine->x_vel)>>4) ;
mine->y_vel -= ((mine->y_vel)>>4) ;
mine->z_vel -= ((mine->z_vel)>>4) ;
mine->shields -= 1 ;

coll_check = check_collision( 	mine->x_pos ,
								mine->y_pos ,
								mine->z_pos ) ;

if (coll_check != 0)
	{
		if (coll_check>0) 
		{
			static_explode( coll_check , 255 ) ;
		}
		else
		{
			dent_ground( mine->x_pos , mine->y_pos , 20 ) ;
		}
		
		mine->shields = 0 ;
	}

// Check for air hits
if (mine->what_hit_me != 0) missile_collision( mine ) ;


}

void add_mine ( ship_stack *ship )
{

ship_stack *mine ;
ship_stack* temp_ship ;

ship->counter = 12 ;

mine = armlink_sorttolist( &ships , ship->x_pos ) ;
if (mine != NULL)
	{

	memset( (void*) ( ((char*) mine) + (sizeof(link_header)) ) , 0 , (sizeof(ship_stack))-(sizeof(link_header)) ) ; 

	mine->x_pos = ship->x_pos ;
	mine->y_pos = ship->y_pos ;
	mine->z_pos = ship->z_pos ;
	mine->x_rot = ship->x_rot ;
	mine->y_rot = ship->y_rot ;
	mine->z_rot = ship->z_rot ;
	mine->x_vel = ship->x_vel ;
	mine->y_vel = ship->y_vel ;
	mine->z_vel = ship->z_vel ;
	mine->type = MINE ;
	mine->shields = 50 ;
	mine->who_owns_me = (long) ship ;
	mine->what_hit_me = 0 ;
	mine->who_hit_me = 0 ;
	mine->special_data = NULL ;
	mine->target = ship->target ;
	
	mine->collision_size = (1<<22) ;
	
	// Any missiles targetting the ship that launched this mine now target the mine
	temp_ship=(ships.info).start_address ;
	while ((temp_ship->header).status==1)
	{
		if ( (temp_ship->type>>4) == WEAPON && temp_ship->target == (void*) ship )
		{
			temp_ship->target = (void*) mine ;
		}
		temp_ship=(temp_ship->header).next_address ;
	}
	
	control_recheckcamera( mine ) ;
	}

}

void wing_pod_control( ship_stack *pod )
{

ship_stack *ship = (ship_stack*) pod->who_owns_me ;
long coll_check ;
long laser_type ;
rotate_node node_data ;
static long pod_shield_adder = 7 ;

pod_shield_adder -= 1 ;
if ( pod_shield_adder <= 0 )
{
	if ( pod->shields < 120 ) pod->shields += 1 ;
	pod_shield_adder = 7 ;
}

if (pod->ref & POD_1)
{
	node_data.x_pos = 512*3+256+128 ;
	node_data.y_pos = -(256*4)+pod->goto_y ;
	node_data.z_pos = 0;
}

if (pod->ref & POD_2)
{
	node_data.x_pos = -512*3-256-128;
	node_data.y_pos = -(256*4)+pod->goto_y ;
	node_data.z_pos = 0;
}

if (pod->ref & POD_3)
{
	node_data.x_pos = 512*4+256+128;
	node_data.y_pos = -(256*5)+pod->goto_y ;
	node_data.z_pos = 256;
}

if (pod->ref & POD_4)
{
	node_data.x_pos = -512*4-256-128;
	node_data.y_pos = -(256*5)+pod->goto_y ;
	node_data.z_pos = 256;
}


node_data.x_pos = (node_data.x_pos<<11) ;
node_data.y_pos = (node_data.y_pos<<11) ;
node_data.z_pos = (node_data.z_pos<<11) ;

node_data.x_rot = (ship->x_rot) ;
node_data.y_rot = (ship->y_rot) ;
node_data.z_rot = (ship->z_rot) ;
rotate_node_from_c( &node_data ) ;

pod->x_pos = (ship->x_pos) + node_data.x_pos ;
pod->y_pos = (ship->y_pos) + node_data.y_pos ;
pod->z_pos = (ship->z_pos) + node_data.z_pos ;

pod->x_rot = ship->x_rot ;
pod->y_rot = ship->y_rot ;
pod->z_rot = ship->z_rot ;

pod->x_vel = ship->x_vel ;
pod->y_vel = ship->y_vel ;
pod->z_vel = ship->z_vel ;

pod->speed = ship->speed ;

pod->goto_y -= ((pod->goto_y)>>2) ;


// Is the pod being used as a laser
if (ship->fire_request == WEAPON_LASER 
	&& pod->ref == (1<<pod_counter) )
{

	if (pod->ref & POD_1 || pod->ref & POD_2)
	{
		laser_type = LASER_POWER_2 ;
	}
	else
	{
		laser_type = LASER_POWER_4 ;
	}
	
	
	add_laser( 	(pod->x_pos) ,//+(temp_data[0]<<12) ,
				(pod->y_pos) ,//-(temp_data[1]<<12) ,
				(pod->z_pos) ,//+(temp_data[2]<<12) ,
				pod->x_vel , pod->y_vel , pod->z_vel ,
				pod->x_rot , pod->y_rot ,
				laser_type , (long) ship , LASER_TYPE_SMALL_SHIP ) ;
	pod->goto_y -= 256 ;
}

// Is the pod being used as a engine
if (ship->thrust_control != 0)
{
	add_smoke( 	(pod->x_pos) ,//+(temp_data[0]<<11) ,
				(pod->y_pos) ,//-(temp_data[1]<<11) ,
				(pod->z_pos) ,//+(temp_data[2]<<11) ,
				pod->x_vel , pod->y_vel , pod->z_vel ,
				WING_POD_SMOKE ,
				0 ) ;
	ship->speed += 16 ;
	pod->goto_y += 32 ;
}


// check for collisions with ground objects n' ground
coll_check = check_collision ( pod->x_pos , pod->y_pos , pod->z_pos ) ;
	
if (coll_check != 0 && coll_check != ship->who_owns_me )
	{
	
	// Is it a collision with a static object
	// If so damage the object based on the speed of the collision
	if (coll_check > 0)
	{
		static_explode ( coll_check , ( (pod->speed)>>2 ) ) ;
	}
	else
	{
		dent_ground( pod->x_pos , pod->y_pos , 8 ) ;
	}
	
	//Damage the pod based on the impact speed
	pod->shields -= ((pod->speed)>>2) ;
	
	}

// If the ship that owns this pod dies then so does the pod
if ( ship->shields <= 0 ) pod->shields = 0 ;


// Is this pod about to die - if so clear pod ref in ship
if (pod->shields <= 0)
{
	ship->ref = ((ship->ref)&(~pod->ref)) ;
}

// Check for air hits
if (pod->what_hit_me != 0) missile_collision( pod ) ;


}


void add_wing_pod ( ship_stack *ship )
{

ship_stack *pod ;
long pod_number = -1 ;

// Lets see if any of the pod slots are free
// Prefered slots - POD_1 ie. last slot checked
if ( (ship->ref & POD_4) == 0 ) pod_number = POD_4 ;
if ( (ship->ref & POD_3) == 0 ) pod_number = POD_3 ;
if ( (ship->ref & POD_2) == 0 ) pod_number = POD_2 ;
if ( (ship->ref & POD_1) == 0 ) pod_number = POD_1 ;

if ( pod_number >=0 )
{
	pod = armlink_sorttolist( &ships , ship->x_pos ) ;
	if (pod != NULL)
		{
		
		memset( (void*) ( ((char*) pod) + (sizeof(link_header)) ) , 0 , (sizeof(ship_stack))-(sizeof(link_header)) ) ; 

		pod->type = WING_POD ;
		pod->shields = 120 ;
		pod->who_owns_me = (long) ship ;
		pod->goto_y = 0 ;
		
		pod->collision_size = 1<<24 ;
		pod->counter = 0 ;
		pod->ref = pod_number ;
		pod->what_hit_me = 0 ;
		pod->who_hit_me = 0 ;
		pod->special_data = NULL ;
		
		pod->collision_size = (1<<22) ;
		
		ship->ref = ((ship->ref)|pod_number) ;

		}
}


}

