#include "Ship_Control.h"
#include "Collision.h"
#include "Explosion.h"
#include "Smoke_Control.h"
#include "Stdio.h"
#include "SF_ArmLink.h"
#include "Sound_Control.h"
#include "Maths_Stuff.h"
#include "Misc_Struct.h"
#include "Laser_Control.h"
#include "Graphic_Struct.h"
#include "Plot_Graphic.h"
#include "Sound_Control.h"
#include "SF_ArmUtils.h"
#include "SF_Sound.h"

void laser_update(void)

{

long temp_long ;
laser_stack *laser ;
laser_stack *laser_temp ;

laser=(lasers.info).start_address ;

while ((laser->header).status==1)
{

 // add on laser velocitys to start and end points of laser
 laser->x_pos += laser->x_vel ; 
 laser->y_pos += laser->y_vel ;
 laser->z_pos += laser->z_vel ;

 laser->x_pos2 += laser->x_vel ;
 laser->y_pos2 += laser->y_vel ;
 laser->z_pos2 += laser->z_vel ;

 // Check for any ground / object collisions	
 temp_long = check_collision ( laser->x_pos , laser->y_pos , laser->z_pos ) ;

 if ( temp_long != 0 && laser->who_owns_me != temp_long )
 {
	 add_smoke( 	laser->x_pos , laser->y_pos , laser->z_pos ,
					0 , 0 , 0 ,
					LASER_HIT_SMOKE ,
					0 );
	 laser->counter = 0 ;

	if (	bonus_collision_ref >= 2 && laser->who_owns_me == (long) players_ship &&
			(temp_long&16383) == temp_long ) 
	{
		add_bonus_from_collision_box( bonus_collision_ref , temp_long );
 	}
	
	 // If the collision check returns a positive value then this means you have hit
	 // a static ground object with the address offset returned
	 // Call static_explode to update the objects hit counter and blow up and delete
	 // the object. Call with object ref and damage ( Max hits counter 254 - average 100)
	 if ( temp_long > 0 )
	 {
 
		 // Laser fired from ground guns don't effect ground objects hits counter
		 if ( laser->who_owns_me > 16384 )
		 static_explode ( temp_long , laser->type ) ;
 
 	 }
 	 else
	 {
 
 		// If its not hit a object then its a ground hit
 		// In which case do ground explosion
 		dent_ground( laser->x_pos , laser->y_pos , laser->type ) ;

 	 }
 }


 // update the counter for the laser when <0 then delete
 laser->counter -= 1 ;

 // Get the adr of the next before any deleting is done
 laser_temp=(laser->header).next_address ;

	if ( (laser->counter) < 0 && laser->type != BEAM_LASER )
	{
		// delete the laser
		armlink_deleteitem(laser,&lasers);
	}

 laser=laser_temp ;
}

}


void aim_laser( long x_pos , long y_pos , long z_pos ,
				long x_aim , long y_aim , long z_aim ,
				long your_x_vel , long your_y_vel , long your_z_vel ,
				long target_x_vel , long target_y_vel , long target_z_vel ,
				long type , long ref )
{

// This is the fire laser at this target routine
// Used by gound gun bases , big ships , satellites etc.
// Given a targets coords it will aim and add the laser

target_struct target ;
long x_dist , y_dist , z_dist , distance ;

// Find the x & y angle between you and the target
target.x_pos = x_pos ;
target.y_pos = y_pos ;
target.z_pos = z_pos ;
target.x_aim = x_aim ;
target.y_aim = y_aim ;
target.z_aim = z_aim ;

target_finder( &target );

//x_dist = ((x_pos-x_aim)>>12)*cosine_table [ target.x_rot>>10 ] ;
//if (x_dist < 0) x_dist = -x_dist ;

//y_dist = ((y_pos-y_aim)>>12)*sine_table [ target.x_rot>>10 ] ;
//if (y_dist < 0) y_dist = -y_dist ;

//z_dist = ((z_pos-z_aim)>>12)*sine_table [ target.y_rot>>10 ] ;
//if (z_dist < 0) z_dist = -z_dist ;

//distance = x_dist + y_dist + z_dist ;

distance = target.distance ;

// Adjust the aim based on distance and velocitys
x_aim += ( ((target_x_vel - your_x_vel)>>10) * (distance>>15) ) ;
y_aim += ( ((target_y_vel - your_y_vel)>>10) * (distance>>15) ) ;
z_aim += ( ((target_z_vel - your_z_vel)>>10) * (distance>>15) ) ;

// Find new angle of fire taking velocity and distance into account
target.x_pos = x_pos ;
target.y_pos = y_pos ;
target.z_pos = z_pos ;
target.x_aim = x_aim ;
target.y_aim = y_aim ;
target.z_aim = z_aim ;

target_finder( &target );


add_laser( 	x_pos , y_pos , z_pos ,
			your_x_vel , your_y_vel , your_z_vel ,
			target.x_rot , target.y_rot ,
			type , ref , LASER_TYPE_GUN_BASE ) ;

}


