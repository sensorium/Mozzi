#!/usr/bin/python3

"""
 * make_standard_tables.py
 * 
 * This file is part of Mozzi.
 *
 * Copyright 2012-2024 Thomas Combriat and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 * This file allows to regenerate the standard tables of Mozzi (sin, cos, cosphase, triangle and saw)
"""

import numpy as np  # numerics
import textwrap
import os as os
#import math as math  # math

folder = ".."+os.sep+".."+os.sep+"tables" + os.sep

def write_header(fout, samplerate,name):
    tablename = name.upper()+str(samplerate)
    fout.write('#ifndef ' + tablename + '_H_' + '\n')
    fout.write('#define ' + tablename + '_H_' + '\n \n')
    fout.write("/**\n This table is part of Mozzi\n Generated with extras/python/make_standard_tables.py\n*/\n\n")
    fout.write('#include <Arduino.h>'+'\n')
    fout.write('#include "mozzi_pgmspace.h"'+'\n \n')
    fout.write('#define ' + tablename + '_NUM_CELLS '+ str(samplerate)+'\n')
    fout.write('#define ' + tablename + '_SAMPLERATE '+ str(samplerate)+'\n \n')
    fout.write('CONSTTABLE_STORAGE(int8_t) ' + tablename + '_DATA [] = {\n')

def write_footer(fout,samplerate,name):
    tablename = name.upper()+str(samplerate)
    fout.write('\n }; \n \n #endif /* ' + tablename + '_H_ */\n')




    
#### SIN

SR = [256,512,1024,2048,4096,8192]
name = "sin"

for s in SR:
    fout = open(folder+name+str(s)+"_int8.h","w")
    
    write_header(fout,s,name)
    
    t = np.linspace(0,2*np.pi,num=s,endpoint=False)
    outstring=""
    for i in range(s):
        outstring+=str(round(127*np.sin(t[i])))+", "
        
    outstring = textwrap.fill(outstring,80)
    fout.write(outstring)
    write_footer(fout,s,name)
    fout.close()

    print("Wrote:"+name.upper()+str(s)+"_INT8")
    
    
#### COS
# Note that this is actually a negative cos

SR = [256,512,1024,2048,4096,8192]
name = "cos"

for s in SR:
    fout = open(folder+name+str(s)+"_int8.h","w")
    
    write_header(fout,s,name)
    
    t = np.linspace(0,2*np.pi,num=s,endpoint=False)
    outstring=""
    for i in range(s):
        outstring+=str(round(-127*np.cos(t[i])))+", "
        
    outstring = textwrap.fill(outstring,80)
    fout.write(outstring)
    write_footer(fout,s,name)
    fout.close()

    print("Wrote:"+name.upper()+str(s)+"_INT8")


#### COSPHASE

SR = [256,2048,8192]
name = "cosphase"

for s in SR:
    fout = open(folder+name+str(s)+"_int8.h","w")
    
    write_header(fout,s,name)
    
    t = np.linspace(0,2*np.pi,num=s,endpoint=False)
    outstring=""
    for i in range(s):
        outstring+=str(round(127*np.cos(t[i])))+", "
        
    outstring = textwrap.fill(outstring,80)
    fout.write(outstring)
    write_footer(fout,s,name)
    fout.close()

    print("Wrote:"+name.upper()+str(s)+"_INT8")




#### TRIANGLE

SR = [512,1024,2048]
name = "triangle"

for s in SR:
    fout = open(folder+name+str(s)+"_int8.h","w")
    
    write_header(fout,s,name)
    
    t = np.linspace(0,2*np.pi,num=s,endpoint=False)
    outstring=""
    for i in range(s):
        if (i<=s/4):
            outstring+=str(round(i*127./s*4))+", "
        elif (i<=s*3/4):
            outstring+=str(round(127-(i-s/4)*(127*2)/(s/2)))+", "
        else:
            outstring+=str(round(-127.+127.*(i-s*3/4)/s*4))+", "
        
    outstring = textwrap.fill(outstring,80)
    fout.write(outstring)
    write_footer(fout,s,name)
    fout.close()

    print("Wrote:"+name.upper()+str(s)+"_INT8")




    
#### SAW

SR = [256,512,1024,2048,4096,8192]
name = "saw"

for s in SR:
    fout = open(folder+name+str(s)+"_int8.h","w")
    
    write_header(fout,s,name)
    
    t = np.linspace(0,2*np.pi,num=s,endpoint=False)
    outstring=""
    for i in range(s):
        outstring+=str(str(round(-127+127*2.*i/(s-1)))+", ")
        
    outstring = textwrap.fill(outstring,80)
    fout.write(outstring)
    write_footer(fout,s,name)
    fout.close()

    print("Wrote:"+name.upper()+str(s)+"_INT8")
