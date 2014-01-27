import array
import os
import textwrap

def generate(outfile, tablename, tablelength, samplerate):
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
        for num in range(tablelength):
            outstring += str(num/32) + ", "  ## for saw line, or put your own generating code here
    finally:
        outstring +=  "};"
        outstring = textwrap.fill(outstring, 80)
        fout.write(outstring)
        fout.write('\n \n #endif /* ' + tablename + '_H_ */\n')
        fout.close()
        print "wrote " + outfile

generate("~/Desktop/phasor8192_uint8.h", "phasor8192", 8192, "8192")
