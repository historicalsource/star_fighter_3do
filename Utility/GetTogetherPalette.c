#include <stdio.h>

int	main()
{
		
FILE*	file_in;
FILE*	file_out;

char	byte1,
		byte2;
		
long	copy;


	file_in=fopen ("Moppit:Coded8bpp:Monochrome.cel","rb");
	file_out=fopen ("Moppit:Coded8bpp:Palettes:Monochrome.pal","wb");
	
	for (copy=0; copy<128; copy++)
		{
		fseek (file_in, 96+(copy*8), SEEK_SET);
		
		byte1 = fgetc (file_in);
		byte2 = fgetc (file_in);
		
		fputc (byte1,file_out);			// Duplicate colour pixel (row 0, word 0, pixel 0)
		fputc (byte2,file_out);
		fputc (byte1,file_out);			// Duplicate colour pixel (row 0, word 0, pixel 1)
		fputc (byte2,file_out);
		fputc (byte1,file_out);			// Duplicate colour pixel (row 0, word 1, pixel 0)
		fputc (byte2,file_out);
		fputc (byte1,file_out);			// Duplicate colour pixel (row 0, word 1, pixel 1)
		fputc (byte2,file_out);
		
		}
		
	fclose(file_out);
	fclose(file_in);
	
	printf("Monochrome Palette file converted\n");
			
}