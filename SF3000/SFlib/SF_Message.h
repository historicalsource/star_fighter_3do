/* File : Message header file */

#ifndef __SF_MESSAGE
#define __SF_MESSAGE

// Includes

#include "SF_Status.h"
#include "SF_Allocation.h"
#include "Textlib.h"
#include "Camera_Struct.h"

/***************************************************************************************************/
/*									 	  DEFINITIONS FOR MENU TEXTS							   */
/***************************************************************************************************/

#define	MMNU0				1
#define	MMNU1				7
#define	MMNU2				14
#define	MMNU2_FORMATION		129
#define	MMNU3				20
#define	MMNU4				26
#define	MMNU4_LOOKAT		30
#define	MMNU5				32
#define	MMNU5_MOVE_CAM		39
#define	MMNU5_EXTERNAL_CAM	40
#define	MMNU6				41
#define	MMNU7				42
#define	MMNU8				43
#define	MMNU9				44
#define	MMNU10				50
#define	MMNU11				57
#define	MMNU11_NEW			148
#define	MMNU12				66
#define	MMNU13				75
#define	MMNU14				84
#define	MMNU15				90
#define	MMNU16				96
#define	MMNU16_INFO			130
#define	MMNU17				99
#define	MMNU18				103
#define	MMNU19				104
#define	MMNU19_CONTROLS		114
#define	MMNU19_EXITS		123
#define	MMNU20				125
#define	MMNU20_MODE			126
#define MMNU_GETNAME		135
#define	MMNU_CHEATS			138
#define	MMNU_USER			151

/***************************************************************************************************/
/*									 	 DEFINITIONS FOR INGAME TEXTS							   */
/***************************************************************************************************/

// Definitions

// GENERAL IN-GAME MESSAGES

#define MTXT__WARNING_INCOMING_SAM					0
#define MTXT__WARNING_INCOMING_ATAM					1
#define MTXT__ENEMY_SHIP_LAUNCHED					2
#define MTXT__ENEMY_FIGHTER_DESTROYED				3
#define MTXT__MOTHERSHIP_DESTROYED					4
#define MTXT__DEFENCE_TARGET_DESTROYED				5
#define MTXT__MISSION_SHIP_DESTROYED				6
#define MTXT__YOU_HAVE_FAILED_YOUR_MISSION			7
#define MTXT__YOU_HAVE_RUN_OUT_OF_TIME				8
#define MTXT__PARACHUTE_DROP_DETECTED				9
#define MTXT__ENEMY_DEFENCES_DOWN					10
#define MTXT__STRATEGIC_TARGET_DESTROYED			11
#define MTXT__STRATEGIC_COMMAND_OVERRUN				12
#define MTXT__DEFENCES_ACTIVE						13
#define MTXT__DOCKING_SUCCESSFUL					14
#define MTXT__MISSION_COMPLETED						15
#define MTXT__RETURN_TO_MOTHERSHIP					16
#define MTXT__GAME_OVER								17
#define MTXT__MISSION_TARGET_DESTROYED				18
#define MTXT__SECRET_MISSION_TARGET_DESTROYED		19
#define MTXT__MESSAGE_FROM							64
#define	MTXT__STRATEGIC_COMMAND						69

// SHIP DESCRIPTIONS

#define MTXT__STARFIGHTER							20
#define MTXT__MOTHERSHIP							21
#define MTXT__STRATOLINER							22
#define MTXT__FIGHTER								23
#define MTXT__SATELLITE								24
#define MTXT__VEHICLE								47
#define MTXT__TANK									48
#define MTXT__ATTACKING								25
#define MTXT__PATROL								26
#define MTXT__FORMATION_WITH						27
#define MTXT__KILL									30
#define MTXT__DEFEND								31
#define MTXT__IMMATERIAL							46
#define MTXT__MULTIPLE_TARGETS						49

// GAME BONUSES AND AWARDS

#define MTXT__PARACHUTE_COLLECTED					32
#define MTXT__CRYSTAL_AWARD							33
#define MTXT__WINGPOD_ADDED							34
#define MTXT__EXTRA_LIFE							35
#define MTXT__MEGASHIP_AWARDED						36
#define MTXT__MEGABOMBS_AWARDED						37
#define MTXT__BEAM_LASERS_AWARDED					38
#define MTXT__MULTIMISSILE_AWARDED					39
#define MTXT__MINES_AWARDED							40
#define	MTXT__UPGRADE								41
#define	MTXT__DAMAGED								42
#define	MTXT__ENGINE								43
#define	MTXT__CONTROL								44
#define	MTXT__SHIELDS								45
#define	MTXT__ECM_AWARDED							63
#define	MTXT__SCANNER_FREE							62

