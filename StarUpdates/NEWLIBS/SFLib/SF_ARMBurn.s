		EXPORT	armburn_addexplosion, armburn_resetexplosions, armburn_updateexplosions, armburn_initialise
		IMPORT	sprite_map
		IMPORT	height_map
		
		AREA	|c$$code|,CODE,READONLY

; ---------------------------------------------------------------------------------------------------

; SF_ARMBURN

; Routines for generating and updating ground / sea sprite explosions on the map

; armburn_addexplosion -

; Passed a pointer to the map, x,y position and explosion size (0-3), checks to see if there is room
; in the explosion list for it to be added. If it can, It will then be updated by
; armburn_updateexplosion next time it's called. Explosions data is held in the 'info.<planet> file'


; armburn_resetexplosions -

; Clears the 'ongoing explosions' block to zeros, stopping all explosions


; armburn_updateexplosions -

; This routine updates all explosions in the active list, plotting explosion sprites on to the map,
; and terminating them when number of animation frames has expired.


; armburn_initialise -

; Called when a new planet-information & sprite file is loaded, it copies the data on what sprites
; to use for explosions etc. into its own private workspace for use.

; ---------------------------------------------------------------------------------------------------
;
; Format of ground animation data is
;
; +0 - [No. of current explosions]
; +4 - [Table]
;
; Table format is :
;
; +0 		- Number of frames before sprite is updated						- 255 is dead explosion
; +1 		- Next sprite number [0 .. 3] to overlay when updated
; +2 		- Sprite that USED to be on map before explosion was started
; +3 		- Offset to sprite data to use [+0 - Land, +4 - Sea]
; +4 - +7	- Map address of explosion piece (0-65535)
	
;*******************************
;Variable lookup table
;*******************************

armburn_spritemap
	DCD	sprite_map
	
armburn_heightmap
	DCD height_map


;*******************************
armburn_resetexplosions		ROUT
;*******************************

; Call this to clear all ongoing explosions (at start of level etc.)

; ----------

; Receives :	Nothing
; Returns :		Nothing
; Corrupts : 	r0

; ----------

	mov r0,#0
	str r0,armburn_currentexplosions
	movs pc,r14
	
	
;*******************************
armburn_addexplosion		ROUT
;*******************************

; Call to add a Ground  / Sea sprite explosion

; ----------

; Receives :	r0 = X map position of explosion
;				r1 = Y map position of explosion
;				r2 = size 0) -> 1 square  (small)
;						  1) -> 3 Cross   (Med 1)
;						  2) -> 5 Diamond (Med 2)
;						  3) -> 5 round   (Large)

; Returns :		Nothing

; Corrupts : 	r0,r1,r2,r3

; ----------

	stmfd sp!,{r4-r11, r14}
	
	adrl r8,armburn_explosionsprites	; Start of explosion sprites data
	
	adr r5,armburn_explosions			; Explosion offset data
	and r3,r2,#3						; Clip explosion size at max
	
	add r2,r0,r1,asl#8					; Actual map address of explosion
	
	ldr r0,armburn_spritemap			; Get address of sprite map
	ldr r1,armburn_heightmap			; Get address of height map
	
	ldr r6,[r5,r3,asl#2]				; Get offset to explosion data
	add r5,r5,r6						; R5 now points to start of explosion size data
	
	mov r4,#65536
	sub r4,r4,#1						; Map mask and value

	ldr r6,armburn_currentexplosions	; Number of active explosions
	adrl r7,armburn_currentexplosionsdata
	add r7,r7,r6,asl #3					; Current store address for explosions data

; ----------

armburn_addexplosionloop

	cmp r6,#508
	strge r6,armburn_currentexplosions
	ldmgefd sp!,{r4-r11, pc}^			; No room left for explosions - Return
	
	ldmia r5!,{r9,r10}					; Get explosion data 	- r9	= Map offset
										;						- r10	= Timer offset
																
	cmp r9,#65536						; Have we finished adding explosions ?
	streq r6,armburn_currentexplosions
	ldmeqfd sp!,{r4-r11, pc}^			; Yes, return
	
										; We are now adding an explosion piece
	
	add r6,r6,#1						; Inc no. of explosions
	add r9,r9,r2						; Get correct position on the map
	and r9,r9,r4						; Clip if necessary
	
	strb r10,[r7],#1					; Store timer 								[+0]
	mov r11,#0
	strb r11,[r7],#1					; Store next sprite number					[+1]
	
	ldrb r10,[r0,r9]
	strb r10,[r7],#2					; Store sprite that used to be there		[+2]
	
	str r9,[r7],#4						; Store map offset address					[+4 .. +7]
		
	ldrb r11,armburn_doheightcheck
	cmp r11,#0
	beq armburn_addrandomexplosiontype	; Pick a random type, 0 or 1
	
	mov r11, #0							; Set to land explosion by default
	
	stmfd sp!, {r6, r7, r8}
	
	mov r8,#65536
	sub r8, r8, #1
	
	ldrb r6,[r1,r9]						; Get height of this sprite & check - FOR SPRITE @ 0,0
	cmp r6,#17							; IF HEIGHT IS <17, USE SEA EXPLOSIONS,		Offset 8
	movlt r11,#8

	addge r7, r9, #1					; Get height of this sprite & check - FOR SPRITE @ 1,0
	andge r7, r7, r8
	ldrgeb r6,[r1,r7]
	cmpge r6,#17
	movlt r11,#8
	
	addge r7, r9, #256					; Get height of this sprite & check - FOR SPRITE @ 0,1
	andge r7, r7, r8
	ldrgeb r6,[r1,r7]
	cmpge r6,#17
	movlt r11,#8
	
	addge r7, r9, #256
	addge r7, r7, #1					; Get height of this sprite & check - FOR SPRITE @ 1,1
	andge r7, r7, r8
	ldrgeb r6,[r1,r7]
	cmpge r6,#17
	movlt r11,#8
	
	ldmfd sp!, {r6, r7, r8}
	
armburn_registerinit

	strb r11,[r7,#-5]					; Store sprites offset to use				[+3]	

armburn_checkforongoingexplosion
										; Check to see that we are not doing an explosion on an
										; ongoing explosion, otherwise the sprite-that-used-to-be
										; -there for reset will be wrong
	
	add r9,r8,r11						; Point at the sprite set
	ldrb r11,[r9,#0]					; Check the 1st sprite
	cmp r10,r11
	ldrneb r11,[r9,#1]					; Check the 2nd sprite
	cmpne r10,r11
	ldrneb r11,[r9,#2]					; Check the 3rd sprite
	cmpne r10,r11
	ldrneb r11,[r9,#3]					; Check the 4th sprite
	cmpne r10,r11	
	bne armburn_addexplosionloop		; There is no active explosion, carry on
	
	sub r6,r6,#1
	sub r7,r7,#8
	b armburn_addexplosionloop			; Take out explosion, and carry on

; ----------

armburn_addrandomexplosiontype

	ldrb r11,[r0,r2]					; Get existing sprite on map
	tst r11,#1
	moveq r11,#0						; Explosion type 0, or 1 ?
	movne r11,#8
	b armburn_registerinit				; Continue


;*******************************
armburn_updateexplosions	ROUT
;*******************************


; Updates all ongoing explosions, terminating them at end

; ----------

; Receives :	r0 = Sprite Map address
;				r1 = Cel_quad data block address
				
; Returns :		Nothing

; Corrupts : 	r1,r2,r3

; ----------

	ldr r2,armburn_currentexplosions
	cmp r2,#0
	moveqs pc,r14
	
	stmfd sp!,{r4-r12, r14}
	
	ldr r12,[r1,#52]						; Get address of 4x4 sprites
	ldr r11,[r1,#68]						; Get address of 512x512 area map
	ldr r9,[r1,#76]							; Get address of cache block
	
	mov r1,r2
	adr r2,armburn_currentexplosionsdata
	adr r3,armburn_explosionsprites
		
	mov r8,#0								; Clear active-found

armburn_updateexplosionsregister

	cmp r8,#0								; Not Found any live ones yet ?
	streq r1,armburn_currentexplosions		; Reset number of explosions back

armburn_updateexplosionsloop

	subs r1,r1,#1							; Get next current explosion
	ldmmifd sp!,{r4-r12,pc}^				; Quit if finished
	
; ----------

	add r4,r2,r1, asl#3						; Get address of current explosion

	ldrb r5,[r4,#0]							; Timer counter
	
	cmp r5,#255								; Is it a dead explosion ?
	beq armburn_updateexplosionsregister	; Yes - Go find next
	
	mov r8,#1								; Mark live one found
	subs r5,r5,#1							; Update live explosion
	movmi r5,#1
	strb r5,[r4,#0]
	bpl armburn_updateexplosionsloop		; No update required
	
; ----------

	ldr r7,[r4,#4]							; Get map offset (0-65535)
	and r6,r7,#255							; X map position
	mov r10,r7, asr #8						; Y map position
	mov r10,r10, asl #10
	add r5,r10,r6, asl#1					; R5 is now position in 512x512 map
	mov r10,#48
	mul r10, r5, r10						; Turn this offset to 6bpp ref
	mov r10, r10, asr#3
	
	ldrb r5,[r4,#1]							; Get next sprite number to overlay on map
	cmp r5,#4								; Should we be picking random one to end up on this go ?
	
	eorge r5,r6,r4, asr#3					; YES - pick random sprite (4, 5, 6 or 7) and terminate it
	eorge r5,r5,r7, asr#8
	andge r5,r5,#3
	addge r5,r5,#4
	movge r6,#255
	strgeb r6,[r4,#0]
	
	ldrb r6,[r4,#3]							; Get offset reqd
	add r6,r5,r6
	ldrb r6,[r3,r6]							; Get actual sprite

	addlt r5,r5,#1							; NO - inc counter and re-store
	strltb r5,[r4,#1]
	
	cmp r6,#255
	ldreqb r6,[r4,#2]						; Are we using original sprite ?
	
	strb r6,[r0,r7]							; UPDATE EXPLOSION ON THE BIG MAP

	add r6,r12,r6,asl#4						; Points at start of 4x4sprite

; ----------
	
	stmfd sp!,{r0-r1}						; Store registers on stack for update
	
	add r14, r11, r10, asr#3				; R14 is the byte address in the 512x512 map
	and r10, r10, #7						; R10 is the bit offset
	rsb r10, r10, #7
	mov r5, #1
	mov r10, r5, asl r10					; R10 is the bit offset number (0, 1, 2, 4, 8 etc.)
	
	stmfd sp!,{r10, r14}
	
	ldr r5, [r6, #0]						; (0,0) PIXEL FROM SPRITE
	mov r5, r5, lsr#26						; Get coded6 pixel1 in r5
	
	ldrb r1, [r14, #0]						; Get byte from 512x512 map
	mov r0, #32

; ----------

armburn_updatepixel1loop	
	
	tst r5, r0
	biceq r1, r1, r10
	orrne r1, r1, r10

	mov r10, r10, asr#1
	cmp r10,#0
	streqb r1, [r14], #1
	ldreqb r1, [r14, #0]
	moveq r10, #128
	
	mov r0, r0, asr#1
	cmp r0, #0
	bgt armburn_updatepixel1loop
	
; ----------

	ldr r5, [r6, #0]						; (1,0) PIXEL FROM SPRITE
	mov r5, r5, lsr#14
	and r5, r5, #63							; Get coded6 pixel2 in r5
	mov r0, #32

; ----------

armburn_updatepixel2loop	

	tst r5, r0
	biceq r1, r1, r10
	orrne r1, r1, r10

	mov r10, r10, asr#1
	cmp r10,#0
	streqb r1, [r14], #1
	ldreqb r1, [r14, #0]
	moveq r10, #128
	
	mov r0, r0, asr#1
	cmp r0, #0
	bgt armburn_updatepixel2loop
	
	strb r1, [r14]
	
; ----------

	ldmfd sp!,{r10, r14}
	add r14, r14, #384
	
	ldr r5, [r6, #8]						; (0,1) PIXEL FROM SPRITE
	mov r5, r5, lsr#26						; Get coded6 pixel3 in r5
	
	ldrb r1, [r14, #0]						; Get byte from 512x512 map
	mov r0, #32

; ----------

armburn_updatepixel3loop	
	
	tst r5, r0
	biceq r1, r1, r10
	orrne r1, r1, r10

	mov r10, r10, asr#1
	cmp r10,#0
	streqb r1, [r14], #1
	ldreqb r1, [r14, #0]
	moveq r10, #128
	
	mov r0, r0, asr#1
	cmp r0, #0
	bgt armburn_updatepixel3loop
	
; ----------

	ldr r5, [r6, #8]						; (1,1) PIXEL FROM SPRITE
	mov r5, r5, lsr#14
	and r5, r5, #63							; Get coded6 pixel4 in r5
	mov r0, #32

; ----------

armburn_updatepixel4loop	

	tst r5, r0
	biceq r1, r1, r10
	orrne r1, r1, r10

	mov r10, r10, asr#1
	cmp r10,#0
	streqb r1, [r14], #1
	ldreqb r1, [r14, #0]
	moveq r10, #128
	
	mov r0, r0, asr#1
	cmp r0, #0
	bgt armburn_updatepixel4loop
	
	strb r1, [r14]
		
	ldmfd sp!,{r0-r1}						; Restore registers from stack
	
	
	mov r6,r7,asr#1							; Clear reference in cache so it gets reset (if necessary)
	and r6,r6,#2_01111110
	mov r10,r7,asr#10
	add r10,r9,r10, asl#7
	
	ldrb r7,[r10,r6]						; Is this segment registered in the cache ?
	cmp r7,#0
	strgtb r8,[r10,r6]						; Yes, mark it to be cleared in cache update
	
	b armburn_updateexplosionsloop			; Update next explosion in the list


;*******************************
armburn_initialise			ROUT
;*******************************

; Called when a new sprite & planet info file is loaded, copies data on explosion sprites into
; private workspace

; ----------

; Receives :	r0 = base address of planet info data

; Returns :		Nothing

; Corrupts : 	Nothing

; ----------

	stmfd sp!,{r4-r9}
	
	ldmia r0,{r5-r6, r7-r8, r9}			; r5, r6 = 8 ground explosion1 sprites (in bytes)
	adr r4,armburn_explosionsprites		; r7, r8 = 8 ground explosion1 sprites (in bytes)
	stmia r4,{r5-r6, r7-r8, r9}			; r9 = Explosion update flags for planet
	ldmfd sp!,{r4-r9}
	movs pc,r14

; ---------------------------------------------------------------------------------------------------
; Explosion data
; ---------------------------------------------------------------------------------------------------

armburn_explosions				; Offsets to start of x,y pos's for 4 explosion sizes

	DCD armburn_spxdata1-armburn_explosions
	DCD armburn_spxdata2-armburn_explosions
	DCD armburn_spxdata3-armburn_explosions
	DCD armburn_spxdata4-armburn_explosions

; Explosion1
; ----------

armburn_spxdata1				; Offset positions and timings for explosion size 1
	DCD 0
	DCD 0
	DCD 65536
	DCD 65536					; END MARKER

; ----------
; Explosion2
; ----------

armburn_spxdata2				; Offset positions and timings for explosion size 2
	DCD 256
	DCD 0
	
	DCD -1
	DCD 0
	
	DCD 1
	DCD 1
	
	DCD -256
	DCD 0
	
	DCD 0
	DCD 1
	
	DCD 65536
	DCD 65536					; END MARKER

; ----------
; Explosion3
; ----------

armburn_spxdata3				; Offset positions and timings for explosion size 3
	DCD 512
	DCD 3
	
	DCD -2
	DCD 3
	
	DCD 2
	DCD 3
	
	DCD -512
	DCD 3
	
	DCD 255
	DCD 2
	
	DCD 257
	DCD 2
	
	DCD -257
	DCD 2
	
	DCD -255
	DCD 2
	
	DCD 256
	DCD 1
	
	DCD -1
	DCD 1
	
	DCD 1
	DCD 1
	
	DCD -256
	DCD 1
	
	DCD 0
	DCD 0
	
	DCD 65536
	DCD 65536					; END MARKER

; -----------
; Explosion 4
; ----------

armburn_spxdata4			; Offset positions and timings for explosion size 4
	DCD 511
	DCD 4
	
	DCD 513
	DCD 4
	
	DCD 254
	DCD 4
	
	DCD 258
	DCD 4
	
	DCD -258
	DCD 4
	
	DCD -254
	DCD 4
	
	DCD -513
	DCD 4
	
	DCD -511
	DCD 4
	
	DCD 512
	DCD 3
	
	DCD -2
	DCD 3
	
	DCD 2
	DCD 3
	
	DCD -512
	DCD 3
	
	DCD 255
	DCD 2
	
	DCD 257
	DCD 2
	
	DCD -257
	DCD 2
	
	DCD -255
	DCD 2
	
	DCD 256
	DCD 1
	
	DCD -1
	DCD 1
	
	DCD 1
	DCD 1
	
	DCD -256
	DCD 1
	
	DCD 0
	DCD 0
	
	DCD 65536
	DCD 65536					; END MARKER

; ----------

armburn_explosionsprites		; Explosion sprites for ground / sea

armburn_groundsprites			; Ground
	DCD 0						; 8 sprites, 1 byte each, 4 for explosion, 4 to randomly end up on
	DCD 0
	
armburn_seasprites				; Sea
	DCD 0						; 8 sprites, 1 byte each, 4 for explosion, 4 to randomly end up on
	DCD 0
	
armburn_doheightcheck			; Explosion flags from info file
	DCB 0						; 0 is pick randomly between explosions, 1 is do height check to decide

armburn_free1
	DCB 0

armburn_free2
	DCB 0

armburn_free3
	DCB 0
	
; ----------

armburn_currentexplosions		; Current explosion markers [Max 8 explosions], 0 = Empty, >0 = Used
	DCD 0

; ----------

armburn_currentexplosionsdata	; 8 bytes per explosion piece - max of 512 explosion pieces
								
	% 512*8
	
	END
	