#!/bin/sh

# this is meant to replace sounds which don't get recorded properly from normal compile and recording check

cd ~/Documents/Arduino/mozzi_compile/examples_sounds_replace/
for group in *

do
	if [ -d "$group" ]; then
		cd "$group";
		#echo "$group";
		for example in *
			do
				if [ -d "$example" ]; then
					cd "$example";
					echo "$example";
					
					cp -av *ogg ~/Documents/Arduino/mozzi_compile/examples/$group/$example/
					cp -av *mp3 ~/Documents/Arduino/mozzi_compile/examples/$group/$example/
			
					cd ..;
					#continue
				fi
			done
		cd ..;
		#continue
	fi
done


exit 0