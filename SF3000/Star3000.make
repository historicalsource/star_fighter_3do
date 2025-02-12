#   File:       Star3000.make
#   Target:     Star3000
#   Sources:    Star3000.c
#               :SFlib:Bit_Control.c
#               :SFlib:Bonus_Control.c
#               :SFlib:PlayCPakStream.c
#               :SFlib:PrepareStream.c
#               :SFlib:Collision.s
#               :SFlib:Update_Frame.c
#               :SFlib:Draw_Frame.c
#               :SFlib:Draw_Land.c
#               :SFlib:Explosion.c
#               :SFlib:Ground_Control.c
#               :SFlib:Laser_Control.c
#               :SFlib:Maths_Stuff.s
#               :SFlib:Plot_Graphic.s
#               :SFlib:Plot_Land.s
#               :SFlib:Rotate_Land.c
#               :SFlib:Setup_Tables.c
#               :SFlib:SF_Access.c
#               :SFlib:SF_NVRam.c
#               :SFlib:SF_ARMAnim.s
#               :SFlib:SF_ARMBurn.s
#               :SFlib:SF_ARMCell.s
#               :SFlib:SF_ARMLink.s
#               :SFlib:SF_ARMSky.s
#               :SFlib:SF_ARMUtils.s
#               :SFlib:SF_Bonus.c
#               :SFlib:SF_Celutils.c
#               :SFlib:SF_Control.c
#               :SFlib:SF_Font.c
#               :SFlib:SF_Io.c
#               :SFlib:SF_Map.c
#               :SFlib:SF_Menu.c
#               :SFlib:SF_Message.c
#               :SFlib:SF_Music.c
#               :SFlib:SF_Palette.c
#               :SFlib:SF_Pyramid.c
#               :SFlib:SF_Screenutils.c
#               :SFlib:SF_Sound.c
#               :SFlib:SF_Status.c
#               :SFlib:SF_Utility.c
#               :SFlib:SF_Video.c
#               :SFlib:SF_War.c
#               :SFlib:Ship_Control.c
#               :SFlib:Smoke_Control.c
#               :SFlib:Sound_Control.c
#               :SFlib:Weapons.c
#               :SFlib:Collision_Update.c
#               :SFlib:Laser_Plot.s
#               :SFlib:Smoke_Plot.s
#               :SFlib:Ship_Command.c
#               :SFlib:Graphics_Set.c
#               :SFlib:bs_joystick.c
#               :SFlib:broker_shell.c
#               :SFlib:sf_joystick.c
#               :SFlib:test_prog.c
#   Created:    Monday, June 5, 1995 8:10:02 pm
#
#   Copyright 3DO Company, 1993, 1994
#   All rights reserved.
#
#		Change History (most recent first):
#
#   		<>		---		Created by Create3DOMake 2.4

#####################################
#	Symbol definitions
#####################################

App				= Star3000
DebugFlag		= 0
SourceDir		= Sylvester:Andrew-C:SF3000:
ObjectDir		= :Objects:
CC				= armcc
ASM				= armasm
LINK			= armlink
WorkingDisk		= ''
StreamDir		= Sylvester:3DO:Streaming:2p1p2:DataStream:
SubscriberDir	= {StreamDir}Subscribers:


#####################################
#	Default compiler options
#####################################

COptions			= -fa -zps1 -za1 -zpj1 -zpm1 -zpz1
SOptions			= -bi
LOptions			= -aif -r -b 0x00
LStackSize			= 8192

#####################################
#	Object files
#####################################

