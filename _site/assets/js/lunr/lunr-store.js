var store = [{
        "title": "ISEA2013 workshop announcement",
        "excerpt":"Mozzi workshop at international Symposium on Electonic Art, June 8-9, 2013 in Sydney. Through guided making, participants will learn to use Mozzi and Arduino to combine sensors with synthesis to make complex, reactive sounds. By the end of the workshop everyone will have created and customised their own responsive sound-generating...","categories": ["workshops"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/workshops/isea2013-workshop-announcement/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "Music Hack Day Barcelona workshop announcement",
        "excerpt":"Mozzi workshop at Music Hack Day, 13-14 June 2013, Barcelona.  ","categories": ["workshops"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/workshops/music-hack-day-barcelona-workshop-announcement/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "ICAD2013 workshop announcement",
        "excerpt":"Mozzi workshop at International Conference on Auditory Display, July 6-10, 2013, Lodz University of Technology, Poland.   ","categories": ["workshops"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/workshops/icad2013-workshop-announcement/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "Audio and Control ADC together",
        "excerpt":"Version 2013-07-29-18:09 enables audio and control rate analog inputs to work at the same time. Audio input is on analog pin A0. Each time an audio sample is output, the audio input is sampled. For each audio input sample, one control input is also converted, by changing the input channel...","categories": ["updates","adc"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/updates/adc/audio-and-control-adc-together/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "Upcoming Mozzi talk at ICMC2013 in Perth",
        "excerpt":"There’ll be a conference presentation about Mozzi as part of ICMC in Perth. The paper will be on Thursday 15 August, 14:15 – 16:15, State Theatre Centre – Studio Underground.  ","categories": ["conference","talk"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/conference/talk/upcoming-mozzi-talk-at-icmc2013-in-perth/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "Simpler adc with mozziAnalogRead",
        "excerpt":"Version 2013-08-25 evolves the adc developments of the previous release, by replacing the various ways of reading analog inputs (adcGetResult(), adcReadAllChannels(), etc.) with mozziAnalogRead(). Use it the same as analogRead(). It doesn’t need any special setup. Audio input still uses getAudioInput(), not mozziAnalogRead(). Here’s the Mozzi/extras/NEWS.txt entry of the changes...","categories": ["updates","adc"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/updates/adc/simpler-adc-with-mozzianalogread/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "Mozzi Marketplace on Sharetribe",
        "excerpt":"We (well, me and my brother Steph) are experimenting with the idea of a Mozzi marketplace where people can sell or swap things made with Mozzi. The first test is with Sharetribe. We hope a marketplace will help ferment the Mozzi culture, but also there’s a remote thought of generating...","categories": ["marketplace","kits"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/marketplace/kits/mozzi-marketplace-on-sharetribe/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "Making Things Purr, Growl and Sing - Mozzi workshop in Melbourne",
        "excerpt":"We will be running a Mozzi synth workshop in Melbourne on Thursday 20th March, 14:00-20:00. Participants will get a grip on Mozzi, and have a chance to work on their own projects, with help from me (Tim, the creator of Mozzi) and my brother Stephen Barrass (a media arts researcher...","categories": ["workshops"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/workshops/making-things-purr/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "New STANDARD_PLUS audio mode removes 16kHz whine",
        "excerpt":"Took a long time coming… version 2014-03-13-14:09 MozziGuts.cpp/h,mozzi_config.h - Added STANDARD_PLUS audio mode. This is now the default audio mode. Just like STANDARD, but with 32768Hz PWM rate, so the whine which some people hear in STANDARD mode is now out of hearing range. A notch filter is not required...","categories": ["updates","audio modes"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/updates/audio%20modes/update-to-version-2014-03-13-14-09/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "STANDARD_PLUS performance improves",
        "excerpt":"Due to recent improvements in output buffer code, STANDARD_PLUS audio mode is now the default, and performs as well as the old STANDARD mode,  while removing the high-pitched PWM carrier frequency whine out of hearing range.  ","categories": ["audio","updates"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/audio/updates/standard-plus-performance-improves/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "Teensy 3.0-3.1 port progress",
        "excerpt":"This is an update regarding work in progress on the port to Teensy 3.0 and 3.1, to let donors to the project know where the work is up to. It’s taking a bit longer than anticipated, but I want to assure people who are waiting that the work is well...","categories": ["ports"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/ports/teensy-3-dot-0-3-dot-1-port-progress/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "Teensy 3.0/3.1 test release",
        "excerpt":"This release is a test of the recent work on the Mozzi Teensy 3 port in response to the donation campaign reaching the grand goal of $1000. Thankyou to all the dozen or so donors, especially my brother and sister, who donated more than half! It should run as usual...","categories": ["ports","Teensy","releases"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/ports/teensy/releases/teensy-3-dot-0-slash-3-dot-1-test-release-version-2014-08-09-14-15/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "yes, you can donate!",
        "excerpt":"Actually, I’m not asking for donations now…  But I am offering paid Mozzi consulting.  ","categories": [],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/yes/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "Use Arduino 1.0.5",
        "excerpt":"Mozzi is broken in Arduino IDE 1.0.6 and 1.6.3.  For now, use Arduino 1.0.5.   The symptom is that more complex sketches run too slowly for smooth audio synthesis.  I’ll try to sort it out!  ","categories": [],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/use-arduino-1-dot-0-5/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "Release v1.0.2",
        "excerpt":"New version released, named Aedes australis, v1.0.2. Updated 2015-05-11-20:23 First (slightly edited) versioned release using GitHub release process. Warning: This release and all previous versions of Mozzi run slow when using Arduino IDE versions greater than 1.0.5, at least on OSX, causing glitches on many of the included examples. Simple...","categories": [],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/release-1-dot-0-2/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "Howto for Arduino 1.6.11",
        "excerpt":"Now Mozzi can work at full speed on the latest Arduino IDE. It works, but now Mozzi needs an update to install smoothly in recent IDE’s. Coming soon, with some other fixes. The problem for Mozzi on Arduino IDE’s after 1.0.5 used to be that sketches were compiled for small...","categories": ["fixes","updates"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/fixes/updates/howto-for-arduino-1-dot-6-11/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "Update 1.0.3rc1 available",
        "excerpt":"release v1.0.3 Mostly bug fixes and maintainance, with update to suit Arduino 1.5+. Though tests show the 1.0.5 IDE still produces faster code than any speed optimisation levels compiled with the newer versions. added library.properties for Arduino 1.5+ MozziGuts.cpp and .h, and mozzi_config.h: added STEREO_HACK config option, set it to...","categories": [],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/update-1-dot-0-3rc1-available/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "Tweak for faster code",
        "excerpt":"If you need your synth to run fast, Arduino versions above 1.5 can be tweaked to optimise compiled code for speed instead of small size. Find Arduino’s platform.txt (on OSX you can find it by searching in Users/your_name/Library/Arduino15). Search and replace -Os with -O2. Save. It’s explained more thoroughly (for...","categories": [],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/tweak-for-faster-code/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "v1.0.3rc2 Teensy bugfix",
        "excerpt":"release v1.0.3rc2      Teensy now compiles, bit of a hack added    || defined(TEENSYDUINO)   to all Teensy conditional compile lines like this:   #if defined(MK20DX128) || defined(MK20DX256) || defined(TEENSYDUINO)  // teensy 3, 3.1   examples, removed commented ADC.h line from all examples, Teensy doesn’t seem to need it anymore  ","categories": ["releases,","bugfix"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/releases,/bugfix/v1-dot-0-3rc2-teensy-bugfix/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "v1.0.3rc4 fixes compile",
        "excerpt":"Removed stray FrequencyTimer files from main folder, which stopped anything compiling…!  ","categories": ["releases,","bugfix"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/releases,/bugfix/v1-dot-0-3rc4-fixes-compile/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "v1.0.3rc5 HIFI compile fix",
        "excerpt":"HIFI sketches compile again. The problems were due to more errors around changes in Mozzi’s modified FrequencyTimer2 library usage which were not tested properly before release.  ","categories": ["releases,","bugfix"],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/releases,/bugfix/v1-dot-0-3rc5-hifi-compile-fix/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "v1.0.3rc6 HIFI works",
        "excerpt":"HIFI actually works this time… ie. it’s unbroken  ","categories": [],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/v1-dot-0-3rc6-hifi-works/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "MozziByte synth shield kickstarter launched",
        "excerpt":"MozziByte, a shield for the Arduino Pro Mini. On kickstarter. Making synths with Mozzi, we frequently use the same basic audio components and connectors, and putting them together every time is a pain. So we’ve created the MozziByte. MozziByte has an amp, phono socket, speaker pins, battery connector, power switch...","categories": [],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/mozzibyte-synth-shield-kickstarter-launched/",
        "teaser":"http://localhost:4000/Mozzi"},{
        "title": "MozziByte workshop at AudioMostly in Wales",
        "excerpt":"The 2018 Conference Programme from 12-14 September at Wrexham Glyndŵr University in Wales, UK includes a MozziByte Workshop for attendees…  ","categories": [],
        "tags": [],
        "url": "http://localhost:4000/Mozzi/mozzibyte-workshop-at-audiomostly/",
        "teaser":"http://localhost:4000/Mozzi"}]
