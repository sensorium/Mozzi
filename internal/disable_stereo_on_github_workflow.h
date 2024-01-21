/* This purely internal header takes care of disabling stereo mode when on a github runner. Intended to be used
 * on platforms that don't support stereo, allowing the compilation to proceed without error. */

#if __has_include("../detect_github_runner.h")  // the marker file we added inside the workflow
                                                // do nothing, if this isn't present
#  if defined(MOZZI_AUDIO_CHANNELS) && (MOZZI_AUDIO_CHANNELS > 1)
#    define GITHUB_RUNNER_ACCEPT_STEREO_IN_MONO
#    undef MOZZI_AUDIO_CHANNELS
#    define MOZZI_AUDIO_CHANNELS MOZZI_MONO
#    warning Disabled stereo compilation while in Github runner.
#  endif

#endif
