import array
import os
import textwrap

def generate(outfilename, tablename, tablelength, samplerate):
    fout = open(os.path.expanduser(outfilename), "w")
    fout.write('#ifndef ' + tablename + '_H_' + '\n')
    fout.write('#define ' + tablename + '_H_' + '\n \n')
    fout.write('#include "Arduino.h"'+'\n')
    fout.write('#include <avr/pgmspace.h>'+'\n \n')
    fout.write('#define ' + tablename + '_NUM_CELLS '+ str(len(values))+'\n')
    fout.write('#define ' + tablename + '_SAMPLERATE '+ str(samplerate)+'\n \n')
    outstring = 'const char __attribute__((progmem)) ' + tablename + '_DATA [] = {'
    try:
        for num in range(tablelength):
            outstring += str(num/32) + ", "  ## for saw line, or put your own generating code here
    finally:
        outstring +=  "};"
        outstring = textwrap.fill(outstring, 80)
        fout.write(outstring)
        fout.write('\n \n #endif /* ' + tablename + '_H_ */\n')
        fout.close()
        print "wrote " + outfilename

generate("~/Desktop/phasor8192_uint8.h", "phasor8192", 8192, "8192")
