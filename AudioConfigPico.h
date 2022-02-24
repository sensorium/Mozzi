#ifndef AUDIOCONFIGPICO_H
#define AUDIOCONFIGPICO_H

/* use with PWM_RATE 65536 and phase corrected */
// #define AUDIO_BITS 10 // closer to 9.986
// #define AUDIO_BIAS 507
// #define PHASE_CORRECT 1
// #define AUDIO_RATE 65536         

/* use with default AUDIO_RATE 32768 and PHASE_CORRECT or #define AUDIO_RATE 65536 */
 #define AUDIO_BITS 11 // closer to 10.987
 #define AUDIO_BIAS 1015
 #define PHASE_CORRECT 1            // phase corrected PWM

// #define PHASE_CORRECT 0          // fast PWM
// #define AUDIO_RATE 65536         // uncomment for faster audio rate with fast pwm

/* use with PWM_RATE 32768 and not phase corrected */
// #define AUDIO_BITS 12 // closer to 11.987
// #define AUDIO_BIAS 2030
// #define PHASE_CORRECT 0


// leave gp0 and gp1 available to uart/midi
#define AUDIO_CHANNEL_1_PIN 2
#define AUDIO_CHANNEL_2_PIN 3

// analog input
#define AUDIO_INPUT_1_PIN 0     // analog pin # + 26 for gp pin


#endif        //  #ifndef AUDIOCONFIGPICO_H
