##    raw2cuttle.py
##    Created 2010-12 by unbackwards@gmail.com
##
##    raw2cuttle converts raw 8 bit sound data to wavetables for Cuttlefish.                                                                                                       ##
##
##    use: raw2cuttle(infilename, outfilename, tablename, samplerate)
##
##    There's an example at the end of this file, copy and modify it
##
##    To use Audacity to produce wavetables for Arduino:
##    Set your Audacity project sample rate.  For repeating waveforms like sine or
##    sawtooth, set the project rate to the size of the wavetable you wish to create,
##    which must be a power of two (eg. 8192), and set the selection format (beneath the editing window)
##    to samples.  Then you can generate and save 1 second of a waveform and it will fit your table length.
##    For a one-shot audio sample such as a recorded sound, set the
##    project rate to the Cuttlefish AUDIO_RATE (16384 in the current
##    version).  Then edit your sounds to a power-of-two number of samples.
##
##    Save by exporting with the format set to "Other uncompressed formats", with
##    options set to "RAW(headerless)" and "Encoding 8 bit signed PCM".



import array
import os
import textwrap

def raw2cuttle(infilename, outfilename, tablename, samplerate):
    fin = open(os.path.expanduser(infilename), "rb")
    print "opened " + infilename
    bytesetad = os.path.getsize(os.path.expanduser(infilename))
    ##print bytesetad
    valuesfromfile = array.array('b') # array of signed char ints
    try:
        valuesfromfile.fromfile(fin, bytesetad)
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
    fout.write('#define ' + tablename + '_NUM_TABLE_CELLS '+ str(len(values))+'\n')
    fout.write('#define ' + tablename + '_SAMPLERATE '+ str(samplerate)+'\n \n')
    outstring = 'prog_char ' + tablename + '_DATA [] PROGMEM = {'
    try:
        for num in values:
            outstring += str(num) + ", "
    finally:
        outstring +=  "};"
        outstring = textwrap.fill(outstring, 80)
        fout.write(outstring)
        fout.write('\n \n #endif /* ' + tablename + '_H_ */\n')
        fout.close()
        print "wrote " + outfilename

############################################################

##example (tested on osx):
##raw2cuttle("~/Desktop/sin4096_int8.raw", "~/Desktop/sin4096_int8.h", "SIN4096","4096")
##raw2cuttle("~/Desktop/organ8192_int8.raw", "~/Desktop/organ8192_int8.h", "ORGAN8192","8192")
##raw2cuttle("~/Desktop/smoothsquare8192_int8.raw", "~/Desktop/smoothsquare8192_int8.h", "SMOOTHSQUARE8192","8192")
##raw2cuttle("~/Desktop/sin2048_int8.raw", "~/Desktop/sin2048_int8.h", "SIN2048","2048")
##raw2cuttle("~/Desktop/bamboo1_1024_int8.raw", "~/Desktop/bamboo1_1024_int8.h", "BAMBOO1_1024","1024")
##raw2cuttle("~/Desktop/bamboo2_1024_int8.raw", "~/Desktop/bamboo2_1024_int8.h", "BAMBOO2_1024","1024")
##raw2cuttle("~/Desktop/bamboo3_2048_int8.raw", "~/Desktop/bamboo3_2048_int8.h", "BAMBOO3_2048","2048")
##raw2cuttle("~/Desktop/static1_16384_int8.raw", "~/Desktop/noise_static_1_16384_int8.h", "NOISE_STATIC_1_16384","16384")
##raw2cuttle("~/Desktop/whitenoise8192.raw", "~/Desktop/WHITENOISE8192_int8.h", "WHITENOISE8192","8192")
##raw2cuttle("~/Desktop/brownnoise8192.raw", "~/DesktopBROWNNOISE8192_int8.h", "BROWNNOISE8192","8192")
raw2cuttle("~/Desktop/pinknoise8192.raw", "~/Desktop/PINKNOISE8192_int8.h", "PINKNOISE8192","8192")
