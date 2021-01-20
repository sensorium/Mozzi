#!/bin/sh

# run in top folder full of subfolders with mozzi projects
# each with makefiles

for file in *

do
	if [ -d "$file" ]; then
		cd "$file";
		#echo "$file";
		for example in *
			do
				if [ -d "$example" ]; then
					cd "$example";
					echo "$example";
					ls
					rm -r Makefile
					cp ../../Makefile ./
					make clean
					rm -r build*
					rm *ogg
					rm *mp3
					make
					make upload
					# rec -c 1 -r 384000 "$example".mp3 trim 00:10 00:10
					rec -c 1 "$example".wav trim 00:5 00:10
					sox -G -c 1 "$example".wav -r 22050 -c 1 "$example".mp3
					sox -G -c 1 "$example".wav -r 22050 -c 1 "$example".ogg
					rm -r build*
					cd ..;
					#continue
				fi
			done
		cd ..;
		#continue
	fi
done


exit 0
