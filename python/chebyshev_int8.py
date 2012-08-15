## generates chebyshev polynomial curve tables for WaveShaper

import array,os,textwrap,math

##resources:
##http://www.obiwannabe.co.uk/html/music/6SS/six-waveshaper.html
##http://mathworld.wolfram.com/ChebyshevPolynomialoftheFirstKind.html
##The first few Chebyshev polynomials of the first kind are
##T_0(x)	=	1
##T_1(x)	=	x
##T_2(x)	=	2x^2-1
##T_3(x)	=	4x^3-3x
##T_4(x)	=	8x^4-8x^2+1
##T_5(x)	=	16x^5-20x^3+5x
##T_6(x)	=	32x^6-48x^4+18x^2-1


def generate(outfilename, tablename, tablelength, curvenum):
    fout = open(os.path.expanduser(outfilename), "w")
    fout.write('#ifndef ' + tablename + '_H_' + '\n')
    fout.write('#define ' + tablename + '_H_' + '\n \n')
    fout.write('#include "Arduino.h"'+'\n')
    fout.write('#include <avr/pgmspace.h>'+'\n \n')
    fout.write('#define ' + tablename + '_NUM_CELLS '+ str(tablelength) +'\n')
    outstring = 'const char __attribute__((progmem)) ' + tablename + '_DATA [] = {'
    try:
        for num in range(tablelength):
            ## range between -1 and 1 first
            x = 2*(float(num-(tablelength/2)))/tablelength

            if curvenum == 3:
                t_x = 4*pow(x,3)-3*x
            elif curvenum == 4:
                t_x = 8*pow(x,4)-8*pow(x,2)+1
            elif curvenum == 5:
                t_x = 16*pow(x,5)-20*pow(x,3)+5*x
            elif curvenum == 6:
                t_x = 32*pow(x,6)-48*pow(x,4)+18*pow(x,2)-1

            scaled = int(math.floor(t_x*127.999))

            outstring += str(scaled) + ", "
    finally:
        outstring +=  "};"
        outstring = textwrap.fill(outstring, 80)
        fout.write(outstring)
        fout.write('\n \n #endif /* ' + tablename + '_H_ */\n')
        fout.close()
        print "wrote " + outfilename


generate("~/Desktop/waveshaper_chebyshev_3rd_256_int8.h", "CHEBYSHEV_3RD_256", 256, 3)
generate("~/Desktop/waveshaper_chebyshev_4th_256_int8.h", "CHEBYSHEV_4TH_256", 256, 4)
generate("~/Desktop/waveshaper_chebyshev_5th_256_int8.h", "CHEBYSHEV_5TH_256", 256, 5)
generate("~/Desktop/waveshaper_chebyshev_6th_256_int8.h", "CHEBYSHEV_6TH_256", 256, 6)
