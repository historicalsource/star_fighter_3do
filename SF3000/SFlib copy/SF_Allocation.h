//	File : Allocation definitions header file

#ifndef __SF_ALLOCATION
#define __SF_ALLOCATION


/* Directory structures */

#define RESOURCES_ROOT	"$boot/SF_Resources/"	// Main resources directory

#define CEL_ROOT		"Cels/"					// For cel files
#define MIS_ROOT		"Missions/"				// For mission & map files
#define SKY_ROOT		"Sky/"					// For sky files
#define GRF_ROOT		"Graphics/"				// For 3d graphics files
#define IMG_ROOT		"Images/"				// For 320x240 image files
#define INF_ROOT		"Info/"					// For planet information files
#define DAT_ROOT		"Data/"					// For internal game data files
#define	PAL_ROOT		"Palettes/"				// For Coded Planet Palettes
#define	TEXT_ROOT		"Text/"					// For in-game / menu texts (language independant)
#define FONT_ROOT 		"Fonts/"				// Filepath of fonts
#define ANIM_ROOT		"Animations/"			// For animation files
#define MUSIC_ROOT		"Music/"				// Filepath of music
#define	VOICE_ROOT		"Voices/"				// Filepath for voice overs
#define VIDEO_ROOT		"Video/"				// Filepath of video files
#define SAMPLE_ROOT		"Samples/"				// Filepath of samples


/* Maximum array allocations */

#define	MAX_MENUS			21		// Max number of menus available
#define	MAX_GRAPHICPLOT		1024	// Max number of graphics that can be added

#define FONT_MAXAVAILABLE	2		// Max numbers of fonts that can be loaded in
#define FONT_MAXMENU		20		// Max number of menutext cels allowed
#define FONT_MAXMESSAGE		8		// Max number of game messages allowed

#define CEL_MAX32			84		// Max Number of K for 32x32 object textures
#define CEL_MAX16  			40		// Max Number of K for 16x16 landscape textures
#define CEL_MAX4			210		// Max Number of 4x4 landscape textures
#define CEL_MAXTEMP			1024	// Max Number of temporary cels per frame - also in ARMCell.s !
#define CEL_MAXCREATION		256		// Max Number of 'created' cel space
#define CEL_MAXGAME			29		// Ammount of memory to allocate to fixed game sprites in K

#endif
