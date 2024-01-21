#!/bin/sh

#part 2 of mozzi release process, after checking recordings and fixing errors

NOW=$(date +"%Y-%m-%d-%H:%M")

#delete wav files
find ~/Documents/Arduino/mozzi_compile/examples -name "*.wav" -print -delete

#delete sounds for silent sketches
find ~/Documents/Arduino/mozzi_compile/examples -name "Skeleton.mp3" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "Skeleton.ogg" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "Audio_and_Control_Input.mp3" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "Audio_and_Control_Input.ogg" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "Audio_Input.mp3" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "Audio_Input.ogg" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "DCfilter.mp3" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "DCfilter.ogg" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "PDresonant.mp3" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "PDresonant.ogg" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "Mozzi_MIDI_Input.mp3" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "Mozzi_MIDI_Input.ogg" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "Mozzi_Processing_Serial.mp3" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "Mozzi_Processing_Serial.ogg" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "Sinewave_PWM_leds_HIFI.mp3" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "Sinewave_PWM_leds_HIFI.ogg" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "Teensy_USB_MIDI_Input.mp3" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "Teensy_USB_MIDI_Input.ogg" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "TwoWire_Read_ADXL345.mp3" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "TwoWire_Read_ADXL345.ogg" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "IntMap_test.mp3" -print -delete
find ~/Documents/Arduino/mozzi_compile/examples -name "IntMap_test.ogg" -print -delete


#copy extra sounds to compile/examples
~/bin/mozzi_copy_extra_sounds.sh

#remove mozzi/examples
mv -v ~/Mozzi/examples ~/.Trash/examples$(date +"%Y-%m-%d-%H:%M")

#copy compile/examples to mozzi/examples
cp -av  ~/Documents/Arduino/mozzi_compile/examples ~/Mozzi/


#remove sound recordings from mozzi/examples
find ~/Mozzi/examples/ -name "*.ogg" -print -delete
find ~/Mozzi/examples/ -name "*.mp3" -print -delete


#remove mozzi/doc
mv -v ~/Mozzi/extras/doc ~/.Trash/doc$(date +"%Y-%m-%d-%H:%M")

#generate and deploy pages
mozzi_generate_page.sh $NOW
#cd ~/mozzi-octopress
#rake deploy

#commit and push mozzi
#cd ~/Mozzi/
#git add .
#git commit -am "update $NOW"
#git push origin master

#not right from here on:///////////////////////////////////////////////////////
# copy Mozzi files without git stuff, or download pushed version from github and rename (so far that's what i've done)
#rm -r /tmp/Mozzi
#mkdir /tmp/Mozzi
#cp ~/Mozzi/*.h /tmp/Mozzi/
#cp ~/Mozzi/*.cpp /tmp/Mozzi/
#cp ~/Mozzi/*.md /tmp/Mozzi/
#cp ~/Mozzi/*.txt /tmp/Mozzi/
#cp -a ~/Mozzi/config /tmp/Mozzi/
#cp -a ~/Mozzi/examples /tmp/Mozzi/
#cp -a ~/Mozzi/samples /tmp/Mozzi/
#cp -a ~/Mozzi/tables /tmp/Mozzi/

# zip the new dir
#cd /tmp/
#zip -r Mozzi Mozzi
#mv Mozzi.zip ~/Desktop/

# remove old Mozzi from internode

#upload new Mozzi to internode



exit 0