void add_laser( long x_pos , long y_pos , long z_pos ,
				long x_vel , long y_vel , long z_vel ,
				long x_rot , long y_rot ,
				long type , long ref , long laser_ref )
				
{

// This is the general purpose laser adding routine which will
// add a laser of a certain type starting from the given position
// going in the direction specified by the x and y rotation at the
// standard laser speed plus the velocitys given ( Normally that of
// the ship firing the laser)

laser_stack *laser ;

long x_dir , y_dir , z_dir ;

laser = armlink_sorttolist( &lasers , x_pos ) ;

// This laser type is only used for the players Beam Laser
if (type == BEAM_LASER) type += 1 ;

if (laser != NULL)

{
	// Setup the direction of the laser based on the x and y rotations given
	// Max value = 4096 ( << 12 = 1 map unit )
 
	if (laser_ref == LASER_TYPE_GUN_BASE)
	{
		// Max value = 8192 ( << 12 = 2 map units )
		x_dir =	( sine_table [ x_rot>>10 ] * 
		cosine_table [ y_rot>>10 ] )>>11 ;
		y_dir = ( cosine_table [ x_rot>>10 ] * 
		cosine_table [ y_rot>>10 ] )>>11 ;
		z_dir = (sine_table [ y_rot>>10 ])<<1 ;
	}
	else
	{
		// Max value = 4096 ( << 12 = 1 map unit )
		x_dir =	( sine_table [ x_rot>>10 ] * 
		cosine_table [ y_rot>>10 ] )>>12 ;
		y_dir = ( cosine_table [ x_rot>>10 ] * 
		cosine_table [ y_rot>>10 ] )>>12 ;
		z_dir = sine_table [ y_rot>>10 ] ;
	}

	// Set start point of laser
	laser->x_pos = x_pos ;
	laser->y_pos = y_pos ;
	laser->z_pos = z_pos ;

	// Set end point of laser 
	laser->x_pos2 = x_pos - (x_dir<<12) - (x_dir<<11) ;
	laser->y_pos2 = y_pos - (y_dir<<12) - (y_dir<<11) ;
	laser->z_pos2 = z_pos + (z_dir<<12) + (z_dir<<11) ;

	// Set the velocity for the laser plus the given velocity
	laser->x_vel = x_vel -	(x_dir<<12) ;
	laser->y_vel = y_vel -	(y_dir<<12) ;
	laser->z_vel = z_vel +	(z_dir<<12) ;

	// Set type as given and max life span counter
	laser->type = type ;
	laser->counter = 40 ;

	// Set the non collision flag
	laser->who_owns_me = ref ;
}

}

void start_beam_lasers()
{

long loop ;

for (loop = 0 ; loop < 32 ; loop += 1)
{
	armlink_releaseitem( beam_lasers [ loop ] , &lasers );
}

beam_laser.on_last_frame = 1 ;
beam_laser.x_rot_bend = 0 ;
beam_laser.y_rot_bend = 0 ;

sound_channel_beam_laser = sound_playsample( SOUND_BEAMLASER , 50, 127, 0 ) ;

}

void stop_beam_lasers()
{

long loop ;

for (loop = 0 ; loop < 32 ; loop += 1)
{
	armlink_suspenditem( beam_lasers [ loop ] , &lasers );
}

beam_laser.on_last_frame = 0 ;

if ( sound_channel_beam_laser != -1 )
{
sound_channel_beam_laser = sound_stopsample ( sound_channel_beam_laser ) ;
}

}

