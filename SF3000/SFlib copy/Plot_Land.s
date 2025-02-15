
	AREA |c$$code|,CODE,READONLY

	EXPORT fast_rotation
	EXPORT machine_code_land_plot
	EXPORT wave_counter
	EXPORT wave_counter2
	EXPORT wave_counter3
	EXPORT plot_land_constants
	EXPORT machine_code_flat_land_plot
	EXPORT land_sort_offset
	
	IMPORT arm_addpolycel16
	IMPORT arm_add4cel4 
	IMPORT plot_static_graphic
	IMPORT arm_addcelfrom512map
	IMPORT arm_addcelfrom128map
	IMPORT arm_addcelfrom32map
	IMPORT plot_list
	IMPORT plot_ship_graphic
	IMPORT plot_smoke
	IMPORT plot_laser
	IMPORT plot_bit_graphic
	IMPORT plot_static_from_grid
	IMPORT test_mode

fast_rotation
	
;r0 pointer to misc_data [ xxx ]
;r1 resolution required 0 - full 1+ de-res

;              misc_data ---
;	byte	word	reg		use
;	---------------------------
;	0		0		4		land x start pos ( 0 - 255 lookup - landscape_heights )
;	4		1		5		land y start pos
;	8		2		6		offset_x
;	12		3		7		offset_y
;	16		4		8		offset_z

;	20		5		9		hoz_x
;	24		6		10		hoz_y
;	28		7		11		hoz_z
;	32		8				vert_x
;	36		9				vert_y
;	40		10				vert_z

;	44		11				land x start pos outer
;	48		12				land y start pos outer
;	52		13				offset x outer
;	56		14				offset y outer	
;	60		15				offset z outer

;	64		16				land x start pos outer 2
;	68		17				land y start pos outer 2
;	72		18				offset x outer 2
;	76		19				offset y outer 2
;	80		20				offset z outer 2

;	84		21				land x start pos outer 3
;	88		22				land y start pos outer 3
;	92		23				offset x outer 3
;	96		24				offset y outer 3
;	100		25				offset z outer 3

;	104		26				land x start pos outer 4
;	108		27				land y start pos outer 4
;	112		28				offset x outer 4
;	116		29				offset y outer 4
;	120		30				offset z outer 4

;	124		26				land x start pos outer 5
;	128		27				land y start pos outer 5
;	132		28				offset x outer 5
;	136		29				offset y outer 5
;	140		30				offset z outer 5

;	144		26				land x start pos outer 6
;	148		27				land y start pos outer 6
;	152		28				offset x outer 6
;	156		29				offset y outer 6
;	160		30				offset z outer 6

;	164		26				land x start pos outer 7
;	168		27				land y start pos outer 7
;	172		28				offset x outer 7
;	176		29				offset y outer 7
;	180		30				offset z outer 7


;Save stuff onto the stack and store stack 
	stmfd sp!,{r4-r12,r14}
	str sp,stack_pointer

	str r1,land_resolution

	mov r3,r0

	ldr r0,rotated_coords
	ldr r1,landscape_heights
	ldr r2,quick_height_table

;############################################################################

;calculate the rest of the quick height lookup table
	mov r4,r2			; copy of base adr of table into r4
	ldmia r4,{r5-r7}	; get the coords for 1 height unit (base step rate)
	mov r8,#0			; first pair at land level
	mov r9,#0
	mov r10,#0
	
	;Height units 17 - 255 are land
	;0-16 Sea
	add r4,r4,#17*16		;start at land level height in list
		
	mov r14,#118		; loop counter (119 cycles of loop)
	
	; loop - calcs 2 coords at a time
	; this is for the above sea level height
calc_table_loop_land

	add r11,r8,r5		; find odd numbered units by adding base step to even
	add r12,r9,r6
	add r13,r10,r7
	
	stmia r4,{r8-r10}	; store 2 pairs 
	add r4,r4,#16
	stmia r4,{r11-r13}
	add r4,r4,#16

	add r8,r11,r5		; find even numbered units by adding base step to odd
	add r9,r12,r6
	add r10,r13,r7
	
	subs r14,r14,#1		; count n' continue ?
	bpl calc_table_loop_land

	stmia r4,{r8-r10}	;store last pair

;###############
;sea level n' below

	mov r4,r2					; copy of base adr into r4
	;start sea at land base height - 4 height units
	mvn r8,r5, asl #2			; first pair at low sea level (land - 4 height units)
	mvn r9,r6, asl #2
	mvn r10,r7, asl #2
	
	;Height units 17 - 255 are land
	;0-16 Sea
		
	mov r14,#7 		; loop counter (8 cycles of loop)
	
	; loop - calcs 2 coords at a time
	; this is for the below sea level height
calc_table_loop_sea

	;change of height at sea level at half the normal step rate

	add r11,r8,r5, asr #1		; find odd numbered units by adding base step to even
	add r12,r9,r6, asr #1
	add r13,r10,r7, asr #1
	
	stmia r4,{r8-r10}	; store 2 pairs 
	add r4,r4,#16
	stmia r4,{r11-r13}
	add r4,r4,#16

	add r8,r11,r5, asr #1		; find even numbered units by adding base step to odd
	add r9,r12,r6, asr #1
	add r10,r13,r7, asr #1
	
	subs r14,r14,#1		; count n' continue ?
	bpl calc_table_loop_sea

	stmia r4,{r8-r10}	;store last pair

;###########################################################################

;load misc vars
	ldmia r3,{r4-r11}
	str r3,rot_data


	;store the top left grid start pos
	;for use later by the land plot code
	str r4,land_x_pos
	str r5,land_y_pos

	;Are we going to plot the high res land ?
	ldr r14,land_resolution
	cmp r14,#0
	bgt skip_high_res_nodes

;r14 x counter
	mov r14,#32
	str r14,y_loop_count

landscape_loop

;get height of node at this point
	add r12,r4,r5, asl #8
	ldrb r12,[r1,r12]

;get rotated height from quick look up table
	add r12,r2,r12, asl #4
	ldmia r12,{r3,r12,r13}

;add on the height offsets
	sub r3,r6,r3, asr #6
	sub r12,r7,r12, asr #6
	sub r13,r8,r13, asr #6

;store the rotated coords on the coord stack
	stmia r0!,{r3,r12,r13}
		
;check counter and continue if >= 0	
	subs r14,r14,#1
	
;move coords along by the x 
	addpl r6,r6,r9
	addpl r7,r7,r10
	addpl r8,r8,r11
	
;move landscape_heights pointer along the x 
	addpl r4,r4,#1
	andpl r4,r4,#255

	bpl landscape_loop ; loop

;reset x loop count
	mov r14,#32

;move landscape_heights pointer along the y
	add r5,r5,#1
	and r5,r5,#255

;move back to start of new line ( 32 units )
	sub r6,r6,r9, asl #5
	sub r7,r7,r10, asl #5
	sub r8,r8,r11, asl #5

	ldr r3,rot_data
