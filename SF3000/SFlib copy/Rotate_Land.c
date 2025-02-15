/* Call rotate land to prepare rotated coords for landscape nodes 

Coord system bit 31-24 = grid pos (out of 256  -  2^8)
             bit 24-12 = int position within grid
			 bit 11-0  = fraction position within grid
	After rotation / scaling each grid section = 4096
*/			 

#define ROT_OFFSET -(2*1024)


#include "Rotate_Land.h"
#include "Plot_Graphic.h"
#include "Plot_Land.h"
#include "Misc_Struct.h"
#include "Maths_Stuff.h"

void rotate_land( void )

{


/* Input these positions on landscape - scale = 2^24 per grid unit
   ie. across the map = 2^32 to auto wrap around 
   Input rotations 1024 = 360 degrees - no z rotation because there is no roll */


// Temp space to pass a bunch of vars to machine code in 
long machine_code_vars [ 64 ] ;



rot_struct *rot_data ;
long shift = 12 ;
long step = 32 ;
long bit_clear = 252 ;
long bit_step = 4 ;
long loop ;
long res = 0 ;
long rot_nodes [ 6 ] ;

/* Calc step rate across grid rotated */

long hoz_x ;// = cosine_table [ camera_x_rotation ] ;
long hoz_y ;//= ( sine_table [ camera_x_rotation ] * cosine_table [ camera_y_rotation ] )>>12 ;
long hoz_z ;//= ( sine_table [ camera_x_rotation ] * sine_table [ camera_y_rotation ] )>>12 ;

long vert_x ;//= cosine_table [ camera_x_rotation+256 ] ;
long vert_y ;//= ( sine_table [ camera_x_rotation+256 ] * cosine_table [ camera_y_rotation ] )>>12 ;
long vert_z ;//= ( sine_table [ camera_x_rotation+256 ] * sine_table [ camera_y_rotation ] )>>12 ;


/* Setup the cosine and sine values of the y rotation for quick access */
long cosine_y = cosine_table [ camera_y_rotation ] ;
long sine_y = - sine_table [ camera_y_rotation ] ;

static long silly = 0 ;

land_sort_offset = (camera_z_position>>24)+4 ;
if ( land_sort_offset < 0 ) land_sort_offset = -land_sort_offset ;

if ( camera_z_position < 0 )
{
	land_sort_offset = (-(camera_z_position>>24)) + 4 ;
}
else
{
	land_sort_offset = (camera_z_position>>24) + 4 ;
}

rot_data = (rot_struct*) (&machine_code_vars[0]) ;

rot_nodes [ 0 ] = 4096 ;
rot_nodes [ 1 ] = 0 ;
rot_nodes [ 2 ] = 0 ;
rot_nodes [ 3 ] = camera_x_rotation<<10 ;
rot_nodes [ 4 ] = camera_y_rotation<<10 ;
rot_nodes [ 5 ] = camera_z_rotation<<10 ;
rotate_land_node_from_c(&rot_nodes);
hoz_x = rot_nodes [ 0 ] ;
hoz_y = rot_nodes [ 1 ] ;
hoz_z = rot_nodes [ 2 ] ;

rot_nodes [ 0 ] = 0 ;
rot_nodes [ 1 ] = 4096 ;
rot_nodes [ 2 ] = 0 ;
rotate_land_node_from_c(&rot_nodes);
vert_x = rot_nodes [ 0 ] ;
vert_y = rot_nodes [ 1 ] ;
vert_z = rot_nodes [ 2 ] ;



// Setup the required data for the full res near by land grid
// This is a 32 * 32 sprite grid - ie. 33 * 33 nodes are required

rot_data->x_pos = ((camera_x_position >> 24) - 16 ) & 255 ;
rot_data->y_pos = ((camera_y_position >> 24) - 16 ) & 255 ;

//rot_data->offset_x = -((((( ( camera_x_position >> 12 ) & 4095 ) ROT_OFFSET) * cosine_table [ camera_x_rotation ] )>>12) +
//					((( ( ( camera_y_position >> 12 ) & 4095 ) ROT_OFFSET) * cosine_table [ camera_x_rotation+256 ] )>>12)) 
//					-(((hoz_x<<4) + (hoz_x>>1) + (vert_x<<4) + (vert_x>>1))) ;

//rot_data->offset_y = (((camera_z_position>>12)*sine_y)>>12) - 
//					((((( ( ( camera_x_position >> 12 ) & 4095 ) ROT_OFFSET) * sine_table [ camera_x_rotation ] )>>12) * cosine_y )>>12) -
//					((((( ( ( camera_y_position >> 12 ) & 4095 ) ROT_OFFSET) * sine_table [ camera_x_rotation+256 ] )>>12) * cosine_y ) >> 12)
//					-(((hoz_y<<4) + (hoz_y>>1) + (vert_y<<4) + (vert_y>>1))) ;

//rot_data->offset_z = (((camera_z_position>>12)*cosine_y)>>12) +
//					((((( ( ( camera_x_position >> 12 ) & 4095 ) ROT_OFFSET) * sine_table [ camera_x_rotation ] )>>12) * sine_y )>>12) +
//					((((( ( ( camera_y_position >> 12 ) & 4095 ) ROT_OFFSET) * sine_table [ camera_x_rotation+256 ] )>>12) * sine_y )>>12)
//					-(((hoz_z<<4) + (hoz_z>>1) + (vert_z<<4) + (vert_z>>1))) ;

rot_nodes [ 0 ] = -((( camera_x_position >> 12 ) & 4095 ) ROT_OFFSET) ;
rot_nodes [ 1 ] = -((( camera_y_position >> 12 ) & 4095 ) ROT_OFFSET) ;
rot_nodes [ 2 ] = camera_z_position>>12 ;
rotate_land_node_from_c(&rot_nodes);
rot_data->offset_x = (rot_nodes [ 0 ]) -(((hoz_x<<4) + (hoz_x>>1) + (vert_x<<4) + (vert_x>>1)));
rot_data->offset_y = (rot_nodes [ 1 ]) -(((hoz_y<<4) + (hoz_y>>1) + (vert_y<<4) + (vert_y>>1)));
rot_data->offset_z = (rot_nodes [ 2 ]) -(((hoz_z<<4) + (hoz_z>>1) + (vert_z<<4) + (vert_z>>1)));


/*
Setup the required data for the other non full res grids
These range from 4 * 4 sprites plotted in one go at a resolution
of 4 * 4 pixels per sprite upto 256 * 256 sprites (full map) at
a resolution of 8 sprites per pixel

The landscape is plotted at different resolutions and further into
the distance based on the height of the camera - as you get higher
you get to see copies of the map wrap round off to the horizon at
max height a distant grid of 16*16 copies of the entire map is plotted

*/

rot_data = (rot_struct*) (&machine_code_vars[11]) ;

for (loop = 0 ; loop<=6 ; loop += 1)
{

/* Setup pointer to outer grid start pos */
rot_data->x_pos = ((camera_x_position >> 24) - step ) & bit_clear ;
rot_data->y_pos = ((camera_y_position >> 24) - step ) & bit_clear ;

//rot_data->offset_x = -((((( ( camera_x_position>>shift) &16383) -8192) *cosine_table[camera_x_rotation])>>12) +
//					  ((( ( ( camera_y_position>>shift) &16383) -8192) *cosine_table[camera_x_rotation+256])>>12))
//					  -(((hoz_x<<5) + (hoz_x<<1) + (vert_x<<5) + (vert_x<<1))) ;

//rot_data->offset_y = ((( camera_z_position>>shift)*sine_y)>>12) - 
//				((((( ( ( camera_x_position>>shift) &16383) -8192) *sine_table[camera_x_rotation]) >>12)*cosine_y)>>12) -
//				((((( ( ( camera_y_position>>shift) &16383) -8192) *sine_table[camera_x_rotation+256]) >>12)*cosine_y)>>12)
//			  -(((hoz_y<<5) + (hoz_y<<1) + (vert_y<<5) + (vert_y<<1))) ;

//rot_data->offset_z = ((( camera_z_position>>shift)*cosine_y)>>12) +
//				((((( ( ( camera_x_position>>shift) &16383) -8192)*sine_table[camera_x_rotation])>>12)*sine_y)>>12) +
//				((((( ( ( camera_y_position>>shift) &16383) -8192)*sine_table[camera_x_rotation+256])>>12)*sine_y)>>12)
//				-(((hoz_z<<5) + (hoz_z<<1) + (vert_z<<5) + (vert_z<<1))) ;


rot_nodes [ 0 ] = -(( (camera_x_position >> shift ) & 16383)-8192 ) ;
rot_nodes [ 1 ] = -(( (camera_y_position >> shift ) & 16383)-8192 ) ;
rot_nodes [ 2 ] = camera_z_position>>shift ;
rotate_land_node_from_c(&rot_nodes);
rot_data->offset_x = (rot_nodes [ 0 ]) -(((hoz_x<<5) + (hoz_x<<1) + (vert_x<<5) + (vert_x<<1)));
rot_data->offset_y = (rot_nodes [ 1 ]) -(((hoz_y<<5) + (hoz_y<<1) + (vert_y<<5) + (vert_y<<1)));
rot_data->offset_z = (rot_nodes [ 2 ]) -(((hoz_z<<5) + (hoz_z<<1) + (vert_z<<5) + (vert_z<<1)));



rot_data += 1 ;
shift += 1 ;
step = (step<<1) ;
bit_clear -= bit_step ;
bit_step = (bit_step<<1) ;

if (bit_clear<0) bit_clear=0 ;

}


// Decide the resolution of the plot based on the camera height
// res default = 0 - full res

if ( camera_z_position > (32*(1<<24)) ) res = 1 ; 
if ( camera_z_position > (48*(1<<24)) ) res = 2 ;
if ( camera_z_position > (64*(1<<24)) ) res = 3 ;


/* Set the 1st pair of coords in quick lookup table 
the machine code will calculate the next 255 */
//quick_height_table [ 0 ] [ 0 ] = sine_y<<3 ;
//quick_height_table [ 0 ] [ 1 ] = cosine_y<<3 ;

//silly = (silly+4)&1023 ;

//silly_x = cosine_table [ (1024-silly) ]>>2 ;
//silly_y = sine_table [ (1024-silly) ]>>2 ;


rot_nodes [ 0 ] = 0 ; // cosine_table [ silly ]<<2 ;
rot_nodes [ 1 ] = 0 ; // sine_table [ silly ]<<2 ;
rot_nodes [ 2 ] = 32768 ;
rotate_land_node_from_c(&rot_nodes);
quick_height_table [ 0 ] [ 0 ] = rot_nodes [ 0 ] ;
quick_height_table [ 0 ] [ 1 ] = rot_nodes [ 1 ] ;
quick_height_table [ 0 ] [ 2 ] = rot_nodes [ 2 ] ;


machine_code_vars [ 5 ] = hoz_x ; 
machine_code_vars [ 6 ] = hoz_y ;
machine_code_vars [ 7 ] = hoz_z ; 
machine_code_vars [ 8 ] = vert_x ; 
machine_code_vars [ 9 ] = vert_y ; 
machine_code_vars [ 10 ] = vert_z ;

fast_rotation( &machine_code_vars , res ) ;

}
