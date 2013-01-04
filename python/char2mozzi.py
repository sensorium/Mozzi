##    char2mozzi.py
##    Created 2010-12 by Tim Barrass
##
##    char2mozzi.py converts raw 8 bit sound data to wavetables for Mozzi.
##
##    Usage: python char2mozzi.py <infilename outfilename "tablename" "samplerate">
##
##    Using Audacity to prepare raw sound files for converting:
##
##    Set your Audacity project sample rate:
##
##		For generated waveforms like sine or sawtooth, set the project
##		rate to the size of the wavetable you wish to create, which must
##		be a power of two (eg. 8192), and set the selection format
##		(beneath the editing window) to samples. Then you can generate
##		and save 1 second of a waveform and it will fit your table
##		length.
##
##		For a recorded audio sample, set the project rate to the
##		Mozzi AUDIO_RATE (16384 in the current version). Then edit
##		your sounds to a power-of-two number of samples.
##
##    Save by exporting with the format set to "Other uncompressed formats",
##    options set to "RAW(headerless)" and "Encoding 8 bit signed PCM".


import sys, array, os, textwrap, random

if len(sys.argv) != 5:
        print 'Usage: char2mozzi.py <infilename outfilename "tablename" "samplerate">'
        sys.exit(1)

[infilename, outfilename, tablename, samplerate] = sys.argv[1:]

def char2mozzi(infilename, outfilename, tablename, samplerate):
    fin = open(os.path.expanduser(infilename), "rb")
    print "opened " + infilename
    bytestoread = os.path.getsize(os.path.expanduser(infilename))
    ##print bytestoread
    valuesfromfile = array.array('b') # array of signed char ints
    try:
        valuesfromfile.fromfile(fin, bytestoread)
    finally:
        fin.close()

    values=valuesfromfile.tolist()
##    print values[0]
##    print values[len(values)-1]
##    print len(values)
    fout = open(os.path.expanduser(outfilename), "w")
    fout.write('#ifndef ' + tablename + '_H_' + '\n')
    fout.write('#define ' + tablename + '_H_' + '\n \n')
    fout.write('#include "Arduino.h"'+'\n')
    fout.write('#include <avr/pgmspace.h>'+'\n \n')
    fout.write('#define ' + tablename + '_NUM_CELLS '+ str(len(values))+'\n')
    fout.write('#define ' + tablename + '_SAMPLERATE '+ str(samplerate)+'\n \n')
    outstring = 'const char __attribute__((progmem)) ' + tablename + '_DATA [] = {'
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
        fout.write('\n \n #endif /* ' + tablename + '_H_ */\n')
        fout.close()
        print "wrote " + outfilename

char2mozzi(infilename, outfilename, tablename, samplerate)
