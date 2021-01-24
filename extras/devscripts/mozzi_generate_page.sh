#!/bin/sh

# args: version-name ($NOW)
if [ -z "$1" ]
  then
    echo "use: mozzi_generate_page.sh <version-name>"
    exit
fi

NOW=$1

#move webpage examples to trash
mv -v ~/Mozzi-page/Mozzi/examples/examples ~/.Trash/mozzi-octopress-examples$(date +"%Y-%m-%d-%H:%M:%S")

#copy compile/examples to webpage examples, for sounds
cp -av  ~/Documents/Arduino/mozzi_compile/examples ~/Mozzi-page/Mozzi/examples/examples

# change to Mozzi-page/Mozzi/examples
cd ~/Mozzi-page/Mozzi/examples/examples/

#run script to build examples web page
~/bin/mozzi_generate_examples_page_buttons.sh > ~/Mozzi-page/Mozzi/examples/example-list.markdown

#change version number everywhere
cd ~/Mozzi/
update_version.sh $NOW

#generate new mozzi docs
cd ~/Mozzi/extras/doxygen-style/
doxygen Doxyfile
# add .nojekyll file to make sure _underscore files get uploaded (now in source folder)
# touch ~/Mozzi/extras/doc/html/.nojekyll

#remove web page docs
mv -v ~/Mozzi-page/Mozzi/doc ~/.Trash/doc$(date +"%Y-%m-%d-%H:%M:%S")

#copy mozzi docs to web page source
cp -av ~/Mozzi/extras/doc ~/Mozzi-page/Mozzi/

#check the page in the browser
cd ~/Mozzi-page/
rake generate
#rake watch

exit 0
