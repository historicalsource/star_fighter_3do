#include "Ship_Struct.h" 
#include "Explosion_Struct.h"
#include "Graphic_Struct.h"

void ship_explosion_section_control( ship_stack* ) ;
ship_stack* add_ship_explosion_section(	ship_stack* ,
										long ,
										long ) ;

void add_section_smoke( long , long , long , long , long , long , long , long , long , long );

void explode_static_graphic ( long , long , long );
void static_explode( long , long );
void add_explosion_line ( 	long , long ,long ,
							long , long , long , long , long ,
							long , long , long , long , long ,
							long , long 
							);

void explode_next_frame( long );
void explosion_update( void );
void explode_ship ( ship_stack * );
void dent_ground ( long , long , long );
void undent_ground ( long , long , long );

ship_stack* add_explosion_section(	long , long , long ,
									long , long , long ,
									long , long , long );

void explosion_section_control( ship_stack* ) ;

void find_group_centre_point(	collision_details_header* ,
								ship_stack* ,
								long );

void explode_static_from_collision_box( long grid_ref , long  explosion_velocity , long owner , long style , long ) ;

void explode_ship_from_collision_box( ship_stack* , long  explosion_velocity , long owner , long style , long , long ) ;


extern explosion_list explosions ;

#define PARACHUTE_NO_EXPLODE	-32768

#define EXPLOSION_STYLE_NORMAL			0
#define EXPLOSION_STYLE_MEGA_BOMB		1
#define EXPLOSION_STYLE_COMET			2
#define EXPLOSION_STYLE_LOADS_OF_BITS	3

#define GROUP_0		1
#define GROUP_1		2
#define GROUP_2		4
#define GROUP_3		8
#define GROUP_4		16
#define GROUP_5		32

