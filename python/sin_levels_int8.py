## generates a set of sin waves at different amplitudes

import array,os,textwrap,math



def generate(outfilename, tablename, tablelength, numtables):
    fout = open(os.path.expanduser(outfilename), "w")
    fout.write('#ifndef ' + tablename + '_H_' + '\n')
    fout.write('#define ' + tablename + '_H_' + '\n \n')
    fout.write('#include "Arduino.h"'+'\n')
    fout.write('#include <avr/pgmspace.h>'+'\n \n')
    fout.write('#define ' + tablename + '_NUM_CELLS '+ str(tablelength) +'\n')
    outstring = 'const char __attribute__((progmem)) ' + tablename + '_DATA [] = {'
    try:
        for num in range(tablelength):
            ## range between 0 and 1 first
            x = float(num)/tablelength
            
            t_x = math.sin(2*math.pi*x)

            scaled = int(math.floor(t_x*128))

            outstring += str(scaled) + ", "
    finally:
        outstring +=  "};"
        outstring = textwrap.fill(outstring, 80)
        fout.write(outstring)
        fout.write('\n \n #endif /* ' + tablename + '_H_ */\n')



        
        fout.close()
        print "wrote " + outfilename


generate("~/Desktop/test_int8.h", "TEST_256", 256, 1)
