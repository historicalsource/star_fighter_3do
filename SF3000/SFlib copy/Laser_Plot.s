	AREA |c$$code|,CODE,READONLY

	EXPORT		plot_laser

	IMPORT		clip_3d_line
	IMPORT		camera_x_position		
	IMPORT		cel_quad
	IMPORT		arm_addpolycel32
	IMPORT		pex_table
	IMPORT		rotate_node_x_y_z_camera

LASER_POWER_1		EQU		1
LASER_POWER_2		EQU		2
LASER_POWER_3		EQU		3
LASER_POWER_4		EQU		4
LASER_POWER_5		EQU		5
LASER_POWER_6		EQU		6
BEAM_LASER			EQU		7
POLY_CLIP_DIST		EQU		1280

;laser_stack structure - see file Laser_Struct.h

laser_link_header	EQU		12
laser_x_pos			EQU		0
laser_y_pos			EQU		4
laser_z_pos			EQU		8
laser_x_pos2		EQU		12
laser_y_pos2		EQU		16
laser_z_pos2		EQU		20
laser_type			EQU		24
laser_counter		EQU		28

clip_distance		EQU		2048

laser_shade			EQU		25

stack_pointer
	DCD 0
	
cel_quad_pointer
	DCD cel_quad
perspective_table
	DCD pex_table

laser_to_plot
	DCD 0

plot_laser

	stmfd sp!,{r4-r12,r14}

	;r0 - pointer to laser

	;add on header size to start of t' laser
	add r0,r0,#laser_link_header

	str r0,laser_to_plot

	;Calculate the rotated coords for the start
	ldmia r0,{r0-r2}
	adrl r3,camera_x_position
	ldmia r3,{r3-r5}
	sub r0,r0,r3
	sub r1,r1,r4
	sub r2,r5,r2
	
	mov r0,r0, asr #12
	mov r1,r1, asr #12
	mov r2,r2, asr #12
		
	bl rotate_node_x_y_z_camera

	;r0-r2 x,y,z coords for laser start
	
	;is the laser too near or behind the camera
	add r4,r1,#clip_distance
	movs r3,r0
	rsbmi r3,r3,#0
	cmp r3,r4
	ldmgtfd sp!,{r4-r12,pc}^
	
	movs r3,r2
	rsbmi r3,r3,#0
	cmp r3,r4
	ldmgtfd sp!,{r4-r12,pc}^
	
	;store the 1st coords
	stmfd sp!,{r0-r2}
	
	;Calculate the rotated coords for the end
	ldr r0,laser_to_plot
	add r0,r0,#laser_x_pos2
	ldmia r0,{r0-r2}
	adrl r3,camera_x_position
	ldmia r3,{r3-r5}
	sub r0,r0,r3
	sub r1,r1,r4
	sub r2,r5,r2
	
	mov r0,r0, asr #12
	mov r1,r1, asr #12
	mov r2,r2, asr #12
		
	bl rotate_node_x_y_z_camera

	;r0-r2 x,y,z coords for laser end
	
	;load back start point of laser r3-r5
	ldmfd sp!,{r3-r5}
	
	
	cmp r1,#POLY_CLIP_DIST
	blt clip_this_laser_1
	
	cmp r4,#POLY_CLIP_DIST
	blt clip_this_laser_2
	
	b dont_clip_this_laser
	
clip_this_laser_1

	cmp r4,#POLY_CLIP_DIST
	ldmltfd sp!,{r4-r12,pc}^

	bl clip_3d_line
	
	b dont_clip_this_laser
	
clip_this_laser_2
	
	bl clip_3d_line
	
