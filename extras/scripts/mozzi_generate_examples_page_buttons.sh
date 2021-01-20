#!/bin/sh

# run from examples top dir, lists all sketches with full paths

for group in *

do
	if [ -d "$group" ]; then
		echo "<h2 id='$group'> $group </h2>"
		cd "$group";
		# echo "***"
		for example in *
			do
				if [ -d "$example" ]; then
					echo "<a id='$example top'></a>"
					echo "$example" # {% render_partial _partials/link-to-top.markdown %}"
					cd $example
						sound="$(ls *.ogg 2>/dev/null)"
					cd ../
					if [ -n "$sound" ]; then
						echo ""
						echo "<audio controls>"
						echo "<source type='audio/ogg' src='https://github.com/sensorium/Mozzi/blob/gh-pages/examples/examples/$group/$example/$example.ogg?raw=true' preload='auto'></source>"
						echo "<source type='audio/mp3' src='https://github.com/sensorium/Mozzi/blob/gh-pages/examples/examples/$group/$example/$example.mp3?raw=true' preload='auto'></source>"
						echo "Your browser does not support the audio element.</audio>"
						echo ""

					else
						echo "...(no recording of this one)"
						echo ""

					fi

					echo "<button type='button' id=""'"$example"expand""'" "onclick='javascript:toggleDisplay($example, $example""expand, $example""collapse)'>show sketch</button>"
					echo "<button id=""'"$example"collapse""'" "style='display:none' onclick='javascript:toggleDisplay($example, $example""expand, $example""collapse)'>hide sketch</button>"
					echo "<div id='$example' style='display:none'>"
					echo {% highlight c++ %}
					echo {% include_relative examples/$group/$example/$example.ino %}
					echo {% endhighlight %}
					# echo "{% include_code examples/$group/$example/$example.ino lang:c++ %}"
					echo "</div>"
					echo "***"

					#continue
				fi
			done
			cd ..;
            #continue
        fi
done


exit 0
