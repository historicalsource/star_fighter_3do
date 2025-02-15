	AREA |c$$code|,CODE,READONLY


	EXPORT check_collision
	EXPORT setup_collision_constants
	EXPORT find_ground_height
	EXPORT big_ship_collision_check
	EXPORT test_coll_count
	EXPORT bonus_collision_ref
	
	IMPORT height_map
	IMPORT poly_map
	IMPORT graphics_data
	IMPORT oppo_rotate_node_from_c

;Graphics_details structure - see file Graphic_Struct.h

clip_size		EQU		0
score			EQU		4
graphic_adr		EQU		8
explosion_adr	EQU		12
					
collision_adr	EQU		16
hits_counter	EQU		20
x_size			EQU		24
y_size			EQU		28
					
z_size			EQU		32
laser_data		EQU		36
missile_data	EQU		40
ship_data		EQU		44
					
smoke_data		EQU		48
thruster_data	EQU		52
missile_aim		EQU		56
ship_2_data		EQU		60

;Collision pointers for static graphics bigger than 1 poly_map unit
;see file Setup_Tables.c

COLL_POINTER	EQU		248
COLL_UP_RIGHT	EQU		248
COLL_UP			EQU		249
COLL_UP_LEFT	EQU		250
COLL_LEFT		EQU		251
COLL_DOWN_LEFT	EQU		252
COLL_DOWN		EQU		253
COLL_DOWN_RIGHT	EQU		254
COLL_RIGHT		EQU		255

;ship_stack structure - see file Ship_Struct.h

ship_header		EQU		12
ship_x_pos		EQU		0
ship_y_pos		EQU		4
ship_z_pos		EQU		8
ship_x_rot		EQU		12
ship_y_rot		EQU		16
ship_z_rot		EQU		20
ship_type		EQU		24

						
height_map_data
	DCD height_map
poly_map_data
	DCD poly_map

find_ground_height
	;r0 - r1 x,y position
	;return r0 - ground height
	stmfd sp!,{r4-r12,r14}
	bl find_height
	mov r0,r7
	ldmfd sp!,{r4-r12,pc}^
	
check_collision
	;Input r0-r2 x,y,z pos in ship units
	;ie 1<<24 = 1 map unit
	
	;mov r0,#0
	;mov pc,r14	;test bail out
	
	;If yer too high its a confirmed fail
	cmp r2,#(1<<29)
	movgt r0,#0
	movgt pc,r14
	
	stmfd sp!,{r4-r12,r14}
	
	;check poly map
	add r3,r0,#16384*1024
	add r4,r1,#16384*1024
	
	mov r3,r3, lsr #25
	mov r4,r4, lsr #25
	add r5,r3,r4, asl #7
	ldr r6,poly_map_data
	ldrb r7,[r6,r5]
	cmp r7,#0		;is there an object here
	bne check_static_object_collision
	
check_collision_with_hills
	
	bl find_height
	
	;check the ground height v collision z
	cmp r7,r2
	
	;output negative height of hill
	;this is because position values output
	;refers to object collision grid pos
	mvngt r0,r7
	movle r0,#0
	
	ldr r4,space_mission_or_wot
	cmp r4,#1
	moveq r0,#0
		
	ldmfd sp!,{r4-r12,pc}^

