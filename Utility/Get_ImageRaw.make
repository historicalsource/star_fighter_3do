#   File:       Get_ImageRaw.make
#   Target:     Get_ImageRaw
#   Sources:    Get_ImageRaw.c
#   Created:    Thursday, October 19, 1995 8:37:54 pm


OBJECTS = Get_ImageRaw.c.o



Get_ImageRaw �� Get_ImageRaw.make {OBJECTS}
	Link -t APPL -c '????' �
		{OBJECTS} �
		"{CLibraries}"CSANELib.o �
		"{CLibraries}"Math.o �
		#"{CLibraries}"Complex.o �
		"{CLibraries}"StdClib.o �
		"{Libraries}"Runtime.o �
		"{Libraries}"Interface.o �
		-o Get_ImageRaw
Get_ImageRaw.c.o � Get_ImageRaw.make Get_ImageRaw.c
	 C -r  Get_ImageRaw.c
