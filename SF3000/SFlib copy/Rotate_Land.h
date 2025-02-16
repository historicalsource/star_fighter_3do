
/* Setup data tables for quick reference look up */
extern long cosine_table [ 2048 ] ;
extern long pex_table [ 32768 ] ;
extern long *sine_table ;
extern char polymap [ 128 ] [ 128 ] ;
extern long quick_height_table [ 256 ] [ 4 ] ;

/* Setup data tables for the maps etc. */
extern char landscape_heights [ 256 ] [ 256 ] ;
extern long rotated_coords [ 4096 ] [ 3 ] ;
extern long screen_coords [ 4096 ] [ 2 ] ;
extern long graphics_data [ 20000 ] ;
extern long star_coords [ 128 ] [ 4 ] ;

extern long silly_x ;
extern long silly_y ;
extern long land_sort_offset ;

void rotate_land( void ) ;
