#ifndef __GRAPHICS_SET_DEFINES
#define __GRAPHICS_SET_DEFINES

#define EARTH_LAND_BASED_GRAPHICS		0
#define EARTH_SEA_BASED_GRAPHICS		1
#define CHEMICAL_GRAPHICS				2
#define THORG_GRAPHICS					3
#define SPACE_GRAPHICS					4
#define DEATH_STAR_GRAPHICS				5

//	#########################################
//	#										#
//	#		Earth Land Based				#
//	#										#
//	#########################################

// These fire lasers if near player
#define		EL_GUN1				1
#define		EL_GUN2				2
#define		EL_SUB				3

// These fire missiles if near player
#define		EL_SAM_BASE1		4
#define		EL_SAM_BASE2		5
#define		EL_SAM_BASE3		6

// These launch ships if near player
#define		EL_HANGER			7
#define		EL_TALL_HANG		8

// Misc graphics
#define		EL_ARCH_TOWER		9
#define		EL_B_OFFICE			10
#define		EL_PYRAMID			11
#define		EL_MEGATOWER2		12
#define		EL_T_OFFICE			13
#define		EL_MEGATOWER		14
#define		EL_FACTORY1			15
#define		EL_FACTORY2			16
#define		EL_L_TOWER2			17
#define		EL_TOWER1			18
#define		EL_TOWER3			19

// These all set to die if left alone
#define		EL_LINK1			20
#define		EL_LINK2			21

// Misc graphics
#define		EL_DOME				22
#define		EL_TREE				23
#define		EL_DOME_S			24
#define		EL_BIG_TREE			25
#define		EL_RADAR_BASE		26

// These all set to die if left alone
#define		EL_X_PIPE			27
#define		EL_B_PIPE1_1		28
#define		EL_B_PIPE1_2		29
#define		EL_PIPE1			30
#define		EL_B_PIPE2_1		31
#define		EL_B_PIPE2_2		32
#define		EL_PIPE2			33

#define		EL_TALL_TREE		34

// These are burnt out graphic to replace blown up buildings etc.

// For tower blocks etc. eg. EL_T_OFFICE
#define		EL_DESTROYED_MEDIUM			35
// For long shapes in the x eg. EL_B_OFFICE
#define		EL_DESTROYED_LONG			36
// For big objects eg. EL_DOME
#define		EL_DESTROYED_BIG			37
// For small objects eg. EL_GUN1
#define		EL_DESTROYED_SMALL			38

// Max no of static graphics
#define		EL_MAX	38


//	#########################################
//	#										#
//	#		Earth Sea Based					#
//	#										#
//	#########################################

// These fire lasers if near player
#define		ES_GUN1				1
#define		ES_GUN2				2
#define		ES_SUB				3

// These fire missiles if near player
#define		ES_SAM_BASE1		4
#define		ES_SAM_BASE2		5
#define		ES_SAM_BOAT			6

// These launch ships if near player
#define		ES_HANGER			7
#define		ES_CARRIER			8

// Misc graphics
#define		ES_CON_TOWER		9
#define		ES_OFFICE_1			10
#define		ES_OFFICE_R			11
#define		ES_TOWER1			12
#define		ES_FACTORY			13
#define		ES_OIL_RIG			14
#define		ES_FACTORY2			15
#define		ES_LIGHTHOUSE		16
#define		ES_PIER				17
#define		ES_L_TOWER			18
#define		ES_TOWER2			19

// These all set to die if left alone
#define		ES_SPACEWALL1		20
#define		ES_SPACEWALL2		21
#define		ES_LINK1			22
#define		ES_LINK2			23

// Misc graphics
#define		ES_TREE				24
#define		ES_BIG_TREE			25

// These are burnt out graphic to replace blown up buildings etc.

#define		ES_DESTROYED_MEDIUM			26
#define		ES_DESTROYED_LONG			27
#define		ES_DESTROYED_BIG			28
#define		ES_DESTROYED_SMALL			29

// Max no of static graphics
#define		ES_MAX	29


//	#########################################
//	#										#
//	#		Chemical graphics set			#
//	#										#
//	#########################################

// These fire lasers if near player
#define		CM_GUN3				1
#define		CM_GUN2				2
#define		CM_GUN1				3

// These fire missiles if near player
#define		CM_SAM_BASE3		4
#define		CM_SAM_BASE2		5
#define		CM_SAM_BASE1		6

// These launch ships if near player
#define		CM_HANG_2			7
#define		CM_DEATH_HANG		8

