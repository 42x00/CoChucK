#ifndef COCHUCK_EXAMPLE_H
#define COCHUCK_EXAMPLE_H

#include "Chuck.h"

Generator<ChuckDur> example(Chuck *VM, ADC &adc, DAC &dac, Blackhole &blackhole,
                            ChuckTime &now, ChuckDur &samp, ChuckDur &second, ChuckDur &ms,
                            ChuckDur &minute, ChuckDur &hour, ChuckDur &day, ChuckDur &week) {
    SinOsc s(VM);
    s >> dac;
    while (true) {
        s.setFrequency(100 + rand() % 1000);
        co_yield 200 * ms;
    }
}

#endif //COCHUCK_EXAMPLE_H