OBJECTS			=	 �
					{ObjectDir}Star3000.c.o �
					{ObjectDir}:SFlib:Bit_Control.c.o �
					{ObjectDir}:SFlib:Bonus_Control.c.o �
					{ObjectDir}:SFlib:PlayCPakStream.c.o �
					{ObjectDir}:SFlib:PrepareStream.c.o �
					{ObjectDir}:SFlib:Collision.s.o �
					{ObjectDir}:SFlib:Draw_Frame.c.o �
					{ObjectDir}:SFlib:Update_Frame.c.o �
					{ObjectDir}:SFlib:Draw_Land.c.o �
					{ObjectDir}:SFlib:Explosion.c.o �
					{ObjectDir}:SFlib:Ground_Control.c.o �
					{ObjectDir}:SFlib:Laser_Control.c.o �
					{ObjectDir}:SFlib:Maths_Stuff.s.o �
					{ObjectDir}:SFlib:Plot_Graphic.s.o �
					{ObjectDir}:SFlib:Plot_Land.s.o �
					{ObjectDir}:SFlib:Rotate_Land.c.o �
					{ObjectDir}:SFlib:Setup_Tables.c.o �
					{ObjectDir}:SFlib:SF_Access.c.o �
					{ObjectDir}:SFlib:SF_NVRam.c.o �
					{ObjectDir}:SFlib:SF_ARMAnim.s.o �
					{ObjectDir}:SFlib:SF_ARMBurn.s.o �
					{ObjectDir}:SFlib:SF_ARMCell.s.o �
					{ObjectDir}:SFlib:SF_ARMLink.s.o �
					{ObjectDir}:SFlib:SF_ARMSky.s.o �
					{ObjectDir}:SFlib:SF_ARMUtils.s.o �
					{ObjectDir}:SFlib:SF_Bonus.c.o �
					{ObjectDir}:SFlib:SF_Celutils.c.o �
					{ObjectDir}:SFlib:SF_Control.c.o �
					{ObjectDir}:SFlib:SF_Font.c.o �
					{ObjectDir}:SFlib:SF_Io.c.o �
					{ObjectDir}:SFlib:SF_Map.c.o �
					{ObjectDir}:SFlib:SF_Menu.c.o �
					{ObjectDir}:SFlib:SF_Message.c.o �
					{ObjectDir}:SFlib:SF_Music.c.o �
					{ObjectDir}:SFlib:SF_Palette.c.o �
					{ObjectDir}:SFlib:SF_Pyramid.c.o �
					{ObjectDir}:SFlib:SF_Screenutils.c.o �
					{ObjectDir}:SFlib:SF_Sound.c.o �
					{ObjectDir}:SFlib:SF_Status.c.o �
					{ObjectDir}:SFlib:SF_Utility.c.o �
					{ObjectDir}:SFlib:SF_Video.c.o �
					{ObjectDir}:SFlib:SF_War.c.o �
					{ObjectDir}:SFlib:Ship_Control.c.o �
					{ObjectDir}:SFlib:Smoke_Control.c.o �
					{ObjectDir}:SFlib:Sound_Control.c.o �
					{ObjectDir}:SFlib:Weapons.c.o �
					{ObjectDir}:SFlib:Collision_Update.c.o �
					{ObjectDir}:SFlib:Laser_plot.s.o �
					{ObjectDir}:SFlib:Smoke_plot.s.o �
					{ObjectDir}:SFlib:Ship_Command.c.o �
					{ObjectDir}:SFlib:Graphics_Set.c.o �
					{ObjectDir}:SFlib:bs_joystick.c.o �
					{ObjectDir}:SFlib:broker_shell.c.o �
					{ObjectDir}:SFlib:sf_joystick.c.o �
					{ObjectDir}:SFlib:test_prog.c.o �
					"{3DOLibs}"cstartup.o

LIBS			=	 �
					"{SubscriberDir}subscriber.lib"		�
					"{StreamDir}dataacq.lib"	�
					"{StreamDir}ds.lib"			�
					"{3DOLibs}codec.lib" �
					"{3DOLibs}Lib3DO.lib" �
					"{3DOLibs}audio.lib" �
					"{3DOLibs}music.lib" �
					"{3DOLibs}operamath.lib" �
					"{3DOLibs}filesystem.lib" �
					"{3DOLibs}graphics.lib" �
					"{3DOLibs}input.lib" �
					"{3DOLibs}clib.lib" �
					"{3DOLibs}swi.lib"

#####################################
#	Default build rules
#####################################

All				�	{App}

{ObjectDir}		�	:

