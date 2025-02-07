#ifndef __SOUND_UPDATE_H
#define __SOUND_UPDATE_H

#include "Ship_Struct.h" 

extern long cosine_table [ 2048 ] ;
extern long* sine_table ;

typedef struct sound_details 
					{
					long stereo_pos ;
					long volume ;
					long distance ;
					} sound_details
					;

void make_sound ( long , long , long , long );

void get_sound_details( sound_details* , long , long , long ) ;

void update_engine_sounds( void ) ;

void stop_engine_sounds( void ) ;

void start_engine_sounds( void ) ;

extern ship_stack* players_ship ;



extern ship_stack* sound_big_ship ;
extern ship_stack* sound_small_ship ;
extern long sound_channel_small_ship ;
extern long sound_channel_big_ship ;
extern long sound_sample_small_ship ;
extern long engine_sounds_on_or_wot ;
extern long sound_channel_shields_low ;

extern long camera_x_position ;
extern long camera_y_position ;
extern long camera_z_position ;

extern long camera_x_velocity ;
extern long camera_y_velocity ;
extern long camera_z_velocity ;


#define SMALL_SHIP_ENGINE_ON		0
#define SMALL_SHIP_ENGINE_OFF		1

#define LASER_SOUND					0
#define MISSILE_SOUND				1
#define LASER_HIT_SOUND				2
#define BIG_EXPLOSION_SOUND			3
#define SMALL_EXPLOSION_SOUND		4
#define LASER_SOUND_1				5
#define LASER_SOUND_2				6
#define LASER_SOUND_3				7
#define LASER_SOUND_4				8
#define LASER_SOUND_5				9
#define LASER_SOUND_6				10
#define LASER_SOUND_7				11
#define LASER_SOUND_8				12
#define MEDIUM_EXPLOSION_SOUND		4
#define VERY_BIG_EXPLOSION_SOUND	3
#define THUD_SOUND					13


#endif

