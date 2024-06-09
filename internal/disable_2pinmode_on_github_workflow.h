/* This purely internal header takes care of disabling stereo mode when on a github runner. Intended to be used
 * on platforms that don't support stereo, allowing the compilation to proceed without error. */

#if __has_include("../detect_github_runner.h")  // the marker file we added inside the workflow
                                                // do nothing, if this isn't present
#  if defined(MOZZI_AUDIO_MODE) && MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
#    undef MOZZI_AUDIO_MODE
#    warning Disabled 2pin pwm output mode on github runner
#  endif

#endif