// Misc graphics
#define		CM_FUEL_TANK		9
#define		CM_CHEM_1			10
#define		CM_CHEM_2			11
#define		CM_RADAR_BASE		12
#define		CM_ROCKIT			13
#define		CM_B_OFFICE			14
#define		CM_CON_TOWER		15
#define		CM_COOL_TOWER		16
#define		CM_NOFFICE_1		17
#define		CM_Nl_TOWER			18
#define		CM_L_TOWER2			19
#define		CM_NTOWER1			20
#define		CM_NTOWER3			21
#define		CM_NFACTORY2		22
#define		CM_FACTORY1			23
#define		CM_ARCH_TOWER		24

// These all set to die if left alone
#define		CM_LINK1			25
#define		CM_LINK2			26
#define		CM_SPACEWALL1		27
#define		CM_SPACEWALL2		28
#define		CM_CX_PIPE			29
#define		CM_CB_PIPE1_1		30
#define		CM_CB_PIPE1_2		31
#define		CM_CPIPE1			32
#define		CM_CB_PIPE2_1		33
#define		CM_CB_PIPE2_2		34
#define		CM_CPIPE2			35

// These are burnt out graphic to replace blown up buildings etc.

#define		CM_DESTROYED_MEDIUM			36
#define		CM_DESTROYED_LONG			37
#define		CM_DESTROYED_BIG			38
#define		CM_DESTROYED_SMALL			39


// Max no of static graphics
#define		CM_MAX	39


//	#########################################
//	#										#
//	#		Thorg graphics set				#
//	#										#
//	#########################################

// These fire lasers if near player
#define		TH_R_GUN1			1
#define		TH_R_GUN2			2
#define		TH_R_GUN3			3

// These fire missiles if near player
#define		TH_R_SAM1			4
#define		TH_R_SAM2			5
#define		TH_R_SAM3			6

// These launch ships if near player
#define		TH_ROCK_HANG		7
#define		TH_T_R_HANG			8

// Misc graphics
#define		TH_C_L_TOWER		9
#define		TH_CHURCH			10
#define		TH_DES_OFF			11
#define		TH_HEAD				12
#define		TH_PALACE_R			13
#define		TH_ROCKHOUSE		14
#define		TH_ROCKTOWER		15
#define		TH_WATCHTOWER		16
#define		TH_CROSS			17
#define		TH_DES_HOUSE		18
#define		TH_DES_TREE			19
#define		TH_GRAVE			20

// These all set to die if left alone
#define		TH_WALL1			21
#define		TH_WALL2			22
#define		TH_C_LINK1			23
#define		TH_C_LINK2			24

// Misc graphics
#define		TH_RADAR_ROCK		25
#define		TH_C_TOWER2			26

// These all set to die if left alone
#define		TH_WALL1_S			27
#define		TH_WALL2_S			28

// These are burnt out graphic to replace blown up buildings etc.

#define		TH_DESTROYED_MEDIUM			29
#define		TH_DESTROYED_LONG			30
#define		TH_DESTROYED_BIG			31
#define		TH_DESTROYED_SMALL			32

// Max no of static graphics
#define		TH_MAX	32


//	#########################################
//	#										#
//	#		Space graphics set				#
//	#										#
//	#########################################

// These fire lasers if near player
#define		SP_SPACE_GUNL		1
#define		SP_SPACE_GUNM		2
#define		SP_SPACE_GUNH		3

// These fire missiles if near player
#define		SP_SPACE_SAML		4
#define		SP_SPACE_SAMM		5
#define		SP_SPACE_SAMH		6

// There are no ship launching hangars in space
#define		SP_FREE				7
#define		SP_FREE2			8

// Space astriods simple ones first 8 - complex last 8
#define		SP_AST_2_H0			9
#define		SP_AST_2_H1			10
#define		SP_AST_2_H2			11
#define		SP_AST_2_H3			12
#define		SP_AST_2_H4			13
#define		SP_AST_2_H5			14
#define		SP_AST_2_H6			15
#define		SP_AST_2_H7			16

// Complex astriods
#define		SP_AST_1_H0			17
#define		SP_AST_1_H1			18
#define		SP_AST_1_H2			19
#define		SP_AST_1_H3			20
#define		SP_AST_1_H4			21
#define		SP_AST_1_H5			22
#define		SP_AST_1_H6			23
#define		SP_AST_1_H7			24


// Max no of static graphics
#define		SP_MAX	24


//	#########################################
//	#										#
//	#		Death Star graphics set			#
//	#										#
//	#########################################

// These fire lasers if near player
#define		DS_GUN_F			1
#define		DS_GUN_F2			2
#define		DS_GUN_F3			3

// These fire missiles if near player
#define		DS_SAM_F			4
#define		DS_SAM_F2			5
#define		DS_SAM_F3			6

// These launch ships if near player
// Only one death hanger is defined
#define		DS_DEATH_HANG		7
#define		DS_FREE				8

