/*
 * MozziGuts_impl_CH32.hpp
 *
 * Implementation for WCH CH32X035 (RISC-V) using Standard Peripheral Library (SPL)
 *
 */

// Suppress "narrowing conversion" error for CH32 (RISC-V GCC is strict)
// This allows legacy Mozzi tables (declared as int8_t but containing uint8_t values) to compile.
#pragma GCC diagnostic ignored "-Wnarrowing"

#if !(IS_CH32())
#  error "Wrong implementation included for this platform"
#endif

#include <Arduino.h>

// Forward declare the ISR to ensure C linkage
extern "C" void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

namespace MozziPrivate {

#define MOZZI_AUDIO_PIN_1 PA6 // High Byte (Main)
#define MOZZI_AUDIO_PIN_2 PA7 // Low Byte (Fine tune) for 2PIN_PWM

static void startAudio() {
    // --- 1. Audio Interrupt Timer Setup (TIM1) ---
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    uint32_t period = (SystemCoreClock / MOZZI_AUDIO_RATE) - 1;
    
    TIM_DeInit(TIM1);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = period;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE); 

    // --- 2. PWM Output Setup (TIM3) ---
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // Enable Clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // Configure TIM3 Time Base
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 255; // 8-bit resolution
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // Common PWM Configuration
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 128; 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // --- Setup Pin 1 (PA6 / CH1) ---
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
    // --- Setup Pin 2 (PA7 / CH2) for Hi-Fi ---
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // Init PA7 as AF_PP too

    TIM_OC2Init(TIM3, &TIM_OCInitStructure); // Init CH2
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif

    TIM_ARRPreloadConfig(TIM3, ENABLE);

    // Enable TIM3
    TIM_Cmd(TIM3, ENABLE);
    TIM_CtrlPWMOutputs(TIM3, ENABLE); 
}

void stopMozzi() {
    TIM_Cmd(TIM1, DISABLE);
    TIM_Cmd(TIM3, DISABLE);
}

void MozziRandPrivate::autoSeed() {}

#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)
void setupMozziADC(int8_t speed) {}
void setupFastAnalogRead(int8_t speed) {}
void adcStartConversion(uint8_t channel) {}
void startSecondADCReadOnCurrentChannel() {}
uint8_t adcPinToChannelNum(uint8_t pin) { return pin; }
#endif

// --- Audio Output Functions ---

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
inline void audioOutput(const AudioOutput f) {
    long out = (long)f.l() + MOZZI_AUDIO_BIAS; 
    if (out < 0) out = 0;
    out = out >> (MOZZI_AUDIO_BITS - 8); 
    if (out > 255) out = 255;
    TIM_SetCompare1(TIM3, (uint16_t)out);
}

#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
inline void audioOutput(const AudioOutput f) {
    // 16-bit output split into two 8-bit channels
    long out = (long)f.l() + MOZZI_AUDIO_BIAS;
    
    // Clamp to 16-bit unsigned range just in case
    if (out < 0) out = 0;
    if (out > 65535) out = 65535;

    // High Byte -> CH1 (PA6)
    TIM_SetCompare1(TIM3, (uint16_t)(out >> 8));
    
    // Low Byte -> CH2 (PA7)
    TIM_SetCompare2(TIM3, (uint16_t)(out & 0xFF));
}
#endif

} // namespace MozziPrivate

extern "C" void TIM1_UP_IRQHandler(void) {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
#if (BYPASS_MOZZI_OUTPUT_BUFFER != true)
        MozziPrivate::defaultAudioOutput();
#endif
    }
}
