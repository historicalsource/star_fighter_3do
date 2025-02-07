#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

char	filename_out [128],
		master_filename [128],
		master_title [128],
		log	[256],
		mchar;

char	byte;

FILE*	file_master;
FILE*	file_in;
FILE*	file_out;

long	get_source=0,
		finished_master=0;

long	aifc_filelength,
		aifc_samplelength,
		aifc_framecount,
		insert,
		frameerror,
		copyloop;
		
	logit ("Converting aifc music files :\n\n");
		
// Open master files list
// ----------------------

	finished_master=0;
	
	sprintf(&master_filename,"Moppit:coded8bpp:Samplelist");
	file_master=fopen (master_filename,"rb");
	
// Main music file search loop
// ---------------------------
	
	do
		{
		get_source=0;
	
// Read in name of aifc sample to convert
// --------------------------------------
	
		do
			{
			mchar=fgetc (file_master);
			
			if (mchar == EOF)
				finished_master=1;				// End of master file ? if so, set finishedmaster
			
			master_title[get_source]=mchar;
			filename_out[get_source]=mchar;
			
			if (mchar >=32)
				get_source++;
			}
		while (mchar >=32 && finished_master==0);
		master_title [get_source] = 0;
		filename_out [get_source] = 'O';
		filename_out [get_source+1] = 0;
		
// Fix output file
// ---------------

		if (finished_master==0 && master_title [0] != '|')
			{

// Open source info file
			
			sprintf(log, "\n\n    - Converting music file '%s'\n", master_title);
			logit (log);
			
			file_in=fopen(master_title, "rb");
			
			if (file_in == NULL)
				{
				logit ("    - COULD NOT OPEN FILE !\n");
				}
			
			else
				
				{
				
				// GET LENGTH OF FILE
				fseek (file_in, 4, SEEK_SET);
				aifc_filelength = (fgetc(file_in)<<24) | (fgetc(file_in)<<16) | (fgetc(file_in)<<8) | fgetc(file_in);
			
				// GET LENGTH OF SAMPLE
				fseek (file_in, 98, SEEK_SET);
				aifc_samplelength = (fgetc(file_in)<<24) | (fgetc(file_in)<<16) | (fgetc(file_in)<<8) | fgetc(file_in);
				
				// GET NUMBER OF FRAMES IN FILE
				fseek (file_in, 34, SEEK_SET);
				aifc_framecount = (fgetc(file_in)<<24) | (fgetc(file_in)<<16) | (fgetc(file_in)<<8) | fgetc(file_in);
				
				// REPORT DETAILS IN FILE
				sprintf(log, "           - Filelength is   : %d\n", aifc_filelength);
				logit (log);
				sprintf(log, "           - Samplelength is : %d\n", aifc_samplelength);
				logit (log);
				sprintf(log, "           - Framecount is : %d\n", aifc_framecount);
				logit (log);
				
				
				frameerror = aifc_framecount & 3;
				
				if (frameerror == 0)
					{
					logit("           - Sample does NOT need adjusting\n");
					}
				
				else
				
					{
					insert = (8 - (aifc_samplelength & 7)) & 7;
					
					sprintf(log, "           - Sample needs %d bytes to DMA Align\n", insert);
					logit (log);
			
					file_out = fopen (filename_out, "wb");
				
					if (file_out == NULL)
						{
						sprintf(log, "           - COULD NOT CREATE OUTPUT FILE '%s'\n", filename_out);
						logit (log);
						}
					
					else
				
						{
					
						// COPY OVER BYTES 0 - 3
						
						for (copyloop = 0; copyloop <=3 ; copyloop++)
							{
							fseek (file_in, copyloop, SEEK_SET);
							fputc (fgetc (file_in), file_out);
							}
							
						// Put in new adjusted filelength
						
						aifc_samplelength+=insert;
						aifc_filelength+=insert;
						aifc_framecount = (aifc_samplelength-8)/2;
						
						fputc ((aifc_filelength>>24) & 255, file_out);
						fputc ((aifc_filelength>>16) & 255, file_out);
						fputc ((aifc_filelength>>8) & 255, file_out);
						fputc ((aifc_filelength>>0) & 255, file_out);
						
						// COPY OVER BYTES 8-33
						
						for (copyloop = 8; copyloop <=33 ; copyloop++)
							{
							fseek (file_in, copyloop, SEEK_SET);
							fputc (fgetc (file_in), file_out);
							}
							
						// Put in new adjusted framecount
						
						fputc ((aifc_framecount>>24) & 255, file_out);
						fputc ((aifc_framecount>>16) & 255, file_out);
						fputc ((aifc_framecount>>8) & 255, file_out);
						fputc ((aifc_framecount>>0) & 255, file_out);
						
						// COPY OVER BYTES 38-97
						
						for (copyloop = 38; copyloop <=97 ; copyloop++)
							{
							fseek (file_in, copyloop, SEEK_SET);
							fputc (fgetc (file_in), file_out);
							}
						
						// Put in new adjusted filelength
						
						fputc ((aifc_samplelength>>24) & 255, file_out);
						fputc ((aifc_samplelength>>16) & 255, file_out);
						fputc ((aifc_samplelength>>8) & 255, file_out);
						fputc ((aifc_samplelength>>0) & 255, file_out);
						
						// Put in inserted bytes to DMA align
						
						for (copyloop = 0; copyloop < insert; copyloop++)
							fputc (0, file_out);
						
						// COPY OVER REST OF SAMPLE DATA UNTIL END OF FILE
						
						fseek (file_in, 102, SEEK_SET);
						do
							{
							byte = fgetc (file_in);
							if (!feof (file_in))
								fputc (byte, file_out);
							}
						while (!feof (file_in));
							
						sprintf(log, "           - FILE FIXED AS '%s'\n", filename_out);
						logit (log);
						}
					fclose (file_out);
					}
				}
			}
		}
	while (finished_master == 0);	
	
	fclose (file_master);
	printf("Sound files converted - See Log\n");
}
