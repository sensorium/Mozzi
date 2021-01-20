#!/bin/sh

# run in top folder full of subfolders with mozzi projects
# each with makefiles

for file in *

do
	if [ -d "$file" ]; then
		cd "$file";
		#echo "$file";
		for example in *HIFI
			do
				if [ -d "$example" ]; then
					cd "$example";
					echo "$example";
					ls
					make clean
					rm -r Makefile
					cp ../../Makefile ./
					rm -r build*
					rm *ogg
					rm *mp3
					make
					make upload
					rec -c 1 -r 22050 "$example".mp3 trim 00:10 00:10
					sox "$example".mp3 -r 16384 "$example".ogg
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