// Death towers which cannot be destroyed
#define		DS_TOWER1			9
#define		DS_TOWER2			10
#define		DS_TOWER3			11
#define		DS_TOWER4			12
#define		DS_TOWER5			13
#define		DS_TOWER6			14
#define		DS_TOWER7			15
#define		DS_TOWER8			16

// Misc graphics
#define		DS_DEATH_DOME		17
#define		DS_STAR_TOWER		18
#define		DS_SPACETOWER		19
#define		DS_DEATH_DOM2		20
#define		DS_DEATH_ARCH		21
#define		DS_COOL_DEATH		22
#define		DS_NMEGATOWER		23
#define		DS_ROCKIT			24

// These all set to die if left alone
#define		DS_SPACEWALL1		25
#define		DS_SPACEWALL2		26

// Death links which cannot be destroyed
#define		DS_DL_LINK1			27
#define		DS_DL_LINK2			28
#define		DS_DH_LINK1			29
#define		DS_DH_LINK2			30

// Misc graphics
#define		DS_DEATH_PYR		31

// These are burnt out graphic to replace blown up buildings etc.

#define		DS_DESTROYED_MEDIUM			32
#define		DS_DESTROYED_LONG			33
#define		DS_DESTROYED_BIG			34
#define		DS_DESTROYED_SMALL			35

// Max no of static graphics
#define		DS_MAX	35


// Misc static graphics that apply to all levels
#define		OBJECT_NOTHING				0
#define		OBJECT_CLOUD				60
#define		OBJECT_BONUS_CRYSTAL_ADDER	61

#define DELAY_EXPLODE_THIS_GRAPHIC		explode_next_frame( (x_grid + (y_grid<<7)) ) ; break ;

// Static Explosion Styles
#define		STATIC_EXPLODE_NOT							collision_map [ y_grid ] [ x_grid ] = 255 ; break ;
#define		STATIC_EXPLODE_SMALL						explode_static_graphic ( x_grid , y_grid , 0 ) ;
#define		STATIC_EXPLODE_SMALL_MUSHROOM				explode_static_graphic ( x_grid , y_grid , 1 ) ;
#define		STATIC_EXPLODE_SMALL_FAST					explode_static_graphic ( x_grid , y_grid , 2 ) ;
#define		STATIC_EXPLODE_SMALL_FAST_MUSHROOM			explode_static_graphic ( x_grid , y_grid , 3 ) ;
#define		STATIC_EXPLODE_MEDIUM						explode_static_graphic ( x_grid , y_grid , 4 ) ;
#define		STATIC_EXPLODE_MEDIUM_MUSHROOM				explode_static_graphic ( x_grid , y_grid , 5 ) ;
#define		STATIC_EXPLODE_MEDIUM_FAST					explode_static_graphic ( x_grid , y_grid , 6 ) ;
#define		STATIC_EXPLODE_MEDIUM_FAST_MUSHROOM			explode_static_graphic ( x_grid , y_grid , 7 ) ;
#define		STATIC_EXPLODE_BIG							explode_static_graphic ( x_grid , y_grid , 8 ) ;
#define		STATIC_EXPLODE_BIG_MUSHROOM					explode_static_graphic ( x_grid , y_grid , 9 ) ;
#define		STATIC_EXPLODE_BIG_FAST						explode_static_graphic ( x_grid , y_grid , 10 ) ;
#define		STATIC_EXPLODE_BIG_FAST_MUSHROOM			explode_static_graphic ( x_grid , y_grid , 11 ) ;
#define		STATIC_EXPLODE_MEGATOWER_2_GROUPS			explode_static_graphic ( x_grid , y_grid , 12 ) ;
#define		STATIC_EXPLODE_TOWER_1_GROUP				explode_static_graphic ( x_grid , y_grid , 13 ) ;
#define		STATIC_EXPLODE_STAR_TOWER_4_GROUPS			explode_static_graphic ( x_grid , y_grid , 14 ) ;
#define		STATIC_EXPLODE_PALACE_REL_3_GROUPS			explode_static_graphic ( x_grid , y_grid , 15 ) ;



#define		STATIC_REPLACE_WITH							collision_map [ y_grid ] [ x_grid ] = 50 ; poly_map [ y_grid ] [ x_grid ] =