void update_beam_lasers()
{

long loop ;
long aim_height , x_grid , y_grid , type ;
long goto_x , goto_y , goto_z ;
laser_stack *laser ;
ship_stack *ship ;
target_struct target ;

graphics_details *details = (graphics_details*) static_graphics_adr ;

long x_rot = players_ship->x_rot ;
long y_rot = players_ship->y_rot ;
long x_pos = players_ship->x_pos ;
long y_pos = players_ship->y_pos ;
long z_pos = players_ship->z_pos ;

if ( sound_channel_beam_laser == -1 )
{
	sound_channel_beam_laser = sound_playsample( SOUND_BEAMLASER , 50, 127, 0 ) ;
}

// Is the target a air or ground object ?
if ( (long) players_ship->target < 16384)
{
	// Check that there is a target to lock onto
	if ( players_ship->target != NULL )
	{
		// Find aim pos for air to ground target

		x_grid = (( (long) players_ship->target)&127) ;
		y_grid = (( (long) players_ship->target)>>7) ;
		type = poly_map [ y_grid ] [ x_grid ] ;

		aim_height = ((height_map [ y_grid<<1 ] [ x_grid<<1 ])-17)<<21 ;
		if (aim_height<0) aim_height =0 ;
		aim_height += ((details+type)->missile_aim) ;

		goto_x = (unsigned) (x_grid<<25) ;
		goto_y = (unsigned) (y_grid<<25) ;
		goto_z = aim_height ;
	}
	else
	{
		// Aim abit in front of the player
		goto_x = players_ship->x_pos + (players_ship->x_vel<<6) ;
		goto_y = players_ship->y_pos + (players_ship->y_vel<<6) ;
		goto_z = players_ship->z_pos + (players_ship->z_vel<<6) ;
	}

}
else
{

	// Find aim pos for ship target
	ship = (ship_stack*) players_ship->target ;
	goto_x = ship->x_pos ;
	goto_y = ship->y_pos ;
	goto_z = ship->z_pos ;

}



for (loop = 0 ; loop < 32 ; loop +=1 )
{

// Find the ideal angle for the laser to head
target.x_pos = x_pos ;
target.y_pos = y_pos ;
target.z_pos = z_pos ;
target.x_aim = goto_x ;
target.y_aim = goto_y ;
target.z_aim = goto_z ;

target_finder( &target );

// Find the relative diff between ideal angle and current angle of beam
// Then limit the thingy
target.x_rot -= x_rot ;
if (target.x_rot>512*1024) target.x_rot -= 1024*1024 ;
if (target.x_rot<-512*1024) target.x_rot += 1024*1024 ;
if (target.x_rot>128*1024) target.x_rot = 128*1024 ;
if (target.x_rot<-128*1024) target.x_rot = -128*1024 ;

target.y_rot -= y_rot ;
if (target.y_rot>512*1024) target.y_rot -= 1024*1024 ;
if (target.y_rot<-512*1024) target.y_rot += 1024*1024 ;
if (target.y_rot>128*1024) target.y_rot = 128*1024 ;
if (target.y_rot<-128*1024) target.y_rot = -128*1024 ;

// Tend towards the ideal direction by scale of loop counter
x_rot += ((target.x_rot * (loop+8) )>>5) ;
y_rot += ((target.y_rot * (loop+8) )>>5) ;

// Add on the x and y benders
//x_rot = (x_rot + ((beam_laser.x_rot_bend * (loop-32) )>>4))&ROT_LIMIT ;
//y_rot = (y_rot + ((beam_laser.y_rot_bend * (loop-32) )>>5))&ROT_LIMIT ;
x_rot = (x_rot + ( cosine_table [ ( (loop<<6) + (beam_laser.x_rot_bend>>8) )&1023 ] << 3 ))&ROT_LIMIT ;
y_rot = (y_rot + ( sine_table [ ( (loop<<6) + (beam_laser.y_rot_bend>>8) )&1023 ] << 3 ))&ROT_LIMIT ;

// Set start and end points for this laser section
laser = beam_lasers [ loop ] ;

laser->x_pos = x_pos ;
laser->y_pos = y_pos ;
laser->z_pos = z_pos ;
if (laser->z_pos < 0) laser->z_pos = -laser->z_pos ;

x_pos -= (( sine_table [ x_rot>>10 ] * 
		cosine_table [ y_rot>>10 ] )<<1) ;
y_pos -= (( cosine_table [ x_rot>>10 ] * 
		cosine_table [ y_rot>>10 ] )<<1) ;
z_pos += (sine_table [ y_rot>>10 ]<<13) ;

laser->x_pos2 = x_pos ;
laser->y_pos2 = y_pos ;
laser->z_pos2 = z_pos ;
if (laser->z_pos2 < 0) laser->z_pos2 = -laser->z_pos2 ;

}

// Change the x and y benders by arm_randomom amount and limit check 'em
beam_laser.x_rot_bend += ((arm_random()&16383)-8192) ;
if ( beam_laser.x_rot_bend > (128*1024) ) beam_laser.x_rot_bend = (128*1024) ;
if ( beam_laser.x_rot_bend < -(128*1024) ) beam_laser.x_rot_bend = -(128*1024) ;

beam_laser.y_rot_bend += ((arm_random()&16383)-8192) ;
if ( beam_laser.y_rot_bend > (128*1024) ) beam_laser.y_rot_bend = (128*1024) ;
if ( beam_laser.y_rot_bend < -(128*1024) ) beam_laser.y_rot_bend = -(128*1024) ;

}