// FLIGHT CONTROL MESSAGES

#define MTXT__ENEMY_VESSEL_HAS_LANDED				50
#define MTXT__FRIENDLY_VESSEL_HAS_LANDED			51
#define MTXT__REQUEST_TERMINATION_OF_LEADER			52
#define MTXT__WARNING_FRIENDLY_FIRE					53
#define MTXT__WARNING_MOTHERSHIP_SHIELDS_LOW		54
#define MTXT__WARNING_MOTHERSHIP_UNDER_ATTACK		55
#define MTXT__MAYDAY_MAYDAY_REQUEST_ASSISTANCE		56
#define MTXT__FORMATION_UNDER_FIRE					57
#define MTXT__UNDER_FIRE_BREAKING_FORMATION			58
#define MTXT__GOING_IN_TO_DEFEND_FORMATION			59
#define MTXT__ENEMY_RADAR_LOCK_DETECTED				60
#define MTXT__REQUEST_TO_JOIN_FORMATION				61
#define MTXT__LOST_IN_DUTY							65

// GENERAL

#define MTXT__TRAINING								70
#define MTXT__EASY									71
#define MTXT__MEDIUM								72
#define MTXT__HARD									73
#define	MTXT__LIVES									74
#define	MTXT__HIGHSCORE								28
#define	MTXT__DEMOTEXTSTART							75

// WEAPONS

#define MTXT__LASER									80
#define MTXT__ATG									81
#define MTXT__ATA									82
#define MTXT__MEGABOMB								83
#define MTXT__BEAM_LASER							84
#define MTXT__MULTI_MISSILE							85
#define MTXT__MINES									86
#define	MTXT__ECM									87
#define	MTXT__ECM_FIRED								67
#define	MTXT__REMAINING								68
#define	MTXT__NO_ECMS								29

// CAMERA STUFF

#define MTXT__PLAYER								90
#define MTXT__TEAM									91
#define MTXT__ENEMY									92
#define MTXT__WEAPON								93
#define MTXT__AVAILABLE								95
#define MTXT__NO									96
#define MTXT__VIEW									97
#define MTXT__TRACKING								98
#define MTXT__FLYBY									99
#define MTXT__VIEW_FROM								100

// Definitions

#define MESSAGE_SCORE		0					// Message types (where it appears on the screen)
#define	MESSAGE_WEAPON		1
#define	MESSAGE_TIMER		2
#define	MESSAGE_TOP			3
#define MESSAGE_BOTTOM		4
#define	MESSAGE_BIG			5
#define	MESSAGE_CAM			6
#define	MESSAGE_NAME		7
#define	MESSAGE_DEMO		8

#define	MESSAGE_FLASH		1					// Bit Flags for messages  (flash, scroll etc.)
#define MESSAGE_SCROLL		2

#define	MESSAGE_NOCURSOR	0					// Cursor types for messages
#define	MESSAGE_CURSOR1		1
#define	MESSAGE_CURSOR2		2


typedef struct	message							// Game text message
				{
				long	message_id,
						timer,
						flags,
						cursor,
						delay,
						scroll_speed;
				TextCel	*textcel;
				}	message;
				
// External Definitions

extern	message			messages [FONT_MAXMESSAGE];
extern	game_status		status;
extern	camera_data		camera [MAX_CAMERAS];
extern	char			*pilot_names [16];

// Function Prototypes

void	message_initialise (long);															// Reset all messages	
void	message_pilot (long, long, long);													// Display a message from a pilot
void	message_add	(long,	long, long, long, long, long);									// Display a game message of given type
void	message_text (long,	char*, long, long, long, long);									// Display game text
void	message_addxy (long, char*, long, long, long, long, long, long, long, long, long);	// Display a game message at X,Y
void	message_update (void);																// Updates all messages & timers

void	message_addscore (long);															// Adds to players score & updates score cel
void	message_addtimer (void);															// Sets mission time left message
void	message_addweapon (void);															// Sets weapons left message
void	message_addcamera (long);															// Displays camera currently used

char*	message_decode (long, long);														// Get message from number
long	message_initialisemissionfiles (long, long, long);									// Load in mission text message
void	message_initialiselanguagefiles (long);												// Load in language message / menu files
long	message_decodetextfile (char *, char **, long);										// Decode a text file

#endif

