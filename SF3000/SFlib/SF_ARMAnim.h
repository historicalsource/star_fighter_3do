//	File : SF_ARMAnim (Header file for ground animation routines)

#ifndef __SF_ARMANIM
#define __SF_ARMANIM
				
// Function Prototypes

//extern void armanim_initialise (void*);				// Reset all animations to active [TAKEN OUT]
//extern void armanim_update (void*, void*);			// Update each animation - CALLED EVERY FRAME [TAKEN OUT]
extern void armtex_initialise (void*, void*);			// Initialise texture animations
extern void armtex_reset (void*);						// Reset all animations
extern void armtex_updateall (void*);					// Update all active animations FORWARDS
extern void armtex_update (void*, long, long);			// Update a single animation BACKWARDS OR FORWARDS
#endif
