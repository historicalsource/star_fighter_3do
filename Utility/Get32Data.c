#include <stdio.h>
#define	hdisk	"Moppit:"

/***************************************************/

void putrun (unsigned char* rle, long run_number, unsigned char run_char, long num_bits)

{
long	bits_count;

	for (bits_count = 0; bits_count < num_bits; bits_count++)
		if ((run_char & (1 << bits_count)) != 0)
			rle [run_number + ((num_bits-1)-bits_count)] = 1;
}

/***************************************************/

void logit (char* log_message)
{
FILE*	file_error;
	
	file_error = fopen("Moppit:ErrorLog", "a");
	fputs(log_message, file_error);
	fclose (file_error);
}

/***************************************************/

void	main()
{

long	description_count,
		sprite_size [512],
		sprite_bitsize [512],
		sprite_mask [512],
		sprite_shading [512],
		sprite_bytes [512],
		sprite_woffset [512],
		word,
		bytes_written,
		
		packed_count=0,
		cel_counter=0,
		int_counter,
		get_source=0,
		finished=0,
		finished2=0,
		finished3=0,
		read_in=0,
		
		x_spr,
		y_spr,
		x_pos,
		y_pos,
		ref,
		
		run_number,
		run_scramble,
		run_scrambleinsert,
		run_offset,
		
		palette_map [16],
		palette_entries,
		palette_found,
		palette_search,
		palette_copyup,
		
		colours [32] [32],
		colour;

char	filename_in [128],
		filename_out [128],
		master_filename [128],
		submaster_filename [128],
		submaster_title [128],
		description [128],
		description_filename [128],
		master_title [128],
		log	[256],
		run_count,
		run_char,
		palette_overflow,
		template [32] [32];
		
FILE*	file_master;
FILE*	file_in;
FILE*	file_out;
FILE*	file_submaster;
FILE*	file_description;

unsigned char 	rle [65536];
unsigned char	chunk [163840];
unsigned char	mchar;
unsigned char	x_pixel;

char	*wordin=(char*) &word;


/***************************************************/
	
	logit ("\n\nConversion of Master 32 cels\n");
	
	sprintf(&master_filename,"%scoded8bpp:MasterList",hdisk);
	file_master=fopen(master_filename,"rb");
	
	do
	{
	
	get_source=0;
	do
		{
		mchar=fgetc (file_master);
		if (feof (file_master))
			finished  = 1;
		else	
			if (mchar>=32)
				master_title[get_source++]=mchar;
		}
	while (mchar !=13 && finished==0);
	master_title[get_source]=0;
	
		// If not end, convert planet type - open sub conversion file
		// ----------------------------------------------------------
		
		if (finished==0)
			{

			sprintf(log, "\n    - Conversion of %s32\n",master_title);
			logit (log);
			
			sprintf(&submaster_filename,"%scoded8bpp:%s32:Files",hdisk,master_title);
			file_submaster=fopen (submaster_filename,"rb");
	
			finished2=0;
			cel_counter=0;
			packed_count=0;
			
			// Main sheet conversion loop
			// --------------------------
			
			do
				{
				get_source=0;
	
				do
					{
					mchar=fgetc (file_submaster);
					if (feof (file_submaster))
						finished2=1;
					else
						if (mchar >=32)
							submaster_title[get_source++]=mchar;
					}
				while (mchar !=13 && finished2==0);
				
				submaster_title[get_source]=0;
			
				// If not end of sheets, convert it
				// --------------------------------
				
				if (finished2 == 0)
					{

					sprintf(log, "   		- Subsheet ->%s\n",submaster_title);
					logit (log);
			
					// Set sprite sizes to default of 32x32
					
					for (get_source = 0; get_source < 64; get_source++)
						{
						sprite_size [get_source] = 32;
						sprite_bitsize [get_source] = 5;
						sprite_mask [get_source] = 0;
						sprite_shading [get_source] = 0;
						sprite_bytes [get_source] = 512;
						sprite_woffset [get_source] = 2;
						}
						
					// Open sprite sheet description - READ IN SPRITE SIZES FOR SHEET
					
					sprintf(&description_filename, "%scoded8bpp:%s32:%s.TXT",hdisk,master_title,submaster_title);
					file_description=fopen(&description_filename, "rb");
					
					if (file_description != NULL)
						{
						
						finished3 = 0;
						description_count = 0;
						
						do
							{
							
							get_source = 0;
							
							do
								{
							
								mchar=fgetc (file_description);
								if (feof (file_description))
									finished3=1;
								else
									if (mchar >=32)
										description [get_source++]=mchar;
								}
							while (mchar !=13 && finished3==0);
							
							if (finished3 == 0)
								{
								description [get_source] = 0;
							
								if (description [0] != '|')								// Decode line, it's not a comment
									{
								
									switch (description [0])
										{
										case 'a' :
											sprite_size [description_count] = 4;
											sprite_bitsize [description_count] = 0;
											sprite_bytes [description_count] = 32;
											sprite_woffset [description_count] = 0;
											break;
										case 'b' :
											sprite_size [description_count] = 8;
											sprite_bitsize [description_count] = 1;
											sprite_bytes [description_count] = 64;
											sprite_woffset [description_count] = 0;
											break;
										case 'c' :
											sprite_size [description_count] = 16;
											sprite_bitsize [description_count] = 2;
											sprite_bytes [description_count] = 128;
											sprite_woffset [description_count] = 0;
											break;
										case 'd' :
											sprite_size [description_count] = 32;
											sprite_bitsize [description_count] = 3;
											sprite_bytes [description_count] = 512;
											sprite_woffset [description_count] = 2;
											break;
										}								
										
										if (description [1] == 't')
											sprite_mask [description_count] = 1;
											
										if (description [2] == 'a')
											sprite_shading [description_count] = 1;
										
										if (description [2] == 'm')
											sprite_shading [description_count] = 2;
										
										description_count+=1;
										
									}
								}
							}
						while (finished3 == 0 && description_count < 64);
						fclose (file_description);
						}
						
	sprintf(&filename_in, "%scoded8bpp:%s32:%s.cel",hdisk,master_title,submaster_title);
	file_in=fopen(&filename_in, "rb");
		
		// SHEET HAS BEEN OPENED FOR READ - GET ALL PIXEL DATA INTO 320 X 256 ARRAY
		// ------------------------------------------------------------------------
		
		if (file_in != NULL)
			{
			fseek (file_in, 88, SEEK_SET);
			
			for (get_source=0; get_source<163840; get_source++)
				chunk [get_source] = fgetc (file_in);
			fclose(file_in);
			
			// READ SPRITE INTO ARRAY
			// ----------------------
			
			for (int_counter = 0; int_counter < description_count; int_counter++)
				{
							
				// Read sprite into template
							
				y_spr = 4-(int_counter >> 3);
				x_spr = int_counter &7;
				
				for (y_pos=0; y_pos<sprite_size [int_counter]; y_pos++)
					for (x_pos=0; x_pos<sprite_size [int_counter]; x_pos++)
						{
						ref=(143360 + (x_spr * 80) + (x_pos*2) - (y_spr * 25600) + (y_pos * 640));			
						colours [x_pos] [y_pos] = ((chunk [ref] << 8) | chunk [ref+1]) & 32767;		// col long
						}
					
					// Analyse palette entries for sprite
					// ----------------------------------
					
					for (get_source=0; get_source < 16; get_source++)
						palette_map [get_source]= 0;
					palette_entries = 0;
					
					palette_overflow = 0;
					for (y_pos=0; y_pos<sprite_size [int_counter]; y_pos++)
						for (x_pos=0; x_pos<sprite_size [int_counter]; x_pos++)
							{
							colour = colours [x_pos] [y_pos];						// Get pixel from sprite
							palette_found = -1;										// Is pixel already registered in the palette ?

							if (palette_entries > 0)
								for (palette_search = 0; palette_search < palette_entries; palette_search++)
									if (colour == palette_map [palette_search])
										palette_found = palette_search;				// Yes, palette entry has been found already
	
							if (palette_found == -1)
								{
								if (palette_entries < 16)
									{
									palette_found = palette_entries;
									palette_map [palette_entries++] = colour;		// Register in palette
									}
								else
									{
									palette_overflow = 1;							// Too many palette entries !!!
									palette_found = 0;								// Just put in colour 0 FOR THE MOMENT !!!!
									}
								}
							template [x_pos] [y_pos] = palette_found;				// Register colour entry in sprite
							}
					
					// Duplicate the palette entries to fill up the 16 (so random explosion pieces can use them)
					
					palette_copyup = 0;
					
					while (palette_entries < 16)
						palette_map [palette_entries++] = palette_map [palette_copyup++];
						
					if (palette_overflow == 1)
						{
						sprintf(log, "    - ERROR : SPRITE %d (%d In Sheet) Has too many colours !\n",cel_counter, int_counter);
						logit (log);
						}
						
					for (get_source = 0; get_source < 65536; get_source++)			// Clear all commands
						rle [get_source] = 0;
					run_number = 0;
					
					// Is the cel smaller when packed - Encode it
					// ------------------------------------------
					
					for (y_pos=0; y_pos<sprite_size [int_counter]; y_pos++)			// Y line count
						{
						
						run_offset = run_number;
						run_number+=8;
						
						// Encode a single row
						// -------------------
						
						x_pixel =0;													// Set to first pixel
						run_scramble = 0;
						
						do
							{
							run_char = template [x_pixel] [y_pos];					// Get pixel
							run_count = 0;
							
							do
								{
								if (run_count++ >= 1 && run_scramble > 0)							// Is this the end of a literal ?
									{
									putrun (rle, run_scrambleinsert, (1 << 6) | (run_scramble-1), 8);	// Yes, insert literal com&count
									run_scramble=0;													// Clear literal count
									}
								x_pixel+=1;															// Move onto next pixel
								}
							while ((template [x_pixel] [y_pos] == run_char) && x_pixel < sprite_size [int_counter]);	// Until not the same or EOL
							
							// End of repeat !

							if (run_count > 1)														// Was the run greater then 1 ?
								{
								putrun (rle, run_number, (3 << 6) | (run_count-1), 8);				// YES, put in repeat com&count
									run_number+=8;
								putrun (rle, run_number, run_char, 4);								// And the character
									run_number+=4;
								}
							else
								{
								if (run_scramble++ == 0)											// No, a literal - first one ?
									{
									run_scrambleinsert = run_number;								// Yes, mark position & move com
									run_number+=8;
									}
								putrun (rle, run_number, run_char, 4);								// Insert literal character
								run_number+=4;
								
								if (x_pixel >= sprite_size [int_counter] && run_scramble >= 1)
									putrun (rle, run_scrambleinsert, (1 << 6) | (run_scramble-1), 8);	// Insert literal com&count @ end
								}
							}
						while (x_pixel < sprite_size [int_counter]);								// Until end
					
						putrun (rle, run_number, 0, 4);												// Put in EOL character
						run_number+=4;
						
						// Insert offset to next
						// ---------------------
						
						run_number = ((run_number + 31) & 524256);									// Make next word boundary
						if ((run_number - run_offset) < 64)
							run_number+=32;															// Make at least 2 words per
							
						putrun (rle, run_offset, (((run_number - run_offset)>>3)-8)>>2, 8);			// Put in offset to next
					}
					
					// Re-Encode packed cel data into bytes
					// ------------------------------------
					
					run_number = run_number >> 3;										// Put counter into bytes
					
					for (run_offset = 0; run_offset < run_number; run_offset++)
						{
						run_scramble = run_offset << 3;
						rle [run_offset] = (	(rle [run_scramble + 0] << 7) |
												(rle [run_scramble + 1] << 6) |
												(rle [run_scramble + 2] << 5) |
												(rle [run_scramble + 3] << 4) |
												(rle [run_scramble + 4] << 3) |
												(rle [run_scramble + 5] << 2) |
												(rle [run_scramble + 6] << 1) |
												(rle [run_scramble + 7] << 0) );
											}

					// Write to output file
					// --------------------
					
					sprintf(&filename_out,"%scoded8bpp:%s32:Processed:S_%d.data",hdisk,master_title, cel_counter);
					file_out=fopen(filename_out,"wb");
					
					// Write bitsize of sprite
									
					word = sprite_bitsize [int_counter] | (sprite_shading [int_counter]<<8);
					fputc (*(wordin+0), file_out);
					fputc (*(wordin+1), file_out);
					fputc (*(wordin+2), file_out);
					fputc (*(wordin+3), file_out);
							
					if (run_number < sprite_bytes [int_counter] && sprite_mask [int_counter] == 0)
						
						// Write packed data to coded4 cel file
						// ------------------------------------
						
						{
						
						packed_count+=1;
						
						// Set important bits of flags word
					
						word = 	(
							(1<<29) | (1<<28) |	(1<<27) | (1<<26) |	(1<<25) | (1<<24) |	(1<<23) | (1<<21) |	
							(1<<18) | (1<<17) |	(1<<14) | (1<<9) | (1<<8) | (1<<10) | (1<<19) | (1<<5)
							);
													
						// Write flags word
					
						fputc (*(wordin+0), file_out);
						fputc (*(wordin+1), file_out);
						fputc (*(wordin+2), file_out);
						fputc (*(wordin+3), file_out);
						
						// Write PLUT Data
						
						for (get_source=0; get_source < 16; get_source++)
							{
							fputc (palette_map [get_source] >>8, file_out);
							fputc (palette_map [get_source] & 255, file_out);
							}
							
						// Write PREAMBLE WORD 0
						
						word = (1<<31) | ((sprite_size [int_counter]-1)<<6) | 3;                                             
						fputc (*(wordin+0), file_out);
						fputc (*(wordin+1), file_out);
						fputc (*(wordin+2), file_out);
						fputc (*(wordin+3), file_out);
						
						// Write PACKED cel data
						
						for (get_source=0; get_source<run_number; get_source++)
							fputc (rle [get_source],file_out);
						}
						
						else

						// Write unpacked data to coded4 cel file
						// --------------------------------------
						
						{
						
						// Set important bits of flags word
					
						word = (
							(1<<29) | (1<<28) |	(1<<27) | (1<<26) |	(1<<25) | (1<<24) | (1<<23) | (1<<21) |
							(1<<18) | (1<<17) |	(1<<14) | (1<<8) | (1<<19) | (1<<10)
							);
						
						if (sprite_mask [int_counter] == 0)
							word = word | (1<<5);				// Pass 000rgb as 000rgb ! (Not transparent)
							
						// Write flags word
					
						fputc (*(wordin+0), file_out);
						fputc (*(wordin+1), file_out);
						fputc (*(wordin+2), file_out);
						fputc (*(wordin+3), file_out);
						
						// Write PLUT Data
						
						for (get_source=0; get_source < 16; get_source++)
							{
							fputc (((palette_map [get_source]) >>8) & 255, file_out);
							fputc ((palette_map [get_source]) & 255, file_out);
							}
							
						// Write PREAMBLE WORD 0
						
						word = ((sprite_size [int_counter]-1)<<6)  | 3;
						fputc (*(wordin+0), file_out);
						fputc (*(wordin+1), file_out);
						fputc (*(wordin+2), file_out);
						fputc (*(wordin+3), file_out);
						
						// Write PREAMBLE WORD 1
						
						word = (sprite_woffset [int_counter]<<24) | (1<<14) | (sprite_size [int_counter]-1);
						fputc (*(wordin+0), file_out);
						fputc (*(wordin+1), file_out);
						fputc (*(wordin+2), file_out);
						fputc (*(wordin+3), file_out);
						
						// Write Unpacked pixel data
							
						for (y_pos = 0; y_pos < sprite_size [int_counter]; y_pos++)
							{
							bytes_written=0;
							for (x_pos = 0; x_pos < sprite_size [int_counter]; x_pos+=2)
								{
								fputc (	(((template [x_pos] [y_pos])<<4) | template [x_pos+1] [y_pos]) ,file_out);
								bytes_written+=1;
								}
							if (bytes_written < 8)
								for (; bytes_written<8; bytes_written++)
									fputc(0, file_out);
							}
						}
					
					fclose(file_out);
					cel_counter ++;
					}												// sprite - in - sheet conversion loop
				}													// if file open != null
			}														// if finished2 == 0
		}															// do loop for sheet files
		while (finished2==0);
		fclose (file_submaster);
		sprintf(log, "    - %s32 Converted %d sprites, %d (%d%%) were packed\n", master_title, cel_counter, packed_count, ((100*packed_count)/cel_counter));
		logit (log);
		}															// if finished == 0
	}																// do loop for planet types
	while (finished==0);
	fclose (file_master);
	printf("Conversion finished - see log file\n");
}								