dont_clip_this_laser
	
	;scale these points
	ldr r6,perspective_table
	bic r1,r1,#31
	bic r4,r4,#31
	ldr r11,[r6,r1, asr #3] ;end point scale
	ldr r12,[r6,r4, asr #3]	;start point scale
	
	mul r7,r0,r11
	mul r8,r2,r11
	mov r7,r7, asr #16
	mov r8,r8, asr #16
	mul r9,r3,r12
	mul r10,r5,r12
	mov r9,r9, asr #16
	mov r10,r10, asr #16
	
	;r7-r8 start x,y r9,r10 end x,y
	;r11 start scale r12 end scale
	
	;find x and y size of line and unsigned size
	;use these to calculate the lasers width
	subs r0,r7,r9
	rsbmi r2,r0,#0
	movpl r2,r0
	subs r1,r8,r10
	rsbmi r3,r1,#0
	movpl r3,r1
	
	;r0-r1 line x,y dist
	;r2-r3 unsigned line dist
	
	mvn r5,#127	;width
	mov r6,#128
	
	;Is the line less than 2 - 1 gradient
	cmp r2,r3, asl #1
	movgt r5,#0			;yes no x width
	
	;is the line greater than 1 - 2 grad
	cmp r3,r2, asl #1
	movgt r6,#0			;yes no y width
	
	;use signed size to flip width
	cmp r0,#0		;is the x size neg
	rsblt r6,r6,#0	;yes - flip y width
	
	cmp r1,#0		;is the y size neg
	rsblt r5,r5,#0	;yes - flip x width

	str sp,stack_pointer	;store the stack we need the regs
	
	;scale the width for start and end of laser
	mul r0,r5,r11
	mul r1,r6,r11
	mul r2,r5,r12
	mul r3,r6,r12
	mov r11,r0, asr #17
	mov r12,r1, asr #17
	mov r13,r2, asr #17
	mov r14,r3, asr #17
	
	;Make sure the polygon is not too small - otherwise the 3do plots nothing
	cmp r11,#0
	moveq r11,#1
	cmp r12,#0
	moveq r12,#1
	cmp r13,#0
	moveq r13,#1
	cmp r14,#0
	moveq r14,#1
	
	;r11-r12 start width
	;r13-r14 end width
	;r7-r8 start of line
	;r9-r10 end of line
	
	;now calculate the 4 points of the laser polygon
	add r0,r7,r11
	add r1,r8,r12
	add r2,r9,r13
	add r3,r10,r14
	sub r4,r9,r13
	sub r5,r10,r14
	sub r6,r7,r11
	sub r7,r8,r12
	
	;r0-r7 4 x,y screen coords for laser
	
	ldr sp,stack_pointer    ;get the stack back
	
	;check its on t screen
	
	;is it off the bottom
	cmp r1,#120
	cmpgt r3,#120
	cmpgt r5,#120
	cmpgt r7,#120
	ldmgtfd sp!,{r4-r12,pc}^
	
	;is it off the top
	cmn r1,#120
	cmnlt r3,#120
	cmnlt r5,#120
	cmnlt r7,#120
	ldmltfd sp!,{r4-r12,pc}^

	;is it off the left
	cmn r0,#160
	cmnlt r2,#160
	cmnlt r4,#160
	cmnlt r6,#160
	ldmltfd sp!,{r4-r12,pc}^
	
	;is it off the right
	cmp r0,#160
	cmpgt r2,#160
	cmpgt r4,#160
	cmpgt r6,#160
	ldmgtfd sp!,{r4-r12,pc}^

	;store coords in polygon data block
	ldr r12,cel_quad_pointer
	add r8,r12,#4
	stmia r8!,{r0-r7}
	
	;shade
	mov r0,#laser_shade
	str r0,[r8,#4]

	;plot polygon
	;ldr r0,cel_quad
	;ldr r1,type of polygon
	
	;What colour based on type
	ldr r1,laser_to_plot
	ldr r1,[r1,#laser_type]
	and r1,r1,#7
	
	rsb r1,r1,#159		;bitmap for polygon
	
	mov r0,r12	;cel_quad
	
	bl arm_addpolycel32
	
	ldmfd sp!,{r4-r12,pc}^

	END








