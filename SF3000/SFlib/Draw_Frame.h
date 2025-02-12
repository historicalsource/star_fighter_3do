#include "SF_Mission.h"
#include "Ship_Struct.h"
#include "ship_control.h"
#include "SF_CelUtils.h"
#include "graphics.h"
#include "camera_struct.h"
#include "SF_Status.h"

#include "Misc_Struct.h"
extern docking_struct docked ;

extern long test_mode ;

/* External Definitions */

extern void rotate_node_from_c(void*);
extern long wave_counter ;
extern long wave_counter2 ;
extern long wave_counter3 ;
extern long camera_x_rotation ;
extern long camera_y_rotation ;
extern long camera_x_position ;
extern long camera_y_position ;
extern long camera_z_position ;
extern long test_grid_flip ;
extern struct camera_data camera [ 3 ] ;

// Camera Stuff
extern ship_stack* ship_viewed_last_frame ;
extern long ship_viewed_last_frame_x_rot ;

extern long camera_x_velocity ;
extern long camera_y_velocity ;
extern long camera_z_velocity ;

extern long collision_box_colour_adder ;

extern game_status status ;
extern long pod_counter ;
extern long* temp_store ;
extern long bonus_shade ;

extern ship_stack* ship_being_viewed ;

extern planet_data planet_info ;

extern ship_stack *players_ship ;

/* Local Function prototypes */

void draw_frame ( long ) ;

