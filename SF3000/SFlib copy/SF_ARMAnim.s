; EXPORT	armanim_initialise, armanim_update [TAKEN OUT]

		EXPORT	armtex_initialise, armtex_reset, armtex_updateall, armtex_update
		AREA	|c$$code|,CODE,READONLY

; ---------------------------------------------------------------------------------------------------

; SF_ARMANIM

; Contains routines for :	1) Animating sprites on the map (data for these comes from the missions file)
;							2) Cycling object texture references (ie. players engine etc.)


; ---------------------------------------------------------------------------------------------------

; MAP ANIMATION ROUTINES

; armanim_initialise -  [TAKEN OUT]

; Passed a pointer to the animations data, resets all animations to active, and sets their frame
; counters to zero so they are updated next frame.


; armanim_update -  [TAKEN OUT]

; Passed a pointer to the animations data, and the map, updates all active animations frame
; counters, when ready for update, resets counter and cycles to the next sprite

; ----------

; MISSION ANIMATIONS DATA FORMAT
; +0 [Word] - Number of animations held (upto 128)
; +4 Individual animations data [8 Bytes each]

; INDIVIDUAL ANIMATION FORMAT
; +0 Current frame counter (dec every frame, when <0, update animation)	[IF 255, Anim is inactive]
; +1 Frame reset counter (value to reset frame counter to when <0)
; +2 X pos of animation on map
; +3 Y pos of animation on map

; +4 - +7, Animation sprites 1 to 4	:	A Value of 255 prevents update for that frame

; ---------------------------------------------------------------------------------------------------

; TEXTURE ANIMATION ROUTINES

; armtex_initialise -

; passed a pointer to the texture animations file, and to the (newly loaded) .32 texture file,
; converts all sprite references in the animations file to actual memory addresses.


; armtex_updateall -

; passed a pointer to the texture animations file and the .32 texture file, all animations marked
; as active (automatic) are updated FORWARDS


; armtex_update -

; as armtex_updateall, but only updates 1 particular animation, this can be updated BACKWARDS or
; forwards

; ----------

; TEXTURE ANIMATIONS DATA FORMAT
; +0 [Word] - Number of animations held
; +4		-> OFFSET TO ANIMATION 1 (FROM +4)
; +8		-> OFFSET TO ANIMATION 2 (FROM +4)
; +12		-> OFFSET TO ANIMATION 3 etc.

; INDIVIDUAL ANIMATION FORMAT
; +0  - Status of animation (0=inactive, 1=active)
; +4  - Current Frame Countdown
; +8  - Frame count restore
; +12 - No of sprites that are being animated
; +16 - In-Sprite animation count						(Current sprite number)
; +20 - In-Sprite animation restore						(Max sprites in animation)
; +24 - In-Sprite loop restore							(Number to go back to in list when > max)

; +28 - FOR EACH SPRITE :
;		+0	- Sprite number to animate
;		+4	- Sprite number to change to (1)
;		+8	- Sprite number to change to (2)
;		+12	- Sprite number to change to (3)
;		+16 - ..... until max number of animation sprites

; ---------------------------------------------------------------------------------------------------
; NOTE : GROUND ANIMATIONS HAVE BEEN TAKEN OUT [16/8/95]
; ---------------------------------------------------------------------------------------------------

;*******************************
;armanim_initialise			ROUT
;*******************************

; Call to reset all animations in list to active, and ready for update next frame

; ----------

; Receives :	r0 = Address of animations data

; Returns :		Nothing

; Corrupts : 	r0,r1,r2

; ----------

;	ldr r1,[r0],#4						; Get number of animations and move to 1st anim
;	mov r2,#0
	
;armanim_initialiseloop

;	subs r1,r1,#1						; End of animations ?
;	movmis pc,r14						; Yes, return
	
;	strb r2,[r0],#8						; Clear frame counter & Move onto next animation
;	b armanim_initialiseloop
	

;*******************************
;armanim_update				ROUT
;*******************************

; Call this to update all animations
; ----------

; Receives :	r0 = Address of animations data
;				r1 = Address of map

; Returns :		Nothing

; Corrupts : 	r0,r1,r2,r3

; ----------

;	stmfd sp!,{r4,r5,r14}
;	ldr r2,[r0],#-4						; Get number of animations
	
;armanim_updateloop

;	subs r2,r2,#1						; End of animations ?
;	ldmmifd sp!,{r4,r5,pc}^				; Yes, return
	
;	add r0,r0,#8						; Get next animation