.c.o	�	.c
	{CC} -i "{3DOIncludes}"  -i "{StreamDir}" -i "{SubscriberDir}Includes:" {COptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c

.s.o	�	.s
	{ASM} -i "{3DOIncludes}" -i "{StreamDir}" -i "{SubscriberDir}Includes:" {SOptions} -o {TargDir}{Default}.s.o {DepDir}{Default}.s


#####################################
#	Target build rules
#####################################

{App} � {App}.make {OBJECTS}
	{LINK} {LOptions} {LDebugOptions} �
		{OBJECTS} �
		{LIBS} �
		-o "{WorkingDisk}"{Targ}
	SetFile "{WorkingDisk}"{Targ} -c 'EaDJ' -t 'PROJ'
	modbin "{WorkingDisk}"{Targ} -stack {LStackSize} {ModbinDebugOptions}
	stripaif "{WorkingDisk}"{Targ} -o {Targ} -s {Targ}.sym
	duplicate -y {3DOAutodup} {Targ} "{3DORemote}"
	duplicate -y {3DOAutodup} {Targ}.sym "{3DORemote}"


#####################################
#	Additional Target Dependencies
#####################################

{ObjectDir}Star3000.c.o			�	{App}.make
{ObjectDir}:SFlib:Bit_Control.c.o			�	{App}.make
{ObjectDir}:SFlib:Bonus_Control.c.o			�	{App}.make
{ObjectDir}:SFlib:PlayCPakStream.c.o			�	{App}.make
{ObjectDir}:SFlib:PrepareStream.c.o			�	{App}.make
{ObjectDir}:SFlib:Collision.s.o			�	{App}.make
{ObjectDir}:SFlib:Draw_Frame.c.o			�	{App}.make
{ObjectDir}:SFlib:Update_Frame.c.o			�	{App}.make
{ObjectDir}:SFlib:Draw_Land.c.o			�	{App}.make
{ObjectDir}:SFlib:Explosion.c.o			�	{App}.make
{ObjectDir}:SFlib:Ground_Control.c.o			�	{App}.make
{ObjectDir}:SFlib:Laser_Control.c.o			�	{App}.make
{ObjectDir}:SFlib:Maths_Stuff.s.o			�	{App}.make
{ObjectDir}:SFlib:Plot_Graphic.s.o			�	{App}.make
{ObjectDir}:SFlib:Plot_Land.s.o			�	{App}.make
{ObjectDir}:SFlib:Rotate_Land.c.o			�	{App}.make
{ObjectDir}:SFlib:Setup_Tables.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Access.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_NVRam.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_ARMAnim.s.o			�	{App}.make
{ObjectDir}:SFlib:SF_ARMBurn.s.o			�	{App}.make
{ObjectDir}:SFlib:SF_ARMCell.s.o			�	{App}.make
{ObjectDir}:SFlib:SF_ARMLink.s.o			�	{App}.make
{ObjectDir}:SFlib:SF_ARMSky.s.o			�	{App}.make
{ObjectDir}:SFlib:SF_ARMUtils.s.o			�	{App}.make
{ObjectDir}:SFlib:SF_Bonus.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Celutils.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Control.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Font.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Io.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Map.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Menu.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Message.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Music.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Palette.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Pyramid.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Screenutils.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Sound.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Status.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Utility.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_Video.c.o			�	{App}.make
{ObjectDir}:SFlib:SF_War.c.o			�	{App}.make
{ObjectDir}:SFlib:Ship_Control.c.o			�	{App}.make
{ObjectDir}:SFlib:Smoke_Control.c.o			�	{App}.make
{ObjectDir}:SFlib:Sound_Control.c.o			�	{App}.make
{ObjectDir}:SFlib:Weapons.c.o			�	{App}.make
{ObjectDir}:SFlib:Collision_Update.c.o			�	{App}.make
{ObjectDir}:SFlib:Laser_plot.s.o			�	{App}.make
{ObjectDir}:SFlib:Smoke_plot.s.o			�	{App}.make
{ObjectDir}:SFlib:Ship_Command.c.o			�	{App}.make
{ObjectDir}:SFlib:Graphics_Set.c.o			�	{App}.make
{ObjectDir}:SFlib:bs_joystick.c.o			�	{App}.make
{ObjectDir}:SFlib:broker_shell.c.o			�	{App}.make
{ObjectDir}:SFlib:sf_joystick.c.o			�	{App}.make
{ObjectDir}:SFlib:test_prog.c.o			�	{App}.make
