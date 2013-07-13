## for converting 32 bit float raw files from Audacity, with values  -128 to 127 int8 Mozzi table

import sys, array, os, textwrap, math

    if len(sys.argv) != 5:
        print 'Usage: float2mozzi.py <infile outfile tablename samplerate>'
        sys.exit(1)

[infile, outfile, tablename, samplerate] = sys.argv[1:]

def float2mozzi(infile, outfile, tablename,samplerate):
    fin = open(os.path.expanduser(infile), "rb")
    print "opened " + infile
    valuesetad = os.path.getsize(os.path.expanduser(infile))/4 ## adjust for number format

    ##print valuesetad
    valuesfromfile = array.array('f')## array of floats
    try:
        valuesfromfile.fromfile(fin,valuesetad)
    finally:
        fin.close()

    values=valuesfromfile.tolist()
##    print values[0]
##    print values[len(values)-1]
##    print len(values)
    fout = open(os.path.expanduser(outfile), "w")
    fout.write('#ifndef ' + tablename + '_H_' + '\n')
    fout.write('#define ' + tablename + '_H_' + '\n \n')
    fout.write('#if ARDUINO >= 100'+'\n')
    fout.write('#include "Arduino.h"'+'\n')
    fout.write('#else'+'\n')
    fout.write('#include "WProgram.h"'+'\n')
    fout.write('#endif'+'\n')   
    fout.write('#include <avr/pgmspace.h>'+'\n \n')
    fout.write('#define ' + tablename + '_NUM_CELLS '+ str(len(values))+'\n')
    fout.write('#define ' + tablename + '_SAMPLERATE '+ str(samplerate)+'\n \n')
    outstring = 'const char __attribute__((section(".progmem.data"))) ' + tablename + '_DATA [] = {'
    try:
        for num in values:
            outstring += str(math.trunc((num*256)+0.5)) + ", "
 ##           outstring += str(num) + ", "
        ##values.fromfile(fin, bytesetad)
    finally:
        outstring +=  "};"
        outstring = textwrap.fill(outstring, 80)
        fout.write(outstring)
        fout.write('\n \n #endif /* ' + tablename + '_H_ */\n')
        fout.close()
        print "wrote " + outfile

float2mozzi(infile, outfile, tablename, samplerate)