void small_fighter_lasers( ship_stack *ship )
{
laser_details_header *laser_header ;
laser_details *laser_data ;
graphics_details *details = (graphics_details*) ships_adr ;

long laser_type ;

// The small ships have 3 laser fire points defined
// The first point defined is the centre laser the next 2 are left / right wing
// There are 7 different laser upgrades 0-6 (ship->performance)->lasers

// Laser Number		Number to fire			Power			Total Power
//		0					1				2					1
//		1					1				3					2
//		2					2				2 + 2				2
//		3					2				3 + 3				4
//		4					2				4 + 4				5
//		5					3				3 + 4 + 3			8
//		6					3				4 + 5 + 4			13
//		7					3				5 + 6 + 5			16

if ( (ship->performance)->weapons[ WEAPON_LASER ] == 255 )
{
	laser_type = (( (long) ship )>>5)&7 ;
}
else
{
	laser_type = (ship->performance)->weapons[ WEAPON_LASER ] ;
}


laser_header = (laser_details_header*) (details+(ship->type))->laser_data ;
laser_data = (laser_details*) ( laser_header + 1 ) ;

// add_small_fighter_laser( laser_details* laser_data , ship_stack* ship , long power )

switch ( laser_type )
{

case 0 :
add_small_fighter_laser( laser_data , ship , LASER_POWER_2 );
make_sound( ship->x_pos , ship->y_pos , ship->z_pos , LASER_SOUND_1 ) ;
break ;

case 1 :
add_small_fighter_laser( laser_data , ship , LASER_POWER_3 );
make_sound( ship->x_pos , ship->y_pos , ship->z_pos , LASER_SOUND_2 ) ;
break ;

case 2 :
add_small_fighter_laser( laser_data+1 , ship , LASER_POWER_2 );
add_small_fighter_laser( laser_data+2 , ship , LASER_POWER_2 );
make_sound( ship->x_pos , ship->y_pos , ship->z_pos , LASER_SOUND_3 ) ;
break ;

case 3 :
add_small_fighter_laser( laser_data+1 , ship , LASER_POWER_3 );
add_small_fighter_laser( laser_data+2 , ship , LASER_POWER_3 );
make_sound( ship->x_pos , ship->y_pos , ship->z_pos , LASER_SOUND_4 ) ;
break ;

case 4 :
add_small_fighter_laser( laser_data+1 , ship , LASER_POWER_4 );
add_small_fighter_laser( laser_data+2 , ship , LASER_POWER_4 );
make_sound( ship->x_pos , ship->y_pos , ship->z_pos , LASER_SOUND_5 ) ;
break ;

case 5 :
add_small_fighter_laser( laser_data , ship , LASER_POWER_4 );
add_small_fighter_laser( laser_data+1 , ship , LASER_POWER_3 );
add_small_fighter_laser( laser_data+2 , ship , LASER_POWER_3 );
make_sound( ship->x_pos , ship->y_pos , ship->z_pos , LASER_SOUND_6 ) ;
break ;

case 6 :
add_small_fighter_laser( laser_data , ship , LASER_POWER_5 );
add_small_fighter_laser( laser_data+1 , ship , LASER_POWER_4 );
add_small_fighter_laser( laser_data+2 , ship , LASER_POWER_4 );
make_sound( ship->x_pos , ship->y_pos , ship->z_pos , LASER_SOUND_7 ) ;
break ;

case 7 :
add_small_fighter_laser( laser_data , ship , LASER_POWER_6 );
add_small_fighter_laser( laser_data+1 , ship , LASER_POWER_5 );
add_small_fighter_laser( laser_data+2 , ship , LASER_POWER_5 );
make_sound( ship->x_pos , ship->y_pos , ship->z_pos , LASER_SOUND_8 ) ;
break ;

}

ship->counter = 2 ;

}

void add_small_fighter_laser( laser_details* laser_data , ship_stack* ship , long power )
{
rotate_node node_data ;

node_data.x_pos = (laser_data->x_pos) ;
node_data.y_pos = (laser_data->y_pos) ;
node_data.z_pos = (laser_data->z_pos) ;
node_data.x_rot = ship->x_rot ;
node_data.y_rot = ship->y_rot ;
node_data.z_rot = ship->z_rot ;
rotate_node_from_c( &node_data );
add_laser( 	(ship->x_pos) + node_data.x_pos ,
			(ship->y_pos) + node_data.y_pos ,
			(ship->z_pos) + node_data.z_pos ,
			ship->x_vel , ship->y_vel , ship->z_vel ,
			ship->x_rot , ship->y_rot ,
			power ,
			(long) ship ,
			LASER_TYPE_SMALL_SHIP ) ;

}

