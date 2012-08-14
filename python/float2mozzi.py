## for converting 32 bit float raw files from Audacity, with values  -128 to 127 int8 Mozzi table

import sys, array, os, textwrap, math

    if len(sys.argv) != 5:
        print 'Usage: float2mozzi.py <infilename outfilename tablename samplerate>'
        sys.exit(1)

[infilename, outfilename, tablename, samplerate] = sys.argv[1:]

def float2mozzi(infilename, outfilename, tablename,samplerate):
    fin = open(os.path.expanduser(infilename), "rb")
    print "opened " + infilename
    valuesetad = os.path.getsize(os.path.expanduser(infilename))/4 ## adjust for number format

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
    fout = open(os.path.expanduser(outfilename), "w")
    fout.write('#ifndef ' + tablename + '_H_' + '\n')
    fout.write('#define ' + tablename + '_H_' + '\n \n')
    fout.write('#include "Arduino.h"'+'\n')
    fout.write('#include <avr/pgmspace.h>'+'\n \n')
    fout.write('#define ' + tablename + '_NUM_CELLS '+ str(len(values))+'\n')
    fout.write('#define ' + tablename + '_SAMPLERATE '+ str(samplerate)+'\n \n')
    outstring = 'const char __attribute__((progmem)) ' + tablename + '_DATA [] = {'
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
        print "wrote " + outfilename

float2mozzi(infilename, outfilename, tablename, samplerate)
