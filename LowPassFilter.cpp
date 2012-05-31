
// adapted from Joe Marshall's 2011
//  low pass filter based on the one from MeeBlip (http://meeblip.noisepages.com)
// used in Joe's OCTOSynth-0.2 http://www.cs.nott.ac.uk/~jqm/?p=605


#include "LowPassFilter.h"


LowPassFilter::LowPassFilter(): filterA(0),filterB(0),filterQ(0),filterF(127)
{}