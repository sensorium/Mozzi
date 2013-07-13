## generates a sin-shaped table with values 0-255 (to be used as an envelope)


import array
import os
import textwrap
import math

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
    fout.write('#define ' + tablename + '_NUM_CELLS '+ str(tablelength)+'\n')
    fout.write('#define ' + tablename + '_SAMPLERATE '+ str(samplerate)+'\n \n')
    outstring = 'const char __attribute__((section(".progmem.data"))) ' + tablename + '_DATA [] = {'
    halftable = tablelength/2
    try:
        for num in range(tablelength):
            ## range between 0 and 1 first
           ## x = float(num)/halftable
            x = float(num)/tablelength
            
            t_x = (math.cos(2*math.pi*x-math.pi)+1)/2

            scaled = int(math.floor(t_x*255.999))

            outstring += str(scaled) + ', '
       ## for num in range(halftable):
       ##     outstring += '0, '
    finally:
        outstring = textwrap.fill(outstring, 80)
        outstring += '\n }; \n \n #endif /* ' + tablename + '_H_ */\n'
        fout.write(outstring)
        fout.close()
        print "wrote " + outfile

## generate("~/Desktop/sin1024_uint8.h", "sin1024_uint", 1024, "1024")
generate("~/Desktop/sin8192_uint8.h", "sin8192_uint", 8192, "8192")
