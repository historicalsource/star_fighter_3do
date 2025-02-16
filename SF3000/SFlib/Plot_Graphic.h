extern long camera_x_rotation;
extern long camera_y_rotation;
extern long camera_z_rotation;
extern long camera_x_position;
extern long camera_y_position;
extern long camera_z_position;
extern long camera_x_velocity;
extern long camera_y_velocity;
extern long camera_z_velocity;


extern long atg_selected;
extern long ata_selected ;

extern void plot_stars(void);
extern void plot_space_stars(void);
extern void setup_rotations(void);
extern void plot_spinning_ship(long,long);

extern void plot_ship_graphic(void*);
extern void plot_bit_graphic(void*);
extern void rotate_node_from_c(void*);
extern void plot_smoke(void*);
extern void machine_code_constants(void*);
extern void rotate_sky(void*);
extern long find_rotation(long ,long);
extern void plot_laser(void*);
extern void plot_bit(void*);
extern void rotate_sky_node(void*);
extern void plot_planets(void);

#ifndef __GRAPHIC_STUFF_H
#define __GRAPHIC_STUFF_H

typedef struct rotate_node 
					{
					long x_pos ;
					long y_pos ;
					long z_pos ;
					long x_rot ;
					long y_rot ;
					long z_rot ;
					} rotate_node
					;

#endif