;	ldrb r3,[r0,#0]						; Get frame counter
;	cmp r3,#255							; Is animation INACTIVE (if frm ctr = 255)
;	beq armanim_updateloop				; Yes, get next
	
; ----------

;	subs r3,r3,#1
;	ldrmib r3,[r0,#1]					; If <0, reset to next frame wait value
;	strb r3,[r0,#0]						; Store frame counter
;	bpl armanim_updateloop				; If not updating, get next animation
	
; ----------

;	ldr r3,[r0,#4]						; Get 4 animation sprites
;	mov r3,r3,ror #8					; Cycle animations round 1
;	str r3,[r0,#4]						; Store shifted animations
	
;	and r3,r3,#255						; Get the sprite we are going to store
;	cmp r3,#255							; Is it a blank (255 ?)
;	beq armanim_updateloop				; Yes, update next

; ----------

;	ldrb r4,[r0,#2]						; X position of anim
;	ldrb r5,[r0,#3]						; Y position of anim
;	add r4,r4,r5,asl#8					; actual position of anim
;	strb r3,[r1,r4]						; store new animation sprite
;	b armanim_updateloop				; update next

; ---------------------------------------------------------------------------------------------------

;*******************************
armtex_initialise			ROUT
;*******************************

; Call to setup all sprite references to texture addresses

; ----------

; Receives :	r0 = pointer to animations file
;				r1 = pointer to .32 texture file

; Returns :		Nothing

; Corrupts : 	r0,r1,r2,r3

; ----------

	stmfd sp!,{r4-r7}
	ldr r2,[r0],#4				; Get number of animations in the file, r0 now points to the base offset list
	
armtex_initialise_animationloop

	subs r2,r2,#1
	ldmmifd sp!,{r4-r7}			
	movmis pc,r14				; Finished ? Return
	
	ldr r3,[r0,r2, asl#2]		; Get offset to animation
	add r3,r3,r0				; r3 now points at animation to reset
	ldr r4,[r3,#12]				; r4 is number of sprites to animate
	ldr r5,[r3,#20]				; r5 is the number of animations per sprite
	
	add r3,r3,#28				; r3 now points at the sprite animations

; ----------

armtex_initialisesingleanimationloop

	subs r4,r4,#1
	bmi armtex_initialise_animationloop
	mov r6, r5
	
	ldr r7, [r3]				; The sprite we are animating - convert its address
	add r7,r1,r7,asl#2			; Get it's address in the offset list
	str r7, [r3],#4				; Store it

; ----------

armtex_initialisesingleanimationspriteloop

	subs r6,r6,#1
	bmi armtex_initialisesingleanimationloop
	
	ldr r7, [r3]				; The sprite we are animating - convert its address
	ldr r7, [r1, r7, asl#2]		; Get its offset in the .32 text file
	str r7, [r3],#4				; Store it
	b armtex_initialisesingleanimationspriteloop

; ----------

; TEXTURE ANIMATIONS DATA FORMAT
; +0 [Word] - Number of animations held
; +4		-> OFFSET TO ANIMATION 1 
; +8		-> OFFSET TO ANIMATION 2
; +12		-> OFFSET TO ANIMATION 3 etc.

; INDIVIDUAL ANIMATION FORMAT
; +0  - Status of animation (0=inactive, 1=active)
; +4  - Current Frame Countdown
; +8  - Frame count restore
; +12 - No of sprites that are being animated
; +16 - In-Sprite animation count						(Current sprite number)
; +20 - In-Sprite animation restore						(Max sprites in animation)
; +24 - In-Sprite loop restore							(Number to go back to in list when > max)

; +28 - FOR EACH SPRITE :
;		+0	- Sprite number to animate
;		+4	- Sprite number to change to (1)
;		+8	- Sprite number to change to (2)
;		+12	- Sprite number to change to (3)
;		+16 - ..... until max number of animation sprites

;*******************************
armtex_reset				ROUT
;*******************************

; Resets all active texture animations to 1st sprite in animation

; ----------

; Receives :	r0 = pointer to animations file

; Returns :		Nothing

; Corrupts : 	r0,r1,r2,r3

; ----------

	stmfd sp!,{r4-r7}
	ldr r2,[r0],#4				; Get number of animations in the file, r0 now points to the base offset list
	
armtex_reset_animationloop

	subs r2,r2,#1
	ldmmifd sp!,{r4-r7}		
	movmis pc,r14				; Finished ? Return
	
	ldr r3,[r0,r2, asl#2]		; Get offset to animation
	add r3,r0,r3				; r3 now points at animation to reset
	add r3,r3,#4
	
	ldmia r3!,{r4,r5}			; Yes - Update its frame counter - see if it needs animating
	str r5,[r3,#-8]

	ldmia r3!,{r4,r5,r6,r7}		; r4 is number of sprites to animate
								; r5 is the in-sprite count
								; r6 is the in-sprite max
								; r7 is the in-sprite loopback

	mov r5,#0					; Reset its counter
	str r5,[r3,#-12]			; Store new in-sprite counter
	
; ----------

armtex_resetsingleanimationloop

	subs r4,r4,#1							; out of sprites in this animation ?
	bmi armtex_reset_animationloop			; Yes, return
	
	ldr r7, [r3],#4							; Get The sprite we are animating
	ldr r1, [r3, r5, asl#2]					; Get the sprite it now is (1st for reset)
	str r1, [r7]							; Replace it in the lookup table of the .32's
	add r3, r3, r6, asl#2					; Go on to next sprite
	b armtex_resetsingleanimationloop


;*******************************
armtex_updateall			ROUT
;*******************************

; Updates all active texture animations FORWARDS

; ----------

; Receives :	r0 = pointer to animations file

; Returns :		Nothing

; Corrupts : 	r0,r1,r2,r3

; ----------

	stmfd sp!,{r4-r7}
	ldr r2,[r0],#4				; Get number of animations in the file, r0 now points to the base offset list
	
armtex_updateall_animationloop

	subs r2,r2,#1
	ldmmifd sp!,{r4-r7}		
	movmis pc,r14				; Finished ? Return
	
	ldr r3,[r0,r2, asl#2]		; Get offset to animation
	add r3,r0,r3				; r3 now points at animation to reset
		
	ldr r4,[r3],#4
	cmp r4,#0								; Is animation active ?
	beq armtex_updateall_animationloop		; No - Get next one
	
	ldmia r3!,{r4,r5}						; Yes - Update its frame counter - see if it needs animating
	subs r4,r4,#1
	strpl r4,[r3,#-8]
	strmi r5,[r3,#-8]
	bpl armtex_updateall_animationloop		; Frame counter is now <0, do not update
	
	ldmia r3!,{r4,r5,r6,r7}					; r4 is number of sprites to animate
											; r5 is the in-sprite count
											; r6 is the in-sprite max
											; r7 is the in-sprite loopback

	add r5,r5,#1							; Inc its counter
	cmp r5,r6								; Over max ?
	movge r5,r7								; Yes, back to loop point
	str r5,[r3,#-12]						; Store new in-sprite counter
	
; ----------

armtex_updateallsingleanimationloop

	subs r4,r4,#1							; out of sprites in this animation ?
	bmi armtex_updateall_animationloop		; Yes, return
	
	ldr r7, [r3],#4							; Get The sprite we are animating
	ldr r1, [r3, r5, asl#2]					; Get the sprite it now is , or was, or something
	str r1, [r7]							; Replace it in the lookup table of the .32's
	add r3, r3, r6, asl#2					; Go on to next sprite
	
	b armtex_updateallsingleanimationloop


;*******************************
armtex_update				ROUT
;*******************************

; Updates just 1 animation BACKWARDS or FORWARDS

; ----------

; Receives :	r0 = pointer to animations file
;				r1 = animation number to update
;				r2 = direction to move it (+1 FORWARDS, -1 BACKWARDS)

; Returns :		Nothing

; Corrupts : 	r0,r1,r2,r3

; ----------

	add r0,r0,#4
	ldr r1,[r0,r1, asl#2]			; Get offset to animation
	add r1,r0,r1					; r1 now points at animation to reset
	add r1,r1,#4

; ----------

	ldmia r1!,{r0,r3}			  	; Yes - Update its frame counter - see if it needs animating
	subs r0,r0,#1
	strpl r0,[r1,#-8]
	strmi r3,[r1,#-8]
	movpls pc,r14					; Frame counter is now <0, do not update

; ----------

	stmfd sp!,{r4-r5}
	ldmia r1!,{r0,r3,r4,r5}			; r0 is number of sprites to animate
									; r3 is the in-sprite count
									; r4 is the in-sprite max
									; r5 is the in-sprite loopback

	adds r3,r3,r2					; Adjust counter
	submis r3, r5, #1				; Gone off beginining ? - Then go to 1 before loop point
	movmi r3,#0						; Is this < 0, then put zero in
	cmp r3,r4						; Over max ?
	movge r3,r5						; Yes, back to loop point
	str r3,[r1,#-12]				; Store new in-sprite counter

armtex_updatesingleanimationloop

	subs r0,r0,#1
	ldmmifd sp!,{r4-r5}
	movmis pc,r14
	
	ldr r2, [r1],#4					; Get The sprite we are animating
	ldr r5, [r1, r3, asl#2]			; Get the sprite it now is , or was, or something
	str r5, [r2]					; Replace it in the lookup table of the .32's

	add r1,r1,r4, asl#2				; Get next sprite
	b armtex_updatesingleanimationloop
	
	END
	