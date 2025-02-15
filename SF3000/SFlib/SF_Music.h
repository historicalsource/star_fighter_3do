/* File : Music Player header file */

#ifndef __SF_MUSIC
#define __SF_MUSIC


// Definitions

#define	MUSIC_PLAYLISTLENGTH	16				// Max number of tracks that can be programmed
#define	MUSIC_PLAYFROMTRACK		0x1				// Music track control commands
#define	MUSIC_MOVETRACK			0x2

// Function Prototypes

long music_initialise (long);					// INITIALISE MUSIC PLAYER
void music_terminate (void);					// TERMINATE MUSIC PLAYER


												// MAIN MUSIC CONTROL ROUTINES
												
void music_resetplaylist (void);				// Reset the playlist
void music_addtoplaylist (long);				// Add a track to the playlist
void music_takefromplaylist (long);				// Take a track out of the play list

void music_play (long, long, long);				// Play a track (or entire track list) with optional repeat
void music_stop (void);							// Stop music from playing
void music_pause (void);						// Pause a track
void music_resume (void);						// Resume a track
void music_maketime (void);						// Wait for buffer fill (to give max time for other loading)
void music_setmastervolume (long);				// Set new volume for playback
long music_query (void);						// Returns current status of player
void music_playvoice (char *,...);				// Start streaming of voice over (stop music if necessary)

												// INTERNAL CONTROL ROUTINES
												
void music_thread (void);						// Music Playing Task
void music_command (int32);						// INTERNAL - Execute command
void music_trackgetnext (long);					// INTERNAL - Play next / previous track
void music_loadin (long);						// INTERNAL - Load in music

// external variables

extern	Item	parent_taskref;					// Task ref of parent

#endif

