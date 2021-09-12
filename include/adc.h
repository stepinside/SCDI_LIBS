// ----------------------------------------------------------------------------
/// @file         adc.h
/// @addtogroup   ADC_LIB   ADC Library (libadc.a, adc.h)
/// @{
/// @brief        The ADC library provides a set of functions for analog to digital conversion.
/// @details      The first function to be called in order to use the adc is
///               adc_init. After that a channel shall be selected by adc_selectChannel
///               before conversions may be trigged by adc_start8 or adc_start10.
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef ADC_H_
#define ADC_H_

// ----------------------------------------------------------------------------
/// @brief        used to set the ADC's reference voltage for the conversion.
// ----------------------------------------------------------------------------
typedef enum
{
    ADC_AREF = 0x0, ///< AREF becomes the reference voltage
    ADC_AVCC = 0x1, ///< AVCC becomes the reference voltage
    ADC_11 = 0x2,   ///< the reference voltage is 1.1V
    ADC_256 = 0x3   ///< the reference voltage is 2.56V
} ADC_RefVoltage;

// ----------------------------------------------------------------------------
/// @brief        used to define the source to trigger the ADC in auto trigger mode.
// ----------------------------------------------------------------------------
typedef enum
{
    ADC_TS_FREE_RUNNING = 0, ///< trigger the ADC whenever it's ready
    ADC_TS_INT0 = 2,         ///< trigger the ADC by the external interrupt 0
    ADC_TS_TIMER0_COMPA = 3, ///< trigger the ADC on a timer0 compare match A
    ADC_TS_TIMER0_OVF = 4,   ///< trigger the ADC when timer0 overruns
    ADC_TS_TIMER1_COMPB = 5, ///< trigger the ADC on a timer1 compare match B
    ADC_TS_TIMER1_OVF = 6,   ///< trigger the ADC when timer1 overruns
} ADC_TriggerSource;

#ifdef __cplusplus
extern "C"
{
#endif

    // ----------------------------------------------------------------------------
    /// @brief        Initializes the ADC. Does actually nothing.
    // ----------------------------------------------------------------------------
    void adc_init();

    // ----------------------------------------------------------------------------
    /// @brief        Selects the ADC input whose voltage shall be converted.
    /// @param[in]    refVoltage  the reference voltage
    /// @param[in]    channelNo   the number of the channel to convert (0 - 15).
    // ----------------------------------------------------------------------------
    void adc_selectChannel(ADC_RefVoltage refVoltage, uint8_t channelNo);

    // ----------------------------------------------------------------------------
    /// @brief        Triggers an 8-Bit analog to digital conversion.
    /// @details      When the conversion is finished, the given callback function
    ///               is called. If the callback function returns 0, no conversion
    ///               will follow, otherwise a new conversion gets automatically
    ///               triggered.
    /// @param[in]    callback    the callback function.
    // ----------------------------------------------------------------------------
    void adc_trigger8(uint8_t (*callback)(uint8_t value));

    // ----------------------------------------------------------------------------
    /// @brief        Triggers a 10-Bit analog to digital conversion.
    /// @details      When the conversion is finished, the given callback function
    ///               is called. If the callback function returns 0, no conversion
    ///               will follow, otherwise a new conversion gets automatically
    ///               triggered.
    /// @param[in]    callback    the callback function.
    // ----------------------------------------------------------------------------
    void adc_trigger10(uint8_t (*callback)(uint16_t value));

    // ----------------------------------------------------------------------------
    /// @brief        Prepares an 8-Bit analog to digital conversion, that gets triggered
    ///               by the selected source.
    /// @details      When the conversion is finished, the given callback function
    ///               is called and the conversion gets automatically retriggered by
    ///               the selected source.
    /// @param[in]    callback    the callback function.
    // ----------------------------------------------------------------------------
    void adc_autoTrigger8(ADC_TriggerSource triggerSource, void (*callback)(uint8_t value));

    // ----------------------------------------------------------------------------
    /// @brief        Prepares a 10-Bit analog to digital conversion, that gets triggered
    ///               by the selected source.
    /// @details      When the conversion is finished, the given callback function
    ///               is called and the conversion gets automatically retriggered by
    ///               the selected source.
    /// @param[in]    callback    the callback function.
    // ----------------------------------------------------------------------------
    void adc_autoTrigger10(ADC_TriggerSource triggerSource, void (*callback)(uint16_t value));

#ifdef __cplusplus
};
#endif

#endif /* ADC_H_ */

/// @}