;move down 1 line in the y
	ldr r12,[r3,#32]
	add r6,r6,r12
	ldr r12,[r3,#36]
	add r7,r7,r12
	ldr r12,[r3,#40]
	add r8,r8,r12

;skip land_heights x pointer back 1 line (32 units)
	sub r4,r4,#32
	and r4,r4,#255

;update y loop counter
	ldr r13,y_loop_count
	subs r13,r13,#1
	strpl r13,y_loop_count
	bpl landscape_loop ; loop

	b dont_skip_high_res_nodes


skip_high_res_nodes
	
	;We are not plotting this grid skip the pointer forward
	ldr r14,skip_nodes_high_res_offset
	add r0,r0,r14

dont_skip_high_res_nodes


;####################################
;#									#
;#	Distant outer grid land coords	#
;#									#
;####################################

;load misc vars
	ldr r3,rot_data
	add r4,r3,#44
	ldmia r4,{r4-r8}
	str r4,land_x_pos_outer
	str r5,land_y_pos_outer

	ldr r14,land_resolution
	cmp r14,#1
	bgt skip_mid_res_nodes
	
	str r14,node_rot_skip_middle

;r14 x counter
	mov r14,#16
	str r14,y_loop_count

;the outer edge of this grid is fixed to match
;the grid outside this one by taking an
;average height on every other node

landscape_outer_loop_with_fix

	;Is the horizontal land pos on a 4 boundary
	tst r4,#2_100
	bne fix_the_height_hoz

	;is the hoz counter on 0 or 16
	tst r14,#2_1111
	beq fix_the_height_vert

the_height_needs_no_fix

;get height of node at this point
	add r12,r4,r5, asl #8
	ldrb r12,[r1,r12]

the_height_is_fixed

;get rotated height from quick look up table
	add r12,r2,r12, asl #4
	ldmia r12,{r3,r12,r13}

;add on the height offsets
	sub r3,r6,r3, asr #6
	sub r12,r7,r12, asr #6
	sub r13,r8,r13, asr #6

;store the rotated coords on the coord stack
	stmia r0!,{r3,r12,r13}

;check x counter and continue if >= 0
	subs r14,r14,#1
	
;move coords along by the x at 4 unit step rate 
	addpl r6,r6,r9, asl #2
	addpl r7,r7,r10, asl #2
	addpl r8,r8,r11, asl #2

;move landscape_heights pointer along the x by 4 units
	addpl r4,r4,#4
	andpl r4,r4,#255

	bpl landscape_outer_loop_with_fix ; loop

	b skip_middle_bit_code

fix_the_height_hoz
	;find the average heihgt between the nodes left and right of this one
	
	;get height of node to left
	sub r12,r4,#4
	and r12,r12,#255
	add r12,r12,r5, asl #8
	ldrb r12,[r1,r12]
	
	;get height of node to right 
	add r13,r4,#4
	and r13,r13,#255
	add r13,r13,r5, asl #8
	ldrb r13,[r1,r13]
	
	;average the heights
	add r12,r12,r13
	mov r12,r12, asr #1
	
	;mov r12,#255
	
	b the_height_is_fixed


fix_the_height_vert
	;find the average heihgt between the nodes left and right of this one
	
	;Is it on a 4 boundary
	tst r5,#2_100
	beq the_height_needs_no_fix
	
	;get height of node to left
	sub r12,r5,#4
	and r12,r12,#255
	add r12,r4,r12, asl #8
	ldrb r12,[r1,r12]
	
	;get height of node to right 
	add r13,r5,#4
	and r13,r13,#255
	add r13,r4,r13, asl #8
	ldrb r13,[r1,r13]
	
	;average the heights
	add r12,r12,r13
	mov r12,r12, asr #1
	
	;mov r12,#255
	
	b the_height_is_fixed

landscape_outer_loop

	;is the hoz counter on 0 or 16
	tst r14,#2_1111
	beq fix_the_height_vert_2

the_height_needs_no_fix_2

;get height of node at this point
	add r12,r4,r5, asl #8
	ldrb r12,[r1,r12]

the_height_is_fixed_2

;get rotated height from quick look up table
	add r12,r2,r12, asl #4
	ldmia r12,{r3,r12,r13}

;add on the height offsets
	sub r3,r6,r3, asr #6
	sub r12,r7,r12, asr #6
	sub r13,r8,r13, asr #6

;store the rotated coords on the coord stack
	stmia r0!,{r3,r12,r13}

;check x counter and continue if >= 0
	subs r14,r14,#1
	
;move coords along by the x at 4 unit step rate 
	addpl r6,r6,r9, asl #2
	addpl r7,r7,r10, asl #2
	addpl r8,r8,r11, asl #2

;move landscape_heights pointer along the x by 4 units
	addpl r4,r4,#4
	andpl r4,r4,#255

	bpl landscape_outer_loop ; loop

	b skip_middle_bit_code

fix_the_height_vert_2
	;find the average heihgt between the nodes left and right of this one
	
	tst r5,#2_100
	beq the_height_needs_no_fix_2
	
	;get height of node to left
	sub r12,r5,#4
	and r12,r12,#255
	add r12,r4,r12, asl #8
	ldrb r12,[r1,r12]
	
	;get height of node to right 
	add r13,r5,#4
	and r13,r13,#255
	add r13,r4,r13, asl #8
	ldrb r13,[r1,r13]
	
	;average the heights
	add r12,r12,r13
	mov r12,r12, asr #1
	
	;mov r12,#255
	
	b the_height_is_fixed_2

;#################################################

landscape_outer_loop_without_middle

	;is the hoz counter on 0 or 16
	tst r14,#2_1111
	beq fix_the_height_vert_3

the_height_needs_no_fix_3

;get height of node at this point
	add r12,r4,r5, asl #8
	ldrb r12,[r1,r12]

the_height_is_fixed_3

;get rotated height from quick look up table
	add r12,r2,r12, asl #4
	ldmia r12,{r3,r12,r13}

;add on the height offsets
	sub r3,r6,r3, asr #6
	sub r12,r7,r12, asr #6
	sub r13,r8,r13, asr #6

;store the rotated coords on the coord stack
	stmia r0!,{r3,r12,r13}
	
;is it time to leave the loop
	cmp r14,#0
	ble skip_middle_bit_code
	
;is the counter at that skip over the middle bit
	cmp r14,#11
	beq skip_over_the_middle_coords
	
;move coords along by the x at 4 unit step rate 
	add r6,r6,r9, asl #2
	add r7,r7,r10, asl #2
	add r8,r8,r11, asl #2
	
	
;move landscape_heights pointer along the x by 4 units
	add r4,r4,#4
	and r4,r4,#255

	subs r14,r14,#1
	bpl landscape_outer_loop_without_middle ; loop

skip_over_the_middle_coords
	add r0,r0,#5*12 ; add onto store pointer

;skip along grid by 24 units
	add r6,r6,r9, asl #4
	add r7,r7,r10, asl #4
	add r8,r8,r11, asl #4

	add r6,r6,r9, asl #3
	add r7,r7,r10, asl #3
	add r8,r8,r11, asl #3

;move landscape_heights pointer along by 24 units
	add r4,r4,#24
	and r4,r4,#255

	subs r14,r14,#6
	bpl landscape_outer_loop_without_middle ; loop
	
fix_the_height_vert_3
	;find the average heihgt between the nodes left and right of this one
	
	;Is it on a 4 boundary
	tst r5,#2_100
	beq the_height_needs_no_fix_3
	
	;get height of node to left
	sub r12,r5,#4
	and r12,r12,#255
	add r12,r4,r12, asl #8
	ldrb r12,[r1,r12]
	
	;get height of node to right 
	add r13,r5,#4
	and r13,r13,#255
	add r13,r4,r13, asl #8
	ldrb r13,[r1,r13]
	
	;average the heights
	add r12,r12,r13
	mov r12,r12, asr #1
	
	;mov r12,#255
	
	b the_height_is_fixed_3

	
;#################################################

skip_middle_bit_code

;reset x loop count
	mov r14,#16

;move landscape_heights pointer along the y
	add r5,r5,#4
	and r5,r5,#255

;move back to start of new line ( 64 units )
	sub r6,r6,r9, asl #6
	sub r7,r7,r10, asl #6
	sub r8,r8,r11, asl #6

	ldr r3,rot_data
;move down 4 units in the y
	ldr r12,[r3,#32]
	add r6,r6,r12, asl #2
	ldr r12,[r3,#36]
	add r7,r7,r12, asl #2
	ldr r12,[r3,#40]
	add r8,r8,r12, asl #2

;skip land_heights x pointer back 1 line (64 units)
	sub r4,r4,#64
	and r4,r4,#255


;update y loop counter and quit if its <0
	ldr r13,y_loop_count
	subs r13,r13,#1
	bmi end_of_land_outer_coords
	
	str r13,y_loop_count
	
	;Is it the bottom line of the grid
	cmp r13,#0
	beq landscape_outer_loop_with_fix
	
	;Calculate the nodes in the middle of the grid or wot ?
	ldr r12,node_rot_skip_middle
	cmp r12,#0
	bne landscape_outer_loop
	
	;are we above the skip middle bit
	cmp r13,#11
	bge landscape_outer_loop ; loop
	
	;are we below the skip middle bit
	cmp r13,#5
	ble landscape_outer_loop
	
	;we must be in the skip middle zone
	;this time we only calculate the far left and right coords
	;as the middle is cover by the detailed first landscape loop
	
	;setup counter
	;mov r14,#11
	b landscape_outer_loop_without_middle

skip_mid_res_nodes

	;We are not plotting this grid skip the pointer forward
	ldr r14,skip_nodes_low_res_offset
	add r0,r0,r14
	
end_of_land_outer_coords

;####################################


;####################################
;#	Super							#
;#	Distant outer grid land coords	#
;#									#
;####################################

;load misc vars
	ldr r3,rot_data
	add r4,r3,#64
	ldmia r4,{r4-r8}
	str r4,land_x_pos_outer_2
	str r5,land_y_pos_outer_2

	ldr r14,land_resolution
	cmp r14,#2
	bgt skip_low_res_nodes
	
	movne r14,#0
	moveq r14,#1
	str r14,node_rot_skip_middle

;r14 x counter
	mov r14,#16
	str r14,y_loop_count


landscape_outer_loop_2

;get height of node at this point
	add r12,r4,r5, asl #8
	ldrb r12,[r1,r12]

;get rotated height from quick look up table
	add r12,r2,r12, asl #4
	ldmia r12,{r3,r12,r13}

;add on the height offsets
	sub r3,r6,r3, asr #7
	sub r12,r7,r12, asr #7		;half the height
	sub r13,r8,r13, asr #7

;store the rotated coords on the coord stack
	stmia r0!,{r3,r12,r13}

;check x counter and continue if >= 0
	subs r14,r14,#1
	
;move coords along by the x at 4 unit step rate 
	addpl r6,r6,r9, asl #2
	addpl r7,r7,r10, asl #2
	addpl r8,r8,r11, asl #2

;move landscape_heights pointer along the x by 4 units
	addpl r4,r4,#8
	andpl r4,r4,#255

	bpl landscape_outer_loop_2 ; loop

	b skip_middle_bit_code_2


;#################################################

landscape_outer_loop_without_middle_2

;get height of node at this point
	add r12,r4,r5, asl #8
	ldrb r12,[r1,r12]

;get rotated height from quick look up table
	add r12,r2,r12, asl #4
	ldmia r12,{r3,r12,r13}

;add on the height offsets
	sub r3,r6,r3, asr #7
	sub r12,r7,r12, asr #7	;half the height
	sub r13,r8,r13, asr #7

;store the rotated coords on the coord stack
	stmia r0!,{r3,r12,r13}
	
;is it time to leave the loop
	cmp r14,#0
	ble skip_middle_bit_code_2
	
;is the counter at that skip over the middle bit
	cmp r14,#6
	beq skip_over_the_middle_coords_2
	
;move coords along by the x at 4 unit step rate 
	add r6,r6,r9, asl #2
	add r7,r7,r10, asl #2
	add r8,r8,r11, asl #2
	
	
;move landscape_heights pointer along the x by 4 units
	add r4,r4,#4*2
	and r4,r4,#255

	subs r14,r14,#1
	bpl landscape_outer_loop_without_middle_2 ; loop

skip_over_the_middle_coords_2
	add r0,r0,#5*12 ; add onto store pointer

;skip along grid by 24 units
	add r6,r6,r9, asl #4
	add r7,r7,r10, asl #4
	add r8,r8,r11, asl #4

	add r6,r6,r9, asl #3
	add r7,r7,r10, asl #3
	add r8,r8,r11, asl #3

;move landscape_heights pointer along by 24 units
	add r4,r4,#24*2
	and r4,r4,#255

	subs r14,r14,#1
	bpl landscape_outer_loop_without_middle_2 ; loop
	
	

;#################################################

skip_middle_bit_code_2


;reset x loop count
	mov r14,#16

;move landscape_heights pointer along the y
	add r5,r5,#4*2
	and r5,r5,#255

;move back to start of new line ( 64 units )
	sub r6,r6,r9, asl #6
	sub r7,r7,r10, asl #6
	sub r8,r8,r11, asl #6		;half the height

	ldr r3,rot_data
;move down 4 units in the y
	ldr r12,[r3,#32]
	add r6,r6,r12, asl #2
	ldr r12,[r3,#36]
	add r7,r7,r12, asl #2
	ldr r12,[r3,#40]
	add r8,r8,r12, asl #2		;half the height

;skip land_heights x pointer back 1 line (64 units)
	sub r4,r4,#128
	and r4,r4,#255


;update y loop counter and quit if its <0
	ldr r13,y_loop_count
	subs r13,r13,#1
	bmi end_of_land_outer_coords_2
	
	str r13,y_loop_count

	;Skip the nodes in the middle or wot ?
	ldr r12,node_rot_skip_middle
	cmp r12,#0
	bne landscape_outer_loop_2
	
	;are we above the skip middle bit
	cmp r13,#11
	bge landscape_outer_loop_2 ; loop
	
	;are we below the skip middle bit
	cmp r13,#5
	ble landscape_outer_loop_2
	
	;we must be in the skip middle zone
	;this time we only calculate the far left and right coords
	;as the middle is cover by the detailed first landscape loop
	
	;setup counter
	mov r14,#11
	b landscape_outer_loop_without_middle_2
	
skip_low_res_nodes

	;We are not plotting this grid skip the pointer forward
	ldr r14,skip_nodes_low_res_offset
	add r0,r0,r14

end_of_land_outer_coords_2
	
;####################################


;####################################
;#	Super super flat				#
;#	Distant outer grid land coords	#
;#									#
;####################################

;load misc vars
	add r4,r3,#84
	ldmia r4,{r4-r8}
	str r4,land_x_pos_outer_3
	str r5,land_y_pos_outer_3

	;skip the middle nodes of this grid or wot ?
	ldr r12,land_resolution
	cmp r12,#3
	moveq r12,#1
	movlt r12,#0
	str r12,node_rot_skip_middle
	
	bl flat_land_rotator

	;load misc vars
	add r4,r3,#104
	ldmia r4,{r4-r8}
	str r4,land_x_pos_outer_4
	str r5,land_y_pos_outer_4
	
	mov r12,#0
	str r12,node_rot_skip_middle

	bl flat_land_rotator
	
	ldr r12,land_resolution
	cmp r12,#0
	beq skip_these_flat_sections
	
	;load misc vars
	add r4,r3,#124
	ldmia r4,{r4-r8}
	str r4,land_x_pos_outer_5
	str r5,land_y_pos_outer_5

	bl flat_land_rotator

	ldr r12,land_resolution
	cmp r12,#1
	ble skip_these_flat_sections

	;load misc vars
	add r4,r3,#144
	ldmia r4,{r4-r8}
	str r4,land_x_pos_outer_6
	str r5,land_y_pos_outer_6

	bl flat_land_rotator

	ldr r12,land_resolution
	cmp r12,#2
	ble skip_these_flat_sections

	;load misc vars
	add r4,r3,#164
	ldmia r4,{r4-r8}
	str r4,land_x_pos_outer_7
	str r5,land_y_pos_outer_7

	bl flat_land_rotator

skip_these_flat_sections

	
;All the nodes have been rotated and stored in rotated_coords - now do the perspective
	ldr r0,rotated_coords		;base address of nodes
	ldr r1,perspective_table 
	ldr r2,screen_coords		;store address of nodes

;Depending on the resolution of the landscape there are different
;amounts of nodes and the start address is offset

	ldr r4,land_resolution

	;get counter for number of nodes to make perspective
	adr r3,landscape_perspective_counters
	ldr r3,[r3,r4, asl #2]
	
	;get address offset for start of rotated node store
	adr r5,land_res_node_offsets
	ldr r5,[r5,r4, asl #2]
	add r0,r0,r5

	;get address offset for stored perspectived nodes
	adr r5,land_res_node_store_offsets
	ldr r5,[r5,r4, asl #2]
	add r2,r2,r5

land_perspective_loop

;load in 3 sets of coords at a time
	ldmia r0!,{r4-r12}
	;r4-r6 1st x,y,z coords rotated 
	;r7-r9 2nd x,y,z 
	;r10-r12 3rd
	;calc perspective screen positions if the coords are in front of camera

;Is the 1st coord in front of the camera - if true calc screen pos
	cmp r5,#0
	ble land_fail_pex_1 ; Skip code if the point is behind the view point
	bic r5,r5,#31
	ldr r5,[r1,r5, asr #3] ; Load perspective scaler value
	mul r13,r4,r5
	mov r4,r13, asr #16 ; Scale the x
	mul r13,r6,r5
	mov r6,r13, asr #16 ; Scale the y
land_fail_pex_1

;2nd coord
	cmp r8,#0
	ble land_fail_pex_2
	bic r8,r8,#31
	ldr r8,[r1,r8, asr #3]
	mul r13,r7,r8
	mov r7,r13, asr #16
	mul r13,r9,r8
	mov r9,r13, asr #16
land_fail_pex_2

;3rd coord
	cmp r11,#0
	ble land_fail_pex_3
	bic r11,r11,#31
	ldr r11,[r1,r11, asr #3]
	mul r13,r10,r11
	mov r10,r13, asr #16
	mul r13,r12,r11
	mov r12,r13, asr #16
land_fail_pex_3

;store the screen coords
	stmia r2!,{r4,r6,r7,r9,r10,r12}

	subs r3,r3,#3
	bpl land_perspective_loop
	

;get the stack pointer and re-load all t' regs and return
;rotated_coords contains all the nodes for the landscape
;screen_coords contains all the perspective x,y screen positions

	ldr sp,stack_pointer
	ldmfd sp!,{r4-r12,pc}^	

;these are the different offsets for the node calculations
;based on the landscape resolution

land_res_node_offsets
	DCD 0
	DCD (33*33)*12
	DCD ((33*33)+(17*17))*12
	DCD ((33*33)+(17*17)+(17*17))*12

land_res_node_store_offsets
	DCD 0
	DCD (33*33)*8
	DCD ((33*33)+(17*17))*8
	DCD ((33*33)+(17*17)+(17*17))*8
	
landscape_perspective_counters
	DCD (33*33)+(17*17)+(17*17)+(17*17)+(17*17)
	DCD (17*17)+(17*17)+(17*17)+(17*17)+(17*17)
	DCD (17*17)+(17*17)+(17*17)+(17*17)+(17*17)
	DCD (17*17)+(17*17)+(17*17)+(17*17)+(17*17)

rot_data
	DCD 0
stack_pointer
	DCD 0
y_loop_count
	DCD 0

skip_nodes_high_res_offset
	DCD (33*33*12)
skip_nodes_low_res_offset
	DCD (17*17*12)

land_resolution
	DCD 0

link_reg
	DCD 0

node_rot_skip_middle
	DCD 0

flat_land_rotator

	str r14,link_reg

;r14 x counter
	mov r14,#16
	str r14,y_loop_count

landscape_outer_loop_3
	
;store the rotated coords on the coord stack
	stmia r0!,{r6-r8}

;check x counter and continue if >= 0
	subs r14,r14,#1
	
;move coords along by the x at 4 unit step rate 
	addpl r6,r6,r9, asl #2
	addpl r7,r7,r10, asl #2
	addpl r8,r8,r11, asl #2

	bpl landscape_outer_loop_3 ; loop

	b skip_middle_bit_code_3


;#################################################

landscape_outer_loop_without_middle_3

;store the rotated coords on the coord stack
	stmia r0!,{r6-r8} ;   ,r12,r13}
	
;is it time to leave the loop
	cmp r14,#0
	ble skip_middle_bit_code_3
	
;is the counter at that skip over the middle bit
	cmp r14,#6
	beq skip_over_the_middle_coords_3
	
;move coords along by the x at 4 unit step rate 
	add r6,r6,r9, asl #2
	add r7,r7,r10, asl #2
	add r8,r8,r11, asl #2

	subs r14,r14,#1
	bpl landscape_outer_loop_without_middle_3 ; loop

skip_over_the_middle_coords_3
	add r0,r0,#5*12 ; add onto store pointer

;skip along grid by 24 units
	add r6,r6,r9, asl #4
	add r7,r7,r10, asl #4
	add r8,r8,r11, asl #4

	add r6,r6,r9, asl #3
	add r7,r7,r10, asl #3
	add r8,r8,r11, asl #3

	subs r14,r14,#1
	bpl landscape_outer_loop_without_middle_3 ; loop

;#################################################

skip_middle_bit_code_3


;reset x loop count
	mov r14,#16

;move back to start of new line ( 64 units )
	sub r6,r6,r9, asl #6
	sub r7,r7,r10, asl #6
	sub r8,r8,r11, asl #6

;move down 4 units in the y
	ldr r12,[r3,#32]
	add r6,r6,r12, asl #2
	ldr r12,[r3,#36]
	add r7,r7,r12, asl #2
	ldr r12,[r3,#40]
	add r8,r8,r12, asl #2

;skip land_heights x pointer back 1 line (64 units)
	sub r4,r4,#384
	and r4,r4,#255

;update y loop counter and quit if its <0
	ldr r13,y_loop_count
	subs r13,r13,#1
	bmi end_of_land_outer_coords_3
	
	str r13,y_loop_count
	
	;are we going to calculate the nodes in the middle of this grid ?
	ldr r12,node_rot_skip_middle
	cmp r12,#0
	bne landscape_outer_loop_3
	
	;are we above the skip middle bit
	cmp r13,#11
	bge landscape_outer_loop_3 ; loop
	
	;are we below the skip middle bit
	cmp r13,#5
	ble landscape_outer_loop_3
	
	;we must be in the skip middle zone
	;this time we only calculate the far left and right coords
	;as the middle is cover by the detailed first landscape loop
	
	;setup counter
	mov r14,#11
	b landscape_outer_loop_without_middle_3
	
end_of_land_outer_coords_3
	
;####################################
	ldr pc,link_reg

dist_plot_list
	DCD plot_list
plot_list_pointer
	DCD 0
search_plot_to_here
	DCD 0

land_sort_offset
	DCD 0

plot_sorted_graphics
		
	;Input search_plot_to_here - set from outside - search list and plot to here 127 - 0
	;each unit is 1 sprite unit (1<<24 in ship units)
	
	ldr r12,plot_list_pointer
	cmp r12,#0
	movlt pc,r14
		
	ldr r11,search_plot_to_here
	;adds r11,r11,#4
	
	ldr r10,land_sort_offset
	adds r11,r11,r10
	
	movmi r11,#0
	cmp r11,#127
	movgt r11,#127
	str r11,search_plot_to_here
	
	stmfd sp!,{r14}
	
plot_list_loop
	ldr r10,dist_plot_list
	
	;get the pointer
	ldr r11,[r10,r12, asl #2]
	
	;Update the pointer
	sub r9,r12,#1
	str r9,plot_list_pointer
	
	cmp r11,#0
	beq next_in_plot_list
	
	;clear this position in list
	mov r9,#0
	str r9,[r10,r12, asl #2]
		
plot_list_loop_2
	
	stmfd sp!,{r11}
	
	;get the address and reference of this item
	add r0,r11,#4
	ldmia r0,{r0-r1}
	
	;branch with link to the required routine
	adr r2,plot_references
	mov r14,pc
	ldr pc,[r2,r1, asl #2]
	
	ldmfd sp!,{r11}
	
	;is there another item in this section
	ldr r11,[r11,#12]
	cmp r11,#0
	bne plot_list_loop_2
	
next_in_plot_list
	
	ldr r12,plot_list_pointer
	ldr r0,search_plot_to_here
	cmp r0,r12
	ble plot_list_loop
	
	ldmfd sp!,{pc}^

; These are the different types of graphics which you may plot
plot_references
	DCD plot_ship_graphic
	DCD plot_smoke
	DCD plot_laser
	DCD plot_bit_graphic
	DCD plot_static_from_grid

machine_code_flat_land_plot
	
	stmfd sp!,{r4-r12,r14}
	
	ldr r0,space_mission_or_wot
	cmp r0,#1
	blne flat_low_res_sorted_land_plotter

	ldmfd sp!,{r4-r12,pc}

			
machine_code_land_plot
	
	stmfd sp!,{r4-r12,r14}
	
	;reset the plot list pointer for moving graphic
	mov r0,#127
	str r0,plot_list_pointer
	
	; Flat land plot not here
	;ldr r0,space_mission_or_wot
	;cmp r0,#1
	;blne flat_low_res_sorted_land_plotter

	;test graphics 
	adrl r0,test_mode
	ldr r0,[r0,#0]
	cmp r0,#1
	streq r0,space_mission_or_wot

	ldr r0,space_mission_or_wot
	cmp r0,#1
	movne r0,#64
	mvneq r0,#15
	
	str r0,search_plot_to_here
	bl plot_sorted_graphics

	mov r0,#0
	str r0,plot_res_skip_middle

	ldr r0,space_mission_or_wot
	cmp r0,#1
	ldmeqfd sp!,{r4-r12,pc}

	ldr r0,land_resolution
	cmp r0,#0
	beq full_resolution_land_plotter
	cmp r0,#1
	beq mid_resolution_land_plotter
	cmp r0,#2
	beq low_resolution_land_plotter
	
	b low_low_resolution_land_plotter
	
		
full_resolution_land_plotter
	
	bl low_res_sorted_land_plotter
				
	bl mid_res_sorted_land_plotter

	bl sorted_land_plotter

	mvn r0,#128
	str r0,search_plot_to_here
	bl plot_sorted_graphics

	ldmfd sp!,{r4-r12,pc}^

mid_resolution_land_plotter
	
	bl low_res_sorted_land_plotter
	
	mov r0,#1
	str r0,plot_res_skip_middle
					
	bl mid_res_sorted_land_plotter

	mvn r0,#128
	
	str r0,search_plot_to_here
	bl plot_sorted_graphics

	ldmfd sp!,{r4-r12,pc}^

low_resolution_land_plotter
	
	mov r0,#1
	str r0,plot_res_skip_middle

	bl low_res_sorted_land_plotter

	mvn r0,#128
	
	str r0,search_plot_to_here
	bl plot_sorted_graphics

	ldmfd sp!,{r4-r12,pc}^
	
low_low_resolution_land_plotter
	
	mvn r0,#128
	
	str r0,search_plot_to_here
	bl plot_sorted_graphics

	ldmfd sp!,{r4-r12,pc}^


;####################################
;#									#
;#									#
;#	Full res land plotter			#
;#									#
;#									#
;####################################

land_x_pos
	DCD 0
land_y_pos
	DCD 0


sorted_land_plotter

	stmfd sp!,{r14}

	ldr r1,land_x_pos
	ldr r2,land_y_pos

	mov r9,#0			; top left corner start (0)
	;mov r9,#33
	add r10,r9,#31		; top right corner start (31)
	add r11,r10,#992	; bottom left corner start ( 1023 )
	;sub r11,r11,#33
	add r12,r11,#31		; bottom right conner start ( 1054 )
	
	mov r8,#31			; outer layer loop counter
	
	mov r3,#15 
	str r3,hill_shade_offset
	
main_land_sort_loop

	mov r0,#1
	str r0,test_grid_plot
	
	;plot the far corners of this land section layer
	stmfd sp!,{r1-r2,r8-r12}
	mov r0,r9
	bl plot_land_section		; top left corner
	
	ldmfd sp,{r1-r2,r8-r12}
	mov r0,r10
	add r1,r1,r8
	bl plot_land_section		; top right corner
	
	ldmfd sp,{r1-r2,r8-r12}
	mov r0,r11
	add r2,r2,r8
	bl plot_land_section		; bottom left corner
	
	ldmfd sp,{r1-r2,r8-r12}
	mov r0,r12
	add r1,r1,r8
	add r2,r2,r8
	bl plot_land_section		; bottom right corner
	
	mov r0,#0
	str r0,test_grid_plot
	
	ldmfd sp,{r1-r2,r8-r12}
	
	mov r5,#1					; horizontal section offset
	mov r6,#33					; vertical section offset
	mov r7,r8, asr #1			; loop counter

	subs r7,r7,#1				; if we are in the centre the 4 corner
	bmi no_land_layer_plot		; sections are all that is needed
	
	;re-arrange stack contents to include the above
	ldmfd sp!,{r1-r2,r8-r12}
	stmfd sp!,{r1-r2,r5-r12}
	
land_layer_plot_loop
	
	;plot right from 'top left' corner
	add r0,r9,r5
	add r1,r1,r5
	bl plot_land_section
	
	;plot down from 'top left' corner
	ldmfd sp,{r1-r2,r5-r12}
	add r0,r9,r6
	add r2,r2,r5
	bl plot_land_section
	
	;plot left from 'top right' corner
	ldmfd sp,{r1-r2,r5-r12}
	sub r0,r10,r5
	add r1,r1,r8
	sub r1,r1,r5
	bl plot_land_section
	
	;plot down from 'top right' corner
	ldmfd sp,{r1-r2,r5-r12}
	add r0,r10,r6
	add r1,r1,r8
	add r2,r2,r5
	bl plot_land_section
	
	;plot right from 'bot left' corner
	ldmfd sp,{r1-r2,r5-r12}
	add r0,r11,r5
	add r1,r1,r5
	add r2,r2,r8
	bl plot_land_section
	
	;plot up from 'bot left' corner
	ldmfd sp,{r1-r2,r5-r12}
	sub r0,r11,r6
	sub r2,r2,r5
	add r2,r2,r8
	bl plot_land_section

	;plot left from 'bot right' corner
	ldmfd sp,{r1-r2,r5-r12}
	sub r0,r12,r5
	sub r1,r1,r5
	add r1,r1,r8
	add r2,r2,r8
	bl plot_land_section
	
	;plot up from 'bot right' corner
	ldmfd sp,{r1-r2,r5-r12}
	sub r0,r12,r6
	add r1,r1,r8
	sub r2,r2,r5
	add r2,r2,r8
	bl plot_land_section


	ldmfd sp!,{r1-r2,r5-r12}
	subs r7,r7,#1				; check loop counter
	addpl r5,r5,#1				; update horizontal and vertical
	addpl r6,r6,#33				; offsets if loop is to continue
	stmplfd sp!,{r1-r2,r5-r12}	; put updated regs on stack
	bpl land_layer_plot_loop
	
	stmfd sp!,{r1-r2,r8-r12}	; restore stack to main loop regs
	
no_land_layer_plot
	
	mov r7,r8, asr #1
	str r7,search_plot_to_here

	;bl update_delay_plot
	
	bl plot_sorted_graphics

	; move the corner pointers in towards centre of grid
	
	ldmfd sp!,{r1-r2,r8-r12}
	
	
	subs r8,r8,#2				; check main loop counter and continue ?
	
	ldmmifd sp!,{pc}^

	cmp r8,#17
	streq r8,hill_shade_offset

	;update ther node pointers for the 4 corners
	add r9,r9,#33+1			; move 'top left'  - right and down
	add r10,r10,#33-1			; move 'top right' - left and down
	sub r11,r11,#33-1			; move 'bot left'  - right and up 
	sub r12,r12,#33+1			; move 'bot right' - left and up
	
	;update the grid x,y pointer to 'top left' corner
	add r1,r1,#1
	add r2,r2,#1
	
	b main_land_sort_loop
				
	
		
;####################################
;#									#
;#									#
;#	Mid res land plotter			#
;#									#
;#									#
;####################################

land_x_pos_outer
	DCD 0
land_y_pos_outer
	DCD 0

mid_res_sorted_land_plotter

	stmfd sp!,{r14}
	
	ldr r1,land_x_pos_outer
	ldr r2,land_y_pos_outer
		
	;base of mid res nodes = + 1089 ( 33*33 )
	
	mov r9,#1024
	add r9,r9,#65		; top left corner start (33*33 = 1089)
	add r10,r9,#15		; top right corner start (1089+15 = 1104)
	add r11,r9,#255		; bottom left corner start ( 1089+255 = 1344 )
	add r12,r11,#15		; bottom right conner start ( 1089+255+15 = 1359 )
	
	mov r8,#15			; outer layer loop counter
	
	mov r3,#11
	str r3,hill_shade_offset
	
mid_res_main_land_sort_loop
	
	;plot the far corners of this land section layer
	stmfd sp!,{r1-r2,r8-r12}
	mov r0,r9
	bl plot_land_section_2		; top left corner
	
	ldmfd sp,{r1-r2,r8-r12}
	mov r0,r10
	add r1,r1,r8, asl #2
	bl plot_land_section_2		; top right corner
	
	ldmfd sp,{r1-r2,r8-r12}
	mov r0,r11
	add r2,r2,r8, asl #2
	bl plot_land_section_2		; bottom left corner
	
	ldmfd sp,{r1-r2,r8-r12}
	mov r0,r12
	add r1,r1,r8, asl #2
	add r2,r2,r8, asl #2
	bl plot_land_section_2		; bottom right corner
	
	ldmfd sp,{r1-r2,r8-r12}
	
	mov r5,#1					; horizontal section offset
	mov r6,#17					; vertical section offset
	mov r7,r8, asr #1			; loop counter

	subs r7,r7,#1						; if we are in the centre the 4 corner
	bmi mid_res_no_land_layer_plot		; sections are all that is needed
	
	;re-arrange stack contents to include the above
	ldmfd sp!,{r1-r2,r8-r12}
	stmfd sp!,{r1-r2,r5-r12}
	
mid_res_land_layer_plot_loop
	
	;plot right from 'top left' corner
	add r0,r9,r5
	add r1,r1,r5, asl #2
	bl plot_land_section_2
	
	;plot down from 'top left' corner
	ldmfd sp,{r1-r2,r5-r12}
	add r0,r9,r6
	add r2,r2,r5, asl #2
	bl plot_land_section_2
	
	;plot left from 'top right' corner
	ldmfd sp,{r1-r2,r5-r12}
	sub r0,r10,r5
	add r1,r1,r8, asl #2
	sub r1,r1,r5, asl #2
	bl plot_land_section_2
	
	;plot down from 'top right' corner
	ldmfd sp,{r1-r2,r5-r12}
	add r0,r10,r6
	add r1,r1,r8, asl #2
	add r2,r2,r5, asl #2
	bl plot_land_section_2
	
	;plot right from 'bot left' corner
	ldmfd sp,{r1-r2,r5-r12}
	add r0,r11,r5
	add r1,r1,r5, asl #2
	add r2,r2,r8, asl #2
	bl plot_land_section_2
	
	;plot up from 'bot left' corner
	ldmfd sp,{r1-r2,r5-r12}
	sub r0,r11,r6
	sub r2,r2,r5, asl #2
	add r2,r2,r8, asl #2
	bl plot_land_section_2

	;plot left from 'bot right' corner
	ldmfd sp,{r1-r2,r5-r12}
	sub r0,r12,r5
	sub r1,r1,r5, asl #2
	add r1,r1,r8, asl #2
	add r2,r2,r8, asl #2
	bl plot_land_section_2
	
	;plot up from 'bot right' corner
	ldmfd sp,{r1-r2,r5-r12}
	sub r0,r12,r6
	add r1,r1,r8, asl #2
	sub r2,r2,r5, asl #2
	add r2,r2,r8, asl #2
	bl plot_land_section_2


	ldmfd sp!,{r1-r2,r5-r12}
	subs r7,r7,#1				; check loop counter
	addpl r5,r5,#1				; update horizontal and vertical
	addpl r6,r6,#17				; offsets if loop is to continue
	stmplfd sp!,{r1-r2,r5-r12}	; put updated regs on stack
	bpl mid_res_land_layer_plot_loop
	
	stmfd sp!,{r1-r2,r8-r12}	; restore stack to main loop regs
	
mid_res_no_land_layer_plot
	
	mov r7,r8, asl #1
	str r7,search_plot_to_here
	
	bl plot_sorted_graphics

	; move the corner pointers in towards centre of grid
	
	ldmfd sp!,{r1-r2,r8-r12}
	
	sub r8,r8,#2				; update main loop counter and continue ?
	
	cmp r8,#9
	moveq r0,#13
	streq r0,hill_shade_offset
	
	ldr r0,plot_res_skip_middle
	cmp r0,#0
	bne mid_res_plot_upto_middle
	
	; check if its the middle section ?
	cmp r8,#6
	ldmltfd sp!,{pc}^
	
mid_res_continue_plot
	
	;update ther node pointers for the 4 corners
	add r9,r9,#17+1				; move 'top left'  - right and down
	add r10,r10,#17-1			; move 'top right' - left and down
	sub r11,r11,#17-1			; move 'bot left'  - right and up 
	sub r12,r12,#17+1			; move 'bot right' - left and up
	
	;update the grid x,y pointer to 'top left' corner
	add r1,r1,#4
	add r2,r2,#4
	
	b mid_res_main_land_sort_loop
		
mid_res_plot_upto_middle	
	
	cmp r8,#1
	bge mid_res_continue_plot

	ldmfd sp!,{pc}^



;###########################################

;#####################################
;#									#
;#									#
;#	Low res land plotter			#
;#									#
;#									#
;#####################################

land_x_pos_outer_2
	DCD 0
land_y_pos_outer_2
	DCD 0

low_res_sorted_land_plotter

	stmfd sp!,{r14}
	
	mov r0,#0
	str r0,land_size
	
	ldr r1,land_x_pos_outer_2
	ldr r2,land_y_pos_outer_2
		
	;base of mid res nodes = + 1089 ( 33*33 )
	;base of low res nodes = 1089 + 289 ( 17*17 )
	
	mov r9,#1280
	add r9,r9,#98		; top left corner start (33*33 + 17*17 = 1378)
	add r10,r9,#15		; top right corner start (1378+15 = 1393)
	add r11,r9,#255		; bottom left corner start ( 1378+255 = 1633 )
	add r12,r11,#15		; bottom right conner start ( 1378+255+15 = 1648 )
	
	mov r8,#15			; outer layer loop counter
	
	mov r3,#7
	str r3,hill_shade_offset
	
low_res_main_land_sort_loop
	
	;plot the far corners of this land section layer
	stmfd sp!,{r1-r2,r8-r12}
	mov r0,r9
	bl plot_land_section_3		; top left corner
	
	ldmfd sp,{r1-r2,r8-r12}
	mov r0,r10
	add r1,r1,r8, asl #3
	bl plot_land_section_3		; top right corner
	
	ldmfd sp,{r1-r2,r8-r12}
	mov r0,r11
	add r2,r2,r8, asl #3
	bl plot_land_section_3		; bottom left corner
	
	ldmfd sp,{r1-r2,r8-r12}
	mov r0,r12
	add r1,r1,r8, asl #3
	add r2,r2,r8, asl #3
	bl plot_land_section_3		; bottom right corner
	
	ldmfd sp,{r1-r2,r8-r12}
	
	mov r5,#1					; horizontal section offset
	mov r6,#17					; vertical section offset
	mov r7,r8, asr #1			; loop counter

	subs r7,r7,#1						; if we are in the centre the 4 corner
	bmi low_res_no_land_layer_plot		; sections are all that is needed
	
	;re-arrange stack contents to include the above
	ldmfd sp!,{r1-r2,r8-r12}
	stmfd sp!,{r1-r2,r5-r12}
	
low_res_land_layer_plot_loop
	
	;plot right from 'top left' corner
	add r0,r9,r5
	add r1,r1,r5, asl #3
	bl plot_land_section_3
	
	;plot down from 'top left' corner
	ldmfd sp,{r1-r2,r5-r12}
	add r0,r9,r6
	add r2,r2,r5, asl #3
	bl plot_land_section_3
	
	;plot left from 'top right' corner
	ldmfd sp,{r1-r2,r5-r12}
	sub r0,r10,r5
	add r1,r1,r8, asl #3
	sub r1,r1,r5, asl #3
	bl plot_land_section_3
	
	;plot down from 'top right' corner
	ldmfd sp,{r1-r2,r5-r12}
	add r0,r10,r6
	add r1,r1,r8, asl #3
	add r2,r2,r5, asl #3
	bl plot_land_section_3
	
	;plot right from 'bot left' corner
	ldmfd sp,{r1-r2,r5-r12}
	add r0,r11,r5
	add r1,r1,r5, asl #3
	add r2,r2,r8, asl #3
	bl plot_land_section_3
	
	;plot up from 'bot left' corner
	ldmfd sp,{r1-r2,r5-r12}
	sub r0,r11,r6
	sub r2,r2,r5, asl #3
	add r2,r2,r8, asl #3
	bl plot_land_section_3

	;plot left from 'bot right' corner
	ldmfd sp,{r1-r2,r5-r12}
	sub r0,r12,r5
	sub r1,r1,r5, asl #3
	add r1,r1,r8, asl #3
	add r2,r2,r8, asl #3
	bl plot_land_section_3
	
	;plot up from 'bot right' corner
	ldmfd sp,{r1-r2,r5-r12}
	sub r0,r12,r6
	add r1,r1,r8, asl #3
	sub r2,r2,r5, asl #3
	add r2,r2,r8, asl #3
	bl plot_land_section_3


	ldmfd sp!,{r1-r2,r5-r12}
	subs r7,r7,#1				; check loop counter
	addpl r5,r5,#1				; update horizontal and vertical
	addpl r6,r6,#17				; offsets if loop is to continue
	stmplfd sp!,{r1-r2,r5-r12}	; put updated regs on stack
	bpl low_res_land_layer_plot_loop
	
	stmfd sp!,{r1-r2,r8-r12}	; restore stack to main loop regs
	
low_res_no_land_layer_plot
	
	; move the corner pointers in towards centre of grid
	mov r7,r8, asl #2
	str r7,search_plot_to_here
	bl plot_sorted_graphics

	
	ldmfd sp!,{r1-r2,r8-r12}
	
	
	sub r8,r8,#2				; update main loop counter and continue ?

	cmp r8,#9
	moveq r0,#9
	streq r0,hill_shade_offset

	ldr r0,plot_res_skip_middle
	cmp r0,#0
	bne low_res_plot_upto_middle

	; check if its the middle section ?
	cmp r8,#6
	ldmltfd sp!,{pc}^
	
low_res_continue_plot
	
	;update ther node pointers for the 4 corners
	add r9,r9,#17+1				; move 'top left'  - right and down
	add r10,r10,#17-1			; move 'top right' - left and down
	sub r11,r11,#17-1			; move 'bot left'  - right and up 
	sub r12,r12,#17+1			; move 'bot right' - left and up
	
	;update the grid x,y pointer to 'top left' corner
	add r1,r1,#8
	add r2,r2,#8
	
	b low_res_main_land_sort_loop

low_res_plot_upto_middle	
	
	cmp r8,#1
	bge low_res_continue_plot

	ldmfd sp!,{pc}^


;###########################################


;#####################################
;#									#
;#									#
;#	Flat Low res land plotter		#
;#									#
;#									#
;#####################################

plot_res_skip_middle
	DCD 0

land_x_pos_outer_3
	DCD 0
land_y_pos_outer_3
	DCD 0
land_x_pos_outer_4
	DCD 0
land_y_pos_outer_4
	DCD 0
land_x_pos_outer_5
	DCD 0
land_y_pos_outer_5
	DCD 0
land_x_pos_outer_6
	DCD 0
land_y_pos_outer_6
	DCD 0
land_x_pos_outer_7
	DCD 0
land_y_pos_outer_7
	DCD 0

flat_low_res_sorted_land_plotter

;because all these distant land sections are flat there
;is no need to plot the list in any particular order

	stmfd sp!,{r14}
	
	ldr r0,land_resolution
	cmp r0,#3
	movne r0,#0
	moveq r0,#1
	str r0,plot_res_skip_middle
	
	mov r0,#1
	str r0,land_size_flat

	mov r0,#2
	str r0,land_res_flat

	mov r0,#1536
	add r0,r0,#131
	;node ref = 1667
	
	ldr r1,land_x_pos_outer_3
	ldr r2,land_y_pos_outer_3
	
	mov r5,#16	; step rate
	
	mov r6,#5
	str r6,flat_shade
	
	;layer 3
	bl flat_low_res_plotter

;################################################

	mov r0,#0
	str r0,plot_res_skip_middle

	mov r0,#2
	str r0,land_size_flat

	mov r0,#1
	str r0,land_res_flat

	mov r0,#2048
	sub r0,r0,#92
	;node ref = 1956
	
	ldr r1,land_x_pos_outer_4
	ldr r2,land_y_pos_outer_4
	
	mov r5,#32	; step rate

	mov r6,#3
	str r6,flat_shade
	
	;layer 4
	bl flat_low_res_plotter
	
;################################################

	;Is this land section to be plotted or wot ?
	ldr r0,land_resolution
	cmp r0,#0
	ldmeqfd sp!,{pc}^
	
	mov r0,#3
	str r0,land_size_flat

	mov r0,#2048
	add r0,r0,#197
	;node ref = 2245
	
	ldr r1,land_x_pos_outer_5
	ldr r2,land_y_pos_outer_5
	
	mov r5,#64	; step rate

	mov r6,#2
	str r6,flat_shade
	
	;layer 5
	bl flat_low_res_plotter

;################################################

	;Is this land section to be plotted or wot ?
	ldr r0,land_resolution
	cmp r0,#1
	ldmlefd sp!,{pc}^

	mov r0,#4
	str r0,land_size_flat

	mov r0,#2560
	sub r0,r0,#26
	;node ref = 2534
	
	ldr r1,land_x_pos_outer_6
	ldr r2,land_y_pos_outer_6
	
	mov r5,#128	; step rate

	mov r6,#1
	str r6,flat_shade
	
	;layer 6
	bl flat_low_res_plotter

;################################################

	mov r0,#0
	str r0,land_res_flat

	;Is this land section to be plotted or wot ?
	ldr r0,land_resolution
	cmp r0,#2
	ldmlefd sp!,{pc}^
	
	mov r0,#5
	str r0,land_size_flat

	mov r0,#3072
	sub r0,r0,#249
	;node ref = 2823
	
	ldr r1,land_x_pos_outer_7
	ldr r2,land_y_pos_outer_7
	
	mov r5,#256	; step rate

	mov r6,#0
	str r6,flat_shade
	
	;layer 7
	bl flat_low_res_plotter
		
	ldmfd sp!,{pc}^

;####################################################


plot_land_constants
	
	;Sets up addresses of constants that are
	;required - called once only on game bootup
	
	stmfd sp!,{r4-r12,r14}
	
	adr r14,rotated_coords
	ldmia r0!,{r1-r10}	
	stmia r14!,{r1-r10}
	
	ldr r1,cel_quad
	add r1,r1,#4
	str r1,cel_quad_plus_4
	
	ldr r1,quick_height_table
	add r1,r1,#17*16
	str r1,quick_height_table_land
	
	ldmfd sp!,{r4-r12,pc}^

cel_quad_plus_4
	DCD 0

rotated_coords			;0		1
	DCD 0
screen_coords			;4		2
	DCD 0
landscape_heights		;8		3
	DCD 0
perspective_table		;12		4
	DCD 0
quick_height_table		;16		5
	DCD 0
poly_map				;20		6
	DCD 0
sprite_map				;24		7
	DCD 0
cel_quad				;28		8
	DCD 0
cosine_table			;32		9
	DCD 0
space_mission_or_wot	;36		10
	DCD 0

;Land height lookup table starting at land base height (17 land units upwards)
quick_height_table_land
	DCD 0




flat_low_res_plotter
	
	stmfd sp!,{r14}
	
	;r0 - node ref
	;r1 - land x pos
	;r2 - land y pos
	
	mov r3,#15		;horizontal counter
	mov r4,#15		;vertical counter
	
	;r5 land step rate
	
flat_low_res_main_land_sort_loop

	stmfd sp!,{r0-r5}
	bl plot_land_section_flat
	ldmfd sp!,{r0-r5}
	
	subs r3,r3,#1		;check horizontal counter
	
	addpl r0,r0,#1		;add onto node ref pointer
	
	addpl r1,r1,r5		;move grid x pointer
	andpl r1,r1,#255
		
	bpl flat_low_res_main_land_sort_loop
	
next_vertical_low_res_flat	

	subs r4,r4,#1
	ldmmifd sp!,{pc}^	;have we done all vertical lines
	
	mov r3,#15			;reset horizontal counter
	
	sub r1,r1,r5, asl #4	;skip grid x pointer back to start off next line
	add r1,r1,r5			;15 land step units
	and r1,r1,#255
	
	add r0,r0,#2		;get node ref pointer onto next line
	
	add r2,r2,r5		;move grid y pointer
	and r2,r2,#255
	
	ldr r6,plot_res_skip_middle
	cmp r6,#0
	bne flat_low_res_main_land_sort_loop
	
	cmp r4,#4
	ble flat_low_res_main_land_sort_loop
	
	cmp r4,#11
	bge flat_low_res_main_land_sort_loop

flat_low_res_land_sort_loop_without_middle
	
	stmfd sp!,{r0-r5}
	bl plot_land_section_flat	
	ldmfd sp!,{r0-r5}
	
	cmp r3,#11
	beq flat_low_res_middle_skip
	
	sub r3,r3,#1
	
	cmp r3,#0			;check horizontal counter
	
	addge r1,r1,r5		;move grid x pointer
	andge r1,r1,#255
	
	addge r0,r0,#1		;move node pointer
	
	bge	flat_low_res_land_sort_loop_without_middle 
	
	b next_vertical_low_res_flat	

	
flat_low_res_middle_skip
	
	;skip over 6 land sections in the middle of this grid
	
	add r0,r0,#7			;move node pointer
	
	sub r3,r3,#7			;new value for hoz counter
	
	add r1,r1,r5, asl #3	;move grid pointer by 7 units
	sub r1,r1,r5
	and r1,r1,#255
	
	b flat_low_res_land_sort_loop_without_middle



;################################
;#								#
;#	High res land section plot	#
;#								#
;################################

test_grid_plot
	DCD 0

plot_land_section

;r0 - top left corner ref of section to plot
;r1 - land_x_pos
;r2 - land_y_pos

;node grid size = 33 * 33
;max sprites in grid 32 * 32
;x and y size 4096 per grid section
;z size defined by landscape_heights
;1 landscape_heighs unit = 256
;45 degree incline = 16 height units

	
	ldr r12,rotated_coords ; base adr for pre rotated landscape nodes 
	;scale up value of counter by 12 to match long [ 3 ] [ xx ] array
	add r12,r12,r0, asl #3
	add r12,r12,r0, asl #2
	
	
	;check to see if the 4 nodes for this section are in front of camera
	
	ldr r3,[r12,#4]			;get y distance of 1st node 

;is this node well behind the camera
	cmn r3,#8192	
	movlt pc,r14
;is this section a possible z clip
	cmp r3,#512
	blt possible_land_section_z_clip
	
	
	;check to see if the land section is off the left or right
	;of the screen by checking its unsigned x against y + constant
	
	ldr r4,[r12,#0]			;get x distance
	cmp r4,#0
	rsbmi r4,r4,#0
	add r3,r3,#10240
	cmp r3,r4
	movlt pc,r14
	
	;check to see if the land section is above or below the screen
	;using the unsigned z
	ldr r4,[r12,#8]
	cmp r4,#0
	rsbmi r4,r4,#0
	cmp r3,r4
	movlt pc,r14
	
	;if the land section is well in front of the camera then
	;there is no need for the following checks
	cmp r3,#0
	bgt land_section_in_front_of_camera
	
	;check the other 3 nodes for this land section
	
	ldr r3,[r12,#4+12]
	cmn r3,#8192
	movlt pc,r14
	cmp r3,#512
	blt possible_land_section_z_clip

	ldr r3,[r12,#4+33*12]
	cmn r3,#8192
	movlt pc,r14
	cmp r3,#512
	blt possible_land_section_z_clip

	ldr r3,[r12,#4+34*12]
	cmn r3,#8192
	movlt pc,r14
	cmp r3,#512
	blt possible_land_section_z_clip

land_section_in_front_of_camera

	;store the link reg
	stmfd sp!,{r14}
	
	;The waves are only updated for the visible sections
	;which are on screen - including sections that fail
	;on the back-face removal 
	;update the wave movement if height is <= 16
	
	;limit grid x n' y pos (r1 - r2) to 255 to allow wrap arround 
	and r1,r1,#255
	and r2,r2,#255

	ldr r11,landscape_heights
	add r12,r1,r2, asl #8
	ldrb r3,[r11,r12]		; height of base corner
	cmp r3,#16				; is it in the sea
	blle update_waves
	
	;store this data for later
	stmfd sp!,{r0-r3}

	;get the screen coords for this section
	;the data be in the order base + 33 , + 32 , + 1 , +0
	
	ldr r14,screen_coords		;long [ 2 ] [ xxx. ] array
	add r14,r14,r0, asl #3		;add on count * 8
	
	;get the 1st and 2nd pairs of screen coords
	add r0,r14,#33*8
	ldmia r0,{r0-r3}
	
	;get the 3rd
	add r4,r14,#8
	ldmia r4,{r4-r5}
	
	;get the 4th
	ldmia r14,{r6-r7}
	
;Checks 3 pairs of x,y screen coords and returns pl or mi
;for clockwise / anticlockwise backface removal
;Input coords r0 - r7 kept intact
;Uses r8-r10
	
	;check r0-r5 for vector sign
	sub r8,r2,r0
	sub r10,r4,r2
	subs r9,r5,r3
	rsbmi r9,r9,#0
	mul r9,r8,r9
	rsbmi r9,r9,#0
	subs r8,r3,r1
	rsbmi r8,r8,#0
	mul r8,r10,r8
	rsbmi r8,r8,#0
	subs r9,r9,r8
	blmi check_second_vector
	
	;check if polygon is off screen
	
	;is it off the bottom
	cmp r1,#120
	cmpgt r3,#120
	cmpgt r5,#120
	cmpgt r7,#120
	bgt land_section_failed
	
	;is it off the top
	cmn r1,#120
	cmnlt r3,#120
	cmnlt r5,#120
	cmnlt r7,#120
	blt land_section_failed

	;is it off the left
	cmn r0,#160
	cmnlt r2,#160
	cmnlt r4,#160
	cmnlt r6,#160
	blt land_section_failed
	
	;is it off the right
	cmp r0,#160
	cmpgt r2,#160
	cmpgt r4,#160
	cmpgt r6,#160
	bgt land_section_failed
	
	;store the coords in the cel data block
	ldr r14,cel_quad_plus_4
	stmia r14,{r0-r7}

	;load back data
	ldmfd sp,{r0-r3}
	;r0 - corner ref
	;r1 - grid x pos
	;r2 - grid y pos
	;r3 - corner height
	;r11 - landscape_heights
	;r12 - grid pos
	

	;calc the landscape shade - value between 0 - 7
	;based on the relative height change along the x and y
	
	;get the height value of x pos + 1
	add r6,r1,#1
	and r6,r6,#255
	add r6,r6,r2, asl #8
	ldrb r6,[r11,r6] 
	sub r6,r3,r6    ; find relative height with base corner
	
	;get the height value of y pos +1
	add r7,r2,#1
	and r7,r7,#255
	add r7,r1,r7, asl #8
	ldrb r7,[r11,r7]
	sub r7,r3,r7    ; find relative height
	
	;find relative height change for x n' y - add n' scale them
	add r6,r6,r7
	cmp r3,#16				;is it a wave
	suble r6,r6,r6, asr #1	;if so less shade
	mov r6,r6, asr #1

	ldr r7,hill_shade_offset
	add r6,r6,r7

	;store the shade value in the polygon data block
	;this value is limited between 0 - 7 at the poly end
	sub r0,r14,#4 ; copy of cel_quad kept from earlier
	str r6,[r0,#40]

	
	;and finally lets plot that polygon
		
	ldr r1,sprite_map			; base adr of sprite map ref grid ( char 64k )
	ldrb r1,[r1,r12]			; get the sprite ref no. for this grid pos
	
	ldr r6,test_grid_plot
	cmp r6,#0
	movne r1,#13
	
	bl arm_addpolycel16
	
land_section_failed

	ldmfd sp!,{r10-r12,r14}
	
	ldmfd sp!,{pc}


check_second_vector
	sub r8,r6,r4
	sub r10,r0,r6
	subs r9,r1,r7
	rsbmi r9,r9,#0
	mul r9,r8,r9
	rsbmi r9,r9,#0
	subs r8,r7,r5
	rsbmi r8,r8,#0
	mul r8,r10,r8
	rsbmi r8,r8,#0
	subs r9,r9,r8
	bmi land_section_failed
	mov pc,r14


wave_counter
	DCD 0
wave_counter2
	DCD 0
wave_counter3
	DCD 0


update_waves
	;mov pc,r14
	ldr r9,cosine_table
	
	ldr r7,wave_counter
	add r7,r7,r2, asl #7
	add r7,r7,r1, asl #6
	mov r8,#1024
	sub r8,r8,#1
	and r7,r7,r8
	ldr r7,[r9,r7, asl #2]
	mov r6,r7, asr #9
	
	ldr r7,wave_counter2
	sub r7,r7,r1, asl #7
	and r7,r7,r8
	ldr r7,[r9,r7, asl #2]
	;add r6,r6,r7, asr #10
	
	add r6,r6,#8
	strb r6,[r11,r12]
	mov pc,r14

possible_land_section_z_clip
	mov pc,r14

hill_shade_offset
	DCD 0


;################################
;#								#
;#	Mid res land section plot	#
;#								#
;################################
	
plot_land_section_2

;r0 - top left corner ref of section to plot
;r1 - land_x_pos
;r2 - land_y_pos

;node grid size = 17 * 17
;max sprites in grid 16 * 16 - mid section 6 * 6
;x and y size 4096*4 per grid section
;z size defined by landscape_heights
;1 landscape_heighs unit = 256
;45 degree incline = 16*4 height units

	
	ldr r12,rotated_coords ; base adr for pre rotated landscape nodes 
	;scale up value of counter by 12 to match long [ 3 ] [ xx ] array
	add r12,r12,r0, asl #3
	add r12,r12,r0, asl #2
	
	
	;check to see if the 4 nodes for this section are in front of camera
	
	ldr r3,[r12,#4]			;get y distance of 1st node 

;there is no need to check for clipping of
;these land sections as they are only plotted 
;in the distance at low level where a clip
;situation may occur

;is this node well behind the camera
	cmp r3,#8192*4	
	movlt pc,r14
	
	;check to see if the land section is off the left or right
	;of the screen by checking its unsigned x against y + constant
	
	ldr r4,[r12,#0]			;get x distance
	cmp r4,#0
	rsbmi r4,r4,#0
	add r3,r3,#10240*4
	cmp r3,r4
	movlt pc,r14
	
	;check to see if the land section is above or below the screen
	;using the unsigned z
	ldr r4,[r12,#8]
	cmp r4,#0
	rsbmi r4,r4,#0
	cmp r3,r4
	movlt pc,r14
	
	;if the land section is well in front of the camera then
	;there is no need for the following checks
	cmp r3,#0
	bgt land_section_in_front_of_camera_2
	
	;check the other 3 nodes for this land section
	
	ldr r3,[r12,#4+12]
	cmp r3,#8192*4
	movlt pc,r14

	ldr r3,[r12,#4+17*12]
	cmp r3,#8192*4
	movlt pc,r14

	ldr r3,[r12,#4+17*12]
	cmp r3,#8192*4
	movlt pc,r14

land_section_in_front_of_camera_2

	;store the link reg
	stmfd sp!,{r14}
	
	;The waves are not updated for these large sections

	;limit grid x n' y pos (r1 - r2) to 255 to allow wrap arround 
	and r1,r1,#252
	and r2,r2,#252
	
	;store this data for later
	stmfd sp!,{r0-r2}

	;get the screen coords for this section
	;the data be in the order base + 17 , + 16 , + 1 , +0
	
	ldr r14,screen_coords		;long [ 2 ] [ xxx. ] array
	add r14,r14,r0, asl #3		;add on count * 8
	
	;get the 1st and 2nd pairs of screen coords
	;add r0,r14,#17*8
	;ldmia r0,{r0-r3}
	
	ldmia r14,{r0-r3}
	
	
	
	;get the 3rd
	;add r4,r14,#8
	;ldmia r4,{r4-r5}
	add r4,r14,#18*8
	ldmia r4,{r4-r5}
	
	;get the 4th
	;ldmia r14,{r6-r7}
	add r6,r14,#17*8
	ldmia r6,{r6-r7}
	
	
;Checks 3 pairs of x,y screen coords and returns pl or mi
;for clockwise / anticlockwise backface removal
;Input coords r0 - r7 kept intact
;Uses r8-r10
	
	;check r0-r5 for vector sign
	sub r8,r2,r0
	sub r10,r4,r2
	subs r9,r5,r3
	rsbmi r9,r9,#0
	mul r9,r8,r9
	rsbmi r9,r9,#0
	subs r8,r3,r1
	rsbmi r8,r8,#0
	mul r8,r10,r8
	rsbmi r8,r8,#0
	subs r9,r9,r8
	blpl check_second_vector_2
	
	;check if polygon is off screen
	
	;is it off the bottom
	cmp r1,#120
	cmpgt r3,#120
	cmpgt r5,#120
	cmpgt r7,#120
	bgt land_section_failed_2
	
	;is it off the top
	cmn r1,#120
	cmnlt r3,#120
	cmnlt r5,#120
	cmnlt r7,#120
	blt land_section_failed_2

	;is it off the left
	cmn r0,#160
	cmnlt r2,#160
	cmnlt r4,#160
	cmnlt r6,#160
	blt land_section_failed_2
	
	;is it off the right
	cmp r0,#160
	cmpgt r2,#160
	cmpgt r4,#160
	cmpgt r6,#160
	bgt land_section_failed_2
	
	;store the coords in the cel data block
	ldr r14,cel_quad_plus_4
	stmia r14,{r0-r7}

	;load back data
	ldmfd sp,{r1-r3}
	;r1 - corner ref
	;r2 - grid x pos
	;r3 - grid y pos	

	;calc the landscape shade - value between 0 - 7
	;based on the relative height change along the x and y

	ldr r11,landscape_heights
	add r12,r2,r3, asl #8
	ldrb r5,[r11,r12]		; height of base corner

	
	;get the height value of x pos + 1
	add r6,r2,#4
	and r6,r6,#255
	add r6,r6,r3, asl #8
	ldrb r6,[r11,r6] 
	sub r6,r5,r6    ; find relative height with base corner
	
	;get the height value of y pos +1
	add r7,r3,#4
	and r7,r7,#255
	add r7,r2,r7, asl #8
	ldrb r7,[r11,r7]
	sub r7,r5,r7    ; find relative height
	
	;find relative height change for x n' y - add n' scale them
	add r6,r6,r7
	mov r6,r6, asr #3
	ldr r7,hill_shade_offset
	add r6,r6,r7

	;store the shade value in the polygon data block
	;this value is limited between 0 - 7 at the poly end
	sub r0,r14,#4 ; copy of cel_quad kept from earlier
	str r6,[r0,#40]
	
	;and finally lets plot that polygon
		
	ldr r1,sprite_map			; base adr of sprite map ref grid ( char 64k )
	
	;r0 cel_quad
	;r1 sprite map start address (known as grid)
	;r2 sprite map x pos (0-255 quad aligned)
	;r3 sprite map y pos

	bl arm_add4cel4
		
land_section_failed_2
	
	;check for any static objects on this grid section
	
	ldmfd sp!,{r10-r12}
	;r10 corner ref
	;r11 grid x pos
	;r12 grid y pos
		
	;Is there a object here
	ldr r9,poly_map
	
	ldmfd sp!,{pc}^


check_second_vector_2
	sub r8,r6,r4
	sub r10,r0,r6
	subs r9,r1,r7
	rsbmi r9,r9,#0
	mul r9,r8,r9
	rsbmi r9,r9,#0
	subs r8,r7,r5
	rsbmi r8,r8,#0
	mul r8,r10,r8
	rsbmi r8,r8,#0
	subs r9,r9,r8
	bpl land_section_failed_2
	mov pc,r14


;################################
;#								#
;#	low res land section plot	#
;#								#
;################################
	
plot_land_section_3

;r0 - top left corner ref of section to plot
;r1 - land_x_pos
;r2 - land_y_pos

;node grid size = 17 * 17
;max sprites in grid 16 * 16 - mid section 6 * 6
;x and y size 4096*4 per grid section
;z size defined by landscape_heights
;1 landscape_heighs unit = 256
;45 degree incline = 16*4 height units

	
	ldr r12,rotated_coords ; base adr for pre rotated landscape nodes 
	;scale up value of counter by 12 to match long [ 3 ] [ xx ] array
	add r12,r12,r0, asl #3
	add r12,r12,r0, asl #2
	
	
	;check to see if the 4 nodes for this section are in front of camera
	
	ldr r3,[r12,#4]			;get y distance of 1st node 

;there is no need to check for clipping of
;these land sections as they are only plotted 
;in the distance at low level where a clip
;situation may occur

;is this node well behind the camera
	cmp r3,#8192	
	movlt pc,r14
	
	;check to see if the land section is off the left or right
	;of the screen by checking its unsigned x against y + constant
	
	ldr r4,[r12,#0]			;get x distance
	cmp r4,#0
	rsbmi r4,r4,#0
	add r3,r3,#10240*4
	cmp r3,r4
	movlt pc,r14
	
	;check to see if the land section is above or below the screen
	;using the unsigned z
	ldr r4,[r12,#8]
	cmp r4,#0
	rsbmi r4,r4,#0
	cmp r3,r4
	movlt pc,r14
	
	;if the land section is well in front of the camera then
	;there is no need for the following checks
	cmp r3,#8192
	bgt land_section_in_front_of_camera_3
	
	;check the other 3 nodes for this land section
	
	ldr r3,[r12,#4+12]
	cmp r3,#8192
	movlt pc,r14

	ldr r3,[r12,#4+17*12]
	cmp r3,#8192
	movlt pc,r14

	ldr r3,[r12,#4+17*12]
	cmp r3,#8192
	movlt pc,r14

land_section_in_front_of_camera_3

	;store the link reg
	stmfd sp!,{r14}
	
	;The waves are not updated for these large sections

	;limit grid x n' y pos (r1 - r2) to 255 to allow wrap arround 
	and r1,r1,#252
	and r2,r2,#252
	
	;store this data for later
	stmfd sp!,{r0-r2}

	;get the screen coords for this section
	;the data be in the order base + 17 , + 16 , + 1 , +0
	
	ldr r14,screen_coords		;long [ 2 ] [ xxx. ] array
	add r14,r14,r0, asl #3		;add on count * 8
	
	;get the 1st and 2nd pairs of screen coords
	;add r0,r14,#17*8
	;ldmia r0,{r0-r3}
	
	ldmia r14,{r0-r3}
	
	
	
	;get the 3rd
	;add r4,r14,#8
	;ldmia r4,{r4-r5}
	add r4,r14,#18*8
	ldmia r4,{r4-r5}
	
	;get the 4th
	;ldmia r14,{r6-r7}
	add r6,r14,#17*8
	ldmia r6,{r6-r7}
	
	
;Checks 3 pairs of x,y screen coords and returns pl or mi
;for clockwise / anticlockwise backface removal
;Input coords r0 - r7 kept intact
;Uses r8-r10
	
	;check r0-r5 for vector sign
	sub r8,r2,r0
	sub r10,r4,r2
	subs r9,r5,r3
	rsbmi r9,r9,#0
	mul r9,r8,r9
	rsbmi r9,r9,#0
	subs r8,r3,r1
	rsbmi r8,r8,#0
	mul r8,r10,r8
	rsbmi r8,r8,#0
	subs r9,r9,r8
	blpl check_second_vector_3
	
	;check if polygon is off screen
	
	;is it off the bottom
	cmp r1,#120
	cmpgt r3,#120
	cmpgt r5,#120
	cmpgt r7,#120
	bgt land_section_failed_3
	
	;is it off the top
	cmn r1,#120
	cmnlt r3,#120
	cmnlt r5,#120
	cmnlt r7,#120
	blt land_section_failed_3

	;is it off the left
	cmn r0,#160
	cmnlt r2,#160
	cmnlt r4,#160
	cmnlt r6,#160
	blt land_section_failed_3
	
	;is it off the right
	cmp r0,#160
	cmpgt r2,#160
	cmpgt r4,#160
	cmpgt r6,#160
	bgt land_section_failed_3
	
	;store the coords in the cel data block
	ldr r14,cel_quad_plus_4
	stmia r14,{r0-r7}

	;load back data
	ldmfd sp,{r1-r3}
	;r1 - corner ref
	;r2 - grid x pos
	;r3 - grid y pos	

	;calc the landscape shade - value between 0 - 7
	;based on the relative height change along the x and y

	ldr r11,landscape_heights
	add r12,r2,r3, asl #8
	ldrb r5,[r11,r12]		; height of base corner

	
	;get the height value of x pos + 1
	add r6,r2,#8
	and r6,r6,#255
	add r6,r6,r3, asl #8
	ldrb r6,[r11,r6] 
	sub r6,r5,r6    ; find relative height with base corner
	
	;get the height value of y pos +1
	add r7,r3,#8
	and r7,r7,#255
	add r7,r2,r7, asl #8
	ldrb r7,[r11,r7]
	sub r7,r5,r7    ; find relative height
	
	;find relative height change for x n' y - add n' scale them
	add r6,r6,r7
	mov r6,r6, asr #4
	;ldr r0,land_shade
	;add r6,r6,#16
	ldr r7,hill_shade_offset
	add r6,r6,r7

	;store the shade value in the polygon data block
	;this value is limited between 0 - 7 at the poly end
	sub r0,r14,#4 ; copy of cel_quad kept from earlier
	str r6,[r0,#40]
	
	;and finally lets plot that polygon
		
	;ldr r1,sprite_map			; base adr of sprite map ref grid ( char 64k )
	
	;r0 cel_quad
	;r1 sprite map start address (known as grid)
	;r2 sprite map x pos (0-255 quad aligned)
	;r3 sprite map y pos

	
	;and finally lets plot that polygon
		
	;ldrb r1,[r1,r12]			; get the sprite ref no. for this grid pos
	
	;cel dat -  x , y grid pos and size 0 = 8 1 = 16 2 = 32 3 = 64 etc.
	;arm_addcelfrom512map (&cel_quad, 128, 128, 0);

	mov r1,r2
	mov r2,r3
	mov r3,#0
	ldr r3,land_size

	bl arm_addcelfrom512map
	
land_section_failed_3
	
	;check for any static objects on this grid section
	
	ldmfd sp!,{r10-r12}

	ldmfd sp!,{pc}^

land_shade
	DCD 0
land_size
	DCD 0


check_second_vector_3
	sub r8,r6,r4
	sub r10,r0,r6
	subs r9,r1,r7
	rsbmi r9,r9,#0
	mul r9,r8,r9
	rsbmi r9,r9,#0
	subs r8,r7,r5
	rsbmi r8,r8,#0
	mul r8,r10,r8
	rsbmi r8,r8,#0
	subs r9,r9,r8
	bpl land_section_failed_3
	mov pc,r14


;################################
;#	flat						#
;#	low res land section plot	#
;#								#
;################################
	
plot_land_section_flat

;r0 - top left corner ref of section to plot
;r1 - land_x_pos
;r2 - land_y_pos

;node grid size = 17 * 17
;max sprites in grid 16 * 16 - mid section 6 * 6
;x and y size 4096*4 per grid section
;z size defined by landscape_heights
;1 landscape_heighs unit = 256
;45 degree incline = 16*4 height units

	
	ldr r12,rotated_coords ; base adr for pre rotated landscape nodes 
	;scale up value of counter by 12 to match long [ 3 ] [ xx ] array
	add r12,r12,r0, asl #3
	add r12,r12,r0, asl #2
	
	
	;check to see if the 4 nodes for this section are in front of camera
	
	ldr r3,[r12,#4]			;get y distance of 1st node 

;there is no need to check for clipping of
;these land sections as they are only plotted 
;in the distance at low level where a clip
;situation may occur

;is this node well behind the camera
	cmp r3,#8192	
	movlt pc,r14
	
	;check to see if the land section is off the left or right
	;of the screen by checking its unsigned x against y + constant
	
	ldr r4,[r12,#0]			;get x distance
	cmp r4,#0
	rsbmi r4,r4,#0
	add r3,r3,#10240*4
	cmp r3,r4
	movlt pc,r14
	
	;check to see if the land section is above or below the screen
	;using the unsigned z
	ldr r4,[r12,#8]
	cmp r4,#0
	rsbmi r4,r4,#0
	cmp r3,r4
	movlt pc,r14
	
	;if the land section is well in front of the camera then
	;there is no need for the following checks
	cmp r3,#8192
	bgt land_section_in_front_of_camera_flat
	
	;check the other 3 nodes for this land section
	
	ldr r3,[r12,#4+12]
	cmp r3,#8192
	movlt pc,r14

	ldr r3,[r12,#4+17*12]
	cmp r3,#8192
	movlt pc,r14

	ldr r3,[r12,#4+17*12]
	cmp r3,#8192
	movlt pc,r14

land_section_in_front_of_camera_flat

	;store the link reg
	stmfd sp!,{r14}
	
	;The waves are not updated for these large sections
	;Also they are flat so there is no vector check
	;limit grid x n' y pos (r1 - r2) to 255 to allow wrap arround 
	and r1,r1,#255
	and r2,r2,#255
	
	;store this data for later
	stmfd sp!,{r1-r2}

	;get the screen coords for this section
	;the data be in the order base + 17 , + 16 , + 1 , +0
	
	ldr r14,screen_coords		;long [ 2 ] [ xxx. ] array
	add r14,r14,r0, asl #3		;add on count * 8
		
	ldmia r14,{r0-r3}
		
	;get the 3rd
	;add r4,r14,#8
	;ldmia r4,{r4-r5}
	add r4,r14,#18*8
	ldmia r4,{r4-r5}
	
	;get the 4th
	;ldmia r14,{r6-r7}
	add r6,r14,#17*8
	ldmia r6,{r6-r7}
		
	;check if polygon is off screen
	
	;is it off the bottom
	cmp r1,#120
	cmpgt r3,#120
	cmpgt r5,#120
	cmpgt r7,#120
	bgt land_section_failed_flat
	
	;is it off the top
	cmn r1,#120
	cmnlt r3,#120
	cmnlt r5,#120
	cmnlt r7,#120
	blt land_section_failed_flat

	;is it off the left
	cmn r0,#160
	cmnlt r2,#160
	cmnlt r4,#160
	cmnlt r6,#160
	blt land_section_failed_flat
	
	;is it off the right
	cmp r0,#160
	cmpgt r2,#160
	cmpgt r4,#160
	cmpgt r6,#160
	bgt land_section_failed_flat
	
	;store the coords in the cel data block
	ldr r14,cel_quad_plus_4
	stmia r14,{r0-r7}

	;load back data
	ldmfd sp,{r1-r2}
	;r2 - grid x pos
	;r3 - grid y pos	


	;store the shade value in the polygon data block
	;this value is limited between 0 - 7 at the poly end
	sub r0,r14,#4 ; copy of cel_quad kept from earlier
	
	;fixed shade value
	ldr r6,flat_shade
	str r6,[r0,#40]
	
	
	;r0 cel_quad
	;r1 sprite map start address 
	;r2 sprite map x pos (0-255 quad aligned)
	;r3 sprite map y pos

	
	;and finally lets plot that polygon
	;cel dat -  x , y grid pos and size 0 = 16 1 = 32 2 = 64 etc.
	;arm_addcelfrom128map (&cel_quad, 128, 128, 0);

	ldr r3,land_size_flat

	ldr r4,land_res_flat
	cmp r4,#1
	bne low_high_res_flat_land

	bl arm_addcelfrom128map
	
land_section_failed_flat
	
	ldmfd sp!,{r10-r11}

	ldmfd sp!,{pc}^

low_high_res_flat_land
	
	blt low_res_flat_land
	
	bl arm_addcelfrom512map

	ldmfd sp!,{r10-r11}

	ldmfd sp!,{pc}^
	
low_res_flat_land

	bl arm_addcelfrom32map

	ldmfd sp!,{r10-r11}

	ldmfd sp!,{pc}^


land_res_flat
	DCD 0

flat_shade
	DCD 0

land_size_flat
	DCD 0

	END
	