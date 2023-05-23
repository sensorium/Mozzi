// NOTE: The contents of this file are copied mostly verbatim from Arduino_Core_STM32, (c) STMicroelectronics, BSD 3-clause license.
static PinName g_current_pin = NC;

#ifndef ADC_SAMPLINGTIME
#if defined(ADC_SAMPLETIME_8CYCLES_5)
#define ADC_SAMPLINGTIME        ADC_SAMPLETIME_8CYCLES_5;
#elif defined(ADC_SAMPLETIME_12CYCLES)
#define ADC_SAMPLINGTIME        ADC_SAMPLETIME_12CYCLES;
#elif defined(ADC_SAMPLETIME_12CYCLES_5)
#define ADC_SAMPLINGTIME        ADC_SAMPLETIME_12CYCLES_5;
#elif defined(ADC_SAMPLETIME_13CYCLES_5)
#define ADC_SAMPLINGTIME        ADC_SAMPLETIME_13CYCLES_5;
#elif defined(ADC_SAMPLETIME_15CYCLES)
#define ADC_SAMPLINGTIME        ADC_SAMPLETIME_15CYCLES;
#elif defined(ADC_SAMPLETIME_16CYCLES)
#define ADC_SAMPLINGTIME        ADC_SAMPLETIME_16CYCLES;
#elif defined(ADC_SAMPLETIME_19CYCLES_5)
#define ADC_SAMPLINGTIME        ADC_SAMPLETIME_19CYCLES_5;
#endif
#endif /* !ADC_SAMPLINGTIME */

#if defined(ADC_VER_V5_V90) && !defined(ADC3_SAMPLINGTIME)
#define ADC3_SAMPLINGTIME       ADC3_SAMPLETIME_24CYCLES_5;
#endif

#if defined(ADC4_SAMPLETIME_19CYCLES_5) && !defined(ADC4_SAMPLINGTIME)
#define ADC4_SAMPLINGTIME       ADC4_SAMPLETIME_19CYCLES_5;
#endif

/*
 * Minimum ADC sampling time is required when reading
 * internal channels so set it to max possible value.
 * It can be defined more precisely by defining:
 * ADC_SAMPLINGTIME_INTERNAL
 * to the desired ADC sample time.
 */
#ifndef ADC_SAMPLINGTIME_INTERNAL
#if defined(ADC_SAMPLETIME_480CYCLES)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_480CYCLES
#elif defined(ADC_SAMPLETIME_384CYCLES)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_384CYCLES
#elif defined(ADC_SAMPLETIME_810CYCLES_5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_810CYCLES_5
#elif defined(ADC_SAMPLETIME_814CYCLES)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_814CYCLES
#elif defined(ADC_SAMPLETIME_640CYCLES_5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_640CYCLES_5
#elif defined(ADC_SAMPLETIME_601CYCLES_5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_601CYCLES_5
#elif defined(ADC_SAMPLETIME_247CYCLES_5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_247CYCLES_5
#elif defined(ADC_SAMPLETIME_239CYCLES_5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_239CYCLES_5
#elif defined(ADC_SAMPLETIME_160CYCLES_5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_160CYCLES_5
#else
#error "ADC sampling time could not be defined for internal channels!"
#endif
#endif /* !ADC_SAMPLINGTIME_INTERNAL */


#ifndef ADC_CLOCK_DIV
#ifdef ADC_CLOCK_SYNC_PCLK_DIV4
#define ADC_CLOCK_DIV       ADC_CLOCK_SYNC_PCLK_DIV4
#elif ADC_CLOCK_SYNC_PCLK_DIV2
#define ADC_CLOCK_DIV       ADC_CLOCK_SYNC_PCLK_DIV2
#elif defined(ADC_CLOCK_ASYNC_DIV4)
#define ADC_CLOCK_DIV       ADC_CLOCK_ASYNC_DIV4
#endif
#endif /* !ADC_CLOCK_DIV */

#ifndef ADC_REGULAR_RANK_1
#define ADC_REGULAR_RANK_1  1
#endif

// adc_read_value() modified to keep only the ADC setup, but not the (blocking) ADC read
bool adc_setup_read(PinName pin, uint32_t resolution) {
  AdcHandle = {};
  ADC_ChannelConfTypeDef  AdcChannelConf = {};
  uint32_t samplingTime = ADC_SAMPLINGTIME;
  uint32_t channel = 0;
  uint32_t bank = 0;

  if ((pin & PADC_BASE) && (pin < ANA_START)) {
#if defined(STM32H7xx) || defined(STM32MP1xx)
#ifdef ADC3
    AdcHandle.Instance = ADC3;
#else
    AdcHandle.Instance = ADC2;
#endif
#else
    AdcHandle.Instance = ADC1;
#if defined(ADC5) && defined(ADC_CHANNEL_TEMPSENSOR_ADC5)
    if (pin == PADC_TEMP_ADC5) {
      AdcHandle.Instance = ADC5;
    }
#endif
#endif
    channel = get_adc_internal_channel(pin);
    samplingTime = ADC_SAMPLINGTIME_INTERNAL;
  } else {
    AdcHandle.Instance = (ADC_TypeDef *)pinmap_peripheral(pin, PinMap_ADC);
    channel = get_adc_channel(pin, &bank);
#if defined(ADC_VER_V5_V90)
    if (AdcHandle.Instance == ADC3) {
      samplingTime = ADC3_SAMPLINGTIME;
    }
#endif
#if defined(ADC4_SAMPLINGTIME)
    if (AdcHandle.Instance == ADC4) {
      samplingTime = ADC4_SAMPLINGTIME;
    }
#endif
  }

  if (AdcHandle.Instance == NP) {
    return 0;
  }

#ifdef ADC_CLOCK_DIV
  AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_DIV;                 /* (A)synchronous clock mode, input ADC clock divided */
#endif
#ifdef ADC_RESOLUTION_12B
  switch (resolution) {
#ifdef ADC_RESOLUTION_6B
    case 6:
      AdcHandle.Init.Resolution          = ADC_RESOLUTION_6B;             /* resolution for converted data */
      break;
#endif
    case 8:
      AdcHandle.Init.Resolution          = ADC_RESOLUTION_8B;             /* resolution for converted data */
      break;
    case 10:
      AdcHandle.Init.Resolution          = ADC_RESOLUTION_10B;            /* resolution for converted data */
      break;
    case 12:
    default:
      AdcHandle.Init.Resolution          = ADC_RESOLUTION_12B;            /* resolution for converted data */
      break;
#ifdef ADC_RESOLUTION_14B
    case 14:
      AdcHandle.Init.Resolution          = ADC_RESOLUTION_14B;            /* resolution for converted data */
      break;
#endif
#ifdef ADC_RESOLUTION_16B
    case 16:
      AdcHandle.Init.Resolution          = ADC_RESOLUTION_16B;            /* resolution for converted data */
      break;
#endif
  }
#else
  UNUSED(resolution);
#endif
#ifdef ADC_DATAALIGN_RIGHT
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;           /* Right-alignment for converted data */
#endif
#ifdef ADC_SCAN_SEQ_FIXED
  AdcHandle.Init.ScanConvMode          = ADC_SCAN_SEQ_FIXED;            /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
#else
  AdcHandle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
#endif
#ifdef ADC_EOC_SINGLE_CONV
  AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;           /* EOC flag picked-up to indicate conversion end */
#endif
#if !defined(STM32F1xx) && !defined(STM32F2xx) && !defined(STM32F4xx) && \
    !defined(STM32F7xx) && !defined(ADC1_V2_5)
  AdcHandle.Init.LowPowerAutoWait      = DISABLE;                       /* Auto-delayed conversion feature disabled */
#endif
#if !defined(STM32F1xx) && !defined(STM32F2xx) && !defined(STM32F3xx) && \
    !defined(STM32F4xx) && !defined(STM32F7xx) && !defined(STM32G4xx) && \
    !defined(STM32H7xx) && !defined(STM32L4xx) && !defined(STM32L5xx) && \
    !defined(STM32MP1xx) && !defined(STM32WBxx) ||  defined(ADC_SUPPORT_2_5_MSPS)
  AdcHandle.Init.LowPowerAutoPowerOff  = DISABLE;                       /* ADC automatically powers-off after a conversion and automatically wakes-up when a new conversion is triggered */
#endif
#ifdef ADC_CHANNELS_BANK_B
  AdcHandle.Init.ChannelsBank          = bank;
#elif defined(ADC_CHANNELS_BANK_A)
  AdcHandle.Init.ChannelsBank          = ADC_CHANNELS_BANK_A;
#endif
  AdcHandle.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
#if !defined(STM32F0xx) && !defined(STM32L0xx)
  AdcHandle.Init.NbrOfConversion       = 1;                             /* Specifies the number of ranks that will be converted within the regular group sequencer. */
#endif
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
#if !defined(STM32C0xx) && !defined(STM32F0xx) && !defined(STM32G0xx) && \
    !defined(STM32L0xx) && !defined(STM32WLxx) && !defined(ADC_SUPPORT_2_5_MSPS)
  AdcHandle.Init.NbrOfDiscConversion   = 0;                             /* Parameter discarded because sequencer is disabled */
#endif
  AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
#if !defined(STM32F1xx) && !defined(ADC1_V2_5)
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because software trigger chosen */
#endif
#if !defined(STM32F1xx) && !defined(STM32H7xx) && !defined(STM32MP1xx) && \
    !defined(ADC1_V2_5)
  AdcHandle.Init.DMAContinuousRequests = DISABLE;                       /* DMA one-shot mode selected (not applied to this example) */
#endif
#ifdef ADC_CONVERSIONDATA_DR
  AdcHandle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;      /* Regular Conversion data stored in DR register only */
#endif
#ifdef ADC_OVR_DATA_OVERWRITTEN
  AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;      /* DR register is overwritten with the last conversion result in case of overrun */
#endif
#ifdef ADC_LEFTBITSHIFT_NONE
  AdcHandle.Init.LeftBitShift          = ADC_LEFTBITSHIFT_NONE;         /* No bit shift left applied on the final ADC conversion data */
#endif

#if defined(STM32F0xx)
  AdcHandle.Init.SamplingTimeCommon    = samplingTime;
#endif
#if defined(STM32C0xx) || defined(STM32G0xx) || defined(STM32U5xx) || \
    defined(STM32WLxx) || defined(ADC_SUPPORT_2_5_MSPS)
  AdcHandle.Init.SamplingTimeCommon1   = samplingTime;              /* Set sampling time common to a group of channels. */
  AdcHandle.Init.SamplingTimeCommon2   = samplingTime;              /* Set sampling time common to a group of channels, second common setting possible.*/
#endif
#if defined(STM32L0xx)
  AdcHandle.Init.LowPowerFrequencyMode = DISABLE;                       /* To be enabled only if ADC clock < 2.8 MHz */
  AdcHandle.Init.SamplingTime          = samplingTime;
#endif
#if !defined(STM32F0xx) && !defined(STM32F1xx) && !defined(STM32F2xx) && \
    !defined(STM32F3xx) && !defined(STM32F4xx) && !defined(STM32F7xx) && \
    !defined(STM32L1xx) && !defined(ADC_SUPPORT_2_5_MSPS)
  AdcHandle.Init.OversamplingMode      = DISABLE;
  /* AdcHandle.Init.Oversample ignore for STM32L0xx as oversampling disabled */
  /* AdcHandle.Init.Oversampling ignored for other as oversampling disabled */
#endif
#if defined(ADC_CFGR_DFSDMCFG) && defined(DFSDM1_Channel0)
  AdcHandle.Init.DFSDMConfig           = ADC_DFSDM_MODE_DISABLE;        /* ADC conversions are not transferred by DFSDM. */
#endif
#ifdef ADC_TRIGGER_FREQ_HIGH
  AdcHandle.Init.TriggerFrequencyMode  = ADC_TRIGGER_FREQ_HIGH;
#endif
#ifdef ADC_VREF_PPROT_NONE
  AdcHandle.Init.VrefProtection = ADC_VREF_PPROT_NONE;
#endif

  AdcHandle.State = HAL_ADC_STATE_RESET;
  AdcHandle.DMA_Handle = NULL;
  AdcHandle.Lock = HAL_UNLOCKED;
  /* Some other ADC_HandleTypeDef fields exists but not required */

  g_current_pin = pin; /* Needed for HAL_ADC_MspInit*/

  if (HAL_ADC_Init(&AdcHandle) != HAL_OK) {
    return 0;
  }

  AdcChannelConf.Channel      = channel;                          /* Specifies the channel to configure into ADC */

#if defined(STM32G4xx) || defined(STM32L4xx) || defined(STM32L5xx) || \
    defined(STM32WBxx)
  if (!IS_ADC_CHANNEL(&AdcHandle, AdcChannelConf.Channel)) {
#else
  if (!IS_ADC_CHANNEL(AdcChannelConf.Channel)) {
#endif
    return 0;
  }
#if defined(ADC_SCAN_SEQ_FIXED) && defined(ADC_RANK_CHANNEL_NUMBER)
  AdcChannelConf.Rank         = ADC_RANK_CHANNEL_NUMBER;          /* Enable the rank of the selected channels when not fully configurable */
#else
  AdcChannelConf.Rank         = ADC_REGULAR_RANK_1;               /* Specifies the rank in the regular group sequencer */
#endif
#if !defined(STM32L0xx)
#if !defined(STM32G0xx)
  AdcChannelConf.SamplingTime = samplingTime;                     /* Sampling time value to be set for the selected channel */
#else
  AdcChannelConf.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;        /* Sampling time value to be set for the selected channel */
#endif
#endif
#if defined(ADC_DIFFERENTIAL_ENDED) && !defined(ADC1_V2_5)
  AdcChannelConf.SingleDiff   = ADC_SINGLE_ENDED;                 /* Single-ended input channel */
  AdcChannelConf.OffsetNumber = ADC_OFFSET_NONE;                  /* No offset subtraction */
#endif
#if !defined(STM32C0xx) && !defined(STM32F0xx) && !defined(STM32F1xx) && \
    !defined(STM32F2xx) && !defined(STM32G0xx) && !defined(STM32L0xx) && \
    !defined(STM32L1xx) && !defined(STM32WBxx) && !defined(STM32WLxx) && \
    !defined(ADC1_V2_5)
  AdcChannelConf.Offset = 0;                                      /* Parameter discarded because offset correction is disabled */
#endif
#if defined (STM32H7xx) || defined(STM32MP1xx)
  AdcChannelConf.OffsetRightShift = DISABLE;                      /* No Right Offset Shift */
  AdcChannelConf.OffsetSignedSaturation = DISABLE;                /* Signed saturation feature is not used */
#endif

  /*##-2- Configure ADC regular channel ######################################*/
  if (HAL_ADC_ConfigChannel(&AdcHandle, &AdcChannelConf) != HAL_OK) {
    /* Channel Configuration Error */
    return 0;
  }

#if defined(ADC_CR_ADCAL) || defined(ADC_CR2_RSTCAL)
  /*##-2.1- Calibrate ADC then Start the conversion process ####################*/
#if defined(ADC_CALIB_OFFSET)
  if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
#elif defined(ADC_SINGLE_ENDED) && !defined(ADC1_V2_5)
  if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED) !=  HAL_OK)
#else
  if (HAL_ADCEx_Calibration_Start(&AdcHandle) !=  HAL_OK)
#endif
  {
    /* ADC Calibration Error */
    return 0;
  }
#endif

  /*##-3- Start the conversion process ####################*/
  if (HAL_ADC_Start(&AdcHandle) != HAL_OK) {
    /* Start Conversion Error */
    return 0;
  }

  return HAL_OK;
}
