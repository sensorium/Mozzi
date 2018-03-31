#!/usr/bin/env python

##@file char2mozzi.py
#  @ingroup util
#	A script for converting raw 8 bit sound data files to wavetables for Mozzi.
#
#	Usage: 
#	>>>char2mozzi.py <infile outfile tablename samplerate>
#	
#	@param infile		The file to convert, RAW(headerless) Signed 8 bit PCM.
#	@param outfile	The file to save as output, a .h file containing a table for Mozzi.
#	@param tablename	The name to give the table of converted data in the new file.
#	@param samplerate	The samplerate the sound was recorded at.  Choose what make sense for you, if it's not a normal recorded sample.
#
#	@note Using Audacity to prepare raw sound files for converting:
#	
#	For generated waveforms like sine or sawtooth, set the project
#	rate to the size of the wavetable you wish to create, which must
#	be a power of two (eg. 8192), and set the selection format
#	(beneath the editing window) to samples. Then you can generate
#	and save 1 second of a waveform and it will fit your table
#	length.
#	
#	For a recorded audio sample, set the project rate to the
#	Mozzi AUDIO_RATE (16384 in the current version). 
#	Samples can be any length, as long as they fit in your Arduino.
#	
#	Save by exporting with the format set to "Other uncompressed formats",
#	"Header: RAW(headerless)" and "Encoding: Signed 8 bit PCM".
#	
#	Now use the file you just exported, as the "infile" to convert.
#	
#	@author Tim Barrass 2010-12
#	@fn char2mozzi

import sys, array, os, textwrap, random

if len(sys.argv) != 5:
        print ('Usage: char2mozzi.py <infile outfile tablename samplerate>')
        sys.exit(1)

[infile, outfile, tablename, samplerate] = sys.argv[1:]

def char2mozzi(infile, outfile, tablename, samplerate):
	fin = open(os.path.expanduser(infile), "rb")
	print ("opened " + infile)
	uint8_tstoread = os.path.getsize(os.path.expanduser(infile))
	##print uint8_tstoread
	valuesfromfile = array.array('b') # array of signed int8_t ints
	try:
		valuesfromfile.fromfile(fin, uint8_tstoread)
	finally:
		fin.close()
	
	values=valuesfromfile.tolist()
	fout = open(os.path.expanduser(outfile), "w")
	fout.write('#ifndef ' + tablename + '_H_' + '\n')
	fout.write('#define ' + tablename + '_H_' + '\n \n')
	fout.write('#if ARDUINO >= 100'+'\n')
	fout.write('#include "Arduino.h"'+'\n')
	fout.write('#else'+'\n')
	fout.write('#include "WProgram.h"'+'\n')
	fout.write('#endif'+'\n')   
	fout.write('#include "mozzi_pgmspace.h"'+'\n \n')
	fout.write('#define ' + tablename + '_NUM_CELLS '+ str(len(values))+'\n')
	fout.write('#define ' + tablename + '_SAMPLERATE '+ str(samplerate)+'\n \n')
	outstring = 'CONSTTABLE_STORAGE(int8_t) ' + tablename + '_DATA [] = {'
	try:
		for i in range(len(values)):
			## mega2560 boards won't upload if there is 33, 33, 33 in the array, so dither the 3rd 33 if there is one
			if (values[i] == 33) and (values[i+1] == 33) and (values[i+2] == 33):
				values[i+2] = random.choice([32, 34])
			outstring += str(values[i]) + ", "
	finally:
		outstring +=  "};"
		outstring = textwrap.fill(outstring, 80)
		fout.write(outstring)
		fout.write('\n\n#endif /* ' + tablename + '_H_ */\n')
		fout.close()
		print ("wrote " + outfile)

char2mozzi(infile, outfile, tablename, samplerate)