find_height

	stmfd sp!,{r14}
	;r0-r1 x,y pos
	;out r7 ground height at this point
	
	;scale down the x,y pos top map units
	mov r3,r0, lsr #24
	mov r4,r1, lsr #24
	
	;find the map height of 4 corner nodes at this pos
	ldr r6,height_map_data
	add r5,r3,r4, asl #8	;grid ref
	ldrb r7,[r6,r5]			;top left
	subs r7,r7,#17
	movmi r7,#0
	
	add r3,r3,#1
	and r3,r3,#255
	add r5,r3,r4, asl #8
	ldrb r8,[r6,r5]			;top right
	subs r8,r8,#17
	movmi r8,#0
	
	add r4,r4,#1
	and r4,r4,#255
	add r5,r3,r4, asl #8
	ldrb r9,[r6,r5]			;bottom right
	subs r9,r9,#17
	movmi r9,#0
	
	sub r3,r3,#1
	and r3,r3,#255
	add r5,r3,r4, asl #8
	ldrb r10,[r6,r5]		;bottom left
	subs r10,r10,#17
	movmi r10,#0
	
	;find position within grid section - r11,r12
	mov r11,r0, lsl #8	;wipe the top 8 bits
	mov r12,r1, lsl #8
	;scale position down to a sensible size (1<<10 - 1 grid unit)
	mov r11,r11, lsr #22
	mov r12,r12, lsr #22
	
	;find vertical height change across section
	sub r3,r10,r7	;left
	sub r4,r9,r8	;right
	
	;scale height change by grid y pos
	mul r0,r3,r12
	mul r1,r4,r12
	
	;scale scaled y height changes by inverse x pos and x pos
	rsb r5,r11,#1024
	mul r14,r0,r5
	mla r14,r1,r11,r14
	;r14 = height add onto top left node for y
	
	;find horizontal height change across section
	sub r3,r8,r7
	sub r4,r9,r10
	
	;scale height change by x grid pos
	mul r0,r3,r11
	mul r1,r4,r11
	
	;scale scaled x height changes by inverse y pos and y pos
	rsb r5,r12,#1024
	mla r14,r0,r5,r14
	mla r14,r1,r12,r14
	
	mov r14,r14, asl #1
	
	;find final height based on top left corner node r7 + offset
	adds r7,r14,r7, asl #21
	movmi r7,#0
	
	ldmfd sp!,{pc}
	
check_coll_coords
	DCD 0,0,0
space_mission_or_wot
	DCD 0


