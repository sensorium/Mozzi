#!/bin/sh

#argument 1 = version num or name
#NOW=$(date +"%Y-%m-%d-%k:%M")
NOW=$1

cd ~/Mozzi/extras/

sed -i.bak 's/version [0-9]*-[0-9]*-[0-9]*-[0-9]*:[0-9]*/version '$NOW'/g' VERSION.txt

sed -i.bak 's/version [0-9]*-[0-9]*-[0-9]*-[0-9]*:[0-9]*/version '$NOW'/g' NEWS.txt

sed -i.bak 's/version [0-9]*-[0-9]*-[0-9]*-[0-9]*:[0-9]*/version '$NOW'/g' doxygen-style/Doxyfile

cd ~/Mozzi

sed -i.bak 's/version [0-9]*-[0-9]*-[0-9]*-[0-9]*:[0-9]*/version '$NOW'/g' README.md

sed -i.bak 's/version=[0-9]*-[0-9]*-[0-9]*-[0-9]*:[0-9]*/version='$NOW'/g' library.properties

#cd ~/Mozzi-page/Mozzi/_includes/custom/

#sed -i.bak 's/version [0-9]*-[0-9]*-[0-9]*-[0-9]*:[0-9]*/version '$NOW'/g' header.html


exit 0
