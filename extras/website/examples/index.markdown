---
layout: single
---

<a name = "top"> </a>

These are the example sketches that come with Mozzi, and the sounds they make.
They're intended to be instructive, rather than musically interesting...  

[01.Basics](#01.Basics) : how to make a sound and simple modifications like gain.  
[02.Control](#02.Control) : changing sound parameters, scheduling, pausing.  
[03.Sensors](#03.Sensors) : using various sensors to change sounds.  
[04.Audio Input](#04.Audio_Input) : sample incoming sound at audio rate.  
[05.Control Filters](#05.Control_Filters) : smoothing and other processing for sensor inputs.  
[06.Synthesis](#06.Synthesis) : for more versatile and detailed sounds.  
[07.Envelopes](#07.Envelopes) : give sounds changing dynamics.  
[08.Samples](#08.Samples) : play recorded sounds in various ways.  
[09.Delays](#09.Delays) : for effects like flanging or reverb.  
[10.Audio Filters](#10.Audio_Filters) : modify audio frequencies.  
[11.Communication](#11.Communication) : non-blocking communication with external devices.  
[12.Misc](#12.Misc) : other useful things.


{% include_relative example-list.markdown %}


<script language="JavaScript">
   function toggleDisplay(stuffToToggle,elementThatExpandsStuff,elementThatCollapsesStuff) {
      if ( stuffToToggle.style.display == 'none' ) {
         stuffToToggle.style.display = '';
         elementThatExpandsStuff.style.display = 'none';
         elementThatCollapsesStuff.style.display = '';
      }
      else {
         stuffToToggle.style.display = 'none';
         elementThatExpandsStuff.style.display = '';
         elementThatCollapsesStuff.style.display = 'none';
      }
   }
</script>
