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

# change mozzi config to STANDARD_PLUS, make sure STANDARD is commented
sed -i.bak 's/^#define AUDIO_MODE STANDARD/\/\/#define AUDIO_MODE STANDARD/'  ~/Mozzi/mozzi_config.h
# make sure STANDARD_PLUS is uncommented, ^ matches start of line
sed -i.bak 's/\/\/#define AUDIO_MODE STANDARD_PLUS/#define AUDIO_MODE STANDARD_PLUS/' ~/Mozzi/mozzi_config.h

# make sure STANDARD is commented, ^ matches start of line
#sed -i.bak 's/^#define AUDIO_MODE STANDARD/\/\/#define AUDIO_MODE STANDARD/' ~/Mozzi/mozzi_config.h
# change mozzi config to STANDARD_PLUS, make sure STANDARD_PLUS is uncommented
#sed -i.bak 's/\/\/#define AUDIO_MODE STANDARD_PLUS/#define AUDIO_MODE STANDARD_PLUS/'  ~/Mozzi/mozzi_config.h

# make sure HIFI is commented, ^ matches start of line
sed -i.bak 's/^#define AUDIO_MODE HIFI/\/\/#define AUDIO_MODE HIFI/'  ~/Mozzi/mozzi_config.h


#compile and record STANDARD examples
~/bin/mozzi_compile_examples.sh

#change mozzi config to HIFI
# make sure HIFI is uncommented
sed -i.bak 's/\/\/#define AUDIO_MODE HIFI/#define AUDIO_MODE HIFI/' ~/Mozzi/mozzi_config.h
# make sure STANDARD is commented, ^ matches start of line
sed -i.bak 's/^#define AUDIO_MODE STANDARD/\/\/#define AUDIO_MODE STANDARD/' ~/Mozzi/mozzi_config.h
# make sure STANDARD_PLUS is commented, ^ matches start of line
sed -i.bak 's/^#define AUDIO_MODE STANDARD_PLUS/\/\/#define AUDIO_MODE STANDARD_PLUS/' ~/Mozzi/mozzi_config.h

#compile and record HIFI examples
~/bin/mozzi_compile_examples_hifi.sh


#check recordings and fix any errors

exit 0
