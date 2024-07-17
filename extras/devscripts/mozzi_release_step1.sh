#!/bin/sh

#to release:
#update all version numbers

#move mozzi/compile/examples to examples_old or trash
mv -v ~/Documents/Arduino/mozzi_compile/examples ~/.Trash/examples$(date +"%Y-%m-%d-%H:%M:%S")

#delete all sound files from Mozzi/examples
find ~/Mozzi/examples/ -name "*.ogg" -print -delete
find ~/Mozzi/examples/ -name "*.mp3" -print -delete

#copy mozzi/examples from mozzi to mozzi_compile
cp -a ~/Mozzi/examples ~/Documents/Arduino/mozzi_compile/

#cd mozzi_compile/examples
cd ~/Documents/Arduino/mozzi_compile/examples/

#compile and record STANDARD examples
~/bin/mozzi_compile_examples.sh

#compile and record HIFI examples
~/bin/mozzi_compile_examples_hifi.sh


#check recordings and fix any errors

exit 0