// Static Explosion Styles ref
#define		REF_STATIC_EXPLODE_SMALL					0
#define		REF_STATIC_EXPLODE_SMALL_MUSHROOM			1
#define		REF_STATIC_EXPLODE_SMALL_FAST				2
#define		REF_STATIC_EXPLODE_SMALL_FAST_MUSHROOM		3
#define		REF_STATIC_EXPLODE_MEDIUM					4
#define		REF_STATIC_EXPLODE_MEDIUM_MUSHROOM			5
#define		REF_STATIC_EXPLODE_MEDIUM_FAST				6
#define		REF_STATIC_EXPLODE_MEDIUM_FAST_MUSHROOM		7
#define		REF_STATIC_EXPLODE_BIG						8
#define		REF_STATIC_EXPLODE_BIG_MUSHROOM				9
#define		REF_STATIC_EXPLODE_BIG_FAST					10
#define		REF_STATIC_EXPLODE_BIG_FAST_MUSHROOM		11
#define		REF_STATIC_EXPLODE_MEGATOWER_2_GROUPS		12
#define		REF_STATIC_EXPLODE_TOWER_1_GROUP			13
#define		REF_STATIC_EXPLODE_STAR_TOWER_4_GROUPS		14
#define		REF_STATIC_EXPLODE_PALACE_REL_3_GROUPS		15

// Crystals are added in explosion.c
#define		STATIC_CRYSTAL_CHANCE_NON

#define		STATIC_CRYSTAL_CHANCE_VERY_POOR
//	add_crystal_from_static( x_grid , y_grid , 0 ) ;

#define		STATIC_CRYSTAL_CHANCE_POOR
//	add_crystal_from_static( x_grid , y_grid , 1 ) ;

#define		STATIC_CRYSTAL_CHANCE_AVERAGE
//	add_crystal_from_static( x_grid , y_grid , 2 ) ;

#define		STATIC_CRYSTAL_CHANCE_GOOD
//	add_crystal_from_static( x_grid , y_grid , 3 ) ;

#define		STATIC_CRYSTAL_CHANCE_VERY_GOOD
//	add_crystal_from_static( x_grid , y_grid , 4 ) ;

#define		STATIC_CRYSTAL_CHANCE_VERY_VERY_GOOD
//	add_crystal_from_static( x_grid , y_grid , 5 ) ;



#define		REF_STATIC_CRYSTAL_CHANCE_VERY_POOR			0
#define		REF_STATIC_CRYSTAL_CHANCE_POOR				1
#define		REF_STATIC_CRYSTAL_CHANCE_AVERAGE			2
#define		REF_STATIC_CRYSTAL_CHANCE_GOOD				3
#define		REF_STATIC_CRYSTAL_CHANCE_VERY_GOOD			4
#define		REF_STATIC_CRYSTAL_CHANCE_VERY_VERY_GOOD	5


#define		EARTH_LAND_HIDDEN_BONUS		check_bonus_earth_land_graphics( x_grid , y_grid ) ;
#define		EARTH_SEA_HIDDEN_BONUS		check_bonus_earth_sea_graphics( x_grid , y_grid ) ;
#define		CHEMICAL_HIDDEN_BONUS		check_bonus_chemical_graphics( x_grid , y_grid ) ;
#define		THORG_HIDDEN_BONUS			check_bonus_thorg_graphics( x_grid , y_grid ) ;
#define		SPACE_HIDDEN_BONUS			check_bonus_space_graphics( x_grid , y_grid ) ;
#define		DEATH_STAR_HIDDEN_BONUS		check_bonus_death_star_graphics( x_grid , y_grid ) ;

void explosion_check_this_graphic( long , long ) ;
void explosion_check_earth_land_graphics( long , long ) ;
void explosion_check_earth_sea_graphics( long , long ) ;
void explosion_check_chemical_graphics( long , long ) ;
void explosion_check_thorg_graphics( long , long ) ;
void explosion_check_space_graphics( long , long ) ;
void explosion_check_death_star_graphics( long , long ) ;

void explosion_next_frame_this_graphic( long , long ) ;
void explosion_next_frame_earth_land_graphics( long , long ) ;
void explosion_next_frame_earth_sea_graphics( long , long ) ;
void explosion_next_frame_chemical_graphics( long , long ) ;
void explosion_next_frame_thorg_graphics( long , long ) ;
void explosion_next_frame_death_star_graphics( long , long ) ;

void check_bonus_earth_land_graphics( long , long ) ;
void check_bonus_earth_sea_graphics( long , long ) ;
void check_bonus_chemical_graphics( long , long ) ;
void check_bonus_thorg_graphics( long , long ) ;
void check_bonus_space_graphics( long , long ) ;
void check_bonus_death_star_graphics( long , long ) ;

long test_display_static_graphics( long ) ;

long test_display_earth_land_graphics( long ) ;
long test_display_earth_sea_graphics( long ) ;
long test_display_chemical_graphics( long ) ;
long test_display_thorg_graphics( long ) ;
long test_display_space_graphics( long ) ;
long test_display_death_star_graphics( long ) ;

extern long which_graphics_set ;
extern char poly_map [ 128 ] [ 128 ] ;
extern char collision_map [ 128 ] [ 128 ] ;

#include "Misc_Struct.h"
extern bonus_crystal_object_struct bonus_crystal_object ;

#endif

