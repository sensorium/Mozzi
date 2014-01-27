## generates a set of sin waves at different amplitudes
## tables go from 1 to numtables+1, as table 0 is all 0's

import array,os,textwrap,math



def generate(outfile, tablename, tablelength, numtables):
    fout = open(os.path.expanduser(outfile), "w")


    
    fout.write('#ifndef ' + tablename + '_H_' + '\n')
    fout.write('#define ' + tablename + '_H_' + '\n \n')
    fout.write('#if ARDUINO >= 100'+'\n')
    fout.write('#include "Arduino.h"'+'\n')
    fout.write('#else'+'\n')
    fout.write('#include "WProgram.h"'+'\n')
    fout.write('#endif'+'\n')   
    fout.write('#include <avr/pgmspace.h>'+'\n \n')
    fout.write('#define ' + tablename + '_NUM_CELLS '+ str(tablelength) +'\n')
    fout.write('const char __attribute__((section(".progmem.data"))) ' + tablename + '_DATA [] = { \n')

    try:
        for tablenum in range(numtables):
            try:
                outstring = '\n'
                for num in range(tablelength):
                    ## range between 0 and 1 first
                    x = float(num)/tablelength
                    
                    t_x = math.sin(2*math.pi*x)*(float(tablenum+1)/numtables)

                    scaled = int(math.floor(t_x*127.999))

                    outstring += str(scaled) + ', '
            finally:
                outstring = textwrap.fill(outstring, 80)
                outstring += '\n'
                fout.write(outstring)
    finally:

        fout.write('}; \n \n #endif /* ' + tablename + '_H_ */\n')
        fout.close()
        print "wrote " + outfile


generate("~/Desktop/sin_64level_int8.h", "SIN_64LEVEL", 256, 64)