setup_collision_constants
	str r0,space_mission_or_wot
	
	;Get start adr of static object data
	ldr r0,graphics_data_adr
	ldr r1,[r0,#0]
	add r0,r0,r1
	str r0,static_object_start_adr
	
	;Get start adr of ship data
	ldr r0,graphics_data_adr
	ldr r1,[r0,#4]
	add r0,r0,r1
	str r0,ship_start_adr
		
	mov pc,r14

graphics_data_adr
	DCD graphics_data

static_object_start_adr
	DCD 0
ship_start_adr
	DCD 0

poly_map_grid_ref
	DCD 0

found_collision_pointer

	;Which pointer is it and what do we do ?
	cmp r7,#COLL_UP_RIGHT
	beq collision_pointer_up_right
	
	cmp r7,#COLL_UP
	beq collision_pointer_up
	
	cmp r7,#COLL_UP_LEFT
	beq collision_pointer_up_left
	
	cmp r7,#COLL_LEFT
	beq collision_pointer_left
	
	cmp r7,#COLL_DOWN_LEFT
	beq collision_pointer_down_left
	
	cmp r7,#COLL_DOWN
	beq collision_pointer_down
	
	cmp r7,#COLL_DOWN_RIGHT
	beq collision_pointer_down_right
	
	;If its not any of the others it must be this on !
	;cmp r7,#COLL_RIGHT
	b collision_pointer_right
	
collision_pointer_up_right
	
	;move map pointers
	add r3,r3,#1
	add r4,r4,#1
	and r3,r3,#127
	and r4,r4,#127
	
	;find poly_map grid ref
	add r5,r3,r4, asl #7
	
	;get type
	ldrb r7,[r6,r5]
	
	;Is it another collision pointer
	cmp r7,#COLL_POINTER
	bge found_collision_pointer
	
	;No its not it must be a object
	b updated_collision_pointer

	
collision_pointer_up
	
	;move map pointers
	add r4,r4,#1
	and r4,r4,#127
	
	;find poly_map grid ref
	add r5,r3,r4, asl #7
	
	;get type
	ldrb r7,[r6,r5]

	;Is it another collision pointer
	cmp r7,#COLL_POINTER
	bge found_collision_pointer
	
	;No its not it must be a object
	b updated_collision_pointer
	

collision_pointer_up_left
	
	;move map pointers
	sub r3,r3,#1
	add r4,r4,#1
	and r3,r3,#127
	and r4,r4,#127
	
	;find poly_map grid ref
	add r5,r3,r4, asl #7
	
	;get type
	ldrb r7,[r6,r5]
	
	;Is it another collision pointer
	cmp r7,#COLL_POINTER
	bge found_collision_pointer
	
	;No its not it must be a object
	b updated_collision_pointer

collision_pointer_left
	
	;move map pointers
	sub r3,r3,#1
	and r3,r3,#127
	
	;find poly_map grid ref
	add r5,r3,r4, asl #7
	
	;get type
	ldrb r7,[r6,r5]
	
	;Is it another collision pointer
	cmp r7,#COLL_POINTER
	bge found_collision_pointer
	
	;No its not it must be a object
	b updated_collision_pointer

collision_pointer_down_left
	
	;move map pointers
	sub r3,r3,#1
	sub r4,r4,#1
	and r3,r3,#127
	and r4,r4,#127
	
	;find poly_map grid ref
	add r5,r3,r4, asl #7
	
	;get type
	ldrb r7,[r6,r5]
	
	;Is it another collision pointer
	cmp r7,#COLL_POINTER
	bge found_collision_pointer
	
	;No its not it must be a object
	b updated_collision_pointer


collision_pointer_down
	
	;move map pointers
	sub r4,r4,#1
	and r4,r4,#127
	
	;find poly_map grid ref
	add r5,r3,r4, asl #7
	
	;get type
	ldrb r7,[r6,r5]
	
	;Is it another collision pointer
	cmp r7,#COLL_POINTER
	bge found_collision_pointer
	
	;No its not it must be a object
	b updated_collision_pointer

collision_pointer_down_right
	
	;move map pointers
	add r3,r3,#1
	sub r4,r4,#1
	and r3,r3,#127
	and r4,r4,#127
	
	;find poly_map grid ref
	add r5,r3,r4, asl #7
	
	;get type
	ldrb r7,[r6,r5]
	
	;Is it another collision pointer
	cmp r7,#COLL_POINTER
	bge found_collision_pointer
	
	;No its not it must be a object
	b updated_collision_pointer

collision_pointer_right
	
	;move map pointers
	add r3,r3,#1
	and r3,r3,#127
	
	;find poly_map grid ref
	add r5,r3,r4, asl #7
	
	;get type
	ldrb r7,[r6,r5]
	
	;Is it another collision pointer
	cmp r7,#COLL_POINTER
	bge found_collision_pointer
	
	;No its not it must be a object
	b updated_collision_pointer

	
	
check_static_object_collision
	;r0-r2 collision check point
	;r3-r4 x,y polymap pos
	;r5 poly_map grid ref
	;r6 poly_map start adr
	;r7 graphic type
	
	
	;Is it a collision pointer or wot ?
	cmp r7,#COLL_POINTER
	bge found_collision_pointer
updated_collision_pointer
	
	;Is this object a cloud or wot ?
	cmp r7,#64
	bge check_collision_with_hills	;yes - go back with no checks
	
	;Is there an object here ?
	;This check is needed because a collision pointer may point to a
	;object that has been blown up
	cmp r7,#0
	beq check_collision_with_hills	;yes - go back with no checks
	
	;stash the collision coords maybe to check against
	;ground if object collision fails
	adr r8,check_coll_coords
	stmia r8,{r0-r2}
	
	str r5,poly_map_grid_ref
	
	;get height of static object
	mov r8,r3, asl #1
	add r8,r8,r4, asl #9
	ldr r9,height_map_data
	ldrb r5,[r9,r8]
	;sea level <= 16 - checked as 0 height
	subs r5,r5,#17
	movmi r5,#0
	movpl r5,r5, asl #21
	;r5 - height off sea level

	;x , y centre point of object
	mov r3,r3, lsl #25
	mov r4,r4, lsl #25

	;r3-r5 x,y,z coords of static
	;find relative distance from object and check collision
	sub r0,r0,r3
	sub r1,r1,r4
	sub r2,r2,r5

	;get start address of collision data
	ldr r3,static_object_start_adr
	add r3,r3,r7, asl #6
	ldr r3,[r3,#collision_adr]

	bl check_collision_boxes

	cmp r0,#0

	;Has the collision failed - if so check the hills	
	adreq r0,check_coll_coords
	ldmeqia r0,{r0-r2}
	beq check_collision_with_hills

	;Its a hit with this static graphics - get grid ref and exit
	ldr r0,poly_map_grid_ref
	ldmfd sp!,{r4-r12,pc}^
	
check_collision_boxes
	;r0-r2 relative distance from centre of object
	;r3 - start address of collision data
	
	;collision data format
	;word		byte		use
	;0			0			Number of boxes -1 (ie. 0 = 1 box)
	;1			4			Free
	;2			8			Free
	;-----------------------start of data
	;3			12			Box Ref		(1=Normal 2=Docking Bay)
	;4			16			X check 1
	;5			20			Y check 1
	;6			24			Z check 1
	;7			28			X check 2
	;8			32			Y check 2
	;9			36			Z check 2
	;-----------------------Next box data - 28 bytes per box
	;10			40			Ref
	;11			44			X 1
	;12			48			Y 1
	;13			52			Z 1
	;14			56			X 2
	;15			60			Etc.......
	
	ldr r4,[r3],#12			;number of boxes + move on pointer
	
;	str r4,test_coll_count
	
collision_box_loop
	ldmia r3!,{r5-r11}
	cmp r0,r6
	cmpgt r1,r7
	cmpgt r2,r8
	blt collision_box_failed
	cmp r0,r9
	cmplt r1,r10
	cmplt r2,r11
	
	blt collision_box_hit
	
collision_box_failed
	subs r4,r4,#1
	bpl collision_box_loop

	;Its failed this collision
	mov r0,#0
	str r0,bonus_collision_ref
	
	mov pc,r14
	
collision_box_hit
	
	ldrb r5,[r3,#-28]
	str r5,bonus_collision_ref
	
	add r0,r5,#1	;coll ref
	mov pc,r14

test_coll_count
	DCD 0

bonus_collision_ref
	DCD 0

big_ship_coords
	DCD 0,0,0
big_ship_rotations
	DCD 0,0,0
big_ship_type
	DCD 0
	
big_ship_collision_check
	;r0 - adr of big ship
	;r1 - adr of small ship
	
	stmfd sp!,{r4-r12,r14}		;stash this lot as it's called from C
	
	add r0,r0,#ship_header	;add on link data headers
	add r1,r1,#ship_header
	
	ldr r2,[r0,#ship_type]	;get the big ship type and store it for later
	str r2,big_ship_type
	
	ldmia r0,{r3-r5}	;get big ships x,y,z pos
	ldmia r1,{r6-r8}	;get small ships x,y,z pos
	sub r3,r3,r6		;work out relative distance
	sub r4,r4,r7
	sub r5,r8,r5
		
	adr r9,big_ship_coords
	stmia r9!,{r3-r5}
	
	;Get the big ships rotation and reverse them 
	add r0,r0,#ship_x_rot
	ldmia r0,{r0-r2}
	
	rsb r1,r1,#1024*1024
	
	stmia r9,{r0-r2}
	
	adr r0,big_ship_coords
	bl oppo_rotate_node_from_c
	
	;get the rotated coords for this ship
	adr r0,big_ship_coords
	ldmia r0,{r0-r2}
	
	ldr r4,big_ship_type

	;get start address of collision data
	ldr r3,ship_start_adr
	add r3,r3,r4, asl #6
	ldr r3,[r3,#collision_adr]

	bl check_collision_boxes

	ldmfd sp!,{r4-r12,pc}^
	
	END
