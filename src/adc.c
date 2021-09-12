#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "adc.h"
#include <uart.h>

static uint8_t (*adc_callback8)(uint8_t) = NULL;
static uint8_t (*adc_callback10)(uint16_t) = NULL;
static void (*adc_callbackAuto8)(uint8_t) = NULL;
static void (*adc_callbackAuto10)(uint16_t) = NULL;

static uint8_t _adc_initialized = 0;

static uint8_t adc_isInitialized()
{
    if (!_adc_initialized)
    {
        uart0_msg("adc: adc_init missing\n");
        return 0;
    }
    return 1;
}

void adc_init()
{
    if (_adc_initialized)
    {
        uart0_msg("adc: already initialized\n");
    }
    _adc_initialized = 1;
}

void adc_selectChannel(ADC_RefVoltage refVoltage, uint8_t channelNo)
{
    if (!adc_isInitialized())
    {
        return;
    }

    ADMUX = ((ADMUX & 0x3F) | // set the reference voltage
             (refVoltage << REFS0));

    if (channelNo <= 7) // select the desired channel
    {
        ADMUX = ((ADMUX & 0xE0) | (channelNo << MUX0));
        ADCSRB &= ~(1 << MUX5);
    }
    else if (channelNo <= 15)
    {
        ADMUX = ((ADMUX & 0xE0) | ((channelNo - 8) << MUX0));
        ADCSRB |= (1 << MUX5);
    }
}

void adc_trigger8(uint8_t (*callback)(uint8_t value))
{
    if (!adc_isInitialized())
    {
        return;
    }

    ADMUX |= (1 << ADLAR); // the result needs to be left aligned to
    // get 8-bit resolution
    ADCSRA = (1 << ADEN) | // enable the ADC
             (1 << ADIE) | // enable the ADC interrupt
             (7 << ADPS0); // set the clock divider to 128
    adc_callbackAuto8 = NULL;
    adc_callbackAuto10 = NULL;
    adc_callback8 = callback; // remember the callback
    adc_callback10 = NULL;
    ADCSRA |= (1 << ADSC); // start a conversion

    sei();
}

void adc_autoTrigger8(ADC_TriggerSource triggerSource, void (*callback)(uint8_t value))
{
    if (!adc_isInitialized())
    {
        return;
    }

    ADMUX |= (1 << ADLAR); // the result needs to be left aligned to
    // get 8-bit resolution
    ADCSRA = (1 << ADEN) |              // enable the ADC
             (1 << ADIE) |              // enable the ADC interrupt
             (7 << ADPS0) |             // set the clock divider to 128
             (1 << ADATE);              // enable automatic trigger
    ADCSRB &= ~(7 << ADTS0);            // clear the trigger source bits
    ADCSRB |= (triggerSource << ADTS0); // set the trigger source bits

    adc_callbackAuto8 = callback;
    adc_callbackAuto10 = NULL;
    adc_callback8 = NULL; // remember the callback
    adc_callback10 = NULL;

    sei();
}

void adc_trigger10(uint8_t (*callback)(uint16_t value))
{
    if (!adc_isInitialized())
    {
        return;
    }

    ADMUX &= ~(1 << ADLAR); // the result needs to be right aligned to
    // get the full (=10-bit) resolution
    ADCSRA = (1 << ADEN) | // enable the ADC
             (1 << ADIE) | // enable the ADC interrupt
             (7 << ADPS0); // set the clock divider to 128
    adc_callbackAuto8 = NULL;
    adc_callbackAuto10 = NULL;
    adc_callback8 = NULL;
    adc_callback10 = callback; // remember the callback
    ADCSRA |= (1 << ADSC);     // start a conversion

    sei();
}

void adc_autoTrigger10(ADC_TriggerSource triggerSource, void (*callback)(uint16_t value))
{
    if (!adc_isInitialized())
    {
        return;
    }

    ADMUX &= ~(1 << ADLAR); // the result needs to be left aligned to
    // get 8-bit resolution
    ADCSRA = (1 << ADEN) |              // enable the ADC
             (1 << ADIE) |              // enable the ADC interrupt
             (7 << ADPS0) |             // set the clock divider to 128
             (1 << ADATE);              // enable automatic trigger
    ADCSRB &= ~(7 << ADTS0);            // clear the trigger source bits
    ADCSRB |= (triggerSource << ADTS0); // set the trigger source bits

    adc_callbackAuto8 = NULL;
    adc_callbackAuto10 = callback;
    adc_callback8 = NULL; // remember the callback
    adc_callback10 = NULL;

    sei();
}

ISR(ADC_vect)
{
    uint8_t retVal = 0;

    if (adc_callback8)
        retVal = (*adc_callback8)(ADCH);
    if (adc_callback10)
        retVal = (*adc_callback10)(ADC);
    if (adc_callbackAuto8)
    {
        (*adc_callbackAuto8)(ADCH);
        return;
    }
    if (adc_callbackAuto10)
    {
        (*adc_callbackAuto10)(ADC);
        return;
    }
    if (retVal) // when the callback function returned 1
    {
        ADCSRA |= (1 << ADSC); // retrigger the conversion
    }
    else
    {
        ADCSRA &= ~(1 << ADEN); // otherwise, turn of the ADC
    }
}
