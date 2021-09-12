#include "sr04.h"
#include <avr/interrupt.h>

enum SR04SensorPhase
{
    USS_WAIT = 0,
    USS_START = 1,
    USS_STOP = 2
};

struct SR04Sensor
{
    enum SR04SensorPhase phase;
    uint16_t startTime;
};

uint16_t sr04Distance_mm[8];
void (*sr04Callback)(uint16_t distance_mm[8]);

uint8_t sr04Sensors;
uint8_t sr04Levels;
uint16_t sr04Ticks;
volatile uint8_t *sr04TriggerPort;
uint8_t sr04TriggerPinNo;

struct SR04Sensor sr04Sensor[8];

// triggers a 10 microsecond pulse
void _sr04_trigger()
{
    (*(sr04TriggerPort - 1)) |= (1 << sr04TriggerPinNo);
    (*sr04TriggerPort) &= ~(1 << sr04TriggerPinNo);

    // 62.5ns * 8(PS) * 20 = 10us
    OCR4A = 19;
    TIMSK4 = (1 << OCIE4A);
    TCNT4 = 0;
    TCCR4A = 0;
    TCCR4B = ((1 << WGM42) | (1 << CS41));

    (*sr04TriggerPort) |= (1 << sr04TriggerPinNo);

    sei();
}

ISR(TIMER4_COMPA_vect)
{
    if ((*sr04TriggerPort) & (1 << sr04TriggerPinNo))
    {
        TCCR4B = ((1 << WGM42) | (1 << CS41) | (1 << CS40));
        (*sr04TriggerPort) &= ~(1 << sr04TriggerPinNo);
        // 62.5ns * 256(PS) = 16us => 16us/58 = 0.27cm
        TCNT4 = 0;
        OCR4A = 127;
        sr04Ticks = 0;
        sr04Levels = 0;
    }
    else
    {
        sr04Ticks++;
    }
}

void sr04_getDistance(uint8_t sensors, volatile uint8_t *pTriggerPort, uint8_t triggerPinNo, void (*callback)(uint16_t distances_mm[8]))
{
    sr04Sensors = sensors;
    sr04Callback = callback;
    sr04TriggerPort = pTriggerPort;
    sr04TriggerPinNo = triggerPinNo;
    sr04Levels = 0;

    // wait in case the sensors are not ready yet;
    while (PINK & sr04Sensors)
    {
    }

    _sr04_trigger();
    PCICR |= (1 << PCIE2);
    PCMSK2 = sensors;
}

ISR(PCINT2_vect)
{
    uint8_t i, j;
    uint8_t curLevels = (PINK & sr04Sensors);
    uint8_t levelsChanged = curLevels ^ sr04Levels;
    uint8_t done = 1;

    sr04Levels = curLevels;

    for (i = 1, j = 0; i > 0; i <<= 1, j++)
    {
        if (sr04Sensors & i)
        {
            if (levelsChanged & i)
            {
                if ((curLevels & i))
                {
                    sr04Sensor[j].phase = USS_START;
                    sr04Sensor[j].startTime = sr04Ticks * 256 + TCNT4;
                    done = 0;
                }
                else
                {
                    sr04Sensor[j].phase = USS_STOP;
                    uint16_t endTime = (sr04Ticks * 256 + TCNT4);
                    if (endTime >= sr04Sensor[j].startTime)
                    {
                        sr04Distance_mm[j] = ((endTime - sr04Sensor[j].startTime) * 16) / 58;
                    }
                    else
                    {
                        sr04Distance_mm[j] = ((65535 - sr04Sensor[j].startTime + endTime) * 16) / 58;
                    }
                }
            }
            else if (sr04Sensor[j].phase != USS_STOP)
            {
                done = 0;
            }
        }
    }
    if (done)
    {
        TCCR4B = 0;
        sr04Callback(sr04Distance_mm);
    }
}
