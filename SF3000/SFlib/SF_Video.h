/* File : Video Player header file */

#ifndef __SF_VIDEO
#define __SF_VIDEO

// Includes

#include "SF_ScreenUtils.h"
#include "PlayCPakStream.h"

// External Variables

extern	ScreenContext *screen;

// Function Prototypes

long video_play (char *video_file);					// Play a Cinepak video sequence
long video_internal_interrupt (PlayerPtr ctx);		// INTERNAL ROUTINE USED BY VIDEO_PLAY TO CHECK KEYPAD

#endif